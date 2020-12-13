project(seacreatures)

add_library(${PROJECT_NAME} STATIC
	SeaCreaturesMain.cpp
	Sharks.cpp
	Sharks.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)