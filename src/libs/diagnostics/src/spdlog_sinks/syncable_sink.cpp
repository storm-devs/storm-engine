#include "syncable_sink.hpp"

#include <cstdio>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <format>

#include <windows.h>

#include <fileapi.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>

storm::logging::sinks::syncable_sink::syncable_sink(const spdlog::filename_t &filename, bool truncate)
{
    file_helper_.open(filename, truncate);
}

void storm::logging::sinks::syncable_sink::log(const spdlog::details::log_msg &msg)
{
    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);
    file_helper_.write(formatted);
}

void storm::logging::sinks::syncable_sink::flush()
{
    file_helper_.flush();
}

void storm::logging::sinks::syncable_sink::set_pattern(const std::string &pattern)
{
    formatter_ = spdlog::details::make_unique<spdlog::pattern_formatter>(pattern);
}

void storm::logging::sinks::syncable_sink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
    formatter_ = std::move(sink_formatter);
}

void storm::logging::sinks::syncable_sink::sync() const
{
#ifdef _WIN32
    const auto success = FlushFileBuffers(reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file_helper_.getfd()))));
    if (!success)
    {
        OutputDebugStringA(std::format("failed to flush:{} ({})", file_helper_.filename(), GetLastError()).c_str());
    }
#else
    fsync(fileno(file_helper_.getfd()));
#endif
}

void storm::logging::sinks::syncable_sink::terminate_immediately()
{
    sync();
    file_helper_.close();
}
