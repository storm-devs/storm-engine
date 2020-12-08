project(sailors)

add_library(${PROJECT_NAME} MODULE
	Sailors.cpp
	Sailors.h
	Sailors.rc
	SailorsEditor.cpp
	SailorsEditor.h
	SailorsMain.cpp
	SailorsMenu.cpp
	SailorsMenu.h
	SailorsWayPoints.cpp
	SailorsWayPoints.h
	d3dx8math.h
	resource.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)