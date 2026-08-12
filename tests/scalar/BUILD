load("//:constants.bzl", "RANDOM_FULLBITS_COPTS")
load("//tests:macros.bzl", "cc_test_binary", "cc_test_gen_scalar", "cc_test_lib_gen")

# Single util function tests

cc_test_gen_scalar(
    name = "test_get_exponent",
)

cc_test_gen_scalar(
    name = "test_get_predabs",
)

cc_test_gen_scalar(
    name = "test_get_pow2",
)

cc_test_gen_scalar(
    name = "test_twosum",
)

cc_test_gen_scalar(
    name = "test_twoprodfma",
)

cc_test_gen_scalar(
    name = "test_rn_mode",
    mode = "dynamic",
)

cc_test_gen_scalar(
    name = "test_config_epoch",
    mode = "dynamic",
)

# Accuracy tests

# Stochastic Rounding rounding mode

cc_test_lib_gen(
    name = "seed-api",
    src = [
        ":test_seed_api.cpp",
        "//src:prism_api.h",
        "//src:xoshiro.h",
        "//src:random-inl.h",
        "//src:target_utils.h",
        "//src:debug_vector-inl.h",
    ],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "sr-accuracy",
    size = "medium",
    src = [
        ":test_sr_accuracy.cpp",
    ],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "sr-accuracy-dbg",
    size = "medium",
    src = [
        ":test_sr_accuracy.cpp",
    ],
    dbg = True,
    mode = "dynamic",
)

# Up/Down rounding mode

cc_test_lib_gen(
    name = "ud-accuracy",
    size = "small",
    src = [
        ":test_ud_accuracy.cpp",
    ],
    mode = "dynamic",
)

cc_test_lib_gen(
    name = "ud-accuracy-dbg",
    size = "small",
    src = [
        ":test_ud_accuracy.cpp",
    ],
    dbg = True,
    mode = "dynamic",
)

# Up/Down rounding mode using random fullbits

cc_test_lib_gen(
    name = "ud-accuracy-fullbits",
    size = "small",
    src = [
        ":test_ud_accuracy.cpp",
    ],
    copts = RANDOM_FULLBITS_COPTS,
    mode = "",
    deps = ["@//src:prism-dynamic-fullbits"],
)

cc_test_lib_gen(
    name = "ud-accuracy-fullbits-dbg",
    size = "small",
    src = [
        ":test_ud_accuracy.cpp",
    ],
    copts = RANDOM_FULLBITS_COPTS,
    dbg = True,
    mode = "",
    deps = ["@//src:prism-dynamic-fullbits-dbg"],
)

# Single tests for testing specific inputs
# These tests are useful for debugging
cc_test_binary(
    name = "sr-single",
    src = [":test_single_sr.cpp"],
    dbg = True,
    mode = "dynamic",
)

cc_test_binary(
    name = "ud-single",
    src = [":test_single_ud.cpp"],
    dbg = True,
    mode = "dynamic",
)

test_suite(
    name = "all",
    tests = [
        ":seed-api",
        ":sr-accuracy",
        ":test_get_exponent",
        ":test_get_pow2",
        ":test_get_predabs",
        ":test_twoprodfma",
        ":test_twosum",
        ":test_rn_mode",
        ":test_config_epoch",
        ":ud-accuracy",
    ],
)
