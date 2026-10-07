
#if defined(PRISM_TEST_HELPER_ASSERT_TOGGLE) == defined(HWY_TARGET_TOGGLE)
#ifdef PRISM_TEST_HELPER_ASSERT_TOGGLE
#undef PRISM_TEST_HELPER_ASSERT_TOGGLE
#else
#define PRISM_TEST_HELPER_ASSERT_TOGGLE
#endif

#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>

#include "src/utils.h"
#include "tests/helper/binomial_test.h"
#include "tests/helper/robust_statistical_test.h"
#include "tests/helper/common.h"
#include "tests/helper/counter.h"
#include "tests/helper/distance.h"
#include "tests/helper/operator.h"
#include "tests/helper/pprint.h"
#include "tests/helper/random.h"

#include "hwy/highway.h"

#include "tests/helper/operator-inl.h"

HWY_BEFORE_NAMESPACE(); // at file scope

namespace prism::tests::helper::HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

inline void hwy_assert_fail() {
  HWY_ASSERT(false); // NOLINT
}

template <typename Op> constexpr void hwy_assert_may_fail() {
#if HWY_NATIVE_FMA
  HWY_ASSERT(false);
#else
  // div might failed when fma is emulated
  HWY_ASSERT(Op::name != "div"); // NOLINT
#endif
}

template <typename T, typename H = typename prism::utils::IEEE754<T>::H>
void assert_errors_eq_ulp(const DistanceError<H> &result) {
  if (result.is_exact) {
    return;
  }
  if (((result.error + result.error_c) != result.ulp)) {
    std::cerr << "error + error_c != ulp" << "\n"
              << result.msg << "\n"
              << "error:   " << hexfloat(result.error) << "\n"
              << "error_c: " << hexfloat(result.error_c) << "\n"
              << "prev:    " << hexfloat(result.prev) << "\n"
              << "next:    " << hexfloat(result.next) << "\n"
              << "ulp:     " << hexfloat(result.ulp) << std::endl;
    hwy_assert_fail();
  }
}

template <typename T, typename H = typename IEEE754<T>::H>
void assert_proba_eq_one(const DistanceError<H> &result, const H reference) {

  if (result.probability_down + result.probability_up != 1) {
    std::cerr << "probability_down + probability_up != 1" << "\n"
              << result.msg << "\n"
              << "probability_down: " << result.probability_down << "\n"
              << "probability_up:   " << result.probability_up << "\n"
              << "reference:        " << hexfloat(reference) << "\n"
              << "prev:             " << hexfloat(result.prev) << "\n"
              << "next:             " << hexfloat(result.next) << "\n"
              << "error:            " << hexfloat(result.error) << "\n"
              << "error_c:          " << hexfloat(result.error_c) << "\n"
              << "ulp:              " << hexfloat(result.ulp) << std::endl;
    hwy_assert_fail();
  }
}

template <typename T, typename H = typename IEEE754<T>::H>
void print_error_assert_is_proba(const DistanceError<H> &result,
                                 Counter<T> &counter,
                                 const bool is_proba_down) {

  const auto count_down = counter.down_count();
  const auto count_up = counter.up_count();
  const auto repetitions = counter.count();
  const auto probability_down_estimated =
      count_down / static_cast<double>(repetitions);
  const auto probability_up_estimated =
      count_up / static_cast<double>(repetitions);

  if (is_proba_down) {
    std::cerr << "Probability ↓ is not in [0, 1] range\n";
  } else {
    std::cerr << "Probability ↑ is not in [0, 1] range\n";
  }

  std::cerr << "-- theoretical -\n"
            << "   probability ↓: " << fmt_proba(result.probability_down)
            << "\n"
            << "   probability ↑: " << fmt_proba(result.probability_up) << "\n"
            << "--- estimated --\n"
            << "     sample size: " << repetitions << "\n"
            << "              #↓: " << count_down << " ("
            << fmt_proba(probability_down_estimated) << ")\n"
            << "              #↑: " << count_up << " ("
            << fmt_proba(probability_up_estimated) << ")\n"
            << std::hexfloat << "" << "              ↓: " << counter.down()
            << "\n"
            << "              ↑: " << counter.up() << "\n"
            << result.msg << std::defaultfloat << flush();
  hwy_assert_fail();
}

