# -----------------------------------------------------------------------------
# Find Zlib
# Define:
# Zlib_FOUND
# Zlib_INCLUDE_DIR
# Zlib_LIBRARY
# Zlib_ROOT_DIR

set(Zlib_FOUND false)

find_path(Zlib_INCLUDE_DIR zlib.h "${STORM_EXTERN_DIR}/zlib")
mark_as_advanced(Zlib_INCLUDE_DIR)

if(Zlib_INCLUDE_DIR)
	set(Zlib_ROOT_DIR "${STORM_EXTERN_DIR}/zlib")
endif()

set(Zlib_LIBRARY_PATHS)
if(CMAKE_CL_64)
	set(Zlib_LIBRARY_PATHS "${Zlib_ROOT_DIR}/x64")
else()
	set(Zlib_LIBRARY_PATHS "${Zlib_ROOT_DIR}/x86")
endif()

find_library(Zlib_LIBRARY zlib ${Zlib_LIBRARY_PATHS} NO_DEFAULT_PATH)

if(Zlib_INCLUDE_DIR AND Zlib_LIBRARY)
	set(Zlib_FOUND true)
endif()
mark_as_advanced(Zlib_LIBRARY)

# message("Zlib_FOUND=${Zlib_FOUND}")
# message("Zlib_INCLUDE_DIR=${Zlib_INCLUDE_DIR}")
# message("Zlib_LIBRARY=${Zlib_LIBRARY}")
# message("Zlib_ROOT_DIR=${Zlib_ROOT_DIR}")

