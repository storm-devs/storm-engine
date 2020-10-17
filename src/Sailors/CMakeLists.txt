project(sailors)

add_library(sailors MODULE 
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

target_include_directories(sailors PRIVATE ${DirectX_INCLUDE_DIR})