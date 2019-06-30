# © 2018 Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 17
    C_STANDARD 90

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/intvector2_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/quaternion_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector2_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector3_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector4_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mat4x4_test.cpp"

    INCLUDE_DIRECTORIES
        "${${PROJECT_NAME}_INCLUDE_DIRECTORIES}"
        "${glm_INCLUDE_DIR}" # REMOVE WHEN QUAT IS FIXED 
)

