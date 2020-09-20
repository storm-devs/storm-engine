cmake_minimum_required(VERSION 3.18)

project(peopleonship)

add_library(peopleonship MODULE 
	PEOPLES_ON_SHIP.cpp
	PEOPLES_ON_SHIP.h
	TWalkGraph.cpp
	TWalkGraph.h
)
target_include_directories(peopleonship PRIVATE common_h)
target_include_directories(peopleonship PRIVATE SoundService)
target_include_directories(peopleonship PRIVATE extern/directx/include)
