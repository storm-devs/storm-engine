add_definitions(-DUNICODE -D_UNICODE) # Use Unicode
add_definitions(-DNOMINMAX) # Use std::max()/std::min()
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Disable deprecation of unsafe functions

if (STORM_ENABLE_STEAM)
    add_definitions(-DSTORM_ENABLE_STEAM=1)
endif()

if (MSVC)
    add_compile_options(/W2) # Warning level
    add_compile_options(/WX) # Treats compiler warnings as errors
    add_compile_options(/std:c++latest) # Language Standard Version
    # add_compile_options(/Oi) # Generate intrinsic functions
    # add_compile_options(/Ot) # Favor fast code
    # add_compile_options(/GL) # Whole program optimization
    # add_compile_options(/MT) # Multithread, static version of the run-time library
    # add_compile_options(/MP) # Build with Multiple Processes
    # add_compile_options(/Gy) # Enable Function-Level Linking
    # add_compile_options(/GR) # Enable Run-Time Type Information
    # add_compile_options(/arch:AVX)
endif()

add_subdirectory(apps)
add_subdirectory(libs)