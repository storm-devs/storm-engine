project(seaoperator)

add_library(seaoperator MODULE 
	SEA_OPERATOR.cpp
	SEA_OPERATOR.h
	TFIFOBuffer.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(seaoperator PRIVATE ${DirectX_INCLUDE_DIR})