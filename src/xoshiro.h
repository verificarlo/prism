#ifndef __PRISM_XOSHIRO_H__
#define __PRISM_XOSHIRO_H__

#include <random>

// inline with external linkage: static locals are shared across all TUs.
inline auto seed_state(bool set = false, uint64_t new_seed = 0) -> uint64_t {
  static bool initialized = false;
  static uint64_t seed = 0;
  if (set) {
    seed = new_seed;
    initialized = true;
  } else if (not initialized) {
    const char *seed_str = getenv("PRISM_SEED");
    if (seed_str != nullptr) {
      char *endptr = nullptr;
      seed = strtoll(seed_str, &endptr, 10);
      if (*endptr != '\0') {
        seed = 0;
      }
    } else {
      std::random_device rd;
      seed = rd();
    }
    initialized = true;
  }
  return seed;
}

__attribute__((unused)) inline auto get_user_seed() -> uint64_t {
  return seed_state();
}

__attribute__((unused)) inline auto set_user_seed(uint64_t seed) -> void {
  seed_state(true, seed);
}

#endif // __PRISM_XOSHIRO_H__

#if defined(PRISM_XOSHIRO_H_) == defined(HWY_TARGET_TOGGLE)
#ifdef PRISM_XOSHIRO_H_
#undef PRISM_XOSHIRO_H_
#else
#define PRISM_XOSHIRO_H_
#endif

#include "src/random-inl.h"

HWY_BEFORE_NAMESPACE(); // at file scope
namespace prism::scalar::xoshiro::HWY_NAMESPACE {

namespace internal {
namespace hn = hwy::HWY_NAMESPACE;
constexpr size_t kCacheSize = 1024 * 8ULL;
using RNG = hn::CachedXoshiro<kCacheSize>;
auto get_rng() -> RNG *;
void init_rng(std::uint64_t seed, std::uint64_t tid);
} // namespace internal

auto uniform(float) -> float;
auto uniform(double) -> double;
auto random() -> std::uint64_t;
auto randombit(std::uint32_t) -> std::uint32_t;
auto randombit(std::uint64_t) -> std::uint64_t;

} // namespace prism::scalar::xoshiro::HWY_NAMESPACE

namespace prism::vector::xoshiro::HWY_NAMESPACE {

namespace internal {
namespace hn = hwy::HWY_NAMESPACE;
using RNG = hn::VectorXoshiro;
using VU32 = hn::Vec<hn::ScalableTag<std::uint32_t>>;
using VU64 = hn::Vec<hn::ScalableTag<std::uint64_t>>;
using VF32 = hn::Vec<hn::ScalableTag<float>>;
using VF64 = hn::Vec<hn::ScalableTag<double>>;
auto get_rng() -> RNG *;
void init_rng(std::uint64_t seed, std::uint64_t tid);
} // namespace internal

auto uniform(float) -> internal::VF32;
auto uniform(double) -> internal::VF64;
auto random(std::uint32_t) -> internal::VU32;
auto random(std::uint64_t) -> internal::VU64;
auto randombit(std::uint32_t) -> internal::VU32;
auto randombit(std::uint64_t) -> internal::VU64;

} // namespace prism::vector::xoshiro::HWY_NAMESPACE
HWY_AFTER_NAMESPACE(); // at file scope

#endif // PRISM_XOSHIRO_H_
