cmake_minimum_required(VERSION 3.18)

project(waterrings)

add_library(waterrings MODULE 
	TIVBufferManager.cpp
	TIVBufferManager.h
	WaterRings.cpp
	WaterRings.h
)
target_include_directories(waterrings PRIVATE common_h)
target_include_directories(waterrings PRIVATE SoundService)
target_include_directories(waterrings PRIVATE extern/directx/include)
