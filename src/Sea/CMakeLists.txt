cmake_minimum_required(VERSION 3.18)

project(sea)

add_library(sea MODULE 
	BumpMap.cpp
	CubeMap.cpp
	EnvMap.cpp
	SEA.cpp
	SEA.h
	SSE.h
	ShipTrail.cpp
	ShipTrail.h
	sea_defines.h
	sea_struct.h
)
target_include_directories(sea PRIVATE common_h)
target_include_directories(sea PRIVATE SoundService)
target_include_directories(sea PRIVATE extern/directx/include)
