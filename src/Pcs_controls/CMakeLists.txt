project(pcscontrols)

add_library(pcscontrols MODULE 
	ControlTree.cpp
	ControlTree.h
	KeyBuffer.cpp
	KeyBuffer.h
	pcs_controls.cpp
	pcs_controls.h
)

target_include_directories(pcscontrols PRIVATE ${DirectX_INCLUDE_DIR})