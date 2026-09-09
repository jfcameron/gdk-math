# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(gdkmath
    VERSION 1.0
    DESCRIPTION "game development kit math lib")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DOCS "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)
option(JFC_BUILD_BENCH "Build the benchmark harness" OFF)

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

if (NOT TARGET gdkmath)
    add_library(gdkmath INTERFACE)

    add_library(gdk::math ALIAS gdkmath)

    set_target_properties(gdkmath PROPERTIES EXPORT_NAME math)

    jfc_generate_cmake_header()

    target_include_directories(gdkmath INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated_include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"

        "$<BUILD_INTERFACE:${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}>"
        "$<INSTALL_INTERFACE:include>")

    target_compile_features(gdkmath INTERFACE cxx_std_20)

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

    target_compile_definitions(gdkmath
        INTERFACE
            GDK_MATH_FAST_TRANSCENDENTALS=${GDK_MATH_FAST_TRANSCENDENTALS_VALUE}
            GDK_MATH_SIMD=${GDK_MATH_SIMD_VALUE})

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/math_config.h.in"
        "${PROJECT_BINARY_DIR}/generated_include/gdk/math_config.h" @ONLY)

    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS gdkmath EXPORT gdkmath-targets)

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(DIRECTORY "${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(FILES "${PROJECT_BINARY_DIR}/generated_include/gdk/math_config.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gdk")

    install(EXPORT gdkmath-targets
        FILE gdkmath-targets.cmake
        NAMESPACE gdk::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdkmath")
endif()

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/gdkmath-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

file(WRITE 
    "${PROJECT_BINARY_DIR}/gdkmath-config.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/gdkmath-targets.cmake\")"
)

install(
    FILES
        "${PROJECT_BINARY_DIR}/gdkmath-config.cmake"
        "${PROJECT_BINARY_DIR}/gdkmath-config-version.cmake"
    DESTINATION 
        "${CMAKE_INSTALL_LIBDIR}/cmake/gdkmath"
)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_BENCH)
    add_subdirectory(bench)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

