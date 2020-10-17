project(render)

add_library(render MODULE 
	Font.cpp
	Font.h
	Screenshot.cpp
	sdevice.cpp
	sdevice.h
	Technique.cpp
	Technique.h
	texture.h
)

target_include_directories(render PRIVATE ${DirectX_INCLUDE_DIR})

target_link_libraries(render PRIVATE ${DirectX_LIBRARY})
target_link_libraries(render PRIVATE ${DirectX_D3DX9_LIBRARY})
target_link_libraries(render PRIVATE ${DirectX_DXERR_LIBRARY})
target_link_libraries(render PRIVATE gdiplus.lib)
