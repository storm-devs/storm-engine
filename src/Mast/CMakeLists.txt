cmake_minimum_required(VERSION 3.18)

project(mast)

add_library(mast MODULE 
	mast.cpp
	mast.h
)
target_include_directories(mast PRIVATE common_h)
target_include_directories(mast PRIVATE SoundService)
target_include_directories(mast PRIVATE extern/directx/include)
