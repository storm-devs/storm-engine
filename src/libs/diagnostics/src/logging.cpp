#include "logging.hpp"

#include "spdlog_sinks/syncable_sink.hpp"
#include "storm/fs.h"

namespace
{

constexpr auto kLogExtension = ".log";

}

namespace storm::logging
{

std::shared_ptr<spdlog::logger> getOrCreateLogger(const std::string &name, const spdlog::level::level_enum level,
                                                  const bool truncate)
{
    auto logger = spdlog::get(name);
    if (logger)
    {
        return logger;
    }

    auto path = fs::GetLogsPath() / name;
    path += kLogExtension;

    logger = spdlog::create<sinks::syncable_sink>(name, path.string(), truncate);
    logger->set_level(level);

    return logger;
}

} // namespace storm::logging