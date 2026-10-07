#ifndef __PRISM_TESTS_HELPER_ROBUST_STATISTICAL_TEST_H__
#define __PRISM_TESTS_HELPER_ROBUST_STATISTICAL_TEST_H__

#include "tests/helper/binomial_test.h"
#include <boost/math/distributions/normal.hpp> // for boost::math::quantile()
#include <cmath>
#include <string>
#include <vector>

namespace prism::tests::helper {

struct RobustTestConfig {
  // Primary statistical parameters
  double base_alpha = 0.01;     // Less strict than 0.000001
  int base_repetitions = 10000; // Base sample size

  // Robustness parameters
  int max_retries = 3;                  // Number of retry attempts
  double retry_alpha_multiplier = 2.0;  // Relax alpha on retries
  double retry_sample_multiplier = 1.5; // Increase sample size on retries

  // Multiple testing correction
  bool use_bonferroni = true;   // Apply Bonferroni correction
  int num_tests_estimate = 100; // Estimated number of tests for correction

  // Adaptive sample size
  bool use_adaptive_sampling = true;
  double min_effect_size = 0.05; // Minimum effect size to detect
  double power = 0.8;            // Statistical power
};

// Robust binomial test with retries and adaptive parameters
class RobustBinomialTest {
private:
  RobustTestConfig config_;

  // Calculate required sample size for given effect size and power
  [[nodiscard]] auto calculate_adaptive_sample_size(double effect_size,
                                                    double alpha,
                                                    double power) const -> int {
    // Using normal approximation for sample size calculation
    // n = (z_alpha/2 + z_beta)^2 * p(1-p) / effect_size^2
    // For simplicity, using conservative estimate
    auto inverse_normal_cdf = [](double p) {
      static const boost::math::normal_distribution<double> dist(0.0, 1.0);
      return boost::math::quantile(dist, p);
    };

    double z_alpha = inverse_normal_cdf(1 - alpha / 2); // two-tailed
    double z_beta = inverse_normal_cdf(power);
    double p = 0.5; // Conservative assumption

    double n =
        std::pow(z_alpha + z_beta, 2) * p * (1 - p) / std::pow(effect_size, 2);
    return std::max(config_.base_repetitions, static_cast<int>(n));
  }

public:
  explicit RobustBinomialTest(const RobustTestConfig &config = {})
      : config_(config) {}

  struct TestResult {
    bool passed = false;
    int attempts_made = 0;
    double final_pvalue = 0.0;
    double final_alpha = 0.0;
    int final_sample_size = 0;
    std::vector<double> pvalues_history;
    std::string failure_reason;
  };

  // Robust test with retries. `successes` out of `trials` is the first sample.
  // A retry draws a fresh, independent sample with `resample(n)`, which runs n
  // new trials and returns the number of successes: reusing the first sample
  // with a larger trial count would only bias the estimated proportion.
  template <typename Resample>
  auto test(int successes, int trials, double expected_probability,
            Resample &&resample) -> TestResult {
    TestResult result;

    // Apply Bonferroni correction if enabled
    double corrected_alpha = config_.base_alpha;
    if (config_.use_bonferroni) {
      corrected_alpha = config_.base_alpha / config_.num_tests_estimate;
    }

    // Adaptive sample size if enabled
    int adaptive_trials = trials;
    if (config_.use_adaptive_sampling) {
      adaptive_trials = calculate_adaptive_sample_size(
          config_.min_effect_size, corrected_alpha, config_.power);

      // If we don't have enough samples, adjust the test
      if (trials < adaptive_trials) {
        result.failure_reason = "Insufficient sample size for robust testing";
        // Don't fail immediately - proceed with available samples but note the
        // limitation
      }
    }

    // Main testing loop with retries
    for (int attempt = 1; attempt <= config_.max_retries; ++attempt) {
      result.attempts_made = attempt;

      // Calculate test parameters for this attempt
      double current_alpha =
          corrected_alpha *
          std::pow(config_.retry_alpha_multiplier, attempt - 1);

      // Calculate increased sample size for retries
      int current_trials = static_cast<int>(
          trials * std::pow(config_.retry_sample_multiplier, attempt - 1));
      int current_successes =
          attempt == 1 ? successes : resample(current_trials);

      // Perform binomial test
      auto binomial_result = binomial_test(current_trials, current_successes,
                                           expected_probability);
      result.pvalues_history.push_back(binomial_result.pvalue);
      result.final_pvalue = binomial_result.pvalue;
      result.final_alpha = current_alpha;
      result.final_sample_size = current_trials;

      // Check if test passes
      if (binomial_result.pvalue >= current_alpha) {
        result.passed = true;
        return result;
      }

      // If this is not the last attempt, prepare for retry
      if (attempt < config_.max_retries) {
        // Sample size is increased for next attempt using
        // retry_sample_multiplier
        continue;
      }
    }

    // All attempts failed
    result.passed = false;
    result.failure_reason = "Statistical test failed after " +
                            std::to_string(config_.max_retries) + " attempts";
    return result;
  }

