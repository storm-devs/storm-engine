project(shadow)

add_library(shadow MODULE 
	shadow.cpp
	shadow.h
)

target_include_directories(shadow PRIVATE ${DirectX_INCLUDE_DIR})