#pragma once

#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/sink.h>

namespace storm::logging::sinks
{

class syncable_sink final : public spdlog::sinks::sink
{
  public:
    syncable_sink(const spdlog::filename_t &filename, bool truncate);

    syncable_sink(const syncable_sink &) = delete;
    syncable_sink(syncable_sink &&) = delete;

    syncable_sink &operator=(const syncable_sink &) = delete;
    syncable_sink &operator=(syncable_sink &&) = delete;

    void log(const spdlog::details::log_msg &msg) override;
    void flush() override;
    void set_pattern(const std::string &pattern) override;
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

    void sync() const;

  protected:
    std::unique_ptr<spdlog::formatter> formatter_;
    spdlog::details::file_helper file_helper_;
};

} // namespace storm::spdlog_sinks
