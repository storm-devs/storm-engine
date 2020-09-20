cmake_minimum_required(VERSION 3.18)

project(tornado)

add_library(tornado MODULE 
	Debris.cpp
	Debris.h
	NoiseCloud.cpp
	NoiseCloud.h
	Pillar.cpp
	Pillar.h
	Tornado.cpp
	Tornado.h
	TornadoParticles.cpp
	TornadoParticles.h
)
target_include_directories(tornado PRIVATE common_h)
target_include_directories(tornado PRIVATE SoundService)
target_include_directories(tornado PRIVATE extern/directx/include)
