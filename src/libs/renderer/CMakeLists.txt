project(renderer)

add_library(${PROJECT_NAME} STATIC
    Effects.cpp
    Effects.h
    Font.cpp
    Font.h
    Screenshot.cpp
    sdevice.cpp
    sdevice.h
    texture.h
    IVBufferManager.cpp
    sprite_renderer.cpp
    primitive_renderer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${DirectX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    common_h
    ${DirectX_LIBRARY}
    ${DirectX_D3DX9_LIBRARY}
    ${DirectX_DXERR_LIBRARY}

    # fixes unresolved external symbol __vsnwprintf referenced in function "long __stdcall StringVPrintfWorkerW
    # caused by DXGetErrorString and DXGetErrorDescription
    legacy_stdio_definitions.lib
)
