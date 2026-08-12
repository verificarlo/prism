#ifdef PRISM_IDE // To enable clangd to parse the file
#include "hwy/highway.h"
#include "hwy/tests/test_util-inl.h"
#include <type_traits>
#define PRISM_PR_MODE_NAMESPACE fakens
#undef HWY_MAX_BYTES
#define HWY_MAX_BYTES UINT32_MAX
#define PRISM_DISPATCH fakens2
namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH::HWY_NAMESPACE {
auto round = [](auto d, auto a, auto b) { return a; };
auto add = [](auto d, auto a, auto b) { return a; };
auto sub = [](auto d, auto a, auto b) { return a; };
auto mul = [](auto d, auto a, auto b) { return a; };
auto div = [](auto d, auto a, auto b) { return a; };
auto sqrt = [](auto d, auto a) { return a; };
auto fma = [](auto d, auto a, auto b, auto c) { return a; };
} // namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH::HWY_NAMESPACE
#endif

#include <type_traits>
#include <cmath>

#ifndef PRISM_PR_MODE_NAMESPACE
#error "PRISM_PR_MODE_NAMESPACE must be defined"
#endif
#ifndef PRISM_PR_MODE
#error "PRISM_PR_MODE must be defined"
#endif

// NOLINTBEGIN
HWY_BEFORE_NAMESPACE();
namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH {

/* Variable size functions */
namespace variable::HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;
namespace pr = PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH::HWY_NAMESPACE;

#if PRISM_PR_MODE == PRISM_SR_MODE
template <typename T>
HWY_FLATTEN void _round(const T *HWY_RESTRICT sigma, const T *HWY_RESTRICT tau,
                        T *HWY_RESTRICT result, const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);
  const auto config = prism::sr::get_config_snapshot<T>();

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto sigma_vec = hn::LoadN(d, sigma + i, lanes);
    auto tau_vec = hn::LoadN(d, tau + i, lanes);
    auto res = pr::round(d, sigma_vec, tau_vec, config);
    hn::StoreN(res, d, result + i, lanes);
  }
}
#elif PRISM_PR_MODE == PRISM_UD_MODE
template <typename T>
HWY_INLINE void _round(const T *HWY_RESTRICT a, T *HWY_RESTRICT result,
                       const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto res = pr::round(d, a_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}
#else
#error "Invalid PRISM_PR_MODE"
#endif

template <typename T>
HWY_FLATTEN void _add(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                      T *HWY_RESTRICT result, const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto b_vec = hn::LoadN(d, b + i, lanes);
    auto res = pr::add(d, a_vec, b_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

template <typename T>
HWY_FLATTEN void _sub(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                      T *HWY_RESTRICT result, const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);
  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto b_vec = hn::LoadN(d, b + i, lanes);
    auto res = pr::sub(d, a_vec, b_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

template <typename T>
HWY_FLATTEN void _mul(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                      T *HWY_RESTRICT result, const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);
  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto b_vec = hn::LoadN(d, b + i, lanes);
    auto res = pr::mul(d, a_vec, b_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

template <typename T>
HWY_FLATTEN void _div(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                      T *HWY_RESTRICT result, const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto b_vec = hn::LoadN(d, b + i, lanes);
    auto res = pr::div(d, a_vec, b_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

template <typename T>
HWY_FLATTEN void _sqrt(const T *HWY_RESTRICT a, T *HWY_RESTRICT result,
                       const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto res = pr::sqrt(d, a_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

template <typename T>
HWY_FLATTEN void _fma(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                      const T *HWY_RESTRICT c, T *HWY_RESTRICT result,
                      const size_t count) {
  using D = hn::ScalableTag<T>;
  const D d{};
  const size_t N = hn::Lanes(d);

  for (size_t i = 0; i < count; i += N) {
    const size_t remaining = count - i;
    const size_t lanes = remaining < N ? remaining : N;
    auto a_vec = hn::LoadN(d, a + i, lanes);
    auto b_vec = hn::LoadN(d, b + i, lanes);
    auto c_vec = hn::LoadN(d, c + i, lanes);
    auto res = pr::fma(d, a_vec, b_vec, c_vec);
    hn::StoreN(res, d, result + i, lanes);
  }
}

/* Variable size specialization */

/* binary32 */

#if PRISM_PR_MODE == PRISM_SR_MODE
inline void _round_f32(const float *HWY_RESTRICT sigma,
                       const float *HWY_RESTRICT tau,
                       float *HWY_RESTRICT result, const size_t count) {
  _round(sigma, tau, result, count);
}
#elif PRISM_PR_MODE == PRISM_UD_MODE
inline void _round_f32(const float *HWY_RESTRICT a, float *HWY_RESTRICT result,
                       const size_t count) {
  _round(a, result, count);
}
#else
#error "Invalid PRISM_PR_MODE"
#endif

inline void _add_f32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
                     float *HWY_RESTRICT result, const size_t count) {
  _add(a, b, result, count);
}

inline void _sub_f32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
                     float *HWY_RESTRICT result, const size_t count) {
  _sub(a, b, result, count);
}

inline void _mul_f32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
                     float *HWY_RESTRICT result, const size_t count) {
  _mul(a, b, result, count);
}

inline void _div_f32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
                     float *HWY_RESTRICT result, const size_t count) {
  _div(a, b, result, count);
}

inline void _sqrt_f32(const float *HWY_RESTRICT a, float *HWY_RESTRICT result,
                      const size_t count) {
  _sqrt(a, result, count);
}

inline void _fma_f32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
                     const float *HWY_RESTRICT c, float *HWY_RESTRICT result,
                     const size_t count) {
  _fma(a, b, c, result, count);
}

/* binary64 */

#if PRISM_PR_MODE == PRISM_SR_MODE
inline void _round_f64(const double *HWY_RESTRICT sigma,
                       const double *HWY_RESTRICT tau,
                       double *HWY_RESTRICT result, const size_t count) {
  _round(sigma, tau, result, count);
}
#elif PRISM_PR_MODE == PRISM_UD_MODE
inline void _round_f64(const double *HWY_RESTRICT a,
                       double *HWY_RESTRICT result, const size_t count) {
  _round(a, result, count);
}
#else
#error "Invalid PRISM_PR_MODE"
#endif

inline void _add_f64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
                     double *HWY_RESTRICT result, const size_t count) {
  _add(a, b, result, count);
}

inline void _sub_f64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
                     double *HWY_RESTRICT result, const size_t count) {
  _sub(a, b, result, count);
}

inline void _mul_f64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
                     double *HWY_RESTRICT result, const size_t count) {
  _mul(a, b, result, count);
}

inline void _div_f64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
                     double *HWY_RESTRICT result, const size_t count) {
  _div(a, b, result, count);
}

