cmake_minimum_required(VERSION 3.18)

project(scriptlibrary)

add_library(scriptlibrary MODULE 
	script_libriary_test.cpp
	script_libriary_test.h
	vano_files.cpp
	vano_files.h
)
target_include_directories(scriptlibrary PRIVATE common_h)
target_include_directories(scriptlibrary PRIVATE SoundService)
target_include_directories(scriptlibrary PRIVATE extern/directx/include)
