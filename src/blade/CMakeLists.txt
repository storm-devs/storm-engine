cmake_minimum_required(VERSION 3.18)

project(blade)

include_directories(
	#${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

add_library(blade MODULE 
	blade.cpp
	blade.h
)
