project(sound)

add_library(${PROJECT_NAME} STATIC
    Sound.cpp
    Sound.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)