project(backgroundshiptask)

include_directories(
	#${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

add_library(backgroundshiptask MODULE 
	BackgroundDate.cpp
	BackgroundDate.h
	BackgroundShipTask.cpp
	BackgroundShipTask.h
)
