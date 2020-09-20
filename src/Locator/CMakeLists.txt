cmake_minimum_required(VERSION 3.18)

project(locator)

add_library(locator MODULE 
	blast.cpp
	blast.h
	locator.cpp
	locator.h
)
target_include_directories(locator PRIVATE common_h)
target_include_directories(locator PRIVATE SoundService)
target_include_directories(locator PRIVATE extern/directx/include)