inline void _sqrt_f64(const double *HWY_RESTRICT a, double *HWY_RESTRICT result,
                      const size_t count) {
  _sqrt(a, result, count);
}

inline void _fma_f64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
                     const double *HWY_RESTRICT c, double *HWY_RESTRICT result,
                     const size_t count) {
  _fma(a, b, c, result, count);
}
} // namespace variable::HWY_NAMESPACE

namespace fixed::HWY_NAMESPACE {
/* Fixed size functions */

namespace hn = hwy::HWY_NAMESPACE;
namespace pr = PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH::HWY_NAMESPACE;

template <typename T, std::size_t N>
HWY_FLATTEN void _addxN(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                        T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    const auto vb = hn::Set(d, b[i]);
    auto res = pr::add(d, va, vb);
    result[i] = hn::GetLane(res);
  }
}

template <typename T, std::size_t N>
HWY_FLATTEN void _subxN(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                        T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    const auto vb = hn::Set(d, b[i]);
    auto res = pr::sub(d, va, vb);
    result[i] = hn::GetLane(res);
  }
}

template <typename T, std::size_t N>
HWY_FLATTEN void _mulxN(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                        T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    const auto vb = hn::Set(d, b[i]);
    auto res = pr::mul(d, va, vb);
    result[i] = hn::GetLane(res);
  }
}

