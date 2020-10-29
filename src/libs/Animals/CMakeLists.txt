project(animals)

add_library(${PROJECT_NAME} MODULE
	Animals.cpp
	Animals.h
	AnimalsDefines.cpp
	AnimalsDefines.h
	TButterflies.cpp
	TButterflies.h
	TButterfly.cpp
	TButterfly.h
	TDynamicObject.cpp
	TDynamicObject.h
	TDynamicSystem.cpp
	TDynamicSystem.h
	TFishSchools.cpp
	TFishSchools.h
	TIVBufferManager.cpp
	TIVBufferManager.h
	TSeagulls.cpp
	TSeagulls.h
	TSharks.cpp
	TSharks.h
	${Storm_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)