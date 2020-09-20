cmake_minimum_required(VERSION 3.18)

project(weather)

add_library(weather MODULE 
	Astronomy.cpp
	Astronomy.h
	Lightning.cpp
	Lightning.h
	Planets.cpp
	Rain.cpp
	Rain.h
	SKY.cpp
	SKY.h
	Stars.cpp
	SunGlow.cpp
	SunGlow.h
	WaterFlare.cpp
	WaterFlare.h
	Weather.cpp
	Weather.h
	typedef.h
)
target_include_directories(weather PRIVATE common_h)
target_include_directories(weather PRIVATE SoundService)
target_include_directories(weather PRIVATE extern/directx/include)
