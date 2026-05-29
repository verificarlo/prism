load("//:constants.bzl", "DYNAMIC_COPTS", "RANDOM_FULLBITS_COPTS", "STATIC_COPTS")
load("//tests:macros.bzl", "cc_test_gen_vector", "cc_test_lib_gen")

XOSHIRO_DEBUGS_COPTS = [
    "-DPRISM_DEBUG_XOSHIRO",
]

# Single util function tests

cc_test_gen_vector(
    name = "test_get_pow2",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_get_exponent",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_twosum",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_fma",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_dekkerprod",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_split",
    dbg = True,
)

cc_test_gen_vector(
    name = "test_splitbit",
    dbg = True,
)

# Random number generator tests

cc_test_lib_gen(
    name = "xoshiro",
    size = "medium",
    src = [
        "//src:srcs-xoshiro",
        "//tests/vector:test_xoshiro.cpp",
    ],
    copts = XOSHIRO_DEBUGS_COPTS,
    dbg = True,
)

cc_test_lib_gen(
    name = "xoshiro-api",
    size = "medium",
    src = [
        "//src:srcs-xoshiro",
        "//tests/vector:test_xoshiro_api.cpp",
    ],
    copts = XOSHIRO_DEBUGS_COPTS,
    dbg = True,
)

cc_test_lib_gen(
    name = "seed-api",
    size = "small",
    src = [
        "//src:srcs-xoshiro",
        "//src:prism_api.h",
        "//tests/vector:test_seed_api.cpp",
    ],
    mode = "dynamic",
)

# Stochastic rounding library tests

cc_test_lib_gen(
    name = "sr-accuracy",
    size = "large",
    src = [
        "//tests/vector:test_sr_accuracy.cpp",
    ],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "sr-accuracy-dbg",
    size = "medium",
    src = [
        "//tests/vector:test_sr_accuracy.cpp",
    ],
    dbg = True,
    mode = "dynamic",
)

# Up/Down rounding library tests

cc_test_lib_gen(
    name = "ud-accuracy",
    size = "medium",
    src = [":test_ud_accuracy.cpp"],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "ud-accuracy-dbg",
    size = "medium",
    src = [":test_ud_accuracy.cpp"],
    dbg = True,
    mode = "dynamic",
)

# Up/Down rounding library tests using random full bits

cc_test_lib_gen(
    name = "ud-accuracy-fullbits",
    size = "medium",
    src = [":test_ud_accuracy.cpp"],
    copts = RANDOM_FULLBITS_COPTS,
    mode = "",
    deps = ["//src:prism-dynamic-fullbits"],
)

cc_test_lib_gen(
    name = "ud-accuracy-fullbits-dbg",
    size = "medium",
    src = [":test_ud_accuracy.cpp"],
    copts = RANDOM_FULLBITS_COPTS,
    dbg = True,
    mode = "",
    deps = ["//src:prism-dynamic-fullbits-dbg"],
)

# Thread tests

cc_test_lib_gen(
    name = "thread",
    size = "medium",
    src = [":test_threads.cpp"],
    copts = DYNAMIC_COPTS,
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "thread-static",
    size = "medium",
    src = [":test_threads.cpp"],
    copts = STATIC_COPTS,
    dbg = True,
    mode = "static",
)

# Performance tests

# Stochastic rounding performance tests

cc_test_lib_gen(
    name = "sr-perf-dynamic",
    size = "medium",
    src = [":test_sr_performance.cpp"],
    copts = DYNAMIC_COPTS,
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "sr-perf-static",
    size = "medium",
    src = [":test_sr_performance.cpp"],
    copts = STATIC_COPTS,
    mode = "static",
)

# Up/Down rounding performance tests

cc_test_lib_gen(
    name = "ud-perf-dynamic",
    size = "medium",
    src = [":test_ud_performance.cpp"],
    copts = DYNAMIC_COPTS,
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "ud-perf-static",
    size = "medium",
    src = [":test_ud_performance.cpp"],
    copts = STATIC_COPTS,
    mode = "static",
)

# Single test for debugging purposes
# Not actual tests

cc_test_lib_gen(
    name = "sr-single",
    src = [":test_single_sr.cpp"],
    dbg = True,
    linkopts = ["-fsanitize=address"],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "ud-single",
    src = [":test_single_ud.cpp"],
    dbg = True,
    linkopts = ["-fsanitize=address"],
    mode = "dynamic",
)

# Test suites

test_suite(
    name = "all",
    tests = [
        ":seed-api",
        ":sr-accuracy",
        ":sr-perf-dynamic",
        ":sr-perf-static",
        ":test_dekkerprod",
        ":test_fma",
        ":test_get_exponent",
        ":test_get_pow2",
        ":test_split",
        ":test_splitbit",
        ":test_twosum",
        ":thread",
        ":thread-static",
        ":ud-accuracy",
        ":ud-accuracy-fullbits",
        ":ud-perf-dynamic",
        ":ud-perf-static",
        ":xoshiro",
        ":xoshiro-api",
    ],
)

test_suite(
    name = "perf",
    tests = [
        ":sr-perf",
        ":sr-perf-native",
        ":ud-perf",
        ":ud-perf-native",
    ],
)
