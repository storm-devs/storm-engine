cmake_minimum_required(VERSION 3.18)

project(backgroundshiptask)

add_library(backgroundshiptask MODULE 
	BackgroundDate.cpp
	BackgroundDate.h
	BackgroundShipTask.cpp
	BackgroundShipTask.h
)
target_include_directories(backgroundshiptask PRIVATE common_h)
target_include_directories(backgroundshiptask PRIVATE SoundService)
target_include_directories(backgroundshiptask PRIVATE extern/directx/include)
