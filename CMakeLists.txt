# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DOCS "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)
option(JFC_BUILD_BENCH "Build the benchmark harness" OFF)

set(GDK_MATH_BACKEND "std" CACHE STRING
    "gdk-math implementation to build against: a directory name under impl/")

option(GDK_MATH_STRICT_FP
    "Build gdk-math for reproducibility across machines rather than for speed" OFF)

set(GDK_MATH_BACKEND_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/impl/${GDK_MATH_BACKEND}")

if (NOT IS_DIRECTORY "${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}")
    message(FATAL_ERROR
        "GDK_MATH_BACKEND is \"${GDK_MATH_BACKEND}\" but ${GDK_MATH_BACKEND_INCLUDE_DIRECTORY} "
        "does not exist. A backend is a directory under impl/ containing gdk/storage.inl, a "
        "gdk/<type>.inl for each type, and gdk/math_batch.inl.")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

if (GDK_MATH_STRICT_FP)
    if (MSVC)
        set(GDK_MATH_FP_FLAGS /fp:strict)
    else()
        set(GDK_MATH_FP_FLAGS -fno-fast-math -ffp-contract=off)
    endif()

    foreach (_gdk_math_fp_flag ${GDK_MATH_FP_FLAGS})
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_gdk_math_fp_flag}>)
    endforeach()
endif()

jfc_project(library
    NAME "gdkmath"
    VERSION 0.9
    DESCRIPTION "game development kit math lib"
    C++_STANDARD 20
    C_STANDARD 90
    TYPE INTERFACE

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}
        
)

if (GDK_MATH_STRICT_FP)
    foreach (_gdk_math_fp_flag ${GDK_MATH_FP_FLAGS})
        target_compile_options(gdkmath INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${_gdk_math_fp_flag}>)
    endforeach()
endif()

if (GDK_MATH_STRICT_FP)
    set(GDK_MATH_STRICT_FP_VALUE 1)
else()
    set(GDK_MATH_STRICT_FP_VALUE 0)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/math_config.h.in"
    "${PROJECT_BINARY_DIR}/generated_include/gdk/math_config.h" @ONLY)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_BENCH)
    add_subdirectory(bench)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

