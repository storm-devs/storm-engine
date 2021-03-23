project(animals)

add_library(${PROJECT_NAME} STATIC
    Animals.cpp
    Animals.h
    AnimalsDefines.cpp
    AnimalsDefines.h
    TButterflies.cpp
    TButterflies.h
    TButterfly.cpp
    TButterfly.h
    TDynamicObject.cpp
    TDynamicObject.h
    TDynamicSystem.cpp
    TDynamicSystem.h
    TFishSchools.cpp
    TFishSchools.h
    TSeagulls.cpp
    TSeagulls.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE common_h)