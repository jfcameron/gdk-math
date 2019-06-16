# © 2018 Joseph Cameron - All Rights Reserved

#if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    jfc_add_tests(
        C++_STANDARD 17
        C_STANDARD 90

        TEST_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/vector2_test.cpp"

        INCLUDE_DIRECTORIES
            "${${PROJECT_NAME}_INCLUDE_DIRECTORIES}"
    )
#else()
#    jfc_log(STATUS "test" "testing not supported on ${CMAKE_SYSTEM_NAME}. Skipping...")
#endif()
