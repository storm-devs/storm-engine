cmake_minimum_required(VERSION 3.17)

project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup third-party dependencies
include(conan)
set(ENV{CONAN_REVISIONS_ENABLED} 1)
conan_add_remote(NAME bincrafters
    URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
    VERIFY_SSL True
)
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)
conan_output_dirs_setup()

include(AutoSetup)

option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)

set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_INSTALL_EXAMPLES OFF)

set(BGFX_BUILD_TOOLS OFF)

set(STORM_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(STORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STORM_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")
set(STORM_EXTERN_RENDERING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/rendering")
set(STORM_EXTERN_MATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/math")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories(${STORM_EXTERN_RENDERING_DIR}/bgfx/examples/common
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty 
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty/dxsdk/include 
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty/khronos
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/examples/common>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/tools/brtshaderc>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bimg/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/3rdparty>
						${STORM_EXTERN_MATH_DIR}/glm/glm
						${GLSL-OPTIMIZER_INCLUDES}
						${GLSLLANG_INCLUDES}
						${SPIRV_CROSS_INCLUDES}
						${SPIRV_TOOLS_INCLUDES}
						${WEBGPU_INCLUDES})

# Build system specific configurations
if( MSVC )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/msvc>
						$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/msvc>)
elseif( MINGW )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/mingw>
		    			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/mingw> )
elseif( APPLE )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/osx>
		    			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/osx> )
endif()

include_directories()



add_definitions("-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")


add_subdirectory(src)

target_link_libraries(engine PRIVATE bgfx dear-imgui meshoptimizer brtshaderc fcpp bx bimg bgfx-vertexlayout bgfx-shader-spirv fcpp glsl-optimizer glslang spirv-cross spirv-tools webgpu example-common)


add_subdirectory(${STORM_EXTERN_RENDERING_DIR})

add_subdirectory(${STORM_EXTERN_MATH_DIR}/glm)




