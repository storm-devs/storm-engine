project(util)

auto_setup_library(${PROJECT_NAME} SOURCE_DIRECTORY source INCLUDE_DIRECTORY include)

auto_setup_library(${PROJECT_NAME}-test DIRECTORY test TYPE OBJECT)
target_link_libraries(${PROJECT_NAME}-test PUBLIC ${PROJECT_NAME} CONAN_PKG::catch2)
