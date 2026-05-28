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

if test -n "$LLVM_CLANG"; then
  CC="$LLVM_CLANG"
fi
if test -n "$LLVM_CLANGXX"; then
  CXX="$LLVM_CLANGXX"
fi

LT_INIT
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AX_LLVM([4.0],[20.1.8],[all])

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

# Checks if clang support -march=native
AC_MSG_CHECKING([if $CLANG supports -march=native])
doit='$CLANG -march=native -E - </dev/null >/dev/null 2>&1'
if AC_TRY_EVAL(doit); then
  AC_DEFINE_UNQUOTED([MARCH_FLAG], ["-march=native"], [The -march=native flag])
  AC_SUBST(MARCH_FLAG, "-march=native")
  AC_MSG_RESULT([yes])
else
  AC_DEFINE_UNQUOTED([MARCH_FLAG], ["-mtune=native"], [The -mtune=native flag])
  AC_SUBST(MARCH_FLAG, "-mtune=native")
  AC_MSG_RESULT([no])
fi

# Generate config files 
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([.bazelrc config.bzl])

AC_OUTPUT
