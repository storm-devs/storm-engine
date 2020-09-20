cmake_minimum_required(VERSION 3.18)

project(seacreatures)

add_library(seacreatures MODULE 
	SeaCreaturesMain.cpp
	Sharks.cpp
	Sharks.h
)
target_include_directories(seacreatures PRIVATE common_h)
target_include_directories(seacreatures PRIVATE SoundService)
target_include_directories(seacreatures PRIVATE extern/directx/include)
