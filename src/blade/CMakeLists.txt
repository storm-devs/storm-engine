cmake_minimum_required(VERSION 3.18)

project(blade)

add_library(blade MODULE 
	blade.cpp
	blade.h
)
target_include_directories(blade PRIVATE common_h)
target_include_directories(blade PRIVATE SoundService)
target_include_directories(blade PRIVATE extern/directx/include)
