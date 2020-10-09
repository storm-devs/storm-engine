# -----------------------------------------------------------------------------
# Find SteamSdk SDK
# Define:
# SteamSdk_FOUND
# SteamSdk_INCLUDE_DIR
# SteamSdk_LIBRARY
# SteamSdk_ROOT_DIR

set(SteamSdk_FOUND false)

if(WIN32) # The only platform it makes sense to check for SteamSdk SDK

  # We start with Microsoft SteamSdk SDK (August 2008) 9.24.1400
  # Example of path is C:\apps_x86\Microsoft SteamSdk SDK (August 2008)\Include
  find_path(SteamSdk_INCLUDE_DIR steam_api.h
    "${Storm_EXTERN_DIR}/steam/public/steam"
    )
  mark_as_advanced(SteamSdk_INCLUDE_DIR)
  if(SteamSdk_INCLUDE_DIR)
    set(SteamSdk_ROOT_DIR "${Storm_EXTERN_DIR}/steam")
  endif()

  set(SteamSdk_LIBRARY_PATHS)
  if(CMAKE_CL_64)
    set(SteamSdk_LIBRARY_PATHS
      "${SteamSdk_ROOT_DIR}/redistributable_bin/win64")
  else()
    set(SteamSdk_LIBRARY_PATHS
      "${SteamSdk_ROOT_DIR}/redistributable_bin")
  endif()

  find_library(SteamSdk_LIBRARY steam_api ${SteamSdk_LIBRARY_PATHS} NO_DEFAULT_PATH)
  if(SteamSdk_INCLUDE_DIR AND SteamSdk_LIBRARY)
    set(SteamSdk_FOUND true)
  endif()
  mark_as_advanced(SteamSdk_LIBRARY)

  # message("SteamSdk_FOUND=${SteamSdk_FOUND}")
  # message("SteamSdk_INCLUDE_DIR=${SteamSdk_INCLUDE_DIR}")
  # message("SteamSdk_LIBRARY=${SteamSdk_LIBRARY}")
  # message("SteamSdk_ROOT_DIR=${SteamSdk_ROOT_DIR}")

endif()
