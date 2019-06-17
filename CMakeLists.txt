# © 2018 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_APP    "Build the app" ON)
option(JFC_BUILD_DOCS   "Build documentation" ON)
option(JFC_BUILD_TESTS  "Build unit tests" ON)

add_subdirectory(thirdparty)

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

        ${glm_INCLUDE_DIR}
        ${nlohmann_INCLUDE_DIR}

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/intvector2.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/src/mat4x4.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/quaternion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector4.cpp
)

#JFC_LOG(FATAL_ERROR "BLAR" "${PROJECT_NAME}")

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

