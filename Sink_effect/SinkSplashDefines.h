#ifndef _SINKSPLASHDEFINES_
#define _SINKSPLASHDEFINES_

#include <stdlib.h>
#include "..\\common_h\\rands.h"

#define SINK_SPLASH_TRACE_INFORMATION

#define PI  3.14159265358979323846f
#define PIm2 (PI * 2.0f)
#define PId2 (PI / 2.0f)

#define MAX_SINKS    25

#define MAX_SPLASHES 25
#define MAX_FLOTSAMS 25
#define SINK_TIME    2000

#define MAX_SPLASH_TIME 120000

#define GRID_STEPS      11
#define GRID_LENGTH     6.0f
#define TRIANGLES_COUNT ((GRID_STEPS - 1)*(GRID_STEPS - 1)*2)

#define SPLASH_START_ARG         0.0f
#define SPLASH_HEIGHT_MULTIPLIER 0.5f
#define SPLASH_DISTORT_DIVIDER   3e4f
#define SPLASH_FADE_TIME		 1000
#define SPLASH_MOVE_Y			 -0.3f

#define MAX_RHO 1.5f
#define SPLASH_RADIUS 0.4f

#define SINK_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)

#define FLOTSAM_H_SPEED 0.8f
#define FLOTSAM_V_SPEED 1.2f
#define FLOTSAM_TURN_SPEED 3.0f
#define FLOTSAM_START_DELTAY 2.0f
#define FLOTSAM_MODELS_COUNT 4
#define FLOTSAM_FLOAT_TIME 30000
#define FLOTSAM_DY 0.3f

#ifndef SINK_SPLASH_TRACE_INFORMATION
	#define SINKSPLASH_TRACE(a)    true
	#define SINKSPLASH_TRACE2(a,b) true
#else
	#define SINKSPLASH_TRACE(a)    _VSYSTEM_API->Trace(a);
	#define SINKSPLASH_TRACE2(a,b) _VSYSTEM_API->Trace(a,b);
#endif

#endif