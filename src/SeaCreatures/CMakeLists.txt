cmake_minimum_required(VERSION 3.18)

project(seacreatures)

add_library(seacreatures MODULE 
	SeaCreaturesMain.cpp
	Sharks.cpp
	Sharks.h
)

target_include_directories(seacreatures PRIVATE ${DirectX_INCLUDE_DIR})