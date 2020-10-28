project(soundservice)

find_package(Fmod REQUIRED)

add_library(${PROJECT_NAME} MODULE
	DebugEntity.cpp
	DebugEntity.h
	SoundDefines.h
	SoundService.cpp
	SoundService.h
	VSoundService.cpp
	VSoundService.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Fmod_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
	common_h
	${Fmod_LIBRARY}
)