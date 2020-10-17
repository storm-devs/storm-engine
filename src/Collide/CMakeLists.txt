project(collide)

include_directories(
	${DirectX_INCLUDE_DIR}
)

add_library(collide MODULE 
	coll.cpp
	local_collide.cpp
	vcollide.h
)
