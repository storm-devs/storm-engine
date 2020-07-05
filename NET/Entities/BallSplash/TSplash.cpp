#include "BallSplashDefines.h"
#include "TSplash.h"

VDX8RENDER *TSplash::renderer = 0;
bool TSplash::startRender = true;
float dU = 1.f / SPLASH_FRAMES_COUNT;

int			TSplash::buffersUsage   = 0;
/*
WORD		*TSplash::iBuffer       = 0;
GRID_VERTEX *TSplash::vBuffer       = 0;
WORD		*TSplash::iBuffer2      = 0;
GRID_VERTEX *TSplash::vBuffer2      = 0;
*/
long TSplash::iBuffer       = 0;
long TSplash::vBuffer       = 0;
long TSplash::iBuffer2      = 0;
long TSplash::vBuffer2      = 0;

long TSplash::texture       = 0;
long TSplash::texture2		= 0;

dword TSplash::lockTicks	= 0;
dword TSplash::fillTicks	= 0; 
dword TSplash::unlockTicks  = 0;
dword TSplash::realizeTicks = 0;
dword TSplash::processCount = 0;
dword TSplash::topIndex		= 0;
dword TSplash::topIndex2	= 0;

dword ambientColor = 0;
//--------------------------------------------------------------------
dword Desaturate(dword _color, float _k)
{
	dword r = (_color >> 16) & 0xFF;
	dword g = (_color >> 8 ) & 0xFF;
	dword b = (_color      ) & 0xFF;
	dword grayed = (r+g+b) / 3;

	r = (dword) (grayed*_k + r*(1.f-_k));
	g = (dword) (grayed*_k + g*(1.f-_k));
	b = (dword) (grayed*_k + b*(1.f-_k));
	return (r << 16) | (g << 8) | b;
}

//--------------------------------------------------------------------
TSplash::TSplash()
	:enabled(false)
	,time(0)
{

}

//--------------------------------------------------------------------
TSplash::~TSplash()
{
	Release();

	if (!--buffersUsage)
	{

		if (iBuffer)
			//delete iBuffer;
			renderer->ReleaseIndexBuffer(iBuffer);
		if (vBuffer)
			//delete vBuffer;
			renderer->ReleaseVertexBuffer(vBuffer);
		if (iBuffer2)
			//delete iBuffer2;
			renderer->ReleaseIndexBuffer(iBuffer2);
		if (vBuffer2)
			//delete vBuffer2;
			renderer->ReleaseVertexBuffer(vBuffer2);
		if (texture)
			renderer->TextureRelease(texture);
		if (texture2)
			renderer->TextureRelease(texture2);
	}
}

