#pragma once

#include <filesystem>

#ifdef _WIN32 // SHGetKnownFolderPath
#include <ShlObj.h>
#else
#include <SDL2/SDL.h>
#endif

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

inline path GetStashPath()
{
    static path path;
    if (path.empty())
    {
#ifdef _WIN32 // SHGetKnownFolderPath
        wchar_t *str = nullptr;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, nullptr, &str);
        path = str;
        path = path / "My Games" / "Sea Dogs";
        CoTaskMemFree(str);
#else
        char *pref_path = nullptr;
        pref_path = SDL_GetPrefPath("Akella", "Sea Dogs");
        path = pref_path;
#endif
    }
    return path;
}

inline path GetLogsPath()
{
    return GetStashPath() / "Logs";
}

inline path GetSaveDataPath()
{
    return GetStashPath() / "SaveData";
}

inline path GetScreenshotsPath()
{
    return GetStashPath() / "Screenshots";
}

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs
