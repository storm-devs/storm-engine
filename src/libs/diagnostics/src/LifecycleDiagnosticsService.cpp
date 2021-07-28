#include "LifecycleDiagnosticsService.hpp"

#include <condition_variable>
#include <cstdio>
#include <mutex>

#include <spdlog/spdlog.h>

#include "storm/fs.h"
#include "vfile_service.h"
#include "spdlog_sinks/syncable_sink.hpp"

#ifdef _UNICODE
#include <tchar.h>
#define sentry_options_set_database_path sentry_options_set_database_pathw
#define sentry_options_set_handler_path sentry_options_set_handler_pathw
#define sentry_options_add_attachment sentry_options_add_attachmentw
#else
#include <cstdlib>
#define _T(x) x
#define _tsystem std::system
#endif

namespace storm::diag
{

class LoggingService final
{
  public:
    ~LoggingService()
    {
        terminate();
    }

    void initialize()
    {
        if (terminate_)
        {
            terminate_ = false;

            static auto terminate_handler = std::get_terminate();
            std::set_terminate([] {
                spdlog::shutdown();
                terminate_handler();
            });

            create_directories(fs::GetLogsPath());

            std::thread worker{[this] { loggingThread(); }};
            worker.detach();
        }
    }

    void terminate()
    {
        using namespace std::chrono_literals;

        if (!terminate_)
        {
            terminate_ = true;
            {
                std::lock_guard lock(mtx_);
                flushRequested_ = true;
            }
            cv_.notify_one();

            std::unique_lock lock(mtx_);
            cv_.wait_for(lock, 5s, [this] { return !flushRequested_; });
            spdlog::shutdown();
        }
    }

    void flushAsync()
    {
        {
            std::lock_guard lock(mtx_);
            flushRequested_ = true;
        }
        cv_.notify_one();
    }

  private:
    std::mutex mtx_;
    std::condition_variable cv_;
    bool flushRequested_{false};
    std::atomic_bool terminate_{true};

    void flushAll(bool doSync) const
    {
        spdlog::apply_all([doSync](std::shared_ptr<spdlog::logger> l) {
            l->flush();

            if (doSync)
            {
                for (auto &sink : l->sinks())
                {
                    if (const auto syncable_sink = std::dynamic_pointer_cast<logging::sinks::syncable_sink>(sink))
                    {
                        syncable_sink->sync();
                    }
                }
            }
        });
    }

    void loggingThread()
    {
        while (!terminate_)
        {
            std::unique_lock lock(mtx_);
            cv_.wait(lock, [this] { return flushRequested_; });

            flushAll(terminate_.load());

            flushRequested_ = false;
            cv_.notify_one();
        }
    }
};

LifecycleDiagnosticsService::LifecycleDiagnosticsService() : loggingService_(std::make_unique<LoggingService>())
{
}

LifecycleDiagnosticsService::~LifecycleDiagnosticsService()
{
    terminate();
}

LifecycleDiagnosticsService::Guard LifecycleDiagnosticsService::initialize(const bool enableCrashReports)
{
    loggingService_->initialize();

    if (!initialized_)
    {
        // TODO: make this crossplatform
        const auto executableDir = std::filesystem::path{std::filesystem::u8path(fio->_GetExecutableDirectory())};
        const auto logsArchive = fs::GetLogsPath().replace_extension(".7z");
        constexpr auto archiverBin = "7za.exe";

        auto *options = sentry_options_new();
        sentry_options_set_dsn(options, "https://1798a1bcfb654cbd8ce157b381964525@o572138.ingest.sentry.io/5721165");
        sentry_options_set_database_path(options, (fs::GetStashPath() / "sentry-db").c_str());
        sentry_options_set_handler_path(options, (executableDir / "crashpad_handler.exe").c_str());
        sentry_options_add_attachment(options, logsArchive.c_str());
        sentry_options_set_before_send(options, beforeCrash, this);

        archiveLogsCmd_ = _T("call \"") + (executableDir / archiverBin).native() + _T("\" a \"") +
                          logsArchive.native() + _T("\" \"") + fs::GetLogsPath().native() + _T("\"");
        sentry_options_set_system_crash_reporter_enabled(options, enableCrashReports);

        initialized_ = sentry_init(options) == 0;
    }

    return Guard(*this);
}

void LifecycleDiagnosticsService::terminate() const
{
    loggingService_->terminate();

    if (initialized_)
    {
        sentry_close();
    }
}

void LifecycleDiagnosticsService::notifyAfterRun() const
{
    static auto latestFlushTimePoint = std::chrono::steady_clock::now();

    if (const auto now = std::chrono::steady_clock::now();
        std::chrono::duration_cast<std::chrono::seconds>(now - latestFlushTimePoint) >= getLoggingFlushPeriod())
    {
        loggingService_->flushAsync();
        latestFlushTimePoint = now;
    }
}

sentry_value_t LifecycleDiagnosticsService::beforeCrash(const sentry_value_t event, void *, void *data)
{
    const auto *self = static_cast<LifecycleDiagnosticsService *>(data);

#ifndef STORM_DIAG_NO_FLUSH_ON_CRASH
    // NB: STORM_DIAG_NO_FLUSH_ON_CRASH shall be defined when debugging stack corruption
    self->loggingService_->terminate();
#endif
    _tsystem(self->archiveLogsCmd_.c_str());

    return event;
}

} // namespace storm::diag
