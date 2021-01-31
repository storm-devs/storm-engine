project(blade)

add_library(${PROJECT_NAME} STATIC
	blade.cpp
	blade.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)