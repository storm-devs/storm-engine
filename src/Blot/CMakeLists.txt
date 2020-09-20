cmake_minimum_required(VERSION 3.18)

project(blot)

add_library(blot MODULE 
	Blots.cpp
	Blots.h
)
target_include_directories(blot PRIVATE common_h)
target_include_directories(blot PRIVATE SoundService)
target_include_directories(blot PRIVATE extern/directx/include)
