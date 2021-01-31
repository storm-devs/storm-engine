project(sea_operator)

add_library(${PROJECT_NAME} STATIC
	SEA_OPERATOR.cpp
	SEA_OPERATOR.h
	TFIFOBuffer.h
	
	${STORM_SOURCE_DIR}/libs/common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)