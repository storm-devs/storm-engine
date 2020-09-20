cmake_minimum_required(VERSION 3.18)

project(collide)

add_library(collide MODULE 
	coll.cpp
	local_collide.cpp
	vcollide.h
)
target_include_directories(collide PRIVATE common_h)
target_include_directories(collide PRIVATE SoundService)
target_include_directories(collide PRIVATE extern/directx/include)
