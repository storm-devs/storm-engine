if (WIN32)
set(SYSTEM_DEPS "legacy_stdio_definitions")
endif()

STORM_SETUP(
    TARGET_NAME renderer
    TYPE storm_module
    DEPENDENCIES common core util ${SYSTEM_DEPS}
)