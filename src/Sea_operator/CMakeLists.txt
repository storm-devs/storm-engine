cmake_minimum_required(VERSION 3.18)

project(seaoperator)

add_library(seaoperator MODULE 
	SEA_OPERATOR.cpp
	SEA_OPERATOR.h
	TFIFOBuffer.h
)
target_include_directories(seaoperator PRIVATE common_h)
target_include_directories(seaoperator PRIVATE SoundService)
target_include_directories(seaoperator PRIVATE extern/directx/include)
