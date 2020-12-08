project(seaoperator)

add_library(${PROJECT_NAME} MODULE
	SEA_OPERATOR.cpp
	SEA_OPERATOR.h
	TFIFOBuffer.h
	
	${Storm_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)