cmake_minimum_required(VERSION 3.18)

project(sound)

add_library(sound MODULE 
	Sound.cpp
	Sound.h
)
target_include_directories(sound PRIVATE common_h)
target_include_directories(sound PRIVATE SoundService)
target_include_directories(sound PRIVATE extern/directx/include)
