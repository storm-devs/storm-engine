cmake_minimum_required(VERSION 3.18)

project(pcscontrols)

add_library(pcscontrols MODULE 
	ControlTree.cpp
	ControlTree.h
	KeyBuffer.cpp
	KeyBuffer.h
	pcs_controls.cpp
	pcs_controls.h
)
target_include_directories(pcscontrols PRIVATE common_h)
target_include_directories(pcscontrols PRIVATE SoundService)
target_include_directories(pcscontrols PRIVATE extern/directx/include)
