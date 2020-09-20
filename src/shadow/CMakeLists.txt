cmake_minimum_required(VERSION 3.18)

project(shadow)

add_library(shadow MODULE 
	shadow.cpp
	shadow.h
)
target_include_directories(shadow PRIVATE common_h)
target_include_directories(shadow PRIVATE SoundService)
target_include_directories(shadow PRIVATE extern/directx/include)
