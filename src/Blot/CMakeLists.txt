project(blot)

include_directories(
	${DirectX_INCLUDE_DIR}
)

add_library(blot MODULE 
	Blots.cpp
	Blots.h
)