template <typename T, typename H = typename IEEE754<T>::H>
void assert_is_proba(const DistanceError<H> &result, Counter<T> &counter) {
  const auto is_proba_down =
      0.0 <= result.probability_down and result.probability_down <= 1.0;
  const auto is_proba_up =
      0.0 <= result.probability_up and result.probability_up <= 1.0;

  if (not is_proba_down) {
    print_error_assert_is_proba<T>(result, counter, true);
  }
  if (not is_proba_up) {
    print_error_assert_is_proba<T>(result, counter, false);
  }
}

template <class D, class V = hn::VFromD<D>, typename T = hn::TFromD<D>,
          typename... Args>
void assert_equal_inputs(D d, Args... args) {
  static_assert((std::is_same_v<V, Args> && ...),
                "All arguments must be highwy vectors");
  int i = 1;
  // Returns true to stop checking. A fold is used instead of iterating over
  // {args...}: sizeless vector types (SVE, RVV) cannot be array elements.
  const auto check = [&](const V &a) -> bool {
    const auto a_min = reduce_min(d, a);
    const auto a_max = reduce_max(d, a);
    if (isnan(a_min) and isnan(a_max)) {
      return true;
    }
    if (a_min != a_max) {
      std::cerr << "Vector does not have equal inputs!\n"
                << "Index: " << i << "\n"
                << "min(a): " << hexfloat(a_min) << "\n"
                << "max(a): " << hexfloat(a_max) << std::endl;
      hwy_assert_fail();
    }
    i++;
    return false;
  };
  (check(args) || ...);
}

template <typename T, typename H = typename prism::utils::IEEE754<T>::H>
void assert_counter_infnan_values(const DistanceError<H> &distance_error,
                                  Counter<T> &counter, const H reference) {

  const auto is_finite_up = isfinite(counter.up());
  const auto is_finite_down = isfinite(counter.down());
  const auto is_finite_reference = isfinite(reference);

  if (is_finite_reference and is_finite_down and is_finite_up) {
    return;
  }

  const auto is_inf_reference = isinf(reference);
  const auto is_nan_reference = isnan(reference);

  bool match = false;

  // check if at least one of the values is nan
  // check if at least one of the values is inf and sign is correct
  const auto is_nan_up = isnan(counter.up());

  match |= is_nan_reference and is_nan_up;
  match |= is_inf_reference and (reference == counter.up());
  match |= (counter.up() == static_cast<T>(distance_error.next));
  match |= (counter.up() == static_cast<T>(distance_error.prev));

  if (not match) {
    std::cerr << "Error in nan/inf comparison\n"
              << "counter.up() != reference\n"
              << "counter.up(): " << counter.up() << "\n"
              << "reference: " << reference << "\n"
              << distance_error.msg << std::defaultfloat << flush();
    hwy_assert_fail();
  }

  const auto is_nan_down = isnan(counter.down());

  match = is_nan_reference and is_nan_down;
  match |= is_inf_reference and (reference == counter.down());
  match |= (counter.down() == static_cast<T>(distance_error.prev));
  match |= (counter.down() == static_cast<T>(distance_error.next));

  if (not match) {
    std::cerr << "Error in nan/inf comparison\n"
              << "counter.down() != reference\n"
              << "counter.down(): " << counter.down() << "\n"
              << "reference: " << reference << "\n"
              << distance_error.msg << std::defaultfloat << flush();
    hwy_assert_fail();
  }
}

