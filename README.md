# gdk-math

math library for 3d games. Strict C++20. 

Right-handed: **+X** right, **+Y** up, **+Z** back, so forward is −Z.

## Types

`vector2`, `vector3`, `vector4`, `quaternion`, `matrix3x3`, `matrix4x4`, each are template classes, their component type defaults to `float`. 
Operations *between* different types are free functions in `math_ops.h`. 
The same operations over many operands at once are in `math_batch.h`.

Two concepts constrain the component type, because the types have two different requirements:

| concept | requires | used by |
| --- | --- | --- |
| `arithmetic_component` | signed arithmetic | `vector2`, `vector3`, `vector4` |
| `floating_point_component` | floating point | `quaternion`, `matrix3x3`, `matrix4x4` |

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

