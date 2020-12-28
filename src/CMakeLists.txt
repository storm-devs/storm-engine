add_definitions(-DUNICODE -D_UNICODE)

if (MSVC)
    add_compile_options(/W1 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(apps)
add_subdirectory(libs)