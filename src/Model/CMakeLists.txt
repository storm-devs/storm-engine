project(model)

add_library(model MODULE 
	model.cpp
	modelr.h
	node.cpp
)

target_include_directories(model PRIVATE ${DirectX_INCLUDE_DIR})