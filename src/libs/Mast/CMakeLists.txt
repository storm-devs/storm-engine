project(mast)

add_library(${PROJECT_NAME} STATIC
    mast.cpp
    mast.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)