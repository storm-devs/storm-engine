project(ship)

add_library(${PROJECT_NAME} MODULE
	FirePlace.cpp
	FirePlace.h
	ShipLights.cpp
	ShipLights.h
	Track.cpp
	Track.h
	ship.cpp
	ship.h
	ship_utils.cpp

	#copied from scripts
	sound.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)