project(sinkeffect)

add_library(${PROJECT_NAME} MODULE
	SinkEffect.H
	SinkEffect.cpp
	SinkSplashDefines.h
	TFlotsam.cpp
	TFlotsam.h
	TIVBufferManager.cpp
	TIVBufferManager.h
	TSink.cpp
	TSink.h
	TSinkSplash.cpp
	TSinkSplash.h
	seps.cpp
	seps.h
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)