project(island)

add_library(${PROJECT_NAME} MODULE
	Foam.cpp
	Foam.h
	ISLAND.cpp
	ISLAND.h
	MapZipper.cpp
	ZapSuperGenerator.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)