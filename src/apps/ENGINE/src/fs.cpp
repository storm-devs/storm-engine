#include "fs.h"
#include <ShlObj.h>

namespace fs
{
path GetStashPath()
{
    static path path;
    if (path.empty())
    {
        wchar_t *str = nullptr;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, nullptr, &str);
        path = str;
        const auto u8str = path.u8string();
        const std::string toUtf8(u8str.begin(), u8str.end());
        path = fs::path(toUtf8) / "My Games" / "Sea Dogs";
        CoTaskMemFree(str);
    }
    return path;
}

path GetLogsPath()
{
    return GetStashPath() / "Logs";
}

path GetSaveDataPath()
{
    return GetStashPath() / "SaveData";
}
} // namespace fs
