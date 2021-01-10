project(rigging)

add_library(${PROJECT_NAME} STATIC
    Flag.cpp
    Flag.h
    Rope.cpp
    Rope.h
    SAILONE.cpp
    SAILONE.h
    Vant.cpp
    Vant.h
    sail.cpp
    sail.h
    script_func.cpp
    script_func.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)