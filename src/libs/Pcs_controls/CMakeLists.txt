project(pcs_controls)

add_library(${PROJECT_NAME} STATIC
    ControlTree.cpp
    ControlTree.h
    KeyBuffer.cpp
    KeyBuffer.h
    pcs_controls.cpp
    pcs_controls.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h input)