project(gameutils)

find_package(DirectX REQUIRED)

add_library(gameutils MODULE 
	GameUtils.cpp
	IslandED.cpp
	IslandED.h
)

target_include_directories(gameutils PRIVATE ${DirectX_INCLUDE_DIR})
