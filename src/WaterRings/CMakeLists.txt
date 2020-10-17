project(waterrings)

add_library(waterrings MODULE 
	TIVBufferManager.cpp
	TIVBufferManager.h
	WaterRings.cpp
	WaterRings.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(waterrings PRIVATE ${DirectX_INCLUDE_DIR})