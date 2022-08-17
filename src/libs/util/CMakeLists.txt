if (NOT WIN32)
	set(SYSTEM_DEPS "libsafec")
endif()

STORM_SETUP(
    TARGET_NAME util
    TYPE library
	DEPENDENCIES ${SYSTEM_DEPS}
    TEST_DEPENDENCIES catch2
)