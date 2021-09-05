#pragma once
#include <filesystem>
#include <string>

#include <spdlog/logger.h>

namespace storm::logging
{

using logger_ptr = std::shared_ptr<spdlog::logger>;

logger_ptr getOrCreateLogger(const std::string &name,
                                                  spdlog::level::level_enum level = spdlog::level::trace,
                                                  bool truncate = true);
} // namespace storm::logging
