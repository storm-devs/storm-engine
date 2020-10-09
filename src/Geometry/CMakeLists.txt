cmake_minimum_required(VERSION 3.18)

project(geometry)

add_library(geometry MODULE 
	geometry_R.h
	geometry.h
	geometry.cpp
	geom_static.cpp
	geom.h
	collide.cpp
	Rdf.h
)

target_include_directories(geometry PRIVATE ${DirectX_INCLUDE_DIR})
