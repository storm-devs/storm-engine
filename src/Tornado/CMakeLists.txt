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

target_include_directories(tornado PRIVATE ${DirectX_INCLUDE_DIR})