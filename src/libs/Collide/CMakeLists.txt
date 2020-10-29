project(collide)

add_library(${PROJECT_NAME} MODULE
	coll.cpp
	local_collide.cpp
	vcollide.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)