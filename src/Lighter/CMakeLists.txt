cmake_minimum_required(VERSION 3.18)

project(lighter)

add_library(lighter MODULE 
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
target_include_directories(lighter PRIVATE common_h)
target_include_directories(lighter PRIVATE SoundService)
target_include_directories(lighter PRIVATE extern/directx/include)
