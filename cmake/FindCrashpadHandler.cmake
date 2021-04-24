# Find crashpad_handler.exe

set(CrashpadHandler_FOUND false)

find_path(CrashpadHandler_ROOT_DIR crashpad_handler.exe "${STORM_EXTERN_DIR}/crashpad")
mark_as_advanced(CrashpadHandler_ROOT_DIR)

set(CrashpadHandler_PATH "${STORM_EXTERN_DIR}/crashpad/crashpad_handler.exe")

if(CrashpadHandler_ROOT_DIR)
    set(CrashpadHandler_FOUND true)
endif()