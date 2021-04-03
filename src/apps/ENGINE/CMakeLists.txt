project(engine)

find_package(Fmod REQUIRED) # required to set ${Fmod_LIBRARY_PATHS}

add_executable(${PROJECT_NAME} WIN32
    compiler.cpp
    compiler.h
    Core.cpp
    data.cpp
    data.h
    dstring_codec.h
    expression.cpp
    file_service.cpp
    file_service.h
    fs.cpp
    fs.h
    ifs.cpp
    ifs.h
    internal_functions.cpp
    Main.cpp
    message_script.h
    messages.h
    program.cpp
    program.h
    s_classtab.cpp
    s_classtab.h
    s_compress.cpp
    s_compress.h
    s_dbg_breaktable.cpp
    s_dbg_breaktable.h
    s_dbg_sourceview.cpp
    s_dbg_sourceview.h
    s_dbg_staticlist.h
    s_dbg_watcherlist.cpp
    s_dbg_watcherlist.h
    s_debug.cpp
    s_debug.h
    s_deftab.cpp
    s_deftab.h
    s_eventmsg.h
    s_eventtab.cpp
    s_eventtab.h
    s_functab.cpp
    s_functab.h
    s_postevents.h
    s_stack.cpp
    s_stack.h
    s_tstack.cpp
    s_tstack.h
    s_vartab.cpp
    s_vartab.h
    services_list.cpp
    services_list.h
    sstring.h
    stats.h
    string_codec.h
    strings_list.cpp
    strings_list.h
    tclass_list.h
    timer.h
    tm_list.cpp
    tm_list.h
    token.cpp
    token.h
    VmaInit.h

    ${STORM_RESOURCE_DIR}/engine/resource.rc
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${STORM_RESOURCE_DIR}/engine
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    common_h
    util
    
    animals
    animation
    ballsplash
    battleinterface
    blade
    blot
    collide
    dialog
    geometry
    island
    lighter
    location
    locator
    mast
    model
    particles
    pcs_controls
    renderer
    rigging
    sailors
    script_library
    sea
    sea_ai
    sea_cameras
    sea_operator
    seacreatures
    seafoam
    shadow
    ship
    sink_effect
    sound
    soundservice
    steamapi
    teleport
    tornado
    touch
    waterrings
    weather
    worldmap
    xinterface

    CONAN_PKG::zlib
    comctl32.lib
    dbghelp.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Fmod_LIBRARY_PATHS}/fmod.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${STORM_SOURCE_DIR}/techniques"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resource/techniques"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${STORM_SOURCE_DIR}/shared"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resource/shared"
)