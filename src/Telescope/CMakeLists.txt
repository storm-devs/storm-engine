cmake_minimum_required(VERSION 3.18)

project(telescope)

add_library(telescope MODULE 
	Telescope.cpp
	Telescope.h
)
target_include_directories(telescope PRIVATE common_h)
target_include_directories(telescope PRIVATE SoundService)
target_include_directories(telescope PRIVATE extern/directx/include)
