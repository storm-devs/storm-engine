project(seafoam)

add_library(${PROJECT_NAME} MODULE
	Seafoam.H
	Seafoam.cpp
	SeafoamDefines.cpp
	SeafoamDefines.h
	TCarcass.cpp
	TCarcass.h
	TIVBufferManager.cpp
	TIVBufferManager.h
	seafoam_ps.cpp
	seafoam_ps.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)