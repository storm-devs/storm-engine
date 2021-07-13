#pragma once

#include "..\common_h\cvector.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\tga.h"
#include "sea_defines.h"

typedef struct
{
    BOOL bUse;
    float fAng, fCos, fSin, fProX, fProZ;
    float fLen, fAmp, fPhase, fPhaseDelta;
} harmonic_t;

typedef struct
{
    string sBumpDir;
    float fBumpTile;
    float fBumpAnimSpeed;
    CVECTOR vSkyColor, vWaterColor;
} seaspecific_t;

struct sea_dat
{
    sea_dat() : aHarmonics(_FL_)
    {
    }

    string sSeaTexture;
    float fWTrans, fWRefl, fWAttn, fFCoeff, fSRefl;
    float fSunAzimuthAngle, fSunHeightAngle, fSunMult, fBumpTile, fBumpScale, fSunShadowScale, fFogStartDistance,
        fFogDensity, fBumpAng, fBumpSpeed, fBumpAnimSpeed, fPenaDepthSmall, fPenaDepthBig, fFogSeaDensity, fRoadStart,
        fRoadPower, fPenaSmallIslandMultiply, fPenaBigIslandMultiply;
    CVECTOR vSkyColor, vWaterColor, vFogColor, vSunColor, vPenaColor, vRoadColor1, vRoadColor2;
    array<harmonic_t> aHarmonics;
    string sBumpDir;
    dword dwBumpMipLevels, dwBumpSize;
    dword dwCubeMapSize;
    dword dwCubeMapVectorsSize;
    D3DFORMAT fmtCubeMap;
    bool bFogEnable;

    seaspecific_t GF2MX, GF3;
};
