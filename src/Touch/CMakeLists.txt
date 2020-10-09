cmake_minimum_required(VERSION 3.18)

project(touch)

add_library(touch MODULE 
	touch.cpp
	touch.h
)

target_include_directories(touch PRIVATE ${DirectX_INCLUDE_DIR})