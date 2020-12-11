project(geometry)

add_library(${PROJECT_NAME} STATIC
	collide.cpp
	geom.h
	geom_static.cpp
	geometry_R.h
	geometry.cpp
	Rdf.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)
