project(dialog)

add_library(${PROJECT_NAME} MODULE
	dialog.cpp
	dialog.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)