# © Joseph Cameron - MIT License

add_executable(gdkmath_bench "${CMAKE_CURRENT_LIST_DIR}/bench.cpp")

target_link_libraries(gdkmath_bench PRIVATE gdkmath)

target_compile_definitions(gdkmath_bench PRIVATE GDK_MATH_BENCH_BACKEND="${GDK_MATH_BACKEND}")

set_target_properties(gdkmath_bench PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

target_compile_options(gdkmath_bench PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/O2,-O2>)
