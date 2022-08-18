#pragma once

// common includes
#include "core.h"

class CorePrivate : public Core
{
public:
    virtual void Init() = 0;

    virtual void InitBase() = 0;
    virtual void ReleaseBase() = 0;

    virtual void CleanUp() = 0;

    virtual bool Run() = 0;
    [[nodiscard]] virtual bool initialized() const = 0;

    virtual void AppState(bool state) = 0;

    virtual void collectCrashInfo() const = 0;

    virtual void SetWindow(std::shared_ptr<storm::OSWindow> window) = 0;
};
