project(test-all)

auto_setup_executable(${PROJECT_NAME} DIRECTORY test)

list(APPEND TEST_LIBRARIES
        common-test
        )

target_link_libraries(${PROJECT_NAME} PUBLIC common-test CONAN_PKG::catch2)