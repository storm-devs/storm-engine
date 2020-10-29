#ifndef _TIMER_H_
#define _TIMER_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#define FILTER_SIZE 16
class TIMER
{
  public:
    TIMER()
    {
        LARGE_INTEGER liFreq;
        QueryPerformanceFrequency(&liFreq);
        fSecondsPerTick = 1.0f / liFreq.QuadPart;
        QueryPerformanceCounter(&liPrevTime);

        PCFreq = 0.0;
        CounterStart = 0;

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
    dword FixedDeltaValue;
    bool Ring;
    float fSecondsPerTick;
    LARGE_INTEGER liPrevTime;

    dword Previous_Time;
    dword Delta_Time;
    float fDeltaTime;
    dword rDelta_Time;
    dword fps;
    dword fps_count;
    dword fps_time;
    dword ADT;
    float ADT_val;
    bool ADT_ON;

    double PCFreq;
    __int64 CounterStart;

    dword Run()
    {
        LARGE_INTEGER liCurTime;
        QueryPerformanceCounter(&liCurTime);
        fDeltaTime = 1000.0f * (liCurTime.QuadPart - liPrevTime.QuadPart) * fSecondsPerTick;
        Delta_Time = long(fDeltaTime);

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

        liPrevTime = liCurTime;

        if (Delta_Time == 0)
            Delta_Time = 1;
        if (FixedDelta)
            return FixedDeltaValue;
        return Delta_Time;
    };

    dword GetDeltaTime()
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

    void StartCounter()
    {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);

        PCFreq = double(li.QuadPart) / 1000000.0; // in microseconds

        QueryPerformanceCounter(&li);
        CounterStart = li.QuadPart;
    }

    double GetCounter()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return double(li.QuadPart - CounterStart) / PCFreq;
    }
};
#endif