template <typename T, std::size_t N>
HWY_FLATTEN void _divxN(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                        T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    const auto vb = hn::Set(d, b[i]);
    auto res = pr::div(d, va, vb);
    result[i] = hn::GetLane(res);
  }
}

template <typename T, std::size_t N>
HWY_FLATTEN void _sqrtxN(const T *HWY_RESTRICT a, T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    auto res = pr::sqrt(d, va);
    result[i] = hn::GetLane(res);
  }
}

template <typename T, std::size_t N>
HWY_FLATTEN void _fmaxN(const T *HWY_RESTRICT a, const T *HWY_RESTRICT b,
                        const T *HWY_RESTRICT c, T *HWY_RESTRICT result) {
  // Use element-wise processing for all targets to avoid FixedTag limitations
  for (std::size_t i = 0; i < N; ++i) {
    using D = hn::ScalableTag<T>;
    const D d{};
    const auto va = hn::Set(d, a[i]);
    const auto vb = hn::Set(d, b[i]);
    const auto vc = hn::Set(d, c[i]);
    auto res = pr::fma(d, va, vb, vc);
    result[i] = hn::GetLane(res);
  }
}

/* _<op>x<size>_<type> */

#define define_fp_xN_unary_op(type, name, size, op)                            \
  void _##op##x##size##_##name(const type *HWY_RESTRICT a,                     \
                               type *HWY_RESTRICT result) {                    \
    _##op##xN<type, size>(a, result);                                          \
  }

#define define_fp_xN_bin_op(type, name, size, op)                              \
  void _##op##x##size##_##name(const type *HWY_RESTRICT a,                     \
                               const type *HWY_RESTRICT b,                     \
                               type *HWY_RESTRICT result) {                    \
    _##op##xN<type, size>(a, b, result);                                       \
  }

#define define_fp_xN_ter_op(type, name, size, op)                              \
  void _##op##x##size##_##name(                                                \
      const type *HWY_RESTRICT a, const type *HWY_RESTRICT b,                  \
      const type *HWY_RESTRICT c, type *HWY_RESTRICT r) {                      \
    _##op##xN<type, size>(a, b, c, r);                                         \
  }

/* 32-bits */
#if HWY_MAX_BYTES >= 4
define_fp_xN_bin_op(float, f32, 1, add);
define_fp_xN_bin_op(float, f32, 1, sub);
define_fp_xN_bin_op(float, f32, 1, mul);
define_fp_xN_bin_op(float, f32, 1, div);
define_fp_xN_unary_op(float, f32, 1, sqrt);
define_fp_xN_ter_op(float, f32, 1, fma);
#endif

/* 64-bits */
#if HWY_MAX_BYTES >= 8
define_fp_xN_bin_op(double, f64, 1, add);
define_fp_xN_bin_op(double, f64, 1, sub);
define_fp_xN_bin_op(double, f64, 1, mul);
define_fp_xN_bin_op(double, f64, 1, div);
define_fp_xN_unary_op(double, f64, 1, sqrt);
define_fp_xN_ter_op(double, f64, 1, fma);

define_fp_xN_bin_op(float, f32, 2, add);
define_fp_xN_bin_op(float, f32, 2, sub);
define_fp_xN_bin_op(float, f32, 2, mul);
define_fp_xN_bin_op(float, f32, 2, div);
define_fp_xN_unary_op(float, f32, 2, sqrt);
define_fp_xN_ter_op(float, f32, 2, fma);
#endif

