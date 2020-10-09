# -----------------------------------------------------------------------------
# Find Fmod SDK
# Define:
# Fmod_FOUND
# Fmod_INCLUDE_DIR
# Fmod_LIBRARY
# Fmod_ROOT_DIR

set(Fmod_FOUND false)

if(WIN32) # The only platform it makes sense to check for Fmod SDK

  # We start with Microsoft Fmod SDK (August 2008) 9.24.1400
  # Example of path is C:\apps_x86\Microsoft Fmod SDK (August 2008)\Include
  find_path(Fmod_INCLUDE_DIR fmod.h
    "${Storm_EXTERN_DIR}/fmod/api/inc"
    )
  mark_as_advanced(Fmod_INCLUDE_DIR)
  if(Fmod_INCLUDE_DIR)
    set(Fmod_ROOT_DIR "${Storm_EXTERN_DIR}/fmod")
  endif()

  set(Fmod_LIBRARY_PATHS)
  if(CMAKE_CL_64)
    set(Fmod_LIBRARY_PATHS
      "${Fmod_ROOT_DIR}/api/lib/x64")
  else()
    set(Fmod_LIBRARY_PATHS
      "${Fmod_ROOT_DIR}/api/lib/x86")
  endif()

  find_library(Fmod_LIBRARY fmod_vc ${Fmod_LIBRARY_PATHS} NO_DEFAULT_PATH)
  if(Fmod_INCLUDE_DIR AND Fmod_LIBRARY)
    set(Fmod_FOUND true)
  endif()
  mark_as_advanced(Fmod_LIBRARY)

  # message("Fmod_FOUND=${Fmod_FOUND}")
  # message("Fmod_INCLUDE_DIR=${Fmod_INCLUDE_DIR}")
  # message("Fmod_LIBRARY=${Fmod_LIBRARY}")
  # message("Fmod_ROOT_DIR=${Fmod_ROOT_DIR}")

endif()
