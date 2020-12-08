project(waterrings)

add_library(${PROJECT_NAME} MODULE
	TIVBufferManager.cpp
	TIVBufferManager.h
	WaterRings.cpp
	WaterRings.h
	
	${Storm_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)