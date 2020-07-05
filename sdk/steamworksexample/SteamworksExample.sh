#!/bin/bash
#
# This is a script which runs the SteamworksExample in the Steam runtime

# The program location
TOP=$(cd "${0%/*}" && echo ${PWD})

PROGRAM="${TOP}/SteamworksExampleLinux"

# Steam sets this environment variable when launching games
if [ "$STEAM_RUNTIME" = "" ]; then
    export STEAM_RUNTIME="${TOP}/../../tools/linux/runtime/i386"
    export LD_LIBRARY_PATH="${STEAM_RUNTIME}/lib/i386-linux-gnu:${STEAM_RUNTIME}/lib:${STEAM_RUNTIME}/usr/lib/i386-linux-gnu:${STEAM_RUNTIME}/usr/lib:${LD_LIBRARY_PATH}"
fi

# Add the current directory to the library path to pick up libsteam_api.so
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:."

exec "${PROGRAM}" "$@"

# vi: ts=4 sw=4 expandtab
