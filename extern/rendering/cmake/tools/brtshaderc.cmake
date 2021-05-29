# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

include( cmake/3rdparty/fcpp.cmake )
include( cmake/3rdparty/glsl-optimizer.cmake )
include( cmake/3rdparty/glslang.cmake )
include( cmake/3rdparty/spirv-cross.cmake )
include( cmake/3rdparty/spirv-tools.cmake )
include( cmake/3rdparty/webgpu.cmake )

if( TARGET brtshaderc )
	return()
endif()

#file( GLOB brtcshader_SOURCES 
#${BGFX_DIR}/tools/shaderc/shaderc_glsl.cpp 
#${BGFX_DIR}/tools/shaderc/shaderc_hlsl.cpp 
#${BGFX_DIR}/tools/shaderc/shaderc_pssl.cpp 
#${BGFX_DIR}/tools/shaderc/shaderc_spirv.cpp 
#${BGFX_DIR}/tools/shaderc/shaderc_metal.cpp
#${BGFX_DIR}/tools/brtshaderc/brtshaderc.cpp )

file( GLOB brtcshader_SOURCES ${BGFX_DIR}/tools/brtshaderc/brtshaderc.cpp )

add_library( brtshaderc STATIC EXCLUDE_FROM_ALL ${brtcshader_SOURCES} )

include_directories(
${BGFX_DIR}/examples/common
${BGFX_DIR}/3rdparty 
${BGFX_DIR}/3rdparty/fcpp
${BGFX_DIR}/3rdparty/dxsdk/include 
${BGFX_DIR}/3rdparty/khronos
$<BUILD_INTERFACE:${BGFX_DIR}/include>
$<BUILD_INTERFACE:${BGFX_DIR}/tools/brtshaderc>
$<BUILD_INTERFACE:${BIMG_DIR}/include>
$<BUILD_INTERFACE:${BX_DIR}/include>
$<BUILD_INTERFACE:${BX_DIR}/3rdparty>)

include_directories(
${GLSL-OPTIMIZER_INCLUDES}
${GLSLLANG_INCLUDES}
${SPIRV_CROSS_INCLUDES}
${SPIRV_TOOLS_INCLUDES}
${WEBGPU_INCLUDES})

# Build system specific configurations
if( MSVC )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/msvc>
						$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/msvc> )
elseif( MINGW )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/mingw>
		    			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/mingw> )
elseif( APPLE )
	include_directories($<BUILD_INTERFACE:${BX_DIR}/include/compat/osx>
		    			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/osx> )
endif()



target_compile_definitions( brtshaderc PRIVATE "-D_CRT_SECURE_NO_WARNINGS" )
set_target_properties( brtshaderc PROPERTIES FOLDER "bgfx/tools" )

if (ANDROID)
    target_link_libraries(brtshaderc PRIVATE log)
elseif (IOS)
	set_target_properties(brtshaderc PROPERTIES MACOSX_BUNDLE ON
											 MACOSX_BUNDLE_GUI_IDENTIFIER brtshaderc)
endif()
