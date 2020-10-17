project(seafoam)

add_library(seafoam MODULE 
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

target_include_directories(seafoam PRIVATE ${DirectX_INCLUDE_DIR})