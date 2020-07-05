 #ifndef _TSPLASH_H_
#define _TSPLASH_H_

#include "..\..\common.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct GRID_VERTEX
{
	CVECTOR pos;
	dword color;
	float tu,tv;
};

struct GRID_VERTEX2
{
	CVECTOR pos;
	dword color;
	float tu,tv;
	float tu2,tv2;
};

class TSplash  
{
public:
	TSplash();
	virtual ~TSplash();

	void Initialize(INIFILE * _ini, IDirect3DDevice8 *_device, SEA_BASE *sea, VDX8RENDER *_renderer);
	void Release();
	void Start(const CVECTOR &_pos, const CVECTOR &_dir);
	bool Process(dword _dTime);
	bool Process2(dword _dTime);
	void PostProcess();
	void PostProcess2();
	void Realize(dword _dTime);
	void Realize2(dword _dTime);
	bool Enabled();

	static bool startRender;
	static dword lockTicks, fillTicks, unlockTicks, realizeTicks, processCount, topIndex, topIndex2;

private:
	float HeightF(dword _time, float _r, float _k);

	bool enabled;

	static VDX8RENDER *renderer;
	SEA_BASE *sea;
	IDirect3DDevice8 *device;

	static long vBuffer, iBuffer, texture, texture2, vBuffer2, iBuffer2;
	static int buffersUsage;
	dword time;

	float width2;
	float distortDivider;
	CVECTOR center,dir;
	float growK;
	float height, midY;
};

#endif 
