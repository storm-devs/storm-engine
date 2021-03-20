project(soundservice)

find_package(Fmod REQUIRED)

add_library(${PROJECT_NAME} STATIC
    DebugEntity.cpp
    DebugEntity.h
    SoundDefines.h
    SoundService.cpp
    SoundService.h
    VSoundService.cpp
    VSoundService.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${Fmod_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    common_h
    ${Fmod_LIBRARY}
)