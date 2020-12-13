project(locator)

add_library(${PROJECT_NAME} STATIC
	blast.cpp
	blast.h
	locator.cpp
	locator.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)