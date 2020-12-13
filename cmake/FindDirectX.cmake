# -----------------------------------------------------------------------------
# Find DirectX SDK
# Define:
# DirectX_FOUND
# DirectX_INCLUDE_DIR
# DirectX_LIBRARY
# DirectX_ROOT_DIR

set(DirectX_FOUND false)

set(DirectX_INCLUDE_DIR "${STORM_EXTERN_DIR}/directx/include")
mark_as_advanced(DirectX_INCLUDE_DIR)
if(DirectX_INCLUDE_DIR)
  set(DirectX_ROOT_DIR "${DirectX_INCLUDE_DIR}/..")
endif()

# dlls are in DirectX_ROOT_DIR/Developer Runtime/x64|x86
# lib files are in DirectX_ROOT_DIR/Lib/x64|x86

set(DirectX_LIBRARY_PATHS)
if(CMAKE_CL_64)
  set(DirectX_LIBRARY_PATHS
    "${DirectX_ROOT_DIR}/Lib/x64")
else()
  set(DirectX_LIBRARY_PATHS
    "${DirectX_ROOT_DIR}/Lib/x86" "${DirectX_ROOT_DIR}/lib")
endif()

find_library(DirectX_LIBRARY d3d9 ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)
find_library(DirectX_D3DX9_LIBRARY d3dx9 ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)
find_library(DirectX_DXERR_LIBRARY DxErr ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)
if(DirectX_INCLUDE_DIR AND DirectX_LIBRARY AND DirectX_D3DX9_LIBRARY AND DirectX_DXERR_LIBRARY)
  set(DirectX_FOUND true)
endif()
mark_as_advanced(DirectX_LIBRARY DirectX_D3DX9_LIBRARY DirectX_DXERR_LIBRARY)

#message("DirectX_FOUND=${DirectX_FOUND}")
#message("DirectX_INCLUDE_DIR=${DirectX_INCLUDE_DIR}")
#message("DirectX_LIBRARY=${DirectX_LIBRARY}")
#message("DirectX_ROOT_DIR=${DirectX_ROOT_DIR}")

