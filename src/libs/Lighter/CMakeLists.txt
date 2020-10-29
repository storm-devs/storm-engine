project(lighter)

add_library(${PROJECT_NAME} MODULE
	LGeometry.cpp
	LGeometry.h
	LObject.cpp
	LObject.h
	LTypes.h
	LightProcessor.cpp
	LightProcessor.h
	Lighter.cpp
	Lighter.h
	LighterMain.cpp
	Lights.cpp
	Lights.h
	OctTree.cpp
	OctTree.h
	Window.cpp
	Window.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)