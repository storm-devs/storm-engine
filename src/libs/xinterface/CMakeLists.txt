STORM_SETUP(
    TARGET_NAME xinterface
    TYPE storm_module
    DEPENDENCIES common ddraw amstrmid util
    TEST_DEPENDENCIES catch2
)