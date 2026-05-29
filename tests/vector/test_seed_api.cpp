#include <cstdint>
#include <iostream>
#include <vector>

// clang-format off
#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "tests/vector/test_seed_api.cpp"  // NOLINT
#include "hwy/foreach_target.h"  // IWYU pragma: keep
#include "hwy/highway.h"
#include "hwy/tests/test_util-inl.h"
// clang-format on

#include "src/prism_api.h"
#include "src/xoshiro.h"

HWY_BEFORE_NAMESPACE();
namespace hwy::HWY_NAMESPACE {
namespace {

namespace vrng = prism::vector::xoshiro::HWY_NAMESPACE;

constexpr std::uint64_t kN = 1 << 10;

void TestVector_SameSeedSameSequence() {
  constexpr uint64_t seed = 0xDEADBEEF42ULL;
  constexpr uint64_t thread_num = 0;

  const ScalableTag<uint64_t> d;
  const size_t lanes = Lanes(d);
  const auto buf1 = hwy::MakeUniqueAlignedArray<uint64_t>(kN);
  const auto buf2 = hwy::MakeUniqueAlignedArray<uint64_t>(kN);

  interflop_prism_set_seed(seed);
  vrng::internal::init_rng(interflop_prism_get_seed(), thread_num);
  for (size_t i = 0; i < kN; i += lanes)
    Store(vrng::random(uint64_t{}), d, buf1.get() + i);

  interflop_prism_set_seed(seed);
  vrng::internal::init_rng(interflop_prism_get_seed(), thread_num);
  for (size_t i = 0; i < kN; i += lanes)
    Store(vrng::random(uint64_t{}), d, buf2.get() + i);

  for (size_t i = 0; i < kN; i++) {
    if (buf1[i] != buf2[i]) {
      std::cerr << "SameSeedSameSequence FAILED at i=" << i << ": " << buf1[i]
                << " != " << buf2[i] << "\n";
      HWY_ASSERT(0);
    }
  }
}

void TestVector_DifferentSeedsDifferentSequences() {
  constexpr uint64_t thread_num = 0;

  const ScalableTag<uint64_t> d;
  const size_t lanes = Lanes(d);
  const auto buf1 = hwy::MakeUniqueAlignedArray<uint64_t>(kN);
  const auto buf2 = hwy::MakeUniqueAlignedArray<uint64_t>(kN);

  interflop_prism_set_seed(1);
  vrng::internal::init_rng(interflop_prism_get_seed(), thread_num);
  for (size_t i = 0; i < kN; i += lanes)
    Store(vrng::random(uint64_t{}), d, buf1.get() + i);

  interflop_prism_set_seed(2);
  vrng::internal::init_rng(interflop_prism_get_seed(), thread_num);
  for (size_t i = 0; i < kN; i += lanes)
    Store(vrng::random(uint64_t{}), d, buf2.get() + i);

  bool all_equal = true;
  for (size_t i = 0; i < kN; i++) {
    if (buf1[i] != buf2[i]) {
      all_equal = false;
      break;
    }
  }
  if (all_equal) {
    std::cerr << "DifferentSeedsDifferentSequences FAILED: sequences are "
                 "identical\n";
    HWY_ASSERT(0);
  }
}

} // namespace
} // namespace hwy::HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy::HWY_NAMESPACE {
namespace {
// NOLINTBEGIN
HWY_BEFORE_TEST(PRISMSeedAPIVectorTest);
HWY_EXPORT_AND_TEST_P(PRISMSeedAPIVectorTest,
                      TestVector_SameSeedSameSequence);
HWY_EXPORT_AND_TEST_P(PRISMSeedAPIVectorTest,
                      TestVector_DifferentSeedsDifferentSequences);
// NOLINTEND
HWY_AFTER_TEST();
} // namespace
} // namespace hwy::HWY_NAMESPACE
HWY_TEST_MAIN();
#endif // HWY_ONCE
