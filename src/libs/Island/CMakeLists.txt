project(island)

add_library(${PROJECT_NAME} STATIC
	Foam.cpp
	Foam.h
	ISLAND.cpp
	ISLAND.h
	MapZipper.cpp
	ZapSuperGenerator.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)