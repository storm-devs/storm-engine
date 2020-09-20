cmake_minimum_required(VERSION 3.18)

project(gameutils)

add_library(gameutils MODULE 
	GameUtils.cpp
	IslandED.cpp
	IslandED.h
)
target_include_directories(gameutils PRIVATE common_h)
target_include_directories(gameutils PRIVATE SoundService)
target_include_directories(gameutils PRIVATE extern/directx/include)
