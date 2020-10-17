project(dialog)

add_library(dialog MODULE 
	dialog.cpp
	dialog.h
)

target_include_directories(dialog PRIVATE ${DirectX_INCLUDE_DIR})
