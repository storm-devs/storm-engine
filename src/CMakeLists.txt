add_definitions(-DUNICODE -D_UNICODE) # Use Unicode
add_definitions(-DNOMINMAX) # Use std::max()/std::min()
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Disable deprecation of unsafe functions

if (STORM_ENABLE_STEAM)
    add_definitions(-DSTORM_ENABLE_STEAM=1)
endif()

if(STORM_ENABLE_CRASH_REPORTS)
	add_definitions(-DSTORM_ENABLE_CRASH_REPORTS=1)
endif()

if (MSVC)
    # Always generate PDBs
    add_compile_options(/Zi)
    add_link_options(/DEBUG)

    add_compile_options(/W2) # Warning level
    add_compile_options(/WX) # Treats compiler warnings as errors
    add_compile_options(/std:c++latest) # Language Standard Version
    add_compile_options(/MP) # Build with Multiple Processes

    add_compile_options("$<$<CONFIG:Release>:/Gy>") # Enable Function-Level Linking
    add_compile_options("$<$<CONFIG:Release>:/Oi>") # Generate intrinsic functions
    add_compile_options("$<$<CONFIG:Release>:/Ot>") # Favor fast code, should be implied by /O2 though
    add_compile_options("$<$<CONFIG:Release>:/GL>") # Whole program optimization

    add_link_options("$<$<CONFIG:Release>:/OPT:REF>") # Eliminate functions and data that are never referenced, needs to be explicitly specified with /DEBUG
    add_link_options("$<$<CONFIG:Release>:/OPT:ICF>") # Perform identical COMDAT folding, needs to be explicitly specified with /DEBUG
    add_link_options("$<$<CONFIG:Release>:/LTCG>") # Enable link-time code generation
else()
    add_compile_options(-std=c++20)

    # Verbose output
    add_compile_options(-v)
    add_link_options(-v)

    # Always generate debug information
    add_compile_options(-g)
    add_link_options(-g)
endif()

# this is crucial to include libs first
add_subdirectory(libs)
add_subdirectory(apps)
