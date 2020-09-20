cmake_minimum_required(VERSION 3.18)

project(setup)

add_library(setup MODULE 
	Resource.h
	Setup.cpp
	Setup.h
	SetupDlg.cpp
	SetupDlg.h
	StdAfx.cpp
	StdAfx.h
)
target_include_directories(setup PRIVATE common_h)
target_include_directories(setup PRIVATE SoundService)
target_include_directories(setup PRIVATE extern/directx/include)
