#ifndef __PRISM_UTILS_H__
#define __PRISM_UTILS_H__

#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "debug.h"

#if defined(__x86_64__)
using Float128 = __float128;
#else
using Float128 = _Float128;
#endif

namespace prism::utils {

template <typename T> struct IEEE754 {};

// specialize IEEE754 for float and double

template <> struct IEEE754<float> {
  using I = std::int32_t;
  using U = std::uint32_t;
  using H = double;
  static constexpr I sign = 1;
  static constexpr I exponent = 8;
  static constexpr I mantissa = 23;
  static constexpr I precision = 24;
  static constexpr I precision10 = 7;
  static constexpr float ulp = 0x1.0p-23F;
  static constexpr float half_ulp = 0x1.0p-24F;
  static constexpr I bias = 127;
  static constexpr U exponent_mask = 0x7F8;
  static constexpr U exponent_mask_scaled = 0x7f800000;
  static constexpr float max_normal = 0x1.fffffep127F;
  static constexpr float min_normal = 0x1.0p-126F;
  static constexpr float min_subnormal = 0x1.0p-149F;
  static constexpr I min_exponent = -126;
  static constexpr I max_exponent = 127;
  static constexpr I min_exponent_subnormal = -149;
  static constexpr U inf_nan_mask = 0x7F800000;
  static constexpr const char *format = "%.6a";
};

template <> struct IEEE754<double> {
  using I = std::int64_t;
  using U = std::uint64_t;
  using H = Float128;
  static constexpr I sign = 1;
  static constexpr I exponent = 11;
  static constexpr I mantissa = 52;
  static constexpr I precision = 53;
  static constexpr I precision10 = 17;
  static constexpr double ulp = 0x1.0p-52;
  static constexpr double half_ulp = 0x1.0p-53;
  static constexpr I bias = 1023;
  static constexpr U exponent_mask = 0x7FF;
  static constexpr U exponent_mask_scaled = 0x7ff0000000000000ULL;
  static constexpr double max_normal = 0x1.fffffffffffffp1023;
  static constexpr double min_normal = 0x1.0p-1022;
  static constexpr double min_subnormal = 0x1.0p-1074;
  static constexpr I min_exponent = -1022;
  static constexpr I max_exponent = 1023;
  static constexpr I min_exponent_subnormal = -1074;
  static constexpr U inf_nan_mask = 0x7FF0000000000000;
  static constexpr const char *format = "%.13a";
};

template <typename T> union binaryN {};

template <> union binaryN<float> {
  float f;
  uint32_t u;
  int32_t i;
};

template <> union binaryN<double> {
  double f;
  uint64_t u;
  int64_t i;
};

// Implement other functions (get_exponent, predecessor, abs, pow2, etc.) using
// templates
// "Emulating round-to-nearest ties-to-zero "augmented” floating-point
// operations using round-to-nearest ties-to-even arithmetic"
// Sylvie Boldo, Christoph Q. Lauter, Jean-Michel Muller
// ALGORITHM 4: MyulpH(𝑎): Computes
// sign(𝑎) · pred(|𝑎|) and sign(𝑎) · ulp𝐻 (𝑎) for |𝑎| > 2𝑒min .
// Uses the FP constant 𝜓 = 1 − 2^{−𝑝} .
// 𝑧 ← RN𝑒 (𝜓𝑎) (= pred(|a|))
// 𝛿 ← RN𝑒 (𝑎 − 𝑧)
// return (z,𝛿)
template <typename T> inline auto get_predecessor_abs(T a) -> T {
  constexpr auto half_ulp = IEEE754<T>::ulp / 2;
  const T phi = 1.0 - half_ulp;
  const T z = a * phi;
  return z;
}

template <typename T, typename U = typename IEEE754<T>::U>
inline auto get_unbiased_exponent(T a) -> U {
  if (a == 0) {
    return 0;
  }
  constexpr U mantissa = IEEE754<T>::mantissa;
  constexpr U exponent_mask = IEEE754<T>::exponent_mask;
  U exp;
  std::memcpy(&exp, &a, sizeof(T));
  exp = ((exp >> mantissa) & exponent_mask);
  return exp;
}

template <typename T, typename I = typename IEEE754<T>::I>
inline auto get_exponent(T a) -> I {
  debug_start();
  if (a == 0) {
    debug_end();
    return IEEE754<T>::min_exponent;
  }
  using U = typename IEEE754<T>::U;
  constexpr I bias = IEEE754<T>::bias;
  constexpr I mantissa = IEEE754<T>::mantissa;
  constexpr U exponent_mask = IEEE754<T>::exponent_mask_scaled;
  debug_print("a = %+.13a\n", a);
  debug_print("bias = %d\n", bias);
  debug_print("mantissa = %d\n", mantissa);
  debug_print("exponent_mask = %d\n", exponent_mask);
  I a_bits;
  std::memcpy(&a_bits, &a, sizeof(T));
  debug_print("a = 0x%016x\n", a_bits);
  const auto raw_exp = (a_bits & exponent_mask) >> mantissa;
  debug_print("raw exponent = %d\n", raw_exp);

  if (raw_exp == 0) {
    debug_print("get_exponent(%.13a) = %d (subnormal)\n", a,
                IEEE754<T>::min_exponent);
    debug_end();
    return IEEE754<T>::min_exponent;
  }

  const I exp = raw_exp - bias;
  debug_print("get_exponent(%.13a) = %d\n", a, exp);
  debug_end();
  return exp;
}

template <typename T> inline auto pow2(int n) -> T {
  // if n <= min_exponent, take into account precision loss due to subnormal
  // numbers
  using U = typename IEEE754<T>::I;
  constexpr auto mantissa = IEEE754<T>::mantissa;
  constexpr auto min_exponent = IEEE754<T>::min_exponent;

  const auto is_subnormal = n < min_exponent;
  const int precision_loss = (is_subnormal) ? min_exponent - n : 0;
  n = (is_subnormal) ? 1 : n;
  T res = (is_subnormal) ? 0 : 1;
  U i;
  std::memcpy(&i, &res, sizeof(U));
  i += static_cast<U>(n) << (mantissa - precision_loss);
  std::memcpy(&res, &i, sizeof(T));

  debug_print("pow2(%d) = %.13a\n", n, res);

  return res;
}

template <typename T> inline auto add_round_odd(T a, T b) -> T {
  // return addition with rounding to odd
  // https://www.lri.fr/~melquion/doc/08-tc.pdf
  T x;
  T e;
  twosum(a, b, x, e);
  using U = typename IEEE754<T>::U;
  U bits;
  std::memcpy(&bits, &x, sizeof(T));
  if (e == 0 || (bits & 1)) {
    return x;
  }

  bits += ((x < 0) == (e < 0)) ? 1 : static_cast<U>(-1);
  std::memcpy(&x, &bits, sizeof(T));
  return x;
}

auto predecessor_float(float a) -> float;
auto predecessor_double(double a) -> double;

auto get_exponent_float(float a) -> int32_t;
auto get_exponent_double(double a) -> int64_t;

auto pow2_float(int32_t n) -> float;
auto pow2_double(int64_t n) -> double;

} // namespace prism::utils

