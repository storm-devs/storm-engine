project(teleport)

add_library(${PROJECT_NAME} STATIC
    teleport.cpp
    teleport.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)