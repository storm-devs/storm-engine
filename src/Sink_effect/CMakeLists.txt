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
	
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)

target_include_directories(sinkeffect PRIVATE ${DirectX_INCLUDE_DIR})