#pragma once
#include <filesystem>
#include <string>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "fs.h"

namespace storm::logging
{
inline auto getOrCreateLogger(const std::string &name, const spdlog::level::level_enum level = spdlog::level::trace,
                              const bool truncate = true)
{
    auto logger = spdlog::get(name);
    if (logger)
    {
        return logger;
    }
    auto path = fs::GetLogsPath() / name;
    path.replace_extension("log");
    logger = spdlog::basic_logger_st(name, path.string(), truncate);
    logger->set_level(level);

    return logger;
}
} // namespace storm::logging
