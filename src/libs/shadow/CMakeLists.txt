project(shadow)

add_library(${PROJECT_NAME} MODULE
	shadow.cpp
	shadow.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)