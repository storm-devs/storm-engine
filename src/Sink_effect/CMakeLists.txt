cmake_minimum_required(VERSION 3.18)

project(sinkeffect)

add_library(sinkeffect MODULE 
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
)
target_include_directories(sinkeffect PRIVATE common_h)
target_include_directories(sinkeffect PRIVATE SoundService)
target_include_directories(sinkeffect PRIVATE extern/directx/include)
