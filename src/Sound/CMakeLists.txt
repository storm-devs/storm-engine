project(sound)

add_library(sound MODULE 
	Sound.cpp
	Sound.h
)

target_include_directories(sound PRIVATE ${DirectX_INCLUDE_DIR})