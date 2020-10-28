project(render)

add_library(${PROJECT_NAME} MODULE
	Font.cpp
	Font.h
	Screenshot.cpp
	sdevice.cpp
	sdevice.h
	Technique.cpp
	Technique.h
	texture.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${DirectX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	common_h
	${DirectX_LIBRARY}
	${DirectX_D3DX9_LIBRARY}
	${DirectX_DXERR_LIBRARY}
	gdiplus.lib
)
