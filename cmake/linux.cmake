if(STORM_USE_CONAN_SDL)
    set(SDL2_LIBRARIES "sdl")
else()
    find_package(SDL2 REQUIRED)
    message(STATUS "SDL2_LIBRARIES="${SDL2_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${SDL2_INCLUDE_DIRS} -fsigned-char")
    message(STATUS "CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS})
endif()

message("Using DXVK-native for D3D9 API")

include(ExternalProject)
ExternalProject_Add(dxvk-native
    GIT_REPOSITORY    https://github.com/Joshua-Ashton/dxvk-native
    GIT_TAG           a2dc99c407340432d4ba5bfa29efa685c27942ea
    GIT_SHALLOW       ON
    BUILD_ALWAYS      OFF
    CONFIGURE_HANDLED_BY_BUILD ON
    CONFIGURE_COMMAND meson setup ../dxvk-native --buildtype=release -Denable_d3d11=false -Denable_d3d10=false -Denable_dxgi=false
    BUILD_COMMAND     ninja
    INSTALL_COMMAND   ""
)
ExternalProject_Get_property(dxvk-native SOURCE_DIR BINARY_DIR)
set(DXVK_NATIVE_INCLUDE_DIRS
    "${SOURCE_DIR}/include/native/directx"
    "${SOURCE_DIR}/include/native/windows"
)
set(NATIVE_D3D9_LIBS ${BINARY_DIR}/src/d3d9/libdxvk_d3d9.so)
include_directories("${DXVK_NATIVE_INCLUDE_DIRS}")
add_custom_target(dependencies ALL DEPENDS dxvk-native)