//--------------------------------------------------------------------
void TSplash::Initialize(INIFILE * _ini, IDirect3DDevice8 *_device, SEA_BASE *_sea, VDX8RENDER *_renderer)
{
	ambientColor = 0;
	sea = _sea;
	if (!renderer)
		renderer = _renderer;

	if (!buffersUsage++)
	{
		/*
		iBuffer  = NEW WORD[TRIANGLES_COUNT*3];
		vBuffer  = NEW GRID_VERTEX[GRID_STEPS*GRID_STEPS];
		iBuffer2 = NEW WORD[VPLANES_COUNT*6];
		vBuffer2 = NEW GRID_VERTEX[VPLANES_COUNT*4];
		*/
		iBuffer = renderer->CreateIndexBuffer(MAX_SPLASHES*TRIANGLES_COUNT*3*sizeof(WORD), D3DUSAGE_WRITEONLY);
		vBuffer = renderer->CreateVertexBuffer(GRID_FVF, MAX_SPLASHES*GRID_STEPS*GRID_STEPS*sizeof(GRID_VERTEX), D3DUSAGE_WRITEONLY);
		iBuffer2 = renderer->CreateIndexBuffer(MAX_SPLASHES*VPLANES_COUNT*6*sizeof(WORD), D3DUSAGE_WRITEONLY);
		vBuffer2 = renderer->CreateVertexBuffer(GRID_FVF2, MAX_SPLASHES*VPLANES_COUNT*4*sizeof(GRID_VERTEX2), D3DUSAGE_WRITEONLY);

		texture = renderer->TextureCreate("explos.tga");
		texture2 = renderer->TextureCreate("splash.tga");

		// INDEXES 1
		WORD *indexes = /*iBuffer*/ (WORD *) renderer->LockIndexBuffer(iBuffer); 

		int startIndex = 0;
		for (int i=0; i<MAX_SPLASHES; i++, startIndex += GRID_STEPS*GRID_STEPS)
		for (int z=0; z < GRID_STEPS - 1; ++z)
		for (int x=0; x < GRID_STEPS - 1; ++x)
		{
			
			*(indexes++) = startIndex + GRID_STEPS * z     + x	 ;
			*(indexes++) = startIndex + GRID_STEPS * (z+1) + x	 ;
			*(indexes++) = startIndex + GRID_STEPS * (z+1) + x + 1;

			*(indexes++) = startIndex + GRID_STEPS * z     + x	 ;
			*(indexes++) = startIndex + GRID_STEPS * (z+1) + x + 1;
			*(indexes++) = startIndex + GRID_STEPS * z     + x + 1;
		}
		renderer->UnLockIndexBuffer(iBuffer);

		// INDEXES 2
		startIndex = 0;
		indexes = /*iBuffer2*/(WORD *) renderer->LockIndexBuffer(iBuffer2); 
		for (i=0; i<MAX_SPLASHES; i++, startIndex += VPLANES_COUNT*4)
		for (int j=0; j<VPLANES_COUNT; j++)
		{
			
			*(indexes++) = startIndex + j*4 + 0;
			*(indexes++) = startIndex + j*4 + 1;
			*(indexes++) = startIndex + j*4 + 2;

			*(indexes++) = startIndex + j*4 + 0;
			*(indexes++) = startIndex + j*4 + 2;
			*(indexes++) = startIndex + j*4 + 3;
		}
		renderer->UnLockIndexBuffer(iBuffer2);
	}
}

//--------------------------------------------------------------------
void TSplash::Release()
{
}

//--------------------------------------------------------------------
void TSplash::Start(const CVECTOR &_pos, const CVECTOR &_dir)
{
	time = 0;
	center = _pos;
	dir = _dir;
	//_VSYSTEM_API->Trace("start dir.x=%f, dir.z=%f", this->dir.x, this->dir.z);
	enabled = true;
	growK = randUpper(2.0f);

	midY = sea->WaveXZ(center.x, center.z) + SPLASH_MOVE_Y;
	distortDivider = rand(SPLASH_DISTORT_DIVIDER) + 10.0f;

	height = randUpper(VPLANES_HEIGHT);
	float a = 0.f;
	width2 = randUpper(VPLANES_WIDTH / 2.f);

	if (!ambientColor)
	{
		renderer->GetRenderState(D3DRS_AMBIENT, &ambientColor);
		ambientColor = Desaturate(ambientColor, 1.0f);
	}
}

//--------------------------------------------------------------------
float TSplash::HeightF(dword _time, float _r, float _k)
{
	float tPhase = ((float) _time) * (2.f * PId2 / (SPLASH_FRAME_DELAY*SPLASH_FRAMES_COUNT));
	float rK = (GRID_LENGTH - _r) / GRID_LENGTH;
	float k = ((float) _time) / SPLASH_FADE_TIME;
	if (k > 1.0f) 
		k = 1.0f;
	k = 1.f - k;
	return (.55f + k*fabsf(cosf(rK*10.f + _time/7e2f)))*rK*rK;
}

