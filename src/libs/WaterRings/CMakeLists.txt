project(waterrings)

add_library(${PROJECT_NAME} STATIC
    WaterRings.cpp
    WaterRings.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)