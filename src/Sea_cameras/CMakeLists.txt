cmake_minimum_required(VERSION 3.18)

project(seacameras)

add_library(seacameras MODULE 
	Common_Camera.h
	DeckCamera.cpp
	DeckCamera.h
	FreeCamera.cpp
	FreeCamera.h
	SEA_CAMERAS.cpp
	SEA_CAMERAS.h
	ShipCamera.cpp
	ShipCamera.h
)
target_include_directories(seacameras PRIVATE common_h)
target_include_directories(seacameras PRIVATE SoundService)
target_include_directories(seacameras PRIVATE extern/directx/include)
