cmake_minimum_required(VERSION 3.18)

project(animals)

find_package(DirectX REQUIRED)

include_directories(
	${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

add_library(animals MODULE 
	Animals.cpp
	Animals.h
	AnimalsDefines.cpp
	AnimalsDefines.h
	TButterflies.cpp
	TButterflies.h
	TButterfly.cpp
	TButterfly.h
	TDynamicObject.cpp
	TDynamicObject.h
	TDynamicSystem.cpp
	TDynamicSystem.h
	TFishSchools.cpp
	TFishSchools.h
	TIVBufferManager.cpp
	TIVBufferManager.h
	TSeagulls.cpp
	TSeagulls.h
	TSharks.cpp
	TSharks.h
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

