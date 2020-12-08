project(ballsplash)

add_library(${PROJECT_NAME} MODULE
	BallSplash.cpp
	BallSplash.h
	BallSplashDefines.cpp
	BallSplashDefines.h
	TSplash.cpp
	TSplash.h
	${Storm_SOURCE_DIR}/libs/Common_h/rands.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)