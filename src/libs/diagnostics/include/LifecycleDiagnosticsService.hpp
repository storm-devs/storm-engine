#pragma once
#include <chrono>
#include <filesystem>
#include <memory>

#define SENTRY_BUILD_STATIC 1
#include <sentry.h>

namespace storm::diag
{

constexpr auto getLoggingFlushPeriod()
{
    using namespace std::chrono_literals;
    return 5s; // configurable
}

class LoggingService;

class LifecycleDiagnosticsService final
{
    class Guard
    {
      public:
        explicit Guard(LifecycleDiagnosticsService &inst) : inst_(inst)
        {
        }

        ~Guard()
        {
            inst_.terminate();
        }

        operator bool() const
        {
            return inst_.initialized_;
        }

      private:
        LifecycleDiagnosticsService &inst_;
    };

  public:
    LifecycleDiagnosticsService();
    ~LifecycleDiagnosticsService();

    [[maybe_unused, nodiscard("This guard shall exist until stack unwind")]] Guard initialize(bool enableCrashReports);
    void terminate() const;
    void notifyAfterRun() const;

  private:
    std::filesystem::path::string_type archiveLogsCmd_;
    bool initialized_{false};
    std::unique_ptr<LoggingService> loggingService_;

    static sentry_value_t beforeCrash(sentry_value_t event, void *, void *data);
};

} // namespace storm::diag
