add_definitions(-DUNICODE -D_UNICODE)

if (MSVC)
    add_compile_options(/W1 /WX /std:c++latest)
    # add_compile_options(/O2 /Oi /Ot /GL /MT /DNDEBUG /Gy /GR /arch:AVX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(apps)
add_subdirectory(libs)