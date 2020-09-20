cmake_minimum_required(VERSION 3.18)

project(island)

add_library(island MODULE 
	Foam.cpp
	Foam.h
	ISLAND.cpp
	ISLAND.h
	MapZipper.cpp
	ZapSuperGenerator.cpp
)
target_include_directories(island PRIVATE common_h)
target_include_directories(island PRIVATE SoundService)
target_include_directories(island PRIVATE extern/directx/include)
