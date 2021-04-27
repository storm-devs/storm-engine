project(common_h)

find_package(DirectX REQUIRED)

auto_setup_library(${PROJECT_NAME} DIRECTORY .)

target_include_directories(${PROJECT_NAME}
    INTERFACE
    ${DirectX_INCLUDE_DIR}
    ${STORM_SOURCE_DIR}/shared
)

target_link_libraries(${PROJECT_NAME} 
    INTERFACE 
    ${DirectX_D3DX9_LIBRARY}
    CONAN_PKG::spdlog
)
