project(input)

set(PUBLIC_SRC
    include/Input.hpp
)

set(PRIVATE_SRC
    src/SDLInput.cpp
    src/SDLInput.hpp
)

add_library(${PROJECT_NAME} STATIC ${PUBLIC_SRC} ${PRIVATE_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::sdl2)
