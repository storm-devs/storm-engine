project(seacreatures)

add_library(seacreatures MODULE 
	SeaCreaturesMain.cpp
	Sharks.cpp
	Sharks.h
)

target_include_directories(seacreatures PRIVATE ${DirectX_INCLUDE_DIR})