# gdk-math
math library for 3d games

API documentation: https://jfcameron.github.io/gdk-math/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library and its tests on linux using gcc
cmake --preset linux-gcc

# build the library and tests
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::math`, or install it and `find_package(gdk-math)`.

## Overview
Everything is in `gdk::math`. `<gdk/math.h>` includes it all; each part is also in `gdk/math/`.

`vector2`, `vector3`, `vector4`, `quaternion`, `matrix3x3`, `matrix4x4`, each are template classes, their component type defaults to `float`.
Operations *between* different types are free functions in `gdk/math/ops.h`.
The same operations over many operands at once are in `gdk/math/batch.h`.

Two concepts constrain the component type, because the types have two different requirements:

| concept | requires | used by |
| --- | --- | --- |
| `arithmetic_component` | signed arithmetic | `vector2`, `vector3`, `vector4` |
| `floating_point_component` | floating point | `quaternion`, `matrix3x3`, `matrix4x4` |

An integer vector does integer arithmetic. The members that need a floating point type, such as `normalize` and `angle_between`, are not offered for it.

The library is right-handed by convention: **+X** right, **+Y** up, **+Z** back, so forward is −Z.

## Batch functions
`transform_points` and the rest of `gdk/math/batch.h` take a source and a destination span. The destination may be the source itself, but not a span that partly overlaps it.

The SIMD kernels are SSE2 on x86 and x86-64, and NEON on arm64 with gcc or clang. Elsewhere, MSVC on arm64 and 32-bit arm included, the batch functions are the plain loop. Their results are bit-identical to the single-operand operators wherever floating point is evaluated at its own precision (`FLT_EVAL_METHOD == 0`): everywhere but 32-bit x86 using the x87.

## Build options

| option | default | |
| --- | --- | --- |
| `GDK_MATH_BACKEND` | `std` | which directory under `impl/` supplies the implementation |
| `GDK_MATH_STRICT_FP` | `OFF` | reproducibility across machines rather than speed |
| `GDK_MATH_FAST_TRANSCENDENTALS` | `OFF` | polynomial `acos`/`sin` in `slerp`: ~1.3x, bounded angular error |
| `GDK_MATH_SIMD` | `ON` | SIMD kernels in the batch functions: ~2-3x on SSE2 targets; NEON not yet measured |
| `GDKMATH_BUILD_TESTS` | top level only | the unit tests |
| `GDKMATH_BUILD_BENCH` | `OFF` | `gdkmath_bench`, for the configured backend |
| `GDKMATH_BUILD_DOCS` | `OFF` | the doxygen documentation |

`GDK_MATH_STRICT_FP`, `GDK_MATH_FAST_TRANSCENDENTALS` and `GDK_MATH_SIMD` each have a preset, and CI builds and tests each one.
