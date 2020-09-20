cmake_minimum_required(VERSION 3.18)

project(model)

add_library(model MODULE 
	model.cpp
	modelr.h
	node.cpp
)
target_include_directories(model PRIVATE common_h)
target_include_directories(model PRIVATE SoundService)
target_include_directories(model PRIVATE extern/directx/include)