/* 128-bits */
#if HWY_MAX_BYTES >= 16
define_fp_xN_bin_op(double, f64, 2, add);
define_fp_xN_bin_op(double, f64, 2, sub);
define_fp_xN_bin_op(double, f64, 2, mul);
define_fp_xN_bin_op(double, f64, 2, div);
define_fp_xN_unary_op(double, f64, 2, sqrt);
define_fp_xN_ter_op(double, f64, 2, fma);

define_fp_xN_bin_op(float, f32, 4, add);
define_fp_xN_bin_op(float, f32, 4, sub);
define_fp_xN_bin_op(float, f32, 4, mul);
define_fp_xN_bin_op(float, f32, 4, div);
define_fp_xN_unary_op(float, f32, 4, sqrt);
define_fp_xN_ter_op(float, f32, 4, fma);
#endif

/* 256-bits */
#if HWY_MAX_BYTES >= 32
define_fp_xN_bin_op(double, f64, 4, add);
define_fp_xN_bin_op(double, f64, 4, sub);
define_fp_xN_bin_op(double, f64, 4, mul);
define_fp_xN_bin_op(double, f64, 4, div);
define_fp_xN_unary_op(double, f64, 4, sqrt);
define_fp_xN_ter_op(double, f64, 4, fma);

define_fp_xN_bin_op(float, f32, 8, add);
define_fp_xN_bin_op(float, f32, 8, sub);
define_fp_xN_bin_op(float, f32, 8, mul);
define_fp_xN_bin_op(float, f32, 8, div);
define_fp_xN_unary_op(float, f32, 8, sqrt);
define_fp_xN_ter_op(float, f32, 8, fma);
#endif

/* 512-bits */
#if HWY_MAX_BYTES >= 64
define_fp_xN_bin_op(double, f64, 8, add);
define_fp_xN_bin_op(double, f64, 8, sub);
define_fp_xN_bin_op(double, f64, 8, mul);
define_fp_xN_bin_op(double, f64, 8, div);
define_fp_xN_unary_op(double, f64, 8, sqrt);
define_fp_xN_ter_op(double, f64, 8, fma);

define_fp_xN_bin_op(float, f32, 16, add);
define_fp_xN_bin_op(float, f32, 16, sub);
define_fp_xN_bin_op(float, f32, 16, mul);
define_fp_xN_bin_op(float, f32, 16, div);
define_fp_xN_unary_op(float, f32, 16, sqrt);
define_fp_xN_ter_op(float, f32, 16, fma);
#endif

} // namespace fixed::HWY_NAMESPACE

} // namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH
HWY_AFTER_NAMESPACE();

#if HWY_ONCE

namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH {

namespace variable {

namespace { // NOLINT

/* Variable size exports */

HWY_EXPORT(_round_f32);
HWY_EXPORT(_add_f32);
HWY_EXPORT(_sub_f32);
HWY_EXPORT(_mul_f32);
HWY_EXPORT(_div_f32);
HWY_EXPORT(_sqrt_f32);
HWY_EXPORT(_fma_f32);

HWY_EXPORT(_round_f64);
HWY_EXPORT(_add_f64);
HWY_EXPORT(_sub_f64);
HWY_EXPORT(_mul_f64);
HWY_EXPORT(_div_f64);
HWY_EXPORT(_sqrt_f64);
HWY_EXPORT(_fma_f64);

} // namespace

/* Variable size functions */

/* binary32 */
#if PRISM_PR_MODE == PRISM_SR_MODE
void round(const float *HWY_RESTRICT sigma, const float *HWY_RESTRICT tau,
           float *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_round_f32)(sigma, tau, result, count);
}
#elif PRISM_PR_MODE == PRISM_UD_MODE
void round(const float *HWY_RESTRICT a, float *HWY_RESTRICT result,
           const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_round_f32)(a, result, count);
}
#else
#error "Invalid PRISM_PR_MODE"
#endif

void addf32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
            float *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_add_f32)(a, b, result, count);
}

void subf32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
            float *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_sub_f32)(a, b, result, count);
}

