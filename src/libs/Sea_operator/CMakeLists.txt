project(sea_operator)

add_library(${PROJECT_NAME} STATIC
    SEA_OPERATOR.cpp
    SEA_OPERATOR.h
    TFIFOBuffer.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)