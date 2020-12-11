project(lighter)

add_library(${PROJECT_NAME} STATIC
	LGeometry.cpp
	LGeometry.h
	Lighter.cpp
	Lighter.h
	LighterLights.cpp
	LighterLights.h
	LighterMain.cpp
	LightProcessor.cpp
	LightProcessor.h
	LObject.cpp
	LObject.h
	LTypes.h
	OctTree.cpp
	OctTree.h
	Window.cpp
	Window.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)