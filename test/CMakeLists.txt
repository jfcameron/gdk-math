# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 17
    C_STANDARD 90

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/constexpr_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/instantiation_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/interpolation_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/layout_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/math_ops_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/matrix3x3_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/matrix4x4_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/matrix_parity_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/quaternion_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector2_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector3_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector4_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector_parity_test.cpp"

    INCLUDE_DIRECTORIES
        "${${PROJECT_NAME}_INCLUDE_DIRECTORIES}"
)

add_library(gdkmath_cxx17_conformance OBJECT "${CMAKE_CURRENT_LIST_DIR}/cxx17_conformance.cpp")

target_include_directories(gdkmath_cxx17_conformance
    PRIVATE "${gdkmath_INCLUDE_DIRECTORIES}")

set_target_properties(gdkmath_cxx17_conformance PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

if (MSVC)
    target_compile_options(gdkmath_cxx17_conformance PRIVATE /permissive- /W4 /WX)
else()
    target_compile_options(gdkmath_cxx17_conformance PRIVATE
        -pedantic-errors -Wall -Wextra -Werror)
endif()

