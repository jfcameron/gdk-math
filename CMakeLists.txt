# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkmath)
    return()
endif()

project(gdk-math
    VERSION 1.0
    DESCRIPTION "vectors, quaternions and matrices for 3d games"
    LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKMATH_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKMATH_BUILD_DOCS "Build documentation" OFF)
option(GDKMATH_BUILD_BENCH "Build the benchmark harness" OFF)

set(GDK_MATH_BACKEND "std" CACHE STRING
    "gdk-math implementation to build against: a directory name under impl/")

option(GDK_MATH_STRICT_FP "Build gdk-math for reproducibility across machines rather than for speed" OFF)

option(GDK_MATH_FAST_TRANSCENDENTALS
    "Approximate acos and sin with polynomials where a bounded angular error is acceptable" OFF)

option(GDK_MATH_SIMD "Use SIMD kernels in the batch functions" ON)

set(GDK_MATH_BACKEND_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/impl/${GDK_MATH_BACKEND}")

if (NOT IS_DIRECTORY "${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}")
    message(FATAL_ERROR
        "GDK_MATH_BACKEND is \"${GDK_MATH_BACKEND}\" but ${GDK_MATH_BACKEND_INCLUDE_DIRECTORY} "
        "does not exist. A backend is a directory under impl/ containing gdk/math/storage.inl, a "
        "gdk/math/<type>.inl for each type, and gdk/math/batch.inl.")
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
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(gdkmath INTERFACE)
add_library(gdk::math ALIAS gdkmath)

set_target_properties(gdkmath PROPERTIES EXPORT_NAME math)

target_include_directories(gdkmath INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated_include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_features(gdkmath INTERFACE cxx_std_20)

foreach (_gdk_math_fp_flag ${GDK_MATH_FP_FLAGS})
    target_compile_options(gdkmath INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${_gdk_math_fp_flag}>)
endforeach()

if (GDK_MATH_FAST_TRANSCENDENTALS)
    set(GDK_MATH_FAST_TRANSCENDENTALS_VALUE 1)
else()
    set(GDK_MATH_FAST_TRANSCENDENTALS_VALUE 0)
endif()

if (GDK_MATH_SIMD)
    set(GDK_MATH_SIMD_VALUE 1)
else()
    set(GDK_MATH_SIMD_VALUE 0)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
    "${PROJECT_BINARY_DIR}/generated_include/gdk/math/config.h" @ONLY)

install(TARGETS gdkmath EXPORT gdk-math-targets)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(FILES "${PROJECT_BINARY_DIR}/generated_include/gdk/math/config.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gdk/math")

install(EXPORT gdk-math-targets
    FILE gdk-math-targets.cmake
    NAMESPACE gdk::
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/gdk-math")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/gdk-math-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)

file(WRITE "${PROJECT_BINARY_DIR}/gdk-math-config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-math-targets.cmake\")
")

install(FILES
    "${PROJECT_BINARY_DIR}/gdk-math-config.cmake"
    "${PROJECT_BINARY_DIR}/gdk-math-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/gdk-math")

if (GDKMATH_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKMATH_BUILD_BENCH)
    add_subdirectory(bench)
endif()

if (GDKMATH_BUILD_DOCS)
    add_subdirectory(docs)
endif()
