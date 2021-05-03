project(window)

set(PUBLIC_SRC
    include/OSWindow.hpp
)

set(PRIVATE_SRC
    src/SDLWindow.cpp
    src/SDLWindow.hpp
)

add_library(${PROJECT_NAME} STATIC ${PUBLIC_SRC} ${PRIVATE_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::sdl2)
