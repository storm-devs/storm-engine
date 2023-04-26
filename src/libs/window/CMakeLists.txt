if (STORM_MESA_NINE)
    add_definitions(-DSTORM_MESA_NINE=1)
endif()

STORM_SETUP(
    TARGET_NAME window
    TYPE library
    DEPENDENCIES ${SDL2_LIBRARIES}
)
