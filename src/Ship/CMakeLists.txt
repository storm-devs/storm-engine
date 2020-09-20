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
target_include_directories(ship PRIVATE common_h)
target_include_directories(ship PRIVATE SoundService)
target_include_directories(ship PRIVATE extern/directx/include)
