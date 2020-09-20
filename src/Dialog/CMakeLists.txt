cmake_minimum_required(VERSION 3.18)

project(dialog)

add_library(dialog MODULE 
	dialog.cpp
	dialog.h
)
target_include_directories(dialog PRIVATE common_h)
target_include_directories(dialog PRIVATE SoundService)
target_include_directories(dialog PRIVATE extern/directx/include)
