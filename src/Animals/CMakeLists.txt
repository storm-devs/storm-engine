cmake_minimum_required(VERSION 3.18)

project(animals)

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
)
target_include_directories(animals PRIVATE common_h)
target_include_directories(animals PRIVATE SoundService)
target_include_directories(animals PRIVATE extern/directx/include)
