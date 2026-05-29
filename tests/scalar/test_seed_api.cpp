#include <cstdint>
#include <vector>

#include <gtest/gtest.h>
#include "hwy/highway.h"

#include "src/prism_api.h"
#include "src/xoshiro.h"

namespace rng = prism::scalar::xoshiro::HWY_NAMESPACE;

TEST(SeedAPITest, GetSetRoundtrip) {
  constexpr uint64_t seed = 42;
  interflop_prism_set_seed(seed);
  EXPECT_EQ(interflop_prism_get_seed(), seed);
}

TEST(SeedAPITest, SetOverwritesPreviousValue) {
  interflop_prism_set_seed(1);
  EXPECT_EQ(interflop_prism_get_seed(), 1ULL);

  interflop_prism_set_seed(99);
  EXPECT_EQ(interflop_prism_get_seed(), 99ULL);
}

TEST(SeedAPITest, ZeroSeed) {
  interflop_prism_set_seed(0);
  EXPECT_EQ(interflop_prism_get_seed(), 0ULL);
}

TEST(SeedAPITest, MaxSeed) {
  interflop_prism_set_seed(UINT64_MAX);
  EXPECT_EQ(interflop_prism_get_seed(), UINT64_MAX);
}

TEST(SeedAPITest, SameSeedSameSequence) {
  constexpr uint64_t seed = 0xDEADBEEF42ULL;
  constexpr uint64_t thread_num = 0;
  constexpr int N = 1000;

  rng::internal::init_rng(seed, thread_num);
  std::vector<uint64_t> seq1(N);
  for (auto &v : seq1)
    v = rng::random();

  rng::internal::init_rng(seed, thread_num);
  std::vector<uint64_t> seq2(N);
  for (auto &v : seq2)
    v = rng::random();

  EXPECT_EQ(seq1, seq2);
}

TEST(SeedAPITest, DifferentSeedsDifferentSequences) {
  constexpr uint64_t thread_num = 0;
  constexpr int N = 1000;

  rng::internal::init_rng(1, thread_num);
  std::vector<uint64_t> seq1(N);
  for (auto &v : seq1)
    v = rng::random();

  rng::internal::init_rng(2, thread_num);
  std::vector<uint64_t> seq2(N);
  for (auto &v : seq2)
    v = rng::random();

  EXPECT_NE(seq1, seq2);
}