template <typename Op, typename T, typename H = typename prism::utils::IEEE754<T>::H>
void print_assert_error(const DistanceError<H> &distance_error,
                        Counter<T> &counter, Args<T> args, const double alpha,
                        const int lane, const int lanes,
                        const std::string &header) {
  const auto precision = 13;
  const auto op_name = Op::name;
  const auto *const ftype = typeid(T).name();
  const auto args_str = get_args_str<T, Op>(args, distance_error.reference);
  const int32_t virtual_precision = prism::sr::get_virtual_precision<T>();

  const auto pdown = fmt_proba(distance_error.probability_down);
  const auto pup = fmt_proba(distance_error.probability_up);
  const auto repetitions = counter.count();
  const auto count_down = counter.down_count();
  const auto count_up = counter.up_count();
  const auto probability_down_estimated =
      count_down / static_cast<double>(repetitions);
  const auto probability_up_estimated =
      count_up / static_cast<double>(repetitions);
  const auto pestdown = fmt_proba(probability_down_estimated);
  const auto pestup = fmt_proba(probability_up_estimated);
  const auto hexup = hexfloat(counter.up());
  const auto hexdown = hexfloat(counter.down());

  std::cerr << header << "\n"
            << "     Lane/#Lanes: " << lane + 1 << "/" << lanes << "\n"
            << "            type: " << ftype << "\n"
            << "              op: " << op_name << "\n"
            << "    v. precision: " << virtual_precision << "\n"
            << "           alpha: " << alpha << "\n"
            << std::hexfloat << std::setprecision(precision) << args_str
            << std::defaultfloat << "" << "-- theoretical -\n"
            << "   probability ↓: " << pdown << "\n"
            << "   probability ↑: " << pup << "\n"
            << "--- estimated --\n"
            << "     sample size: " << repetitions << "\n"
            << "              #↓: " << count_down << " (" << pestdown << ")\n"
            << "              #↑: " << count_up << " (" << pestup << ")\n"
            << std::hexfloat << "" << "              ↓: " << hexdown << "\n"
            << "              ↑: " << hexup << "\n"
            << distance_error.msg << "\n"
            << std::defaultfloat << flush();
}

template <typename Op, typename T, typename H = typename prism::utils::IEEE754<T>::H>
void assert_unique_value_eq_reference(const DistanceError<H> &distance_error,
                                      Counter<T> &counter, Args<T> args,
                                      const double alpha) {

  if (not distance_error.is_exact) {
    return;
  }

  if (distance_error.error == 0) {
    return;
  }
  if (distance_error.prev == counter.down() or
      distance_error.prev == counter.up()) {
    return;
  }
  if (distance_error.next == counter.down() or
      distance_error.next == counter.up()) {
    return;
  }
  if (distance_error.error <= prism::utils::IEEE754<T>::min_subnormal) {
    return;
  }

  print_assert_error<Op>(distance_error, counter, args, alpha, 1, 1,
                         "Unique value is not equal to reference");
  hwy_assert_may_fail<Op>();
}

template <typename Op, typename T, typename H = typename prism::utils::IEEE754<T>::H>
void assert_value_eq_reference(const DistanceError<H> &distance_error,
                               Counter<T> &counter, Args<T> args,
                               const double alpha, const int lane,
                               const int lanes) {

  if (distance_error.is_exact) {
    return;
  }

  const auto frequency = 1.0 / counter.count();

  if (distance_error.probability_down < frequency or
      distance_error.probability_up < frequency) {
    return;
  }

  // Handle cases where there are only two possible values
  if (counter.size() == 2) {
    if (counter.down() != static_cast<T>(distance_error.prev)) {
       print_assert_error<Op>(distance_error, counter, args, alpha, lane, lanes,
                              "Value ↓ is not equal to reference");
       hwy_assert_may_fail<Op>();
    }
    if (counter.up() != static_cast<T>(distance_error.next)) {
       print_assert_error<Op>(distance_error, counter, args, alpha, lane, lanes,
                              "Value ↑ is not equal to reference");
       hwy_assert_may_fail<Op>();
    }
  } else {
    for (auto const& [val, freq] : counter.data()) {
      if (val != static_cast<T>(distance_error.prev) && val != static_cast<T>(distance_error.next)) {
        print_assert_error<Op>(distance_error, counter, args, alpha, lane, lanes,
                               "Value " + hexfloat(val) + " is not equal to reference bounds [" +
                               hexfloat(static_cast<T>(distance_error.prev)) + ", " +
                               hexfloat(static_cast<T>(distance_error.next)) + "]");
        hwy_assert_may_fail<Op>();
      }
    }
  }
}

