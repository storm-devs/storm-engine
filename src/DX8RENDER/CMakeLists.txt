cmake_minimum_required(VERSION 3.18)

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
target_include_directories(render PRIVATE common_h)
target_include_directories(render PRIVATE SoundService)
target_include_directories(render PRIVATE extern/directx/include)
