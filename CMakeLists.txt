# © 2018 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_APP   "Build the app" ON)
option(JFC_BUILD_DOCS  "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)

jfc_project(library
    NAME "gdkmath"
    VERSION 0.0
    DESCRIPTION "game development kit math lib"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        
    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/intvector2.cpp
)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

