project(blot)

add_library(${PROJECT_NAME} STATIC
	Blots.cpp
	Blots.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)