cmake_minimum_required(VERSION 3.18)

project(peopleonship)

add_library(peopleonship MODULE 
	PEOPLES_ON_SHIP.cpp
	PEOPLES_ON_SHIP.h
	TWalkGraph.cpp
	TWalkGraph.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(peopleonship PRIVATE ${DirectX_INCLUDE_DIR})