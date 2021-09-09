if (STORM_ENABLE_STEAM)
    set(steamworks steamworks)
endif()

STORM_SETUP(
    TARGET_NAME steam_api
    TYPE storm_module
    DEPENDENCIES common ${steamworks}
)