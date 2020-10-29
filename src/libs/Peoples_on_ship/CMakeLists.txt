project(peopleonship)

add_library(${PROJECT_NAME} MODULE
	PEOPLES_ON_SHIP.cpp
	PEOPLES_ON_SHIP.h
	TWalkGraph.cpp
	TWalkGraph.h
	
	${Storm_SOURCE_DIR}/libs/Common_h/rands.cpp

	#copied from scripts
	SeaPeople.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)