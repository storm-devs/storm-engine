#pragma once

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "..\common_h\model.h"
#include "..\common_h\sea_base.h"
//#include "..\geom_lib\geos.h"
#include "TIVBufferManager.h"

#define MAX_RINGS     25
#define FADE_IN_TIME  200
#define FADE_OUT_TIME 1200

#define GRID_STEPS_COUNT 3
#define TRIANGLES_COUNT  ((GRID_STEPS_COUNT - 1)*(GRID_STEPS_COUNT - 1)*2)
#define Y_DELTA          .01f

#define RING_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct RING_VERTEX
{
	CVECTOR pos;
	dword color;
	float tu,tv;
};

enum tRingState {RING_WALK, RING_RUN, RING_SWIM};
struct tRing
{
	bool active;
	long activeTime;
	long ivIndex;
	float x,z;
	tRingState state;	
	bool firstUpdate;
	float cosA, sinA;
};

class WaterRings : public ENTITY
{
public:
	WaterRings();
	~WaterRings();
	bool Init();
	void Realize(dword _dTime);
	dword _cdecl ProcessMessage(MESSAGE &message);

private:
	void UpdateGrid(int _ringI, WORD *iPointer, RING_VERTEX *vPointer, long vOffset);

	VDX8RENDER       *renderService;
	SEA_BASE		 *sea;
	TIVBufferManager *ivManager;
	long             ringTexture;
	tRing			 rings[MAX_RINGS];
};