void mulf32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
            float *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_mul_f32)(a, b, result, count);
}

void divf32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
            float *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_div_f32)(a, b, result, count);
}

void sqrtf32(const float *HWY_RESTRICT a, float *HWY_RESTRICT result,
             const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_sqrt_f32)(a, result, count);
}

void fmaf32(const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,
            const float *HWY_RESTRICT c, float *HWY_RESTRICT result,
            const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_fma_f32)(a, b, c, result, count);
}

/* binary64 */
#if PRISM_PR_MODE == PRISM_SR_MODE
void round(const double *HWY_RESTRICT sigma, const double *HWY_RESTRICT tau,
           double *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_round_f64)(sigma, tau, result, count);
}
#elif PRISM_PR_MODE == PRISM_UD_MODE
void round(const double *HWY_RESTRICT a, double *HWY_RESTRICT result,
           const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_round_f64)(a, result, count);
}
#else
#error "Invalid PRISM_PR_MODE"
#endif

void addf64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
            double *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_add_f64)(a, b, result, count);
}

void subf64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
            double *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_sub_f64)(a, b, result, count);
}

void mulf64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
            double *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_mul_f64)(a, b, result, count);
}

void divf64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
            double *HWY_RESTRICT result, const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_div_f64)(a, b, result, count);
}

void sqrtf64(const double *HWY_RESTRICT a, double *HWY_RESTRICT result,
             const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_sqrt_f64)(a, result, count);
}

void fmaf64(const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,
            const double *HWY_RESTRICT c, double *HWY_RESTRICT result,
            const size_t count) {
  return HWY_DYNAMIC_DISPATCH(_fma_f64)(a, b, c, result, count);
}

/* Single vector instructions with dynamic dispatch */

#define define_array_unary_op_dynamic(name, count)                             \
  void name##f32##x##count(const float *HWY_RESTRICT a,                        \
                           float *HWY_RESTRICT result) {                       \
    return name##f32(a, result, count);                                        \
  }                                                                            \
  void name##f64x##count(const double *HWY_RESTRICT a,                         \
                         double *HWY_RESTRICT result) {                        \
    return name##f64(a, result, count);                                        \
  }

#define define_array_bin_op_dynamic(name, count)                               \
  void name##f32##x##count(const float *HWY_RESTRICT a,                        \
                           const float *HWY_RESTRICT b,                        \
                           float *HWY_RESTRICT result) {                       \
    return name##f32(a, b, result, count);                                     \
  }                                                                            \
  void name##f64x##count(const double *HWY_RESTRICT a,                         \
                         const double *HWY_RESTRICT b,                         \
                         double *HWY_RESTRICT result) {                        \
    return name##f64(a, b, result, count);                                     \
  }

#define define_array_ter_op_dynamic(name, count)                               \
  void name##f32##x##count(                                                    \
      const float *HWY_RESTRICT a, const float *HWY_RESTRICT b,                \
      const float *HWY_RESTRICT c, float *HWY_RESTRICT result) {               \
    return name##f32(a, b, c, result, count);                                  \
  }                                                                            \
  void name##f64x##count(                                                      \
      const double *HWY_RESTRICT a, const double *HWY_RESTRICT b,              \
      const double *HWY_RESTRICT c, double *HWY_RESTRICT result) {             \
    return name##f64(a, b, c, result, count);                                  \
  }

/* 64-bits */
define_array_bin_op_dynamic(add, 2);
define_array_bin_op_dynamic(sub, 2);
define_array_bin_op_dynamic(mul, 2);
define_array_bin_op_dynamic(div, 2);
define_array_unary_op_dynamic(sqrt, 2);
define_array_ter_op_dynamic(fma, 2);

/* 128-bits */
define_array_bin_op_dynamic(add, 4);
define_array_bin_op_dynamic(sub, 4);
define_array_bin_op_dynamic(mul, 4);
define_array_bin_op_dynamic(div, 4);
define_array_unary_op_dynamic(sqrt, 4);
define_array_ter_op_dynamic(fma, 4);

