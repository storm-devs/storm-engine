project(sound)

add_library(${PROJECT_NAME} MODULE
	Sound.cpp
	Sound.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)