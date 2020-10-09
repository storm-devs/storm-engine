cmake_minimum_required(VERSION 3.18)

project(ship)

add_library(ship MODULE 
	FirePlace.cpp
	FirePlace.h
	ShipLights.cpp
	ShipLights.h
	Track.cpp
	Track.h
	ship.cpp
	ship.h
	ship_utils.cpp
)

target_include_directories(ship PRIVATE ${DirectX_INCLUDE_DIR})