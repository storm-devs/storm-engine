project(touch)

add_library(${PROJECT_NAME} MODULE
	touch.cpp
	touch.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)