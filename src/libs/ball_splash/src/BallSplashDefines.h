#pragma once

#include "rands.h"
#include <stdlib.h>

#include <Windows.h>
#include <d3d9types.h>

#define BALLSPLASH_TRACE_INFORMATION

constexpr int MAX_SPLASHES = 100;
constexpr float SPLASH_RADIUS = 0.15f;
constexpr float MAX_RHO = 0.1f; // radians from vertical

constexpr float SPLASH_START_ARG = 0.0f;
constexpr float SPLASH_HEIGHT_MULTIPLIER = 1.0f;
constexpr float SPLASH_DISTORT_DIVIDER = 3e4f;
constexpr int SPLASH_FADE_IN_TIME = 700;
constexpr int SPLASH_FADE_TIME = 4700;
constexpr float SPLASH_MOVE_Y = -0.05f;

constexpr int GRID_STEPS = 8;
constexpr float GRID_LENGTH = 4.1f;
constexpr int TRIANGLES_COUNT = ((GRID_STEPS - 1) * (GRID_STEPS - 1) * 2);

constexpr int VPLANES_COUNT = 4;
constexpr float VPLANES_HEIGHT = 10.f;
constexpr float VPLANES_WIDTH = 4.0f;
constexpr int SPLASH_FRAME_DELAY = 65;
constexpr int SPLASH_FRAMES_COUNT = 64;
constexpr float SPLASH_MOVE_Y2 = -0.5f;

constexpr int GRID_FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
constexpr int GRID_FVF2 = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2);

#ifndef BALLSPLASH_TRACE_INFORMATION
#define BALLSPLASH_TRACE(a) true
#define BALLSPLASH_TRACE2(a, b) true
#else
#define BALLSPLASH_TRACE(a) _VSYSTEM_core.Trace(a);
#define BALLSPLASH_TRACE2(a, b) _VSYSTEM_core.Trace(a, b);
#endif
