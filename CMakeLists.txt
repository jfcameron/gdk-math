# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DOCS "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)
option(JFC_BUILD_BENCH "Build the benchmark harness" OFF)

set(GDK_MATH_BACKEND "std")

set(GDK_MATH_BACKEND_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/impl/${GDK_MATH_BACKEND}")

if (NOT IS_DIRECTORY "${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}")
    message(FATAL_ERROR
        "GDK_MATH_BACKEND is \"${GDK_MATH_BACKEND}\" but ${GDK_MATH_BACKEND_INCLUDE_DIRECTORY} "
        "does not exist. A backend is a directory under impl/ containing gdk/storage.inl and a "
        "gdk/<type>.inl for each type. See README.md.")
endif()

jfc_project(library
    NAME "gdkmath"
    VERSION 0.9
    DESCRIPTION "game development kit math lib"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE INTERFACE

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GDK_MATH_BACKEND_INCLUDE_DIRECTORY}
        
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

