project(gameutils)

add_library(${PROJECT_NAME} MODULE
	GameUtils.cpp
	IslandED.cpp
	IslandED.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)