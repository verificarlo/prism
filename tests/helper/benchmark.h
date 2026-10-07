#ifndef __PRISM_TESTS_HELPER_BENCHMARK_H__
#define __PRISM_TESTS_HELPER_BENCHMARK_H__

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <vector>

namespace prism::tests::helper {

// Time per call of a function, robust to the timer granularity (~32 ns on
// some Arm systems, comparable to one vector operation) and to interruptions:
// calls are batched so that each timed sample lasts at least kMinSampleTime,
// and order statistics over kSamples samples are reported instead of a mean.
struct BenchmarkResult {
  double median; // seconds per call
  double p10;
  double p90;
  std::size_t batch;   // calls per sample
  std::size_t samples; // timed samples
};

template <typename F> auto benchmark(F &&f) -> BenchmarkResult {
  using clock = std::chrono::steady_clock;
  constexpr double kMinSampleTime = 50e-6;
  constexpr std::size_t kSamples = 101;
  constexpr std::size_t kMaxBatch = std::size_t{1} << 30;

  const auto time_batch = [&](std::size_t batch) {
    const auto start = clock::now();
    for (std::size_t i = 0; i < batch; i++) {
      f();
    }
    return std::chrono::duration<double>(clock::now() - start).count();
  };

  // Calibrate the batch size; this also warms up caches and the clock speed.
  std::size_t batch = 1;
  while (batch < kMaxBatch && time_batch(batch) < kMinSampleTime) {
    batch *= 2;
  }

  std::vector<double> times(kSamples);
  for (auto &t : times) {
    t = time_batch(batch) / static_cast<double>(batch);
  }
  std::sort(times.begin(), times.end());
  const auto quantile = [&](double q) {
    return times[static_cast<std::size_t>(q * (kSamples - 1))];
  };
  return {quantile(0.5), quantile(0.1), quantile(0.9), batch, kSamples};
}

// One line per measurement: [elements] ns/element, then ns/call quantiles.
inline void print_benchmark(std::size_t elements, const BenchmarkResult &r) {
  std::fprintf(stderr,
               "[%-4zu] %8.3f ns/elem  median %9.2f ns/call "
               "[p10 %9.2f, p90 %9.2f] (%zu x %zu calls)\n",
               elements, r.median * 1e9 / static_cast<double>(elements),
               r.median * 1e9, r.p10 * 1e9, r.p90 * 1e9, r.samples, r.batch);
}

} // namespace prism::tests::helper

#endif // __PRISM_TESTS_HELPER_BENCHMARK_H__
