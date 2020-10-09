cmake_minimum_required(VERSION 3.18)

project(locator)

add_library(locator MODULE 
	blast.cpp
	blast.h
	locator.cpp
	locator.h
)

target_include_directories(locator PRIVATE ${DirectX_INCLUDE_DIR})