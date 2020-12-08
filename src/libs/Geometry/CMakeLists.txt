project(geometry)

add_library(${PROJECT_NAME} MODULE
	geometry_R.h
	geometry.cpp
	geom_static.cpp
	geom.h
	collide.cpp
	Rdf.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)
