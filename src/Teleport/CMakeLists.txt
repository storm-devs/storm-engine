cmake_minimum_required(VERSION 3.18)

project(teleport)

add_library(teleport MODULE 
	teleport.cpp
	teleport.h
)
target_include_directories(teleport PRIVATE common_h)
target_include_directories(teleport PRIVATE SoundService)
target_include_directories(teleport PRIVATE extern/directx/include)
