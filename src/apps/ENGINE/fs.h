#pragma once
#include <filesystem>

/* Filesystem proxy */
namespace fs
{
using namespace std::filesystem;

path GetStashPath();
path GetLogsPath();
path GetSaveDataPath();
} // namespace fs