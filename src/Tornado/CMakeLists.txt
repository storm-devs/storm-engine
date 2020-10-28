project(tornado)

add_library(${PROJECT_NAME} MODULE
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

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)