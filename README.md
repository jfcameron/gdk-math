# gdk-math
math library for 3d games

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler 
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the demo programs on linux using gcc
cmake --preset linux-gcc-demo

# build the library and demo programs
cmake --build --preset linux-gcc-demo
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

## Overview
`vector2`, `vector3`, `vector4`, `quaternion`, `matrix3x3`, `matrix4x4`, each are template classes, their component type defaults to `float`. 
Operations *between* different types are free functions in `math_ops.h`. 
The same operations over many operands at once are in `math_batch.h`.

Two concepts constrain the component type, because the types have two different requirements:

| concept | requires | used by |
| --- | --- | --- |
| `arithmetic_component` | signed arithmetic | `vector2`, `vector3`, `vector4` |
| `floating_point_component` | floating point | `quaternion`, `matrix3x3`, `matrix4x4` |

The library is right-handed by convention: **+X** right, **+Y** up, **+Z** back, so forward is −Z.

## Build options

| option | default | |
| --- | --- | --- |
| `GDK_MATH_BACKEND` | `std` | which directory under `impl/` supplies the implementation |
| `GDK_MATH_STRICT_FP` | `OFF` | reproducibility across machines rather than speed |
| `GDK_MATH_FAST_TRANSCENDENTALS` | `OFF` | polynomial `acos`/`sin` in `slerp`: ~1.3x, bounded angular error |
| `GDK_MATH_SIMD` | `ON` | SIMD kernels in the batch functions: ~2-3x, bit-identical to scalar |
| `JFC_BUILD_TESTS` | `ON` | | whether or not to build tests
| `JFC_BUILD_BENCH` | `OFF` | builds one `gdkmath_bench_<backend>` per directory under `impl/` |
| `JFC_BUILD_DOCS` | `ON` | | whether or not to generate documentation

See `CMakePresets.json` for the configurations CI uses.