//--------------------------------------------------------------------
bool TSplash::Process(dword _dTime)
{
	if (!enabled)
		return false;

	++TSplash::processCount;
	time += _dTime;
	
	if (time > SPLASH_FADE_TIME)
	{
		enabled = false;
		return false;
	}

	float rho = rand(MAX_RHO);
	float r = SPLASH_RADIUS;
	int x,z;

	static GRID_VERTEX *startVertices;
	midY = sea->WaveXZ(center.x, center.z) + SPLASH_MOVE_Y;

	dword ticksLock;
	RDTSC_B(ticksLock);
	if (startRender)
	{
		startVertices = (GRID_VERTEX *) renderer->LockVertexBuffer(vBuffer);
		startRender = false;
	}
	RDTSC_E(ticksLock);
	TSplash::lockTicks += ticksLock;
	dword ticksFill;
	RDTSC_B(ticksFill);

	GRID_VERTEX *vertices = startVertices + topIndex*GRID_STEPS*GRID_STEPS;
	float stepSize = ((float)GRID_LENGTH) / ((float)GRID_STEPS);
	float halfSize = GRID_LENGTH / 2.0f;
	CVECTOR curPos;	
	float localHeight;

	//calculate alpha
	int dt = time - SPLASH_FRAMES_COUNT*SPLASH_FRAME_DELAY / 2;
	dword alpha;
	if (time < SPLASH_FADE_IN_TIME)
		alpha =  (dword) (255.f * time / SPLASH_FADE_IN_TIME);
	else if (dt < 0)
		alpha = 0xFF;
	else
		alpha = (dword) (255.f - dt * 255.f/(SPLASH_FADE_TIME - SPLASH_FRAMES_COUNT*SPLASH_FRAME_DELAY/2));
	alpha = (alpha << 24) | (0x00FFFFFF & ambientColor);

	//fill vertices
	for (z=0; z < GRID_STEPS; ++z)
	for (x=0; x < GRID_STEPS; ++x)
	{
		vertices->pos.x = center.x + growK * (stepSize * x - halfSize);
		vertices->pos.z = center.z + growK * (stepSize * z - halfSize);

		if ((x > 0) && (z > 0) && (x < GRID_STEPS-1) && (z < GRID_STEPS-1))
		{
			curPos.x = (center.x + stepSize * x) - halfSize;
			curPos.z = (center.z + stepSize * z) - halfSize;
			rho = sqrtf((curPos.x-center.x)*(curPos.x-center.x)+(curPos.z-center.z)*(curPos.z-center.z));
			localHeight = HeightF(time, rho, (GRID_LENGTH - rho) / GRID_LENGTH);

			vertices->pos.y = midY + localHeight;
		}
		else
			vertices->pos.y = midY+2*SPLASH_MOVE_Y;

		vertices->color = alpha;
		vertices->tu = ((float) x) / (GRID_STEPS-1);
		vertices->tv = ((float) z) / (GRID_STEPS-1);
		++vertices;
	}

	RDTSC_E(ticksFill);
	TSplash::fillTicks += ticksFill;
	++topIndex;
	return true;
}

//--------------------------------------------------------------------
void TSplash::PostProcess()
{
	if (startRender)
		return;
	dword ticksUnlock;
	RDTSC_B(ticksUnlock);
	renderer->UnLockVertexBuffer(vBuffer);
	RDTSC_E(ticksUnlock);
	TSplash::unlockTicks += ticksUnlock;
}

