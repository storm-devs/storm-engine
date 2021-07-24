STORM_SETUP(
    TARGET_NAME diagnostics
    TYPE library
    DEPENDENCIES 
        common util
        spdlog sentry-native
)