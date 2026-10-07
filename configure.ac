m4_define([PRISM_VERSION], [m4_esyscmd([grep -oPZ '(?<=__version__ = ")[^"]*' VERSION | tr -d '\n'])])

AC_PREREQ([2.69])
AC_INIT([prism], [PRISM_VERSION], [])
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([silent-rules foreign])

dnl Prefer LLVM toolchain compilers when available
if test -n "$LLVM_PATH"; then
  prism_llvm_tool_path="$LLVM_PATH/bin$PATH_SEPARATOR$PATH"
else
  prism_llvm_tool_path="$PATH"
fi
AC_PATH_PROG([LLVM_CLANG], [clang], [], [$prism_llvm_tool_path])
AC_PATH_PROG([LLVM_CLANGXX], [clang++], [], [$prism_llvm_tool_path])

if test -n "$LLVM_CLANG" && test -z "${CC+set}"; then
  CC="$LLVM_CLANG"
fi
if test -n "$LLVM_CLANGXX" && test -z "${CXX+set}"; then
  CXX="$LLVM_CLANGXX"
fi

LT_INIT
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Bridge LLVM_PATH → with_llvm so AX_LLVM finds llvm-config from the same
dnl installation used for CC/CXX above. Only applied when --with-llvm is absent
dnl (AC_ARG_WITH in AX_LLVM treats a preset with_llvm as given).
if test -z "${with_llvm+set}" && test -n "$LLVM_PATH"; then
  with_llvm="$LLVM_PATH"
fi
dnl LLVM 21+ is only exercised by the aarch64 SVE CI job so far.
AX_LLVM([4.0],[23.99.99],[all])

AC_SUBST(LLVM_CPPFLAGS, $LLVM_CPPFLAGS)
AC_SUBST(LLVM_LDFLAGS, $LLVM_LDFLAGS)
AC_SUBST(LLVM_VERSION_MAJOR, $LLVM_VERSION_MAJOR)

AC_PATH_PROG([CLANG], [clang], "", $LLVM_BINDIR/ $PATH)
if test -z "$CLANG"; then
   AC_PATH_PROGS([CLANG], [clang-"$LLVM_VERSION_MAJOR"."$LLVM_VERSION_MINOR" clang], "")
fi
if test -z "$CLANG"; then
  AC_MSG_ERROR([Could not find clang])
fi
AC_DEFINE_UNQUOTED([CLANG_PATH], ["$CLANG"], [CLANG path])
AC_SUBST(CLANG_PATH, $CLANG)

AC_PATH_PROG([CLANGXX], [clang++], "", $LLVM_BINDIR/ $PATH)
if test -z "$CLANGXX"; then
   AC_PATH_PROGS([CLANGXX], [clang++-"$LLVM_VERSION_MAJOR"."$LLVM_VERSION_MINOR" clang++], "")
fi
if test -z "$CLANGXX"; then
  AC_MSG_ERROR([Could not find clang++])
fi
AC_DEFINE_UNQUOTED([CLANGXX_PATH], ["$CLANGXX"], [CLANGXX path])
AC_SUBST(CLANGXX_PATH, $CLANGXX)

AC_PATH_PROG([LLVMLINK], [llvm-link], "", $LLVM_BINDIR/ $PATH)
if test -z "$LLVMLINK"; then
   AC_PATH_PROGS([LLVMLINK], [llvm-link-"$LLVM_VERSION_MAJOR"."$LLVM_VERSION_MINOR" llvm-link], "")
fi
if test -z "$LLVMLINK"; then
  AC_MSG_ERROR([Could not find llvm-link])
fi
AC_DEFINE_UNQUOTED([LLVMLINK_PATH], ["$LLVMLINK"], [LLVMLINK path])
AC_SUBST(LLVMLINK_PATH, $LLVMLINK)


# Check for parallel (required for the test infrastructure)
AC_PATH_PROG(PARALLEL, [parallel], [])
if test -z "$PARALLEL"; then
  AC_MSG_ERROR([Could not find parallel])
fi

# Check for bazel
AC_PATH_PROG(BAZEL, [bazel], [])
if test -z "$BAZEL"; then
  AC_PATH_PROG(BAZEL, [bazelisk], [])
fi
if test -z "$BAZEL"; then
  AC_MSG_ERROR([Could not find bazel])
fi

AC_ARG_WITH(
  [arch],
  [AS_HELP_STRING([--with-arch=ARCH], [target CPU architecture for static dispatch (default: native)])],
  [PRISM_ARCH="$withval"],
  [PRISM_ARCH="native"]
)

AS_CASE(
  ["$PRISM_ARCH"],
  [yes|no|""], [AC_MSG_ERROR([--with-arch requires an architecture, for example x86-64-v3])],
  [-*], [AC_MSG_ERROR([--with-arch must not begin with '-'])],
  [*[!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._+-]*],
    [AC_MSG_ERROR([--with-arch contains invalid characters])]
)

AC_MSG_CHECKING([if $CLANG supports -march=$PRISM_ARCH])
MARCH_FLAG="-march=$PRISM_ARCH"
doit='$CLANG $MARCH_FLAG -E - </dev/null >/dev/null 2>&1'
if AC_TRY_EVAL(doit); then
  AC_MSG_RESULT([yes])
else
  if test "x$PRISM_ARCH" = "xnative"; then
    MARCH_FLAG="-mtune=native"
    AC_MSG_RESULT([no; using $MARCH_FLAG])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([$CLANG does not support -march=$PRISM_ARCH])
  fi
fi

dnl Experimental Arm support: NEON by default, SVE on request.
AC_ARG_ENABLE(
  [experimental-sve],
  [AS_HELP_STRING([--enable-experimental-sve], [also build Arm SVE targets (experimental; default: NEON only)])],
  [],
  [enable_experimental_sve=no]
)
AS_CASE(
  ["$enable_experimental_sve"],
  [yes], [PRISM_EXPERIMENTAL_SVE=True],
  [no], [PRISM_EXPERIMENTAL_SVE=False],
  [AC_MSG_ERROR([--enable-experimental-sve takes no value])]
)
AC_SUBST([PRISM_EXPERIMENTAL_SVE])

AC_MSG_CHECKING([whether the target is aarch64])
AS_CASE(
  [`$CLANG -dumpmachine 2>/dev/null`],
  [aarch64*|arm64*], [prism_aarch64=yes],
  [prism_aarch64=no]
)
AC_MSG_RESULT([$prism_aarch64])

if test "x$prism_aarch64" = "xyes"; then
  AC_MSG_WARN([Arm (aarch64) support is experimental])

  dnl -march=native silently falls back to a generic armv8 CPU when clang does
  dnl not know the host CPU, which leaves the static library at baseline NEON.
  dnl Compare predefined macros: target-cpu alone is not enough, since some
  dnl clang versions report "generic" but still enable the detected features.
  if test "x$PRISM_ARCH" = "xnative"; then
    prism_native_macros=`$CLANG -march=native -dM -E -x c /dev/null 2>/dev/null | sort`
    prism_armv8_macros=`$CLANG -march=armv8-a -dM -E -x c /dev/null 2>/dev/null | sort`
    if test "x$prism_native_macros" = "x$prism_armv8_macros"; then
      AC_MSG_WARN([$CLANG does not recognize this CPU: -march=native targets a generic armv8 CPU. Pass --with-arch (e.g. armv8.2-a+fp16+dotprod, armv9-a+sve2) to target its features.])
    fi
  fi

  AC_MSG_CHECKING([whether $MARCH_FLAG enables SVE])
  if $CLANG $MARCH_FLAG -dM -E -x c /dev/null 2>/dev/null |
       grep -q '__ARM_FEATURE_SVE '; then
    prism_march_sve=yes
  else
    prism_march_sve=no
  fi
  AC_MSG_RESULT([$prism_march_sve])
  dnl With SVE targets disabled, Highway's static target is NEON while the
  dnl translation units would still be compiled for SVE: GCC then refuses to
  dnl inline across the target mismatch, and SVE could still be emitted by
  dnl the compiler outside Highway. Build for the same CPU without SVE instead.
  if test "x$prism_march_sve" = "xyes" && test "x$enable_experimental_sve" != "xyes"; then
    MARCH_FLAG="$MARCH_FLAG+nosve"
    AC_MSG_WARN([SVE targets are disabled: using $MARCH_FLAG (NEON). Use --enable-experimental-sve to build SVE targets.])
  fi

  dnl Highway only generates SVE code with clang >= 21 (SVE2_128) or >= 22
  dnl (SVE, SVE2, SVE_256); see HWY_BROKEN_SVE* in hwy/detect_targets.h.
  if test "x$enable_experimental_sve" = "xyes"; then
    prism_clang_major=`$CLANG -dumpversion 2>/dev/null | cut -d. -f1`
    if test -n "$prism_clang_major" && test "$prism_clang_major" -lt 21; then
      AC_MSG_WARN([clang $prism_clang_major is too old for Highway SVE targets (clang >= 22 required, >= 21 for SVE2_128): no SVE code will be generated])
    elif test -n "$prism_clang_major" && test "$prism_clang_major" -lt 22; then
      AC_MSG_WARN([clang $prism_clang_major only supports the SVE2_128 Highway target (clang >= 22 for SVE, SVE2 and SVE_256)])
    fi
  fi
elif test "x$enable_experimental_sve" = "xyes"; then
  AC_MSG_WARN([--enable-experimental-sve has no effect on non-aarch64 targets])
fi

AC_DEFINE_UNQUOTED([MARCH_FLAG], ["$MARCH_FLAG"], [Target architecture flag])
AC_SUBST([MARCH_FLAG])

# Generate config files
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([.bazelrc config.bzl])

AC_OUTPUT
