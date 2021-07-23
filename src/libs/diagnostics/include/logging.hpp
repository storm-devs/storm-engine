#pragma once
#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

namespace storm::logging
{

std::shared_ptr<spdlog::logger> getOrCreateLogger(const std::string &name,
                                                  spdlog::level::level_enum level = spdlog::level::trace,
                                                  bool truncate = true);
} // namespace storm::logging
