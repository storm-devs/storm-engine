project(artifact)

add_library(${PROJECT_NAME} MODULE
	Artifact.cpp
	Artifact.h
	bsps.cpp
	bsps.h
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)