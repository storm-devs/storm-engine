if (WIN32)
set(SYSTEM_DEPS "legacy_stdio_definitions")
else()
set(SYSTEM_DEPS "${DXVK_NATIVE_D3D9_LIB}")
endif()

STORM_SETUP(
    TARGET_NAME renderer
    TYPE storm_module
    DEPENDENCIES core directx util ${SYSTEM_DEPS}
)