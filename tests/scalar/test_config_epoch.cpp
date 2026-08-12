// A process-wide precision or rounding-mode change must reach threads that are
// already running instrumented arithmetic.
//
// Both settings live in thread-local storage, seeded from process-wide
// defaults on first use. Before the config epoch existed, a setter wrote only
// the default, so it was a silent no-op for every thread that had already
// rounded once -- while the getter kept reporting the new value. That is the
// shape of the bug these tests pin down: a worker keeps rounding at the old
// precision and the caller has no way to tell.
//
// An ATen OpenMP team is modelled here by long-lived std::thread workers, which
// is the property that matters: the thread executed arithmetic before the
// setter ran, and is still alive after it.

#include <future>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "src/prism_api.h"
#include "src/sr_scalar.h"
#include "src/utils.h"

namespace srd = prism::sr::scalar::dynamic_dispatch;

namespace {

// Rounding at t=8 is visibly coarser than at hardware precision: the exact sum
// is representable in binary32 but not on the t=8 grid, and RN pulls it back to
// 1.0. A thread stuck at the old precision returns the exact sum instead.
constexpr float kA = 1.0F;
constexpr float kB = 0x1.8p-10F;
constexpr float kExactSum = 0x1.006p0F;  // 1.0 + 1.5 * 2^-10, exact at t=24
constexpr float kRoundedAt8 = 1.0F;      // nearest multiple of 2^-7

class ConfigEpochTest : public ::testing::Test {
protected:
  void SetUp() override {
    interflop_prism_set_rounding_mode(INTERFLOP_PRISM_RN);
    interflop_prism_set_default_virtual_precision_binary32(24);
    interflop_prism_set_default_virtual_precision_binary64(53);
  }

  void TearDown() override {
    interflop_prism_set_rounding_mode(INTERFLOP_PRISM_SR);
    interflop_prism_set_default_virtual_precision_binary32(24);
    interflop_prism_set_default_virtual_precision_binary64(53);
  }
};

TEST_F(ConfigEpochTest, SetReachesAThreadThatAlreadyRounded) {
  std::promise<void> warmed_up;
  std::promise<void> precision_changed;
  auto warmed_up_signal = warmed_up.get_future();
  auto change_signal = precision_changed.get_future();

  float before = 0.0F;
  float after = 0.0F;
  int32_t reported_after = 0;

  std::thread worker([&] {
    before = srd::addf32(kA, kB);
    warmed_up.set_value();

    change_signal.wait();
    after = srd::addf32(kA, kB);
    reported_after = interflop_prism_get_virtual_precision_binary32();
  });

  warmed_up_signal.wait();
  interflop_prism_set_default_virtual_precision_binary32(8);
  precision_changed.set_value();
  worker.join();

  EXPECT_EQ(before, kExactSum);
  EXPECT_EQ(after, kRoundedAt8);
  EXPECT_EQ(reported_after, 8);
}

TEST_F(ConfigEpochTest, SetReachesEveryWorkerInAPool) {
  constexpr size_t kWorkers = 8;

  std::promise<void> precision_changed;
  auto change_signal = precision_changed.get_future().share();
  std::vector<std::promise<void>> warmed_up(kWorkers);
  std::vector<std::future<void>> warmed_up_signals;
  warmed_up_signals.reserve(kWorkers);
  for (auto &p : warmed_up) {
    warmed_up_signals.push_back(p.get_future());
  }

  std::vector<float> results(kWorkers, 0.0F);
  std::vector<std::thread> workers;
  workers.reserve(kWorkers);

  for (size_t i = 0; i < kWorkers; i++) {
    workers.emplace_back([&, i] {
      // Round once so the thread caches the current configuration.
      (void)srd::addf32(kA, kB);
      warmed_up[i].set_value();

      change_signal.wait();
      results[i] = srd::addf32(kA, kB);
    });
  }

  for (auto &signal : warmed_up_signals) {
    signal.wait();
  }
  interflop_prism_set_default_virtual_precision_binary32(8);
  precision_changed.set_value();
  for (auto &worker : workers) {
    worker.join();
  }

  for (size_t i = 0; i < kWorkers; i++) {
    EXPECT_EQ(results[i], kRoundedAt8) << "worker " << i << " kept rounding at "
                                       << "the precision it cached";
  }
}

TEST_F(ConfigEpochTest, RoundingModeReachesARunningThread) {
  interflop_prism_set_default_virtual_precision_binary32(8);
  // The worker starts in SR and must end in RN, so a thread that kept the mode
  // it cached at startup fails rather than passing by coincidence.
  interflop_prism_set_rounding_mode(INTERFLOP_PRISM_SR);

  std::promise<void> warmed_up;
  std::promise<void> mode_changed;
  auto warmed_up_signal = warmed_up.get_future();
  auto change_signal = mode_changed.get_future();

  int32_t reported = INTERFLOP_PRISM_RN;
  bool deterministic = false;

  std::thread worker([&] {
    (void)srd::addf32(kA, kB);
    warmed_up.set_value();

    change_signal.wait();
    reported = interflop_prism_get_rounding_mode();

    // Under RN the threshold is fixed at one half, so repeating the same
    // inexact operation is bit-reproducible. Under SR it would not be.
    const float first = srd::addf32(kA, kB);
    deterministic = true;
    for (int i = 0; i < 64; i++) {
      deterministic = deterministic && (srd::addf32(kA, kB) == first);
    }
  });

  warmed_up_signal.wait();
  interflop_prism_set_rounding_mode(INTERFLOP_PRISM_RN);
  mode_changed.set_value();
  worker.join();

  EXPECT_EQ(reported, INTERFLOP_PRISM_RN);
  EXPECT_TRUE(deterministic);
}

TEST_F(ConfigEpochTest, ThreadOverrideHoldsUntilTheNextProcessWideSet) {
  std::promise<void> override_set;
  std::promise<void> default_changed;
  auto override_signal = override_set.get_future();
  auto change_signal = default_changed.get_future();

  int32_t under_override = 0;
  int32_t after_global_set = 0;

  std::thread worker([&] {
    interflop_prism_set_thread_virtual_precision_binary32(12);
    under_override = interflop_prism_get_virtual_precision_binary32();
    override_set.set_value();

    change_signal.wait();
    after_global_set = interflop_prism_get_virtual_precision_binary32();
  });

  override_signal.wait();
  // The main thread keeps the default while the worker runs at 12.
  EXPECT_EQ(interflop_prism_get_virtual_precision_binary32(), 24);
  interflop_prism_set_default_virtual_precision_binary32(8);
  default_changed.set_value();
  worker.join();

  EXPECT_EQ(under_override, 12);
  EXPECT_EQ(after_global_set, 8);
}

TEST_F(ConfigEpochTest, DefaultGetterReportsTheDefaultNotTheThreadValue) {
  interflop_prism_set_thread_virtual_precision_binary32(10);

  EXPECT_EQ(interflop_prism_get_virtual_precision_binary32(), 10);
  EXPECT_EQ(interflop_prism_get_default_virtual_precision_binary32(), 24);

  interflop_prism_set_default_virtual_precision_binary32(16);

  EXPECT_EQ(interflop_prism_get_virtual_precision_binary32(), 16);
  EXPECT_EQ(interflop_prism_get_default_virtual_precision_binary32(), 16);
}

} // namespace
