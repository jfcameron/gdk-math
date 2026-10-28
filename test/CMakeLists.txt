# © Joseph Cameron - MIT License

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/batch_test.cpp"
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

    LIBRARIES
        gdkmath
)

add_library(gdkmath_cxx20_conformance OBJECT "${CMAKE_CURRENT_LIST_DIR}/cxx20_conformance.cpp")

target_link_libraries(gdkmath_cxx20_conformance PRIVATE gdkmath)

set_target_properties(gdkmath_cxx20_conformance PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

if (MSVC)
    target_compile_options(gdkmath_cxx20_conformance PRIVATE /permissive- /W4 /WX)
else()
    target_compile_options(gdkmath_cxx20_conformance PRIVATE
        -pedantic-errors -Wall -Wextra -Wfloat-conversion -Werror)
endif()


if (NOT CMAKE_CROSSCOMPILING)
    add_test(NAME gdk-math_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