// `resample(n)` draws n new samples for this lane and returns the number of
// results rounded down; it is used by the robust test to retry.
template <class M, typename Op, typename T, typename Resample,
          typename H = typename prism::utils::IEEE754<T>::H>
void assert_binomial_test(const DistanceError<H> &distance_error,
                          Counter<T> &counter, const BinomialTest &test,
                          Args<T> args, const double alpha, const int lane,
                          const int lanes, Resample &&resample) {

  const auto frequency = 1.0 / counter.count();

  if (distance_error.probability_down < frequency or
      distance_error.probability_up < frequency) {
    return;
  }

  // Use robust statistical testing to reduce false positives
  static auto robust_config = prism::tests::helper::get_robust_test_config();
  robust_config.base_alpha = alpha;
  robust_config.num_tests_estimate = lanes; // Use actual number of lanes for Bonferroni correction
  if constexpr (not M::is_sr) {
    // Rejections are not asserted in UD mode: retrying would only cost time.
    robust_config.max_retries = 1;
  }

  prism::tests::helper::RobustBinomialTest robust_test(robust_config);
  auto pdown = static_cast<double>(distance_error.probability_down);
  auto result = robust_test.test(counter.down_count(), counter.count(), pdown,
                                 resample);

  if (!result.passed) {
    // Log robust test details for debugging
    std::cerr << "Robust test details:\n"
              << "  Attempts made: " << result.attempts_made << "\n"
              << "  Final p-value: " << result.final_pvalue << "\n"
              << "  Final alpha: " << result.final_alpha << "\n"
              << "  Sample size: " << result.final_sample_size << "\n"
              << "  Failure reason: " << result.failure_reason << "\n"
              << "  p-values:";
    for (const auto pvalue : result.pvalues_history) {
      std::cerr << " " << pvalue;
    }
    std::cerr << "\n  Seeds: PRISM_SEED=" << prism_seed()
              << " PRISM_TEST_SEED=" << test_seed() << "\n";

    print_assert_error<Op>(distance_error, counter, args, alpha, lane, lanes,
                           "Robust statistical test rejected null hypothesis!");
    if constexpr (M::is_sr) {
      hwy_assert_may_fail<Op>();
    }
  }
}

template <class Op, class D, class V = hn::VFromD<D>,
          typename T = hn::TFromD<D>, typename... Args>
auto eval_op(int repetitions, D d, T prev, T next, Args... args) -> std::vector<Counter<T>> {

#ifdef SR_DEBUG
  const size_t lanes = 1;
  std::cerr << "rep: " << repetitions << std::endl;
#else
  const auto lanes = hn::Lanes(d);
#endif
  Op op{};

  std::vector<Counter<T>> c;
  c.reserve(lanes);
  for (size_t i = 0; i < lanes; i++) c.emplace_back(prev, next);

  for (int i = 0; i < repetitions; i++) {
    const auto v = op(d, args...);
    for (size_t j = 0; j < lanes; j++) {
      auto vj = hn::ExtractLane(v, j);
      c[j][vj]++;
    }
  }

  return c;
}

template <class D, class V = hn::VFromD<D>, typename T = hn::TFromD<D>,
          typename... Args>
