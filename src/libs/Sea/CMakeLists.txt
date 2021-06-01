project(sea)

add_library(${PROJECT_NAME} STATIC
    EnvMap.cpp
    SEA.cpp
    SEA.h
    sea_defines.h
    sea_struct.h
    SSE.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)