#pragma once

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "..\common_h\model.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\geos.h"
#include "bsps.h"

#define GRID_WIDTH		 20.f
#define GRID_HEIGHT		 20.f
#define GRID_FRAMES		 32
#define GRID_FRAME_DELAY 60


#define ARTIFACT_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2)
struct ARTIFACT_VERTEX
{
	CVECTOR pos;
	dword color;
	float tu,tv;
	float tu2,tv2;
};

class Artifact : public ENTITY
{
public:
	Artifact();
	~Artifact();
	bool Init();
	void Realize(dword _dTime);
	dword _cdecl ProcessMessage(MESSAGE &message);

private:

	VDX8RENDER       *renderService;
	SEA_BASE		 *sea;
	BSPS_PS          *particles;
	SHIP_BASE		 *myShip;
	CVECTOR			 emitterDelta;
	long			 texture, iBuffer, vBuffer;
	dword			 time;
	float			 rotationZ;
	CVECTOR			 pos;
};
