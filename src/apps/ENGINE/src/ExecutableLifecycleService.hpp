#pragma once

#include "fs.h"

#include <filesystem>
#include <set>
#include <string>

namespace storm::except
{
class ExecutableLifecycleService
{
  public:
    ~ExecutableLifecycleService();

    bool initialize(bool enableCrashReports);

    // all attachments should be specified before calling `initialize()`
    void addAttachment(const std::filesystem::path &path);

  protected:
    bool initialized_{false};
    std::set<std::string> attachments_;
};
} // namespace storm::except
