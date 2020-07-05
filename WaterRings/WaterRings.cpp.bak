#include "..\common_h\rands.h"
#include "WaterRings.h"

#define PI		3.14159265358979323846f
#define PIm2	(PI * 2.0f)
#define PId2	(PI / 2.0f)
#define PId4	(PI / 4.0f)

INTERFACE_FUNCTION
CREATE_CLASS(WaterRings)

//------------------------------------------------------------------------------------
WaterRings::WaterRings()
	:ivManager(0)
{
}

//------------------------------------------------------------------------------------
WaterRings::~WaterRings()
{
	if (ivManager)
		delete ivManager;
	renderService->TextureRelease(ringTexture);
}

//------------------------------------------------------------------------------------
bool WaterRings::Init()
{
	GUARD(WaterRings::Init())

	_CORE_API->LayerAdd("realize",GetID(),65551);

	ENTITY_ID seaID;
	_CORE_API->FindClass(&seaID, "sea", 0);
	sea = (SEA_BASE*) _CORE_API->GetEntityPointer(&seaID);

	renderService = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!renderService)	_THROW("No service: dx8render");

	ivManager = NEW TIVBufferManager(renderService 
									,RING_FVF
									,sizeof(RING_VERTEX)
									,TRIANGLES_COUNT*3
									,GRID_STEPS_COUNT*GRID_STEPS_COUNT
									,MAX_RINGS);

	ringTexture = renderService->TextureCreate("ring.tga");

	for (int i=0; i<MAX_RINGS;i++)
	{
		rings[i].ivIndex = ivManager->ReserveElement();
		rings[i].activeTime = 0;
		rings[i].active = false;
		rings[i].firstUpdate = true;
		rings[i].x = 0.f;
		rings[i].z = 0.f;
	}

	return true;
	UNGUARD
}

//------------------------------------------------------------------------------------
void WaterRings::Realize(dword _dTime)
{
	if (!sea)
		return;

	//update buffers for rings
	ivManager->LockBuffers();	
	WORD        *iPointer;
	RING_VERTEX *vPointer;
	long		vOffset;
	for (int i=0; i<MAX_RINGS; i++)
	{
		//check if ring needs to be removed
		if (rings[i].activeTime > (FADE_IN_TIME+FADE_OUT_TIME))
			rings[i].active = false;
		ivManager->GetPointers(rings[i].ivIndex, (WORD **) &iPointer, (void **) &vPointer, &vOffset);
		UpdateGrid(i, iPointer, vPointer, vOffset);

		if (rings[i].active)
			rings[i].activeTime += _dTime;
	}
	ivManager->UnlockBuffers();

	renderService->TextureSet(0, ringTexture);
	ivManager->DrawBuffers("waterring");
}


//------------------------------------------------------------------------------------
dword _cdecl WaterRings::ProcessMessage(MESSAGE &message)
{
	//add new ring
	for (int i=0; i<MAX_RINGS;i++)
	{
		if (!rings[i].active)
		{ //found free element, add ring here
			rings[i].activeTime = 0;
			rings[i].active = true;
			rings[i].x = message.Float()/*+randCentered(0.15f)*/;
			rings[i].z = message.Float()/*+randCentered(0.15f)*/;
			
			float a = message.Float()+PI+randCentered(PId2/1.5f);
			rings[i].cosA = cosf(a);
			rings[i].sinA = sinf(a);

			bool walk = message.Long() != 0;
			bool run  = message.Long() != 0;
			bool swim = message.Long() != 0;
			if (!(walk || run || swim))
			{
				rings[i].activeTime = 0;
				rings[i].active = false;
				return 0;
			}

			if (swim)
				rings[i].state = RING_SWIM;
			else if (run)
				rings[i].state = RING_RUN;
			else
				rings[i].state = RING_WALK;

			return 0;
		}
	}
	/*
	switch(message.Long())
	{
		case MSG_BLADE_SET:
			return 0;
		break;
	}
	*/

	return 0;
}

//------------------------------------------------------------------------------------
void WaterRings::UpdateGrid(int _ringI, WORD *_iPointer, RING_VERTEX *_vPointer, long _vOffset)
{
	Assert(_iPointer);
	Assert(_vPointer);

	float a;
	tRing *ring = &rings[_ringI];
	int x,z;

	if (ring->firstUpdate)
	{
		WORD *indexes = _iPointer;
		for (z=0; z < GRID_STEPS_COUNT - 1; ++z)
		for (x=0; x < GRID_STEPS_COUNT - 1; ++x)
		{
			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * z     + x	 );
			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * (z+1) + x	 );
			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * (z+1) + x + 1);

			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * z     + x	 );
			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * (z+1) + x + 1);
			*(indexes++) = (WORD) (_vOffset + GRID_STEPS_COUNT * z     + x + 1);
		}
		ring->firstUpdate = false;
	}

	if (ring->activeTime < FADE_IN_TIME)
		a = (float) ring->activeTime / FADE_IN_TIME;
	else
		a = 1.f - ((float) (ring->activeTime-FADE_IN_TIME) / FADE_OUT_TIME);


	float midX = (GRID_STEPS_COUNT-1) / 2.f;
	float midZ = (GRID_STEPS_COUNT-1) / 2.f;
	RING_VERTEX *ringVertex = _vPointer;
	float gX, gZ;
	if (ring->active)
	{
		dword texA = ((dword) (a*50)) << 24;
		float r = .4f + 1.5f*ring->activeTime / (FADE_IN_TIME+FADE_OUT_TIME);

		for (z=0; z < GRID_STEPS_COUNT; ++z)
		for (x=0; x < GRID_STEPS_COUNT; ++x)
		{
			ringVertex->color = texA;
			gX = (x-midX)/midX;
			gZ = (z-midZ)/midZ;
			ringVertex->pos.x = ring->x + r*(gX*ring->cosA + gZ*ring->sinA);
			ringVertex->pos.z = ring->z + r*(gZ*ring->cosA - gX*ring->sinA);
			ringVertex->pos.y = Y_DELTA + sea->WaveXZ(ringVertex->pos.x, ringVertex->pos.z);
			//if (ring->state == RING_WALK)
			{
				ringVertex->tu = ((float) x / (GRID_STEPS_COUNT - 1))*.5f;
				ringVertex->tv = (float) z / (GRID_STEPS_COUNT - 1);
			} 
			/*
			else if (ring->state == RING_RUN)
			{
				ringVertex->tu = ((float) x / (GRID_STEPS_COUNT - 1))*.5f+.5f;
				ringVertex->tv = (float) z / (GRID_STEPS_COUNT - 1);
			}
			*/
			++ringVertex;
		}
	}
	else
	{
		for (z=0; z < GRID_STEPS_COUNT; ++z)
		for (x=0; x < GRID_STEPS_COUNT; ++x)
		{
			ringVertex->color = 0x00000000;
			ringVertex->pos.x = 0.f;
			ringVertex->pos.z = 0.f;
			ringVertex->pos.y = 0.f;
			ringVertex->tu = 0.f;
			ringVertex->tv = 0.f;
			++ringVertex;
		}
	}


}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