  // Sequential testing approach - stop early if we have strong evidence
  TestResult sequential_test(const std::vector<bool> &observations,
                             double expected_probability) {
    TestResult result;

    double corrected_alpha = config_.base_alpha;
    if (config_.use_bonferroni) {
      corrected_alpha = config_.base_alpha / config_.num_tests_estimate;
    }

    int successes = 0;
    for (size_t i = 0; i < observations.size(); ++i) {
      if (observations[i])
        successes++;

      // Only test after minimum sample size
      if (i >= 100 &&
          (i + 1) % 1000 == 0) { // Test every 1000 observations after 100
        auto binomial_result =
            binomial_test(i + 1, successes, expected_probability);
        result.pvalues_history.push_back(binomial_result.pvalue);

        // Strong evidence for null hypothesis (early acceptance)
        if (binomial_result.pvalue > 0.1) {
          result.passed = true;
          result.final_pvalue = binomial_result.pvalue;
          result.final_alpha = corrected_alpha;
          result.final_sample_size = i + 1;
          result.attempts_made = 1;
          return result;
        }

        // Strong evidence against null hypothesis with sufficient confidence
        if (binomial_result.pvalue < corrected_alpha / 10) {
          result.passed = false;
          result.final_pvalue = binomial_result.pvalue;
          result.final_alpha = corrected_alpha;
          result.final_sample_size = i + 1;
          result.attempts_made = 1;
          result.failure_reason = "Strong evidence of statistical deviation";
          return result;
        }
      }
    }

    // Final test with all data
    auto binomial_result =
        binomial_test(observations.size(), successes, expected_probability);
    result.final_pvalue = binomial_result.pvalue;
    result.final_alpha = corrected_alpha;
    result.final_sample_size = observations.size();
    result.attempts_made = 1;
    result.passed = binomial_result.pvalue >= corrected_alpha;

    if (!result.passed) {
      result.failure_reason = "Final statistical test rejection";
    }

    return result;
  }
};

// Utility function for environment-based configuration
inline RobustTestConfig get_robust_test_config() {
  RobustTestConfig config;

  // Allow environment variable overrides
  if (const char *env_alpha = getenv("PRISM_TEST_ALPHA")) {
    config.base_alpha = std::stod(env_alpha);
  }

  if (const char *env_retries = getenv("PRISM_TEST_MAX_RETRIES")) {
    config.max_retries = std::stoi(env_retries);
  }

  if (const char *env_bonferroni = getenv("PRISM_TEST_DISABLE_BONFERRONI")) {
    config.use_bonferroni = (std::string(env_bonferroni) != "1");
  }

  return config;
}

} // namespace prism::tests::helper

#endif // __PRISM_TESTS_HELPER_ROBUST_STATISTICAL_TEST_H__