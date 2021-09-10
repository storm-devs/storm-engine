STORM_SETUP(
    TARGET_NAME engine
    TYPE executable
    DEPENDENCIES     
	
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
    sentry-native
    sdl2
    zlib

    # system
    comctl32
    dbghelp
    winhttp
)