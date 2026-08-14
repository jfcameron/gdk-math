# © Joseph Cameron - All Rights Reserved

# TODO: bench currently does not support MSVC, should fix that
if (MSVC)
    message(STATUS "gdk-math: JFC_BUILD_BENCH is ON, but the bench needs gcc/clang extended asm. Skipping.")

    return()
endif()

file(GLOB GDK_MATH_BACKEND_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../impl/*")

foreach(GDK_MATH_BACKEND_PATH ${GDK_MATH_BACKEND_PATHS})
    if (NOT IS_DIRECTORY "${GDK_MATH_BACKEND_PATH}")
        continue()
    endif()

    get_filename_component(GDK_MATH_BACKEND "${GDK_MATH_BACKEND_PATH}" NAME)

    set(GDK_MATH_BENCH_TARGET "gdkmath_bench_${GDK_MATH_BACKEND}")

    add_executable(${GDK_MATH_BENCH_TARGET} "${CMAKE_CURRENT_LIST_DIR}/bench.cpp")

    target_include_directories(${GDK_MATH_BENCH_TARGET} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${GDK_MATH_BACKEND_PATH}")

    target_compile_definitions(${GDK_MATH_BENCH_TARGET} PRIVATE
        GDK_MATH_BENCH_BACKEND="${GDK_MATH_BACKEND}")

    set_target_properties(${GDK_MATH_BENCH_TARGET} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

    target_compile_options(${GDK_MATH_BENCH_TARGET} PRIVATE -O2)
endforeach()
