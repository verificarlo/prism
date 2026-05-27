#include "src/debug.h"
#include "src/eft.h"
#include "src/utils.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cmath>
#include <cstdint>
#include <cstring>
#include <unistd.h>

#if defined(PRISM_SR_SCALAR_INL_H) == defined(HWY_TARGET_TOGGLE)
#ifdef PRISM_SR_SCALAR_INL_H
#undef PRISM_SR_SCALAR_INL_H
#else
#define PRISM_SR_SCALAR_INL_H
#endif

#include "src/random-inl.h"
#include "src/xoshiro.h"

HWY_BEFORE_NAMESPACE(); // at file scope

namespace prism::sr::scalar::HWY_NAMESPACE {

namespace rng = prism::scalar::xoshiro::HWY_NAMESPACE;

template <typename T> auto isnumber(const T a, const T b) -> bool {
  using U = typename prism::utils::IEEE754<T>::U;
  debug_start();
  constexpr auto naninf_mask = prism::utils::IEEE754<T>::inf_nan_mask;
  constexpr int32_t mantissa = prism::utils::IEEE754<T>::mantissa;
  prism::utils::binaryN<T> a_bits = {.f = a};
  prism::utils::binaryN<T> b_bits = {.f = b};
  const U a_uint = a_bits.u;
  const U b_uint = b_bits.u;

  const int32_t t = prism::sr::get_virtual_precision<T>();
  bool ret = false;
  if ((t - 1) < mantissa) {
    // At reduced virtual precision, zero operands can produce results
    // that need rounding. Only reject inf/nan.
    ret = ((a_uint & naninf_mask) != naninf_mask) and
          ((b_uint & naninf_mask) != naninf_mask);
  } else {
    // At hardware precision, zero operands produce exact results.
    // fast check for a or b is not 0, inf or nan
    ret = ((a_uint != 0) and ((a_uint & naninf_mask) != naninf_mask)) and
          ((b_uint != 0) and ((b_uint & naninf_mask) != naninf_mask));
  }
  debug_print("a_bits = 0x%016x\n", a_uint);
  debug_print("b_bits = 0x%016x\n", b_uint);
  debug_print("isnumber(%.13a, %.13a) = %d\n", a, b, ret);
  debug_end();
  return ret;
}

// Virtual Stochastic Rounding
//
// Based on the algorithm in Fasi and Mikaitis: Algorithms for Stochastically
// Rounded Elementary Arithmetic Operations, originally implemented in Verrou,
// extended to
//  - support fma op
//  - support variable precision
//
// Given sigma + tau (an error-free representation of the exact result x),
// returns SR_t(x): the stochastically rounded result at virtual precision t.
//
// The result is returned directly.
//
// Proof of exact sign evaluation for D = (rho - pi) + tau:
//
// 1. |rho - pi| > |tau|
//    In this case, (rho - pi) might suffer rounding error in float.
//    However, since |tau| is bounded by 0.5 * ULP_hardware, and the
//    distance to the zero-boundary is greater than |tau|, the rounding
//    error cannot pull the result across zero. The boolean outcome of the
//    sign check remains invariant to the rounding.
//
// 2. |rho - pi| <= |tau|
//    Since |tau| < 0.5 * ULP_hardware, this condition implies rho and pi
//    share the same exponent and thus satisfy the Sterbenz Lemma
//    (y/2 <= x <= 2y). Thus, the subtraction (rho - pi) is exact.
//
//    (rho - pi) + tau can be inexact but we are only interested in its sign
//    and IEEE-754 guarantees monotonic rounding.
// ------------------------------------------------------------------------
template <typename T> inline auto round(const T sigma, const T tau) -> T {
  const int32_t t = prism::sr::get_virtual_precision<T>();
  using prism::utils::get_exponent;
  using prism::utils::get_predecessor_abs;
  using prism::utils::IEEE754;
  using prism::utils::pow2;

  debug_start();

  // compute trunc_t(sigma), the truncated value at precision t
  const T trunc = prism::sr::truncate_mantissa(sigma, t);

  // rho is the distance from sigma to trunc
  // the computation is exact in IEEE-754
  const T rho = sigma - trunc;

  // x = sigma + tau is exactly representable in precision t
  if (rho == 0 && tau == 0) {
    debug_end();
    return trunc;
  }

  // The distance between x and the truncated representable at precision t is
  // noted delta = x - trunc = rho + tau, with rho > tau
  const bool sign_tau = (tau < 0);
  const bool sign_rho = (rho < 0);
  const bool sign_trunc = (trunc < 0);
  const bool sign_delta = (rho == 0) ? sign_tau : sign_rho;
  const T sign_dir = sign_delta ? T{-1} : T{1};

  // If the distance pushes us in the opposite direction of our value,
  // we are moving towards zero, so the predecessor might drop an exponent.
  const int32_t eta = (sign_delta != sign_trunc)
                          ? get_exponent(get_predecessor_abs(trunc))
                          : get_exponent(trunc);

  // Compute ulp at precision t
  const T ulp_t = sign_dir * pow2<T>(eta - (t - 1));

  // For ulp_t subnormals we scale the variables by 2^64 to lift them into the
  // normal range to preserve full random precision when generating pi.
  // (scaling is exact)
  constexpr int32_t min_exp = IEEE754<T>::min_exponent;
  const T scale = (eta - (t - 1) <= min_exp) ? pow2<T>(64) : T{1};
  const T sc_rho = rho * scale;
  const T sc_tau = tau * scale;
  const T sc_ulp = ulp_t * scale;

  // We sample pi in Uniform(0, sc_ulp)
  const T z = rng::uniform(T{});
  const T pi = sc_ulp * z;

  // We want to check if P < |x - trunc| where P = abs(pi).
  // We evaluate this by checking the sign of D:
  const T D = (sc_rho - pi) + sc_tau;

  // When delta and D have the same sign then the random threshold is crossed
  // and we must round-up. We use the unscaled ulp_t for the result.
  const T rnd = (D * sign_dir >= 0) ? ulp_t : T{0};

  debug_print("z         = %+.13a\n", z);
  debug_print("sigma     = %+.13a\n", sigma);
  debug_print("trunc     = %+.13a\n", trunc);
  debug_print("rho       = %+.13a\n", rho);
  debug_print("eta       = %d\n", eta);
  debug_print("ulp_t     = %+.13a\n", ulp_t);
  debug_print("D         = %+.13a\n", D);
  debug_end();

  return trunc + rnd;
}

template <typename T> inline auto add(const T a, const T b) -> T {
  debug_start();
  if (not isnumber(a, b)) {
    debug_end();
    return a + b;
  }
  T tau;
  T sigma;
  twosum(a, b, sigma, tau);
  const T res = round(sigma, tau);
  debug_print("sr_add(%+.13a, %+.13a) = %+.13a\n", a, b, res);
  debug_end();
  return res;
}

template <typename T> inline auto sub(T a, T b) -> T { return add(a, -b); }

template <typename T> inline auto mul(T a, T b) -> T {
  debug_start();
  if (not isnumber(a, b)) {
    debug_end();
    return a * b;
  }
  T tau;
  T sigma;
  twoprodfma(a, b, sigma, tau);
  const T res = round(sigma, tau);
  debug_end();
  return res;
}

template <typename T> inline auto div(const T a, const T b) -> T {
  debug_start();
  if (not isnumber(a, b)) {
    debug_end();
    return a / b;
  }
  const T sigma = a / b;
  debug_print("sigma = %+.13a / %+.13a = %+.13a\n", a, b, sigma);

  const T tau = std::fma(-sigma, b, a) / b;

  debug_print("-sigma * b + a = %+.13a * %+.13a + %+.13a = %+.13a\n", -sigma, b,
              a, std::fma(-sigma, b, a));
  debug_print("tau = (-%+.13a * %+.13a + %+.13a) / %+.13a\n", -sigma, b, a, b);

  const T res = round(sigma, tau);
  debug_print("sr_div(%+.13a, %+.13a) = %+.13a\n", a, b, res);
  debug_end();
  return res;
}

template <typename T> inline auto sqrt(const T a) -> T {
  const T sigma = std::sqrt(a);
  if (not std::isfinite(a) or a <= 0) {
    return sigma;
  }
  const T tau_p = std::fma(-sigma, sigma, a);
  const T tau = tau_p / (2 * sigma);
  return round(sigma, tau);
}

/*
"Exact and Approximated error of the FMA"
Sylvie Boldo, Jean-Michel Muller
---
Algorithm 5 (ErrFmaNearest):
  r1 = ◦(ax + y)
  (u1, u2) = Fast2Mult(a, x)
  (α1, α2) = 2Sum(y, u2)
  (β1, β2) = 2Sum(u1, α1)
  γ = ◦(◦(β1 − r1) + β2)
  r2 = ◦(γ + α2)
*/
template <typename T> inline auto fma(const T a, const T b, const T c) -> T {
  if (not std::isfinite(a) or not std::isfinite(b) or not std::isfinite(c)) {
    return std::fma(a, b, c);
  }
  debug_start();
  T u1;
  T u2;
  T alpha1;
  T alpha2;
  T beta1;
  T beta2;
  T gamma;
  T r1;
  T r2;
  r1 = std::fma(a, b, c);
  twoprodfma(a, b, u1, u2);
  twosum(c, u2, alpha1, alpha2);
  twosum(u1, alpha1, beta1, beta2);
  gamma = (beta1 - r1) + beta2;
  r2 = gamma + alpha2;
  const T res = round(r1, r2);
  debug_print("sr_fma(%+.13a, %+.13a, %+.13a) = %+.13a\n", a, b, c, res);
  debug_end();
  return res;
}

/* binary32 */
inline auto addf32(float a, float b) -> float { return add(a, b); }
inline auto subf32(float a, float b) -> float { return sub(a, b); }
inline auto mulf32(float a, float b) -> float { return mul(a, b); }
inline auto divf32(float a, float b) -> float { return div(a, b); }
inline auto sqrtf32(float a) -> float { return sqrt(a); }
inline auto fmaf32(float a, float b, float c) -> float { return fma(a, b, c); }

/* binary64 */
inline auto addf64(double a, double b) -> double { return add(a, b); }
inline auto subf64(double a, double b) -> double { return sub(a, b); }
inline auto mulf64(double a, double b) -> double { return mul(a, b); }
inline auto divf64(double a, double b) -> double { return div(a, b); }
inline auto sqrtf64(double a) -> double { return sqrt(a); }
inline auto fmaf64(double a, double b, double c) -> double {
  return fma(a, b, c);
}

} // namespace prism::sr::scalar::HWY_NAMESPACE

HWY_AFTER_NAMESPACE();

#endif // PRISM_SR_SCALAR_INL_H
