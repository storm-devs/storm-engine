project(ballsplash)

add_library(${PROJECT_NAME} STATIC
    BallSplash.cpp
    BallSplash.h
    BallSplashDefines.cpp
    BallSplashDefines.h
    TSplash.cpp
    TSplash.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)