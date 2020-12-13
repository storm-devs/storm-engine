project(sink_effect)

add_library(${PROJECT_NAME} STATIC
	SinkEffect.H
	SinkEffect.cpp
	SinkSplashDefines.h
	TFlotsam.cpp
	TFlotsam.h
	TSink.cpp
	TSink.h
	TSinkSplash.cpp
	TSinkSplash.h
	seps.cpp
	seps.h
	
	${STORM_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)