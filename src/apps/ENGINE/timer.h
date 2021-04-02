#pragma once

#include <Windows.h>

#define USE_HIGH_FREQUENCY
#define FILTER_SIZE 16
#include <cstdint>

class TIMER
{
  public:
    TIMER() : FixedDeltaValue(0), Previous_Time(0), fDeltaTime(0)
    {
#ifdef USE_HIGH_FREQUENCY
        LARGE_INTEGER liFreq;
        QueryPerformanceFrequency(&liFreq);
        fSecondsPerTick = 1.0f / liFreq.QuadPart;
        QueryPerformanceCounter(&liPrevTime);
#else
        Previous_Time = GetTickCount();
#endif
        Delta_Time = 20;
        rDelta_Time = Delta_Time;
        fps = 0;
        fps_count = 0;
        fps_time = 0;
        Ring = false;
        FixedDelta = false;
        ADT = 1;
        ADT_ON = true;
        ADT_val = 10;
    };
    bool FixedDelta;
    uint32_t FixedDeltaValue;
    bool Ring;
#ifdef USE_HIGH_FREQUENCY
    float fSecondsPerTick;
    LARGE_INTEGER liPrevTime;
#endif
    uint32_t Previous_Time;
    uint32_t Delta_Time;
    float fDeltaTime;
    uint32_t rDelta_Time;
    uint32_t fps;
    uint32_t fps_count;
    uint32_t fps_time;
    uint32_t ADT;
    float ADT_val;
    bool ADT_ON;
    uint32_t Run()
    {
#ifdef USE_HIGH_FREQUENCY
        LARGE_INTEGER liCurTime;
        QueryPerformanceCounter(&liCurTime);
        fDeltaTime = 1000.0f * (liCurTime.QuadPart - liPrevTime.QuadPart) * fSecondsPerTick;
        Delta_Time = long(fDeltaTime);
#else
        uint32_t Current_Time;
        Current_Time = GetTickCount();
        Delta_Time = Current_Time - Previous_Time;
        rDelta_Time = Delta_Time;
#endif
        rDelta_Time = Delta_Time;
        if (Delta_Time > 100)
        {
            Delta_Time = 100;
        }

        fps_time += rDelta_Time;
        fps_count++;
        if (fps_time >= 1000)
        {
            fps_time = fps_time - 1000;
            fps = fps_count;
            fps_count = 0;
            Ring = true;
        }
        else
            Ring = false;
#ifdef USE_HIGH_FREQUENCY
        liPrevTime = liCurTime;
#else
        Previous_Time = Current_Time;
#endif

        if (Delta_Time == 0)
            Delta_Time = 1;
        if (FixedDelta)
            return FixedDeltaValue;
        return Delta_Time;
    };

    uint32_t GetDeltaTime()
    {
        if (FixedDelta)
            return FixedDeltaValue;
        return Delta_Time;
    }

    void SetDelta(long DeltaTime)
    {
        if (DeltaTime < 0)
        {
            FixedDelta = false;
        }
        else
        {
            FixedDeltaValue = DeltaTime;
            FixedDelta = true;
        }
    }
};
