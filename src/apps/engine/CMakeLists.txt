# this is something I really don't like but it seems there's not better way to ensure mimalloc is linked before c-runtime
include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
if (WIN32)
  set(MIMALLOC_DEP "$<IF:$<CONFIG:Release>,${CONAN_LIB_DIRS_MIMALLOC_RELEASE}/mimalloc.lib,${CONAN_LIB_DIRS_MIMALLOC_DEBUG}/mimalloc-debug.lib>")
else()
  set(MIMALLOC_DEP "$<IF:$<CONFIG:Release>,${CONAN_LIB_DIRS_MIMALLOC_RELEASE}/libmimalloc.so,${CONAN_LIB_DIRS_MIMALLOC_DEBUG}/libmimalloc-debug.so>")
endif()

STORM_SETUP(
    TARGET_NAME engine
    TYPE executable
    DEPENDENCIES

    ${MIMALLOC_DEP}

    # internal
    common
    diagnostics
    util
    window

    # VM
    animals
    animation
    ball_splash
    battle_interface
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
    sea_creatures
    sea_foam
    shadow
    ship
    sink_effect
    sound
    sound_service
    steam_api
    teleport
    tornado
    touch
    water_rings
    weather
    worldmap
    xinterface

    # external
    fast_float
    mimalloc
    sentry-native
    sdl
    zlib

    # system
    comctl32
    dbghelp
    winhttp
)