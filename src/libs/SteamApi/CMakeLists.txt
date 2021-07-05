project(steamapi)

set(PRIVATE_SRC
    src/SteamApi.cpp
    src/SteamApiScriptLib.cpp
)

if(STORM_ENABLE_STEAM)
    set(PRIVATE_SRC ${PRIVATE_SRC}
        src/achievements.hpp
        src/achievements.cpp
    )

    add_definitions(-DSTORM_ENABLE_STEAM)
    find_package(SteamSdk REQUIRED)
endif()

add_library(${PROJECT_NAME} STATIC ${PRIVATE_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SteamSdk_INCLUDE_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h ${SteamSdk_LIBRARY})