cmake_minimum_required(VERSION 3.18)

project(teleport)

add_library(teleport MODULE 
	teleport.cpp
	teleport.h
)

target_include_directories(teleport PRIVATE ${DirectX_INCLUDE_DIR})