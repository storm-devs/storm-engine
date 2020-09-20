cmake_minimum_required(VERSION 3.18)

project(artifact)

add_library(artifact MODULE 
	Artifact.cpp
	Artifact.h
	bsps.cpp
	bsps.h
)
target_include_directories(artifact PRIVATE common_h)
target_include_directories(artifact PRIVATE SoundService)
target_include_directories(artifact PRIVATE extern/directx/include)
