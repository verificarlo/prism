```bash
_|_|_|    _|_|_|    _|_|_|    _|_|_|  _|      _|
_|    _|  _|    _|    _|    _|        _|_|  _|_|
_|_|_|    _|_|_|      _|      _|_|    _|  _|  _|
_|        _|    _|    _|          _|  _|      _|
_|        _|    _|  _|_|_|  _|_|_|    _|      _|
```
---

# Probabilistic Rounding with Instruction Set Management

This library provides a **vectorized implementation** of two probabilistic rounding modes:

1. **Up-Down Rounding Mode**: Add +/- 1 ulp with equal probabilities (1/2). Do not preserve exact operations.
2. **Stochastic Rounding**: As described in [Fasi and Mikaitis: Algorithms for Stochastically Rounded Elementary Arithmetic Operations](https://ieeexplore.ieee.org/document/9387551), extended here to support the FMA operator.

The library leverages the [Highway library](https://github.com/google/highway), a high-performance C++ library for portable vector instructions across platforms. It uses **dynamic dispatch** to efficiently execute functions across different architectures.

### Features

The library is available in three interfaces:
- **Array Interface**: Supports probabilistic rounding (PR) on contiguous arrays, providing a simple and flexible interface.
- **Dynamic Interface**: Provides an interface for single vector instructions with **dynamic dispatch** to automatically select the best implementation for the target architecture.
- **Static Interface**: Provides an interface for single vector instructions with **static dispatch**, delivering optimal performance by bypassing architecture selection. This mode is not portable across architectures.

This combination of features makes the library versatile for scientific computing, numerical analysis, and high-performance applications requiring probabilistic rounding.

## Binary releases

Linux x86-64 binaries are attached to each [GitHub release](https://github.com/verificarlo/prism/releases). Choose the highest architecture level supported by every machine that will run the static-dispatch library:

| Archive suffix | Minimum CPU features |
| --- | --- |
| `x86-64` | Baseline x86-64 |
| `x86-64-v2` | SSE3, SSSE3, SSE4.1, SSE4.2, and POPCNT |
| `x86-64-v3` | AVX, AVX2, BMI1/2, F16C, FMA, and related features |
| `x86-64-v4` | AVX-512 foundation and the standard v4 extensions |

For example, set the desired release version and install its baseline package under `/usr/local`:

```bash
VERSION=X.Y.Z
curl -LO "https://github.com/verificarlo/prism/releases/download/v${VERSION}/prism-${VERSION}-linux-x86-64.tar.gz"
sudo tar -C /usr/local --strip-components=1 -xzf "prism-${VERSION}-linux-x86-64.tar.gz"
sudo ldconfig
```

Each release also includes `SHA256SUMS`. The archives are built on Ubuntu 22.04 with LLVM 18 and contain shared and static libraries, public headers, and the generated LLVM IR files. The dynamic-dispatch library selects a supported vector target at runtime; the static-dispatch library requires the CPU level named by the archive.

## Requirements

- clang, clang++
- parallel ([install](https://www.gnu.org/software/parallel/))
- **bazelisk** ([install](https://github.com/bazelbuild/bazelisk/releases))

## Build and install from source

The default build requests `-march=native` and falls back to `-mtune=native` when the compiler does not support it. Pass `--with-arch` to build the static-dispatch library for a specific CPU level.

```bash
./autogen.sh
./configure
# ./configure --with-arch=x86-64-v3
make
make install
```

## Tests

```bash
bazel test tests:all
```

## Current status

The library has only been tested on X86-64 architectures for the moment.
