#ifndef _BALLSPLASHDEFINES_
#define _BALLSPLASHDEFINES_

#include <stdlib.h>

#include "..\..\common.h"

#define BALLSPLASH_TRACE_INFORMATION

#define PI  3.14159265358979323846f
#define PIm2 (PI * 2.0f)
#define PId2 (PI / 2.0f)

#define MAX_SPLASHES  100
#define SPLASH_RADIUS 0.15f
#define MAX_RHO       0.1f //radians from vertical

#define SPLASH_START_ARG         0.0f
#define SPLASH_HEIGHT_MULTIPLIER 1.0f
#define SPLASH_DISTORT_DIVIDER   3e4f
#define SPLASH_FADE_IN_TIME		 700
#define SPLASH_FADE_TIME		 4700
#define SPLASH_MOVE_Y            -0.05f

#define GRID_STEPS      8
#define GRID_LENGTH     4.1f
#define TRIANGLES_COUNT ((GRID_STEPS - 1)*(GRID_STEPS - 1)*2)

#define VPLANES_COUNT         4
#define VPLANES_HEIGHT        10.f
#define VPLANES_WIDTH         4.0f
#define SPLASH_FRAME_DELAY    65
#define SPLASH_FRAMES_COUNT   64
#define SPLASH_MOVE_Y2		  -0.5f

#define GRID_FVF  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)
#define GRID_FVF2 (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2)

#ifndef BALLSPLASH_TRACE_INFORMATION
	#define BALLSPLASH_TRACE(a)    true
	#define BALLSPLASH_TRACE2(a,b) true
#else
	#define BALLSPLASH_TRACE(a)    _VSYSTEM_API->Trace(a);
	#define BALLSPLASH_TRACE2(a,b) _VSYSTEM_API->Trace(a,b);
#endif

#endif