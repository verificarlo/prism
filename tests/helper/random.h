#ifndef __PRISM_TESTS_HELPER_RANDOM_H__
#define __PRISM_TESTS_HELPER_RANDOM_H__

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <string>

namespace prism::tests::helper {

// Statistical tests are seeded deterministically so that failures are
// reproducible. Two seeds are involved:
//  - PRISM_TEST_SEED: seed of the test input generator (RNG below);
//  - PRISM_SEED: seed of the PRISM random generator (read by the library).
// Both default to fixed values and can be overridden from the environment.
// A given pair reproduces a run only with the same test binary and filter,
// since streams are consumed in execution order.

constexpr std::uint64_t kDefaultTestSeed = 20240917;
constexpr const char *kDefaultPrismSeed = "20240917";

inline auto test_seed() -> std::uint64_t {
  static const std::uint64_t seed = [] {
    const char *env = std::getenv("PRISM_TEST_SEED");
    return env != nullptr ? std::strtoull(env, nullptr, 10) : kDefaultTestSeed;
  }();
  return seed;
}

// Set PRISM_SEED before the library draws its first random number, unless
// the user already set it.
inline const bool prism_seed_initialized = [] {
  return setenv("PRISM_SEED", kDefaultPrismSeed, /*overwrite=*/0) == 0;
}();

inline auto prism_seed() -> std::string {
  const char *env = std::getenv("PRISM_SEED");
  return env != nullptr ? env : "";
}

// Each generator gets its own stream: test_seed() combined with a per-instance
// index, so that generators created together are not correlated.
inline auto next_rng_seed() -> std::seed_seq {
  static std::atomic<std::uint64_t> instance{0};
  const std::uint64_t i = instance++;
  const std::uint64_t s = test_seed();
  return std::seed_seq{static_cast<std::uint32_t>(s),
                       static_cast<std::uint32_t>(s >> 32),
                       static_cast<std::uint32_t>(i),
                       static_cast<std::uint32_t>(i >> 32)};
}

struct RNG {
private:
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;

  static auto make_gen() -> std::mt19937 {
    auto seq = next_rng_seed();
    return std::mt19937(seq);
  }

public:
  explicit RNG(double a = 0.0, double b = 1.0) : gen(make_gen()), dis(a, b){};
  auto operator()() -> double { return dis(gen); }
};

}; // namespace prism::tests::helper

#endif // __PRISM_TESTS_HELPER_RANDOM_H__
