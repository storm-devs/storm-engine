#pragma once

//#define SEAFOAM_TRACE_INFORMATION

#define TRACE_STEPS_Y 5
#define TRACE_STEPS_Z 13 // 13
#define MAX_SHIPS 64

#define MAX_Z_STEP 2.5f
#define FOAM_SHIFT_X 0.35f // .7
#define FOAM_SHIFT_Z 0.1f  // .7
#define SPEED_DELTA_H 1.4f
#define CONST_MINUS_Y 0.3f

#define MEASURE_POINTS 3

#define MIN_DRAWABLE_SPEED 0.5f

#ifndef SEAFOAM_TRACE_INFORMATION
#define SEAFOAM_TRACE(a) true
#define SEAFOAM_TRACE2(a, b) true
#else
#define SEAFOAM_TRACE(a) _VSYSTEM_core.Trace(a);
#define SEAFOAM_TRACE2(a, b) _VSYSTEM_core.Trace(a, b);
#endif
/*
//--------------------------------------------------------------------
float rand(float r);
float randUpper(float r);
float randCentered(float r);
//--------------------------------------------------------------------
*/
