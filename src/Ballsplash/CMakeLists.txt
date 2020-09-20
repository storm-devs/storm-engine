cmake_minimum_required(VERSION 3.18)

project(ballsplash)

add_library(ballsplash MODULE 
	BallSplash.cpp
	BallSplash.h
	BallSplashDefines.cpp
	BallSplashDefines.h
	TSplash.cpp
	TSplash.h
)
target_include_directories(ballsplash PRIVATE common_h)
target_include_directories(ballsplash PRIVATE SoundService)
target_include_directories(ballsplash PRIVATE extern/directx/include)
