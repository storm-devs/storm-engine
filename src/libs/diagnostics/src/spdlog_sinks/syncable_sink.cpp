#include "syncable_sink.hpp"

#include <cstdio>

#ifdef SPDLOG_WCHAR_FILENAMES
static_assert(_UNICODE);
#include <tchar.h>
#else
#include <cstdlib>
#define _T(x) x
#define _tfopen fopen
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <io.h>
#include <fileapi.h>
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
    FILE *file = _tfopen(file_helper_.filename().c_str(), _T("r"));
#ifdef _WIN32
    FlushFileBuffers(reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file))));
#else
    fsync(fileno(file));
#endif
}