//--------------------------------------------------------------------
bool TSplash::Process2(dword _dTime)
{
	if (!enabled)
		return false;

	if (time > (SPLASH_FRAMES_COUNT-1)*SPLASH_FRAME_DELAY)
		return false;

	// VERTICES 2
	static GRID_VERTEX2 *startVertices2;
	dword ticksLock;
	RDTSC_B(ticksLock);
	if (startRender)
	{
		startVertices2 = (GRID_VERTEX2 *) renderer->LockVertexBuffer(vBuffer2);
		startRender = false;
	}
	RDTSC_E(ticksLock);
	TSplash::lockTicks += ticksLock;
	dword ticksFill;
	RDTSC_B(ticksFill);

	GRID_VERTEX2 *vertices = startVertices2 + topIndex*4*VPLANES_COUNT;
	float u = dU * (int) (time / SPLASH_FRAME_DELAY);
	if (u > (1.f-dU))
		u = 1.f-dU;

	dword alpha = (dword) ((((float) (time % SPLASH_FRAME_DELAY)) / SPLASH_FRAME_DELAY) * 0xFF);
	alpha = (alpha << 24) | (0x00FFFFFF & ambientColor);

	float a = 0.f;
	float dx = VPLANES_WIDTH / 2.f;
	float dy = 0.f;
	for (int i=0; i<VPLANES_COUNT; i++, a+= PI/VPLANES_COUNT)
	{
		vertices->pos.x = center.x - width2*cosf(a);
		vertices->pos.z = center.z - width2*sinf(a);
		vertices->pos.y = midY + height + SPLASH_MOVE_Y2;
		vertices->tu = u;
		vertices->tu2 = vertices->tu + dU;
		vertices->tv = 0.f;
		vertices->tv2 = vertices->tv;
		vertices->color = alpha;
		++vertices;

		vertices->pos.x = center.x - width2*cosf(a);
		vertices->pos.z = center.z - width2*sinf(a);
		vertices->pos.y = midY + SPLASH_MOVE_Y2;
		vertices->tu = u;
		vertices->tu2 = vertices->tu + dU;
		vertices->tv = 1.f;
		vertices->tv2 = vertices->tv;
		vertices->color = alpha;
		++vertices;

		vertices->pos.x = center.x + width2*cosf(a);
		vertices->pos.z = center.z + width2*sinf(a);
		vertices->pos.y = midY + SPLASH_MOVE_Y2;
		vertices->tu = u+dU;
		vertices->tu2 = vertices->tu + dU;
		vertices->tv = 1.f;
		vertices->tv2 = vertices->tv;
		vertices->color = alpha;
		++vertices;

		vertices->pos.x = center.x + width2*cosf(a);
		vertices->pos.z = center.z + width2*sinf(a);
		vertices->pos.y = midY + height + SPLASH_MOVE_Y2;
		vertices->tu = u+dU;
		vertices->tu2 = vertices->tu + dU;
		vertices->tv = 0.f;
		vertices->tv2 = vertices->tv;
		vertices->color = alpha;
		++vertices;
	}

	RDTSC_E(ticksFill);
	TSplash::fillTicks += ticksFill;
	++topIndex;
	return true;
}

//--------------------------------------------------------------------
void TSplash::PostProcess2()
{
	if (startRender)
		return;
	dword ticksUnlock;
	RDTSC_B(ticksUnlock);
	renderer->UnLockVertexBuffer(vBuffer2);
	RDTSC_E(ticksUnlock);
	TSplash::unlockTicks += ticksUnlock;
}

//--------------------------------------------------------------------
void TSplash::Realize(dword _dTime)
{
	if (startRender)
		return;

	dword ticksRealize;
	RDTSC_B(ticksRealize);

	static dword ambient, tfactor, oldAmbient, alpha;
	CMatrix m;
	renderer->SetTransform(D3DTS_WORLD, (D3DMATRIX *) m);
	renderer->TextureSet(0, texture);

	renderer->DrawBuffer(vBuffer, 
						 sizeof(GRID_VERTEX), 
						 iBuffer, 
						 0, 
						 GRID_STEPS*GRID_STEPS*topIndex, 
						 0, 
						 TRIANGLES_COUNT*topIndex,
						 "splash");

	RDTSC_E(ticksRealize);
	TSplash::realizeTicks += ticksRealize;
}

//--------------------------------------------------------------------
void TSplash::Realize2(dword _dTime)
{
	if (!topIndex)
		return;

	dword ticksRealize;
	RDTSC_B(ticksRealize);

	static dword ambient, fogColor, oldAmbient;
	CMatrix m;
	renderer->SetTransform(D3DTS_WORLD, (D3DMATRIX *) m);
	renderer->TextureSet(0, texture2);
	renderer->TextureSet(1, texture2);

	renderer->DrawBuffer(vBuffer2, 
						 sizeof(GRID_VERTEX2), 
						 iBuffer2, 
						 0, 
						 VPLANES_COUNT*4*topIndex, 
						 0, 
						 VPLANES_COUNT*2*topIndex, 
						 0);

	RDTSC_E(ticksRealize);
	TSplash::realizeTicks += ticksRealize;
}

//--------------------------------------------------------------------
bool TSplash::Enabled()
{
	return enabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
