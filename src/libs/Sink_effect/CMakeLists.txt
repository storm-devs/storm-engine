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
)

target_include_directories(${PROJECT_NAME} PUBLIC ${DirectX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)