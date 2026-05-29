load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//:constants.bzl", "COPTS", "DEBUG_COPTS", "DYNAMIC_COPTS", "RANDOM_FULLBITS_COPTS", "RANDOM_PARTIALBITS_COPTS", "STATIC_COPTS")

exports_files([
    "prism_api.cpp",
    "prism_api.h",
    "debug.h",
    "debug_vector-inl.h",
    "eft.h",
    "random-inl.h",
    "sr_scalar.h",
    "sr_scalar_static.cpp",
    "sr_scalar_dynamic.cpp",
    "sr_vector.h",
    "sr_vector-inl.h",
    "sr_vector_static.cpp",
    "sr_vector_dynamic.cpp",
    "target_utils.h",
    "ud_scalar.h",
    "ud_scalar_static.cpp",
    "ud_scalar_dynamic.cpp",
    "ud_vector.h",
    "ud_vector-inl.h",
    "ud_vector_static.cpp",
    "ud_vector_dynamic.cpp",
    "utils.h",
    "xoshiro.h",
    "xoshiro_vector.cpp",
])

# File groups

# Xoshiro random number generator

filegroup(
    name = "srcs-xoshiro",
    srcs = [
        "debug_vector-inl.h",
        "random-inl.h",
        "target_utils.h",
        "xoshiro.h",
        "xoshiro_vector.cpp",
    ],
    visibility = ["//visibility:public"],
)

# PRISM API headers

filegroup(
    name = "headers-prism",
    srcs = [
        "sr_scalar.h",
        "sr_vector.h",
        "ud_scalar.h",
        "ud_vector.h",
    ],
)

# PRISM dynamic implementation

filegroup(
    name = "srcs-prism-dynamic",
    srcs = glob(
        [
            "*.cpp",
            "*.h",
        ],
        exclude = [
            "*static*",
        ],
    ),
    visibility = ["//visibility:public"],
)

# PRISM static implementation

filegroup(
    name = "srcs-prism-static",
    srcs = glob(
        [
            "*.cpp",
            "*.h",
        ],
        exclude = [
            "*dynamic*",
        ],
    ),
    visibility = ["//visibility:public"],
)

# Libraries

# Xoshiro random number generator

cc_library(
    name = "prism-rng",
    srcs = [":srcs-xoshiro"],
    hdrs = ["xoshiro.h"],
    copts = COPTS + DYNAMIC_COPTS + DEBUG_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

# PRISM dynamic dispatch library

cc_library(
    name = "prism-dynamic",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-dynamic-dbg",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS + DEBUG_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

# PRISM dynamic library using full bits for random numbers.

cc_library(
    name = "prism-dynamic-fullbits",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS + RANDOM_FULLBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-dynamic-fullbits-dbg",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS + DEBUG_COPTS + RANDOM_FULLBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-dynamic-partialbits",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS + RANDOM_PARTIALBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-dynamic-partialbits-dbg",
    srcs = [":srcs-prism-dynamic"],
    hdrs = [":headers-prism"],
    copts = COPTS + DYNAMIC_COPTS + DEBUG_COPTS + RANDOM_PARTIALBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

# PRISM static dispatch library

cc_library(
    name = "prism-static",
    srcs = [":srcs-prism-static"],
    hdrs = [":headers-prism"],
    copts = COPTS + STATIC_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-static-dbg",
    srcs = [":srcs-prism-static"],
    hdrs = [":headers-prism"],
    copts = COPTS + STATIC_COPTS + DEBUG_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

# PRISM static library using full bits for random numbers.

cc_library(
    name = "prism-static-partialbits",
    srcs = [":srcs-prism-static"],
    hdrs = [":headers-prism"],
    copts = COPTS + STATIC_COPTS + RANDOM_PARTIALBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

cc_library(
    name = "prism-static-partialbits-dbg",
    srcs = [":srcs-prism-static"],
    hdrs = [":headers-prism"],
    copts = COPTS + STATIC_COPTS + DEBUG_COPTS + RANDOM_PARTIALBITS_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@hwy"],
)

filegroup(
    name = "prism-all",
    srcs = [
        ":prism-dynamic",
        ":prism-dynamic-dbg",
        ":prism-static",
        ":prism-static-dbg",
    ],
)
