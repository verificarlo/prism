"""
"""

load("//:config.bzl", "MARCH_FLAG")

# Compiler options

# Slow, but full-precision random number generation.
RANDOM_FULLBITS_COPTS = [
    "-DPRISM_RANDOM_FULLBITS",
]

# Fast, but partial-precision random number generation.
RANDOM_PARTIALBITS_COPTS = [
    "-UPRISM_RANDOM_FULLBITS",
]

COPTS = [
    "-std=c++17",
    "-Wfatal-errors",
    "-O2",
    "-Wall",
    "-Wno-psabi",
] + RANDOM_PARTIALBITS_COPTS

NATIVE_COPTS = [
    MARCH_FLAG,
]

STATIC_COPTS = [
    "-DWARN_FMA_EMULATION",
    "-DHWY_COMPILE_ONLY_STATIC",
    "-DPRISM_DISPATCH=static_dispatch",
] + NATIVE_COPTS

DYNAMIC_COPTS = [
    "-DHWY_COMPILE_ALL_ATTAINABLE",
    "-DPRISM_DISPATCH=dynamic_dispatch",
]

DEBUG_COPTS = [
    "-DPRISM_DEBUG",
    "-Og",
    "-g",
    "-fno-omit-frame-pointer",
]

SANITIZE_COPTS = [
    "-fsanitize=address",
    "-fsanitize=undefined",
    "-fsanitize=leak",
    "-fsanitize=thread",
]