namespace prism::sr {
// Rounding mode constants
constexpr int32_t PRISM_SR = 0; // Stochastic Rounding
constexpr int32_t PRISM_RN = 1; // Round-to-Nearest (untied, ties away from zero)

// Process-wide configuration.
//
// Virtual precision and rounding mode are kept thread-locally so that threads
// may round independently, but they are *set* process-wide: a caller changing
// precision between two phases of a computation means it for every thread, not
// only its own. A thread cannot be written to from outside, so the two are
// reconciled by an epoch. Every process-wide setter publishes new defaults and
// bumps the epoch; a thread compares the epoch against the one it last observed
// and refreshes its copy when they differ. Each arithmetic operation captures
// one configuration snapshot at entry. That snapshot costs one acquire load of
// the epoch and keeps precision and rounding mode consistent for the full
// operation.
//
// Without this, a setter is a silent no-op for any thread that has already
// executed instrumented arithmetic: the thread copied the default on first use
// and never looks at it again, while the getter keeps reporting the new value.
inline std::atomic<int32_t> default_virtual_precision_f32{
    utils::IEEE754<float>::precision};
inline std::atomic<int32_t> default_virtual_precision_f64{
    utils::IEEE754<double>::precision};
inline std::atomic<int32_t> default_rounding_mode{PRISM_SR};

// Bumped by every process-wide setter. Release/acquire pairing with the
// defaults above: a thread that sees a new epoch also sees the values that
// were published before it.
inline std::atomic<uint32_t> config_epoch{0};

// Per-thread configuration. The initializers cover threads created before the
// first setter runs; every later change arrives through the epoch.
inline thread_local int32_t virtual_precision_f32 =
    default_virtual_precision_f32.load(std::memory_order_relaxed);
inline thread_local int32_t virtual_precision_f64 =
    default_virtual_precision_f64.load(std::memory_order_relaxed);
inline thread_local int32_t rounding_mode =
    default_rounding_mode.load(std::memory_order_relaxed);
inline thread_local uint32_t observed_epoch = 0;

// Adopts the process-wide configuration if it changed since this thread last
// looked. All three settings refresh together, so a kernel cannot mix a
// precision from one epoch with a rounding mode from another.
inline void refresh_thread_config() {
  const uint32_t epoch = config_epoch.load(std::memory_order_acquire);
  if (epoch == observed_epoch) {
    return;
  }
  virtual_precision_f32 =
      default_virtual_precision_f32.load(std::memory_order_relaxed);
  virtual_precision_f64 =
      default_virtual_precision_f64.load(std::memory_order_relaxed);
  rounding_mode = default_rounding_mode.load(std::memory_order_relaxed);
  observed_epoch = epoch;
}

struct ConfigSnapshot {
  int32_t virtual_precision;
  int32_t rounding_mode;
};

// Refresh once, then copy the configuration relevant to one arithmetic
// operation. Callers must reuse this snapshot instead of consulting TLS again
// midway through the operation.
template <typename T> inline auto get_config_snapshot() -> ConfigSnapshot {
  refresh_thread_config();
  if constexpr (std::is_same_v<T, float>) {
    return {virtual_precision_f32, rounding_mode};
  } else if constexpr (std::is_same_v<T, double>) {
    return {virtual_precision_f64, rounding_mode};
  } else {
    static_assert(!sizeof(T), "get_config_snapshot: unsupported type");
  }
}

template <typename T> inline auto get_virtual_precision() -> int32_t {
  refresh_thread_config();
  if constexpr (std::is_same_v<T, float>) {
    return virtual_precision_f32;
  } else if constexpr (std::is_same_v<T, double>) {
    return virtual_precision_f64;
  } else {
    static_assert(!sizeof(T), "get_virtual_precision: unsupported type");
  }
}

inline auto get_rounding_mode() -> int32_t {
  refresh_thread_config();
  return rounding_mode;
}

// Thread-local override. Stamps the current epoch so the value survives until
// the next process-wide setter, which discards it.
template <typename T> inline void set_virtual_precision(int32_t t) {
  constexpr int32_t precision = prism::utils::IEEE754<T>::mantissa + 1;
  assert(t >= 2 && t <= precision);

  refresh_thread_config();
  if constexpr (std::is_same_v<T, float>) {
    virtual_precision_f32 = t;
  } else if constexpr (std::is_same_v<T, double>) {
    virtual_precision_f64 = t;
  } else {
    static_assert(!sizeof(T), "set_virtual_precision: unsupported type");
  }
}

inline void set_rounding_mode(int32_t mode) {
  assert(mode == PRISM_SR || mode == PRISM_RN);
  refresh_thread_config();
  rounding_mode = mode;
}

// Process-wide setters. Publish the new value, then bump the epoch so every
// other thread picks it up on its next operation.
template <typename T> inline void set_default_virtual_precision(int32_t t) {
  constexpr int32_t precision = prism::utils::IEEE754<T>::mantissa + 1;
  assert(t >= 2 && t <= precision);

  if constexpr (std::is_same_v<T, float>) {
    default_virtual_precision_f32.store(t, std::memory_order_relaxed);
  } else if constexpr (std::is_same_v<T, double>) {
    default_virtual_precision_f64.store(t, std::memory_order_relaxed);
  } else {
    static_assert(!sizeof(T), "set_default_virtual_precision: unsupported type");
  }
  config_epoch.fetch_add(1, std::memory_order_release);
}

inline void set_default_rounding_mode(int32_t mode) {
  assert(mode == PRISM_SR || mode == PRISM_RN);
  default_rounding_mode.store(mode, std::memory_order_relaxed);
  config_epoch.fetch_add(1, std::memory_order_release);
}

// Helper to mask off the lower bits of the mantissa to match a virtual
// precision t
template <typename T>
inline auto truncate_mantissa(const T val, const int32_t t) -> T {
  constexpr int32_t mantissa = prism::utils::IEEE754<T>::mantissa;

  // If virtual precision meets or exceeds hardware, no truncation needed
  if (t >= mantissa + 1) {
    return val;
  }

  using UintT = typename prism::utils::IEEE754<T>::U;
  UintT bits;
  std::memcpy(&bits, &val, sizeof(T));

  const int32_t shift = mantissa - (t - 1);
  const UintT mask = ~((static_cast<UintT>(1) << shift) - 1);
  bits &= mask;

  T res;
  std::memcpy(&res, &bits, sizeof(T));
  return res;
}
} // namespace prism::sr

#endif // __PRISM_UTILS_H__
