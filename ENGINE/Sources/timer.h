#ifndef _TIMER_H_
#define _TIMER_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#define USE_HIGH_FREQUENCY
#define FILTER_SIZE	16
class TIMER
{
public:
	TIMER() 
	{
#ifdef USE_HIGH_FREQUENCY		
		LARGE_INTEGER liFreq;
		QueryPerformanceFrequency( &liFreq );
		fSecondsPerTick = 1.0f / liFreq.QuadPart;	
		QueryPerformanceCounter(&liPrevTime);
#else
		Previous_Time = GetTickCount(); 
#endif
		Delta_Time = 20; 
		rDelta_Time = Delta_Time;
		fps = 0; fps_count = 0; fps_time = 0; 
		Ring = false;
		FixedDelta = false;
		ADT = 1;
		ADT_ON = true;
		ADT_val = 10;
	};
	bool  FixedDelta;
	dword FixedDeltaValue;
	bool  Ring;
#ifdef USE_HIGH_FREQUENCY		
	float			fSecondsPerTick;
	LARGE_INTEGER	liPrevTime;
#endif
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
	dword Run()
	{
#ifdef USE_HIGH_FREQUENCY		
		LARGE_INTEGER	liCurTime;
		QueryPerformanceCounter( &liCurTime );
		fDeltaTime = 1000.0f * (liCurTime.QuadPart - liPrevTime.QuadPart) * fSecondsPerTick;
		Delta_Time = long(fDeltaTime);
#else
		dword Current_Time;
		Current_Time = GetTickCount();
		Delta_Time = Current_Time - Previous_Time;
		rDelta_Time = Delta_Time;
#endif
		rDelta_Time = Delta_Time;
		if(Delta_Time > 100) 
		{
			Delta_Time = 100;
		}

		fps_time += rDelta_Time;
		fps_count++;
		if(fps_time >= 1000)
		{
			fps_time = fps_time - 1000;
			fps = fps_count;
			fps_count = 0;
			Ring = true;
		} else Ring = false;
#ifdef USE_HIGH_FREQUENCY
		liPrevTime = liCurTime;
#else
		Previous_Time = Current_Time;
#endif
		
		if(Delta_Time == 0) Delta_Time = 1;
		if(FixedDelta) return FixedDeltaValue;
		return Delta_Time;
	};

	dword GetDeltaTime()
	{
		if(FixedDelta) return FixedDeltaValue;
		return Delta_Time;
	}

	void SetDelta(long DeltaTime)
	{
		if(DeltaTime < 0)
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
#endif