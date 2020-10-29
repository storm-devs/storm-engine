project(locator)

add_library(${PROJECT_NAME} MODULE
	blast.cpp
	blast.h
	locator.cpp
	locator.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)