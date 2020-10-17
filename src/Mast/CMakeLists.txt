project(mast)

add_library(mast MODULE 
	mast.cpp
	mast.h
)

target_include_directories(mast PRIVATE ${DirectX_INCLUDE_DIR})