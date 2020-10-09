cmake_minimum_required(VERSION 3.18)

project(sea)

add_library(sea MODULE 
	EnvMap.cpp
	SEA.cpp
	SEA.h
	SSE.h
)

target_include_directories(sea PRIVATE ${DirectX_INCLUDE_DIR})

target_link_libraries(sea PRIVATE ${DirectX_D3DX9_LIBRARY})