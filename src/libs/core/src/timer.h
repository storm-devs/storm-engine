#pragma once

#include <chrono>
#include <cstdint>

class TIMER
{
    using Clock = std::chrono::high_resolution_clock;
    using Milliseconds = std::chrono::milliseconds;
    using TimePoint = std::chrono::time_point<Clock>;          // Time Point used to get time deltas.
    using Duration = std::chrono::duration<float, std::milli>; // Time difference between time points.

  public:
    bool Ring = false;            // If true, COMPILER::ProcessFrame adds a runtime event.
    bool FixedDelta = false;      // Is the time delta between frames constant
    uint32_t FixedDeltaValue = 0; // Value of constant time delta between frames
    uint32_t Delta_Time = 20; // The time difference between current and last frame in milliseconds, as an unsigned int.
    float fDeltaTime = 0.0f;  // The time difference between current and last frame, as a float
    uint32_t rDelta_Time = Delta_Time;
    uint32_t fps = 0;            // Frames per second based on the last second.
    uint32_t fps_count = 0;      // Running total of frames rendered in the last second, which resets to 0 every second.
    uint32_t fps_time = 0;       // A one-second timer which is used to update the FPS counter on the screen.
    TimePoint Current, Previous; // Timepoints of when each iteration of the timer has started and finished.
    static constexpr Milliseconds kMaxDelta = Milliseconds{100}; // Maximum time delta per frame.
    static constexpr Milliseconds kFPSUpdateInterval =
        Milliseconds{1000}; // How often to raise the event to update the FPS counter
    static_assert(kMaxDelta < kFPSUpdateInterval, "Max delta time should be less than fps update interval");

    TIMER()
    {
        Current = Previous = Clock::now();
    }

    void RefreshFPS()
    {
        // Modulo operator is not necessary as we have limited maximum frame delta, so fps_time values over twice this
        // value are not possible.
        fps_time -= (uint32_t)kFPSUpdateInterval.count();
        // Refresh the fps counter
        fps = fps_count;
        fps_count = 0;
        Ring = true; // This raises an event which causes the fps display to update
    }

    uint32_t Run()
    {
        Current = Clock::now();
        Delta_Time = (uint32_t)Duration(Current - Previous).count();
        if (Delta_Time > (uint32_t)kMaxDelta.count())
        {
            Delta_Time = (uint32_t)kMaxDelta.count();
        }

        rDelta_Time = Delta_Time;
        fDeltaTime = (float)Delta_Time;
        fps_count++;
        fps_time += Delta_Time;
        if (fps_time >= kFPSUpdateInterval.count())
        {
            RefreshFPS();
        }
        else
        {
            Ring = false;
        }

        if (Delta_Time == 0)
        {
            rDelta_Time = Delta_Time = 1;
            fDeltaTime = 1.0f;
        }

        Previous = Current;

        if (FixedDelta)
        {
            return FixedDeltaValue;
        }
        return Delta_Time;
    }

    uint32_t GetDeltaTime()
    {
        if (FixedDelta)
        {
            return FixedDeltaValue;
        }
        return Delta_Time;
    }

    void SetDelta(const int32_t DeltaTime)
    {
        if (DeltaTime <= 0)
        {
            FixedDelta = false;
            return;
        }
        FixedDeltaValue = DeltaTime;
        FixedDelta = true;
    }
};
