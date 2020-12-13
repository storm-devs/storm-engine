project(waterrings)

add_library(${PROJECT_NAME} STATIC
	WaterRings.cpp
	WaterRings.h
	
	${STORM_SOURCE_DIR}/libs/common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)