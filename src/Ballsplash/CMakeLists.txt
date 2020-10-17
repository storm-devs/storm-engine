project(ballsplash)

include_directories(
	#${Storm_SOURCE_DIR}/Common_h
	${DirectX_INCLUDE_DIR}
)

add_library(ballsplash MODULE 
	BallSplash.cpp
	BallSplash.h
	BallSplashDefines.cpp
	BallSplashDefines.h
	TSplash.cpp
	TSplash.h
	${Storm_SOURCE_DIR}/Common_h/rands.cpp
)