/* 256-bits */
define_array_bin_op_dynamic(add, 8);
define_array_bin_op_dynamic(sub, 8);
define_array_bin_op_dynamic(mul, 8);
define_array_bin_op_dynamic(div, 8);
define_array_unary_op_dynamic(sqrt, 8);
define_array_ter_op_dynamic(fma, 8);

/* 512-bits */
define_array_bin_op_dynamic(add, 16);
define_array_bin_op_dynamic(sub, 16);
define_array_bin_op_dynamic(mul, 16);
define_array_bin_op_dynamic(div, 16);
define_array_unary_op_dynamic(sqrt, 16);
define_array_ter_op_dynamic(fma, 16);

/* 1024-bits */
define_array_bin_op_dynamic(add, 32);
define_array_bin_op_dynamic(sub, 32);
define_array_bin_op_dynamic(mul, 32);
define_array_bin_op_dynamic(div, 32);
define_array_unary_op_dynamic(sqrt, 32);
define_array_ter_op_dynamic(fma, 32);

} // namespace variable

namespace fixed {

/* Fixed size exports */

namespace {

/* 32-bits */
#if HWY_MAX_BYTES >= 4
HWY_EXPORT(_addx1_f32);
HWY_EXPORT(_subx1_f32);
HWY_EXPORT(_mulx1_f32);
HWY_EXPORT(_divx1_f32);
HWY_EXPORT(_sqrtx1_f32);
HWY_EXPORT(_fmax1_f32);
#endif

/* 64-bits */
#if HWY_MAX_BYTES >= 8
HWY_EXPORT(_addx1_f64);
HWY_EXPORT(_subx1_f64);
HWY_EXPORT(_mulx1_f64);
HWY_EXPORT(_divx1_f64);
HWY_EXPORT(_sqrtx1_f64);
HWY_EXPORT(_fmax1_f64);

HWY_EXPORT(_addx2_f32);
HWY_EXPORT(_subx2_f32);
HWY_EXPORT(_mulx2_f32);
HWY_EXPORT(_divx2_f32);
HWY_EXPORT(_sqrtx2_f32);
HWY_EXPORT(_fmax2_f32);
#endif

/* 128-bits */
#if (HWY_MAX_BYTES >= 16) && (HWY_TARGET != HWY_SCALAR)
HWY_EXPORT(_addx2_f64);
HWY_EXPORT(_subx2_f64);
HWY_EXPORT(_mulx2_f64);
HWY_EXPORT(_divx2_f64);
HWY_EXPORT(_sqrtx2_f64);
HWY_EXPORT(_fmax2_f64);

HWY_EXPORT(_addx4_f32);
HWY_EXPORT(_subx4_f32);
HWY_EXPORT(_mulx4_f32);
HWY_EXPORT(_divx4_f32);
HWY_EXPORT(_sqrtx4_f32);
HWY_EXPORT(_fmax4_f32);
#endif

/* 256-bits */
#if (HWY_MAX_BYTES >= 32) && (HWY_TARGET != HWY_SCALAR)
HWY_EXPORT(_addx4_f64);
HWY_EXPORT(_subx4_f64);
HWY_EXPORT(_mulx4_f64);
HWY_EXPORT(_divx4_f64);
HWY_EXPORT(_sqrtx4_f64);
HWY_EXPORT(_fmax4_f64);

HWY_EXPORT(_addx8_f32);
HWY_EXPORT(_subx8_f32);
HWY_EXPORT(_mulx8_f32);
HWY_EXPORT(_divx8_f32);
HWY_EXPORT(_sqrtx8_f32);
HWY_EXPORT(_fmax8_f32);
#endif

/* 512-bits */
#if (HWY_MAX_BYTES >= 64) && (HWY_TARGET != HWY_SCALAR)
HWY_EXPORT(_addx8_f64);
HWY_EXPORT(_subx8_f64);
HWY_EXPORT(_mulx8_f64);
HWY_EXPORT(_divx8_f64);
HWY_EXPORT(_sqrtx8_f64);
HWY_EXPORT(_fmax8_f64);

HWY_EXPORT(_addx16_f32);
HWY_EXPORT(_subx16_f32);
HWY_EXPORT(_mulx16_f32);
HWY_EXPORT(_divx16_f32);
HWY_EXPORT(_sqrtx16_f32);
HWY_EXPORT(_fmax16_f32);
#endif

/* 1024-bits */
#if (HWY_MAX_BYTES >= 128) && (HWY_TARGET != HWY_SCALAR)
HWY_EXPORT(_addx16_f64);
HWY_EXPORT(_subx16_f64);
HWY_EXPORT(_mulx16_f64);
HWY_EXPORT(_divx16_f64);
HWY_EXPORT(_sqrtx16_f64);
HWY_EXPORT(_fmax16_f64);
#endif

} // namespace

constexpr auto sf32x2 = sizeof(float) * 2;
constexpr auto sf64x2 = sizeof(double) * 2;
constexpr auto sf32x4 = sizeof(float) * 4;
constexpr auto sf64x4 = sizeof(double) * 4;
constexpr auto sf32x8 = sizeof(float) * 8;
constexpr auto sf64x8 = sizeof(double) * 8;
constexpr auto sf32x16 = sizeof(float) * 16;
constexpr auto sf64x16 = sizeof(double) * 16;

#ifdef PRISM_IDE
typedef float f32x2_v __attribute__((vector_size(8)));
typedef double f64x2_v __attribute__((vector_size(16)));
typedef float f32x4_v __attribute__((vector_size(16)));
typedef double f64x4_v __attribute__((vector_size(32)));
typedef float f32x8_v __attribute__((vector_size(32)));
typedef double f64x8_v __attribute__((vector_size(64)));
typedef float f32x16_v __attribute__((vector_size(64)));
typedef double f64x16_v __attribute__((vector_size(128)));
#endif

union f32x2_u {
  f32x2_v vector;
  alignas(sf32x2) float array[2];
};
union f64x2_u {
  f64x2_v vector;
  alignas(sf64x2) double array[2];
};
union f32x4_u {
  f32x4_v vector;
  alignas(sf32x4) float array[4];
};

union f64x4_u {
  f64x4_v vector;
  alignas(sf64x4) double array[4];
};
union f32x8_u {
  f32x8_v vector;
  alignas(sf32x8) float array[8];
};
union f64x8_u {
  f64x8_v vector;
  alignas(sf64x8) double array[8];
};
union f32x16_u {
  f32x16_v vector;
  alignas(sf32x16) float array[16];
};
union f64x16_u {
  f64x16_v vector;
  alignas(sf64x16) double array[16];
};

/* Single vector functions with static dispatch */

/* 64-bits */
#define define_static_unary_op(type, op, size)                                 \
  type##x##size##_v op##type##x##size(const type##x##size##_v a) {             \
    type##x##size##_u a_union = {.vector = a};                                 \
    type##x##size##_u result_union;                                            \
    HWY_STATIC_DISPATCH(_##op##x##size##_##type)                               \
    (a_union.array, result_union.array);                                       \
    return result_union.vector;                                                \
  }

#define define_static_binary_op(type, op, size)                                \
  type##x##size##_v op##type##x##size(const type##x##size##_v a,               \
                                      const type##x##size##_v b) {             \
    type##x##size##_u a_union = {.vector = a};                                 \
    type##x##size##_u b_union = {.vector = b};                                 \
    type##x##size##_u result_union;                                            \
    HWY_STATIC_DISPATCH(_##op##x##size##_##type)                               \
    (a_union.array, b_union.array, result_union.array);                        \
    return result_union.vector;                                                \
  }

#define define_static_ternary_op(type, op, size)                               \
  type##x##size##_v op##type##x##size(const type##x##size##_v a,               \
                                      const type##x##size##_v b,               \
                                      const type##x##size##_v c) {             \
    type##x##size##_u a_union = {.vector = a};                                 \
    type##x##size##_u b_union = {.vector = b};                                 \
    type##x##size##_u c_union = {.vector = c};                                 \
    type##x##size##_u result_union;                                            \
    HWY_STATIC_DISPATCH(_##op##x##size##_##type)                               \
    (a_union.array, b_union.array, c_union.array, result_union.array);         \
    return result_union.vector;                                                \
  }

#if HWY_MAX_BYTES >= 8

/* binary32 */

define_static_binary_op(f32, add, 2);
define_static_binary_op(f32, sub, 2);
define_static_binary_op(f32, mul, 2);
define_static_binary_op(f32, div, 2);
define_static_unary_op(f32, sqrt, 2);
define_static_ternary_op(f32, fma, 2);

#endif

/* 128-bits */

#if HWY_MAX_BYTES >= 16

/* binary64 */

define_static_binary_op(f64, add, 2);
define_static_binary_op(f64, sub, 2);
define_static_binary_op(f64, mul, 2);
define_static_binary_op(f64, div, 2);
define_static_unary_op(f64, sqrt, 2);
define_static_ternary_op(f64, fma, 2);

/* binary32 */

define_static_binary_op(f32, add, 4);
define_static_binary_op(f32, sub, 4);
define_static_binary_op(f32, mul, 4);
define_static_binary_op(f32, div, 4);
define_static_unary_op(f32, sqrt, 4);
define_static_ternary_op(f32, fma, 4);

#endif

/* 256-bits */

#if HWY_MAX_BYTES >= 32

/* binary64 */

define_static_binary_op(f64, add, 4);
define_static_binary_op(f64, sub, 4);
define_static_binary_op(f64, mul, 4);
define_static_binary_op(f64, div, 4);
define_static_unary_op(f64, sqrt, 4);
define_static_ternary_op(f64, fma, 4);

/* binary32 */

define_static_binary_op(f32, add, 8);
define_static_binary_op(f32, sub, 8);
define_static_binary_op(f32, mul, 8);
define_static_binary_op(f32, div, 8);
define_static_unary_op(f32, sqrt, 8);
define_static_ternary_op(f32, fma, 8);

#endif

/* 512-bits */

#if HWY_MAX_BYTES >= 64

/* binary64 */

define_static_binary_op(f64, add, 8);
define_static_binary_op(f64, sub, 8);
define_static_binary_op(f64, mul, 8);
define_static_binary_op(f64, div, 8);
define_static_unary_op(f64, sqrt, 8);
define_static_ternary_op(f64, fma, 8);

/* binary32 */

define_static_binary_op(f32, add, 16);
define_static_binary_op(f32, sub, 16);
define_static_binary_op(f32, mul, 16);
define_static_binary_op(f32, div, 16);
define_static_unary_op(f32, sqrt, 16);
define_static_ternary_op(f32, fma, 16);

#endif // 512-bits

/* 1024-bits */
#if HWY_MAX_BYTES >= 128

/* binary64 */

define_static_binary_op(f64, add, 16);
define_static_binary_op(f64, sub, 16);
define_static_binary_op(f64, mul, 16);
define_static_binary_op(f64, div, 16);
define_static_unary_op(f64, sqrt, 16);
define_static_ternary_op(f64, fma, 16);

define_static_binary_op(f32, add, 32);
define_static_binary_op(f32, sub, 32);
define_static_binary_op(f32, mul, 32);
define_static_binary_op(f32, div, 32);
define_static_unary_op(f32, sqrt, 32);
define_static_ternary_op(f32, fma, 32);

#endif // 1024-bits

} // namespace fixed

} // namespace PRISM_PR_MODE_NAMESPACE::PRISM_DISPATCH
// NOLINTEND

#endif // HWY_ONCE
