project(sea)

add_library(${PROJECT_NAME} MODULE
	EnvMap.cpp
	SEA.cpp
	SEA.h
	SSE.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)