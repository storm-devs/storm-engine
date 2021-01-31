project(sailors)

add_library(${PROJECT_NAME} STATIC
	Sailors.cpp
	Sailors.h
	SailorsEditor.cpp
	SailorsEditor.h
	SailorsMenu.cpp
	SailorsMenu.h
	SailorsWayPoints.cpp
	SailorsWayPoints.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)