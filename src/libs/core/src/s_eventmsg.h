#pragma once

#include "message.h"
#include "platform/platform.hpp"

class S_EVENTMSG
{
  public:
    uint32_t nTime;
    uint32_t nPeriod;

  public:
    MESSAGE *pMessageClass;
    char *pEventName;
    bool bProcess;
    bool bInvalide;

    S_EVENTMSG()
    {
        bInvalide = false;
        bProcess = false;
        pMessageClass = nullptr;
        nTime = 0;
        nPeriod = 0;
        pEventName = nullptr;
    };

    S_EVENTMSG(const char *_name, MESSAGE *_pc, uint32_t _period)
    {
        bInvalide = false;
        bProcess = false;
        pMessageClass = _pc;
        nTime = 0;
        nPeriod = _period;

        const auto len = strlen(_name) + 1;
        pEventName = new char[len];
        strcpy_s(pEventName, len, _name);
    };

    ~S_EVENTMSG()
    {
        if (pMessageClass)
            delete pMessageClass;
        if (pEventName)
            delete pEventName;
    };

    bool ProcessTime(uint32_t _DeltaTime)
    {
        nTime += _DeltaTime;
        bProcess = true;
        if (nPeriod == 0)
            return true;
        if (nTime >= nPeriod)
            return true;
        return false;
    };

    void Invalidate()
    {
        bInvalide = true;
    };
};