auto get_lane(D d, Args... args) -> std::array<T, sizeof...(Args)> {
  return {hn::GetLane(args)...};
}

template <class M, class Op, class D, class V = hn::VFromD<D>,
          typename T = hn::TFromD<D>, typename... Args>
void CheckDistributionResults(D d, const ConfigTest &config, Args... args) {
  using H = typename IEEE754<T>::H;

  static_assert(sizeof...(Args) == Op::arity,
                "Number of arguments must match the arity of the operation");
  static_assert(Op::arity <= 3,
                "Operation arity must be less than or equal to 3");
  static_assert((std::is_same_v<Args, V> && ...),
                "All arguments must be of type highway vector V");

  const auto scalar_args_array = get_lane(d, args...);
  helper::Args<T> scalar_args = {scalar_args_array.begin(),
                                 scalar_args_array.end()};

  // ensure that we have vector of same value
  assert_equal_inputs(d, args...);

  H reference = Op::reference(scalar_args);
  auto distance_error = compute_distance_error<Op>(scalar_args, reference);

  auto counters = eval_op<Op>(config.repetitions, d, static_cast<T>(distance_error.prev), static_cast<T>(distance_error.next), args...);
  assert_errors_eq_ulp<T>(distance_error);
  assert_proba_eq_one<T>(distance_error, reference);

  size_t lane = 0;
  for (auto &counter : counters) {

    auto count_down = counter.down_count();
    if (distance_error.is_exact) {
      return;
    }

    // Skip statistical test when one of the representatives overflows to +-inf.
    if (not isfinite(static_cast<T>(distance_error.prev)) or
        not isfinite(static_cast<T>(distance_error.next))) {
      return;
    }

    if (not isfinite(counter.up()) or not isfinite(counter.down())) {
      assert_counter_infnan_values(distance_error, counter, reference);
      return;
    }

    assert_is_proba(distance_error, counter);

    auto pdown = static_cast<double>(distance_error.probability_down);

    // Bonferroni correction, divide by the number of lanes
    const auto lanes = hn::Lanes(d);
    const auto alpha_bon = (config.alpha / 2) / lanes;

    // check values for sr rounding mode only
    if constexpr (M::is_sr) {
      // Assert value is equal to reference if exact operation
      assert_unique_value_eq_reference<Op>(distance_error, counter, scalar_args,
                                           config.alpha);

      // Assert values are equal to reference if not exact operation
      assert_value_eq_reference<Op>(distance_error, counter, scalar_args,
                                    config.alpha, lane, lanes);

      pdown = static_cast<double>(distance_error.probability_down);
    } else {
      // check that probabilities are equal to 0.5 for ud rounding mode
      pdown = 0.5;
    }

    // binomial test
    auto test = binomial_test(config.repetitions, count_down, pdown);
    const auto resample = [&](int repetitions) {
      return eval_op<Op>(repetitions, d, static_cast<T>(distance_error.prev),
                         static_cast<T>(distance_error.next), args...)[lane]
          .down_count();
    };
    assert_binomial_test<M, Op>(distance_error, counter, test, scalar_args,
                                alpha_bon, lane, lanes, resample);

    lane++;
    debug_reset();
  }
}

template <typename Args, class M, class Op, class D, class V = hn::VFromD<D>,
          typename T = hn::TFromD<D>>
void CheckDistributionResultsWrapper(D d, const ConfigTest &config,
                                     Args &&args) {
  // args holds scalars; broadcast them here (see generic test drivers).
  std::apply(
      [&](auto... scalar_args) {
        CheckDistributionResults<M, Op>(d, config,
                                        hn::Set(d, scalar_args)...);
      },
      std::forward<Args>(args));
}

}; // namespace prism::tests::helper::HWY_NAMESPACE

HWY_AFTER_NAMESPACE(); // at file scope

#endif // PRISM_TEST_HELPER_ASSERT_TOGGLE