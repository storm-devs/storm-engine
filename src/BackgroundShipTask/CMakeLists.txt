project(backgroundshiptask)

add_library(${PROJECT_NAME} MODULE
	BackgroundDate.cpp
	BackgroundDate.h
	BackgroundShipTask.cpp
	BackgroundShipTask.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)