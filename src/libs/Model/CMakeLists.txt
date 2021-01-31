project(model)

add_library(${PROJECT_NAME} STATIC
	model.cpp
	modelr.h
	node.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)