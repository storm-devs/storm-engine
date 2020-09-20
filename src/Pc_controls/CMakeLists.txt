cmake_minimum_required(VERSION 3.18)

project(pccontrols)

add_library(pccontrols MODULE 
	device_controls.h
	di_gamepad.cpp
	di_gamepad.h
	di_keyboard.cpp
	di_keyboard.h
	di_mouse.cpp
	di_mouse.h
	pc_controls.cpp
	pc_controls.h
)
target_include_directories(pccontrols PRIVATE common_h)
target_include_directories(pccontrols PRIVATE SoundService)
target_include_directories(pccontrols PRIVATE extern/directx/include)
