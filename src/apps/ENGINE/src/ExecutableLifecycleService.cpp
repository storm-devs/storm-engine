#include "ExecutableLifecycleService.hpp"

#include "fs.h"
#include "vfile_service.h"

#define SENTRY_BUILD_STATIC 1
#include <sentry.h>

namespace storm::except
{
ExecutableLifecycleService::~ExecutableLifecycleService()
{
    if (initialized_)
    {
        sentry_shutdown();
    }
}

bool ExecutableLifecycleService::initialize(const bool enableCrashReports)
{
    if (!initialized_)
    {
        sentry_options_t *options = sentry_options_new();
        sentry_options_set_dsn(options, "https://1798a1bcfb654cbd8ce157b381964525@o572138.ingest.sentry.io/5721165");
        sentry_options_set_database_path(options, (fs::GetStashPath() / "sentry-db").string().c_str());
        sentry_options_set_handler_path(
            options, (fs::path{fio->_GetExecutableDirectory()} / "crashpad_handler.exe").string().c_str());
        for (const auto &attachment : attachments_)
        {
            sentry_options_add_attachment(options, attachment.c_str());
        }
        sentry_options_set_system_crash_reporter_enabled(options, enableCrashReports);

        initialized_ = sentry_init(options) == 0;
    }

    return initialized_;
}

void ExecutableLifecycleService::addAttachment(const std::filesystem::path &path)
{
    attachments_.insert(path.string());
}
} // namespace storm::except
