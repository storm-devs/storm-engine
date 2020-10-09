cmake_minimum_required(VERSION 3.18)

project(soundservice)

find_package(Fmod REQUIRED)

add_library(soundservice MODULE 
	DebugEntity.cpp
	DebugEntity.h
	SoundDefines.h
	SoundService.cpp
	SoundService.h
	VSoundService.cpp
	VSoundService.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(soundservice PRIVATE ${Fmod_INCLUDE_DIR})
target_include_directories(soundservice PRIVATE ${DirectX_INCLUDE_DIR})

target_link_libraries(soundservice PRIVATE ${Fmod_LIBRARY})