project(weather)

add_library(${PROJECT_NAME} MODULE
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

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)