#pragma once
#include <filesystem>

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

path GetStashPath();
path GetLogsPath();
path GetSaveDataPath();

constexpr char ENGINE_INI_FILE_NAME[] = "engine.ini";
} // namespace fs