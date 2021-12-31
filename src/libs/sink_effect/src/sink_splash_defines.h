#pragma once

#include "windows.h"
#include "d3d9types.h"

#define SINK_SPLASH_TRACE_INFORMATION

namespace sink_effect
{
constexpr int MAX_SINKS = 25;

constexpr int MAX_SPLASHES = 25;
constexpr int MAX_FLOTSAMS = 25;
constexpr int SINK_TIME = 2000;

constexpr int MAX_SPLASH_TIME = 120000;

constexpr int GRID_STEPS = 11;
constexpr float GRID_LENGTH = 6.0f;
constexpr int TRIANGLES_COUNT = (GRID_STEPS - 1) * (GRID_STEPS - 1) * 2;

constexpr float SPLASH_START_ARG = 0.0f;
constexpr float SPLASH_HEIGHT_MULTIPLIER = 0.5f;
constexpr float SPLASH_DISTORT_DIVIDER = 3e4f;
constexpr int SPLASH_FADE_TIME = 1000;
constexpr float SPLASH_MOVE_Y = -0.3f;

constexpr float MAX_RHO = 1.5f;
constexpr float SPLASH_RADIUS = 0.4f;

constexpr int SINK_FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);

constexpr float FLOTSAM_H_SPEED = 0.8f;
constexpr float FLOTSAM_V_SPEED = 1.2f;
constexpr float FLOTSAM_TURN_SPEED = 3.0f;
constexpr float FLOTSAM_START_DELTAY = 2.0f;
constexpr int FLOTSAM_MODELS_COUNT = 4;
constexpr int FLOTSAM_FLOAT_TIME = 30000;
constexpr float FLOTSAM_DY = 0.3f;
} // namespace sink_effect

#ifndef SINK_SPLASH_TRACE_INFORMATION
#define SINKSPLASH_TRACE(a) true
#define SINKSPLASH_TRACE2(a, b) true
#else
#define SINKSPLASH_TRACE(a) _VSYSTEM_core.Trace(a);
#define SINKSPLASH_TRACE2(a, b) _VSYSTEM_core.Trace(a, b);
#endif
