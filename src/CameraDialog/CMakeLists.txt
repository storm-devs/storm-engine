cmake_minimum_required(VERSION 3.18)

project(cameradialog)

add_library(cameradialog MODULE 
	CameraDialog.cpp
	CameraDialog.h
)
target_include_directories(cameradialog PRIVATE common_h)
target_include_directories(cameradialog PRIVATE SoundService)
target_include_directories(cameradialog PRIVATE extern/directx/include)
