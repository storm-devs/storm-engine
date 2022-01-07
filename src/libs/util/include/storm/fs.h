#pragma once

#include <filesystem>

#include <ShlObj.h>

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

inline path GetStashPath()
{
    static path path;
    if (path.empty())
    {
        wchar_t *str = nullptr;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, nullptr, &str);
        path = str;
        path = path / "My Games" / "Sea Dogs";
        CoTaskMemFree(str);
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
