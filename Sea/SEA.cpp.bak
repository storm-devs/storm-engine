#include "Sea.h"
#include "..\common_h\tga.h"
#include "..\engine\program\sea_ai\script_defines.h"
#include "SSE.h"

//#define OLD_WORLD_POS

#define NUM_VERTEXS		65500
#define NUM_INDICES		165000

#define FRAMES		64
#define XWIDTH		128
#define YWIDTH		128

#define MIPSLVLS	4

#define GC_CONSTANT             0 	// Global constants = {0.0, 1.0, 0.5, 0.0000152590218967 = (0.5 / 32767.5)}
#define GC_CONSTANT2            1 	// Global constants 2 = {2.0, -1.0, 0.00036621652552071 = (12 / 32767.5), fog}
#define GC_ANIMATION			2 	// Animation frames(0.0-1.0) for 4 stages = {stage0, stage1, stage2, stage3}
#define GC_SHADOW_CONST1	    3  
#define GC_SHADOW_CONST2		7 
#define GC_LIGHT4               11 	//
#define GC_LIGHT3               13 
#define GC_LIGHT2               15
#define GC_LIGHT1               17 
#define GC_LIGHT0               19 
#define GC_MATERIAL             21 	// 
#define GC_CAMERA_POS           23 	// Local Camera position = {x, y, z, 0.0}
#define GC_MTX_WVP              24	// c[0] = mWorld * mView * mProjection

#define GC_FREE					28

INTERFACE_FUNCTION
CREATE_CLASS(SEA)

SEA * SEA::pSea = null;
bool SEA::bIntel = false;
bool SEA::bSSE = false;
bool SEA::bDisableSSE = false;

SEA::SEA() : 
	aBlocks(_FL_, 128),
	aBumps(_FL_),
	aNormals(_FL_),
	aThreads(_FL_),
	aEventCalcBlock(_FL_),
	aThreadsTest(_FL_),
	aBumpMaps(_FL_),
	aSeaTrash(_FL_, 512),
	aTrashRects(_FL_, 512),
	aSeaLights(_FL_, 512),
	aLightsRects(_FL_, 512)
{
	dwMaxDim = 65536 * 2;
	dwMinDim = 128;

	fMaxSeaHeight = 20.0f;
	fGridStep = 0.06f;
	fLodScale = 0.4f;

	fAmp1 = 1.0f;
	fFrame1 = 0.0f;
	fAnimSpeed1 = 1.0f;
	vMove1 = 0.0f;
	fScale1 = 0.4f;

	fAmp2 = 1.0f;
	fFrame2 = 10.0f;
	fAnimSpeed2 = 1.0f;
	vMove2 = 0.0f;
	fScale2 = 2.0f;

	fFoamV = 3.0f;
	fFoamK = 0.0f;
	fFoamUV = 3.0f;
	fFoamTextureDisturb = 0.7f;
	iFoamTexture = -1;
	bIniFoamEnable = false;
	bFoamEnable = false;

	iSeaLightTexture = -1;
	fLastLightTime = 0.0f;

	fLastTrashTime = 0.0f;
	iSeaTrashTexture = -1;
	bUnderSea = false;
	bUnderSeaStarted = false;
	bUnderSeaEnable = false;
	bStarted = false;
	bSimpleSea = false;

	fPosShift = 2.0f;

	fFogSeaDensity = 0.0f;
	fFogStartDistance = 0.0f;
	vFogColor = 0.0f;
	bFogEnable = false;

	pSeaFrame1 = NEW float[XWIDTH * YWIDTH];
	pSeaFrame2 = NEW float[XWIDTH * YWIDTH];
	pSeaNormalsFrame1 = NEW float[2 * XWIDTH * YWIDTH];
	pSeaNormalsFrame2 = NEW float[2 * XWIDTH * YWIDTH];

	iVSeaBuffer = -1;
	iISeaBuffer = -1;

	bTempFullMode = false;

	// HyperThreading section
	hEventCalcMaps = CreateEvent(null, false, false, null);

	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&cs1);

	pSea = this;
	bIntel = intel.IsIntelCPU();
    bSSE = intel.IsSSE();

	bStop = false;

	vWorldOffset = 0.f;

	pRenderTargetBumpMap = null;
}

SEA::~SEA()
{
	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&cs1);
	
	for (long i=0; i<aThreads; i++)
	{
		if (aThreads[i]) TerminateThread(aThreads[i], 0);
		if (aEventCalcBlock[i]) CloseHandle(aEventCalcBlock[i]);
	}

	if (hEventCalcMaps) CloseHandle(hEventCalcMaps);

	Render().Release(pReflection);
	Render().Release(pReflectionSunroad);
	Render().Release(pEnvMap);
	Render().Release(pSunRoadMap);
	Render().Release(pZStencil);
	Render().Release(pReflectionSurfaceDepth);
	Render().Release(pVolumeTexture);
	Render().Release(pRenderTargetBumpMap);

	Render().TextureRelease(iSeaTrashTexture);
	Render().TextureRelease(iSeaLightTexture);

	if (iVSeaBuffer >= 0) Render().ReleaseVertexBuffer(iVSeaBuffer); iVSeaBuffer = -1;
	if (iISeaBuffer >= 0) Render().ReleaseIndexBuffer(iISeaBuffer); iISeaBuffer = -1;

	if (iFoamTexture >= 0) Render().TextureRelease(iFoamTexture); iFoamTexture = -1;

	DELETE(pIndices);
	DELETE(pVSea);

	for (long i=0; i<aBumpMaps.Size(); i++) Render().Release(aBumpMaps[i]);

	for (long i=0; i<aBumps; i++) DELETE(aBumps[i]);
	for (long i=0; i<aNormals; i++) DELETE(aNormals[i]);


	DELETE(pSeaFrame1);
	DELETE(pSeaFrame2);
	DELETE(pSeaNormalsFrame1);
	DELETE(pSeaNormalsFrame2);
}

void SEA::SFLB_CreateBuffers()
{
	iVSeaBuffer = Render().CreateVertexBuffer(0, NUM_VERTEXS * sizeof(SeaVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
	iISeaBuffer = Render().CreateIndexBuffer(NUM_INDICES * 3 * sizeof(word), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);

	pIndices = NEW dword[NUM_VERTEXS*3];
	pVSea = NEW SeaVertex[NUM_VERTEXS];
}

bool SEA::Init()
{
	INIFILE * pEngineIni = fio->OpenIniFile(api->EngineIniFileName());
	bool bDisableHyperThreading = (pEngineIni) ? pEngineIni->GetLong(null, "HyperThreading", 1) == 0 : false;
	bDisableSSE = (pEngineIni) ? pEngineIni->GetLong(null, "DisableSSE", 0) != 0 : false;
	bIniFoamEnable = (pEngineIni) ? pEngineIni->GetLong("Sea", "FoamEnable", 1) != 0 : false;
    bool bEnableSSE = (pEngineIni) ? pEngineIni->GetLong(null, "EnableSSE", 0) != 0 : false;   //boal
    DELETE(pEngineIni);
    if (bEnableSSE)
    {
        bSSE = true;  //boal
	}

	if (bDisableHyperThreading) bHyperThreading = false;
	if (bDisableSSE) bSSE = false;
	if (!bIntel) bHyperThreading = false;
    
	if (bHyperThreading)
	{
		dword dwLogicals, dwCores, dwPhysicals;
		intel.CPUCount(&dwLogicals, &dwCores, &dwPhysicals);
		api->Trace("Total logical: %d, Total cores: %d, Total physical: %d", dwLogicals, dwCores, dwPhysicals);

		dword dwNumThreads = dwLogicals * dwCores - 1;
	
		for (dword i=0; i<dwNumThreads; i++)
		{
			HANDLE & hEvent = aEventCalcBlock[aEventCalcBlock.Add()];
			hEvent = CreateEvent(null, false, false, null);

			HANDLE & hThread = aThreads[aThreads.Add()];
			hThread = CreateThread(null, 0, (LPTHREAD_START_ROUTINE)SEA::ThreadExecute, (void*)i, CREATE_SUSPENDED, null);
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			ResumeThread(hThread);

			aThreadsTest.Add();
		}

		bHyperThreading = dwNumThreads > 0;
	}

	iFoamTexture = Render().TextureCreate("weather\\sea\\pena\\pena.tga");

	Render().CreateTexture( XWIDTH, YWIDTH, MIPSLVLS, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pRenderTargetBumpMap);

	SFLB_CreateBuffers();

	pVolumeTexture = Render().CreateVolumeTexture(XWIDTH, YWIDTH, FRAMES, 4, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
	/*pAnimTexture = Render().CreateAnimationTexture(pVolumeTexture, _FL_);
	pAnimTexture->SetAnimSpeed(20.0f);*/
	
	Render().CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pEnvMap);
	Render().CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pSunRoadMap);
	Render().CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pZStencil);

	Render().CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflection);
	Render().CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflectionSunroad);
	Render().CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pReflectionSurfaceDepth);

	iSeaTrashTexture = Render().TextureCreate("seatrash.tga");
	iSeaLightTexture = Render().TextureCreate("sealight.tga");

	byte bMin = 0xFF;
	byte bMax = 0;

	array<byte*> aTmpBumps(_FL_);

	dword i;

	for (i=0; i<FRAMES; i++)
	{
		char str[256];
		char * pFBuffer = null;
		dword dwSize;
		sprintf(str, "resource\\sea\\sea%.4d.tga", i);
		//sprintf(str, "resource\\sea\\sea0000.tga", i);
		fio->LoadFile(str, &pFBuffer, &dwSize);
		if (!pFBuffer) 
		{
			api->Trace("Sea: Can't load %s", str);
			return false;
		}

		char * pFB = pFBuffer + sizeof(TGA_H);

		byte * pBuffer = NEW byte[XWIDTH * YWIDTH];
		aTmpBumps.Add(pBuffer);
		
		for (dword y=0; y<YWIDTH; y++)
			for (dword x=0; x<XWIDTH; x++)
			{
				byte bB = (*pFB);
				//bB = byte(float(bB - 79.0f) * 255.0f / (139.0f - 79.0f));
				if (bB < bMin) bMin = bB;
				if (bB > bMax) bMax = bB;
				pBuffer[x + y * XWIDTH] = bB & 0xFF;
				pFB += sizeof(dword);
			}

		DELETE(pFBuffer);
	}

	for (i=0; i<FRAMES; i++)
	{
		byte * pBuffer = NEW byte[XWIDTH * YWIDTH];
		aBumps.Add(pBuffer);

		for (dword y=0; y<YWIDTH; y++)
			for (dword x=0; x<XWIDTH; x++)
			{
				dword dwAddress = x + y * YWIDTH;
				float b1, b2, b3, b4, b5; // -2 -1 0 1 2

				b1 = 0.08f * float(aTmpBumps[(i - 2) & (FRAMES - 1)][dwAddress]);
				b2 = 0.17f * float(aTmpBumps[(i - 1) & (FRAMES - 1)][dwAddress]);
				b3 = 0.50f * float(aTmpBumps[(i - 0) & (FRAMES - 1)][dwAddress]);
				b4 = 0.17f * float(aTmpBumps[(i + 1) & (FRAMES - 1)][dwAddress]);
				b5 = 0.08f * float(aTmpBumps[(i + 2) & (FRAMES - 1)][dwAddress]);

				pBuffer[dwAddress] = aTmpBumps[(i - 0) & (FRAMES - 1)][dwAddress];
			}
	}

	for (i=0; i<aTmpBumps(); i++) DELETE(aTmpBumps[i]);

	BuildVolumeTexture();
	
	EditMode_Update();

	api->Trace("Intel CPU: %s, SSE: %s, HyperThreading: %s", (bIntel) ? "Yes" : "No", (bSSE) ? "On" : "Off", (bHyperThreading) ? "On" : "Off");

	return true;
}

void SEA::BuildVolumeTexture()
{
	array<CVECTOR*> aVectors(_FL_);
	dword i, j;

	aNormals.DelAllWithPointers();

	D3DLOCKED_BOX	box[4];
	
	if (pVolumeTexture)
	for (i=0; i<4; i++)
		pVolumeTexture->LockBox(i, &box[i], 0, 0);

	for (i=0; i<aBumpMaps.Size(); i++) Render().Release(aBumpMaps[i]);
	aBumpMaps.DelAll();

	dword dwTexelSize = 4;
	char * pDst = (char*)NEW char[XWIDTH * YWIDTH * dwTexelSize];

	// build normals
	for (i=0; i<FRAMES; i++)
	{
		dword * pBuffer = NEW dword[XWIDTH * YWIDTH];
		aNormals.Add(pBuffer);

		CVECTOR * pVectors = NEW CVECTOR[XWIDTH * YWIDTH];
		aVectors.Add(pVectors);

		for (dword y=0; y<YWIDTH; y++)
			for (dword x=0; x<XWIDTH; x++)
			{
#define GET_MASSIVE(dx,dy)	(float(pMassive[((x+dx)&(XWIDTH-1)) + ((y+dy)&(XWIDTH-1)) * XWIDTH] & 0xFF) / 255.0f)
				byte * pMassive = aBumps[i];
	
				float fCenter	= GET_MASSIVE(0, 0);

				float fLeft		= GET_MASSIVE(-1, 0) - fCenter; 
				float fRight	= GET_MASSIVE(1, 0) - fCenter; 
				float fTop		= GET_MASSIVE(0, -1) - fCenter; 
				float fBottom	= GET_MASSIVE(0, 1) - fCenter; 

				CVECTOR vRes, vRes1, d0, d1, d2, d3, d4, d5, d6, d7;

				dword dwNums = 0;
				if (fLeft < 0.0f) dwNums++;
				if (fRight < 0.0f) dwNums++;
				if (fTop < 0.0f) dwNums++;
				if (fBottom < 0.0f) dwNums++;

				float d = 1.0f;
				d0 = CVECTOR(-1.f, d * fLeft, 0.f);
				d1 = CVECTOR(0.f, d * fTop, -1.f);
				d2 = CVECTOR(1.f, d * fRight, 0.f);
				d3 = CVECTOR(0.f, d * fBottom, 1.f);

				//res = !((d0^d1) + (d2^d3)); 

				CVECTOR v1 = (d1^d0);
				CVECTOR v2 = (d3^d2);
				CVECTOR v3 = (d0^d3);
				CVECTOR v4 = (d2^d1);

				vRes = vRes1 = !((d1^d0) + (d3^d2) + (d0^d3) + (d2^d1)); 
				if (bSimpleSea)
				{
					vRes1 = !(vRes * CVECTOR(100.0f, 1.0f, 100.0f));
				}
				//CVECTOR vRes1 = !(vRes * CVECTOR(100.0f, 1.0f, 100.0f));
				dword dwRes = MAKELONG(short(vRes.x * 32767.5f), short(vRes.z * 32767.5f));

				aNormals[i][x + y * XWIDTH] = dwRes;
				aVectors[i][x + y * XWIDTH] = vRes1;

				long red	= fftol((vRes1.x * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
				long green	= fftol((vRes1.y * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
				long blue	= fftol((vRes1.z * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol

				if (pVolumeTexture)
				{
					if (bSimpleSea)
						*(dword*)&(((char*)box[0].pBits)[i * box[0].SlicePitch + y * box[0].RowPitch + x * 4]) = ARGB(0x80, blue, blue, red);
					else
						*(dword*)&(((char*)box[0].pBits)[i * box[0].SlicePitch + y * box[0].RowPitch + x * 4]) = ARGB(0x80, blue, green, red);
				}
			}

		if (!pVolumeTexture)
		{
			D3DSURFACE_DESC		d3dsd;
			D3DLOCKED_RECT		d3dlr; 
			IDirect3DTexture8	* * pBumpMap;
			HRESULT				hr;

	  		pBumpMap = &aBumpMaps[aBumpMaps.Add()];
			hr = Render().CreateTexture( XWIDTH, YWIDTH, MIPSLVLS, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, pBumpMap);

			// generate mip levels for random bump 
			for (dword lev=0; lev<MIPSLVLS; lev++)
			{
				(*pBumpMap)->GetLevelDesc(lev, &d3dsd);
				(*pBumpMap)->LockRect(lev, &d3dlr, 0, 0);

				dword * pDstT = (dword*)pDst;
				for(dword y=0; y<d3dsd.Height; y++)
				{
					for(dword x=0; x<d3dsd.Width; x++)
					{
						CVECTOR vTmp = 0.0f;
						long dwMult = 1<<(lev);
						for (dword y1 = y * dwMult; y1 < (y + 1) * dwMult; y1++)
							for (dword x1 = x * dwMult; x1 < (x + 1) * dwMult; x1++)
								vTmp += aVectors[i][(x1 & (XWIDTH-1)) + (y1 & (YWIDTH-1)) * XWIDTH];
						
						vTmp *= (1.0f / float(dwMult * dwMult));
						
						int red		= int((vTmp.x * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						int green	= int((vTmp.y * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						int blue	= int((vTmp.z * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
						*pDstT++ = ARGB(255, blue, green, red);
					}
				}

				// simple copy 
				BYTE * pDstTemp = (BYTE *)d3dlr.pBits;
				for(y=0; y<d3dsd.Height; y++)
				{
					memcpy(pDstTemp, &pDst[y * d3dsd.Width * dwTexelSize], d3dsd.Width * dwTexelSize);
					pDstTemp += (dword)d3dlr.Pitch;
				}

				(*pBumpMap)->UnlockRect(lev);
			}
		}

	}

	for (j=1; j<4; j++)
	{
		CVECTOR * pVectors = NEW CVECTOR[(XWIDTH >> j) * (YWIDTH >> j)];

		for (i=0; i<FRAMES >> j; i++)
		{
			// calculate current pVectors
			for (dword y=0; y<YWIDTH >> j; y++)
				for (dword x=0; x<XWIDTH >> j; x++)
				{
					long iNumVectors = 0;
					CVECTOR vVec = 0.0f;
					dword dwW = 1 << j;
					for (dword k = i * dwW; k < (i + 1) * dwW; k++)
					{
						for (dword yy = y * dwW; yy < (y + 1) * dwW; yy++)
							for (dword xx = x * dwW; xx < (x + 1) * dwW; xx++)
							{	
								iNumVectors++;
								vVec += aVectors[k][xx + yy * XWIDTH];
							}
					}
					pVectors[x + y * (XWIDTH >> j)] = vVec / float(iNumVectors);
				}

			//
			if (pVolumeTexture)
			for (dword y=0; y<YWIDTH >> j; y++)
				for (dword x=0; x<XWIDTH >> j; x++)
				{
					long red	= fftol((pVectors[x + y * (XWIDTH >> j)].x * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
					long green	= fftol((pVectors[x + y * (XWIDTH >> j)].y * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol
					long blue	= fftol((pVectors[x + y * (XWIDTH >> j)].z * 0.5f + 0.5f) * 255.0f);		// FIX-ME no ftol

					if (bSimpleSea)
						*(dword*)&(((char*)box[j].pBits)[i * box[j].SlicePitch + y * box[j].RowPitch + x * 4]) = ARGB(0x80, blue, blue, red);
					else
						*(dword*)&(((char*)box[j].pBits)[i * box[j].SlicePitch + y * box[j].RowPitch + x * 4]) = ARGB(0x80, blue, green, red);
				}
		}
		DELETE(pVectors);
	}

	if (pVolumeTexture)
	for (i=0; i<4; i++)
		pVolumeTexture->UnlockBox(i);

	aVectors.DelAllWithPointers();
	DELETE(pDst);
}

bool SEA::EditMode_Update()
{
	//return true;
	v4SeaColor = CVECTOR4(10.0f / 255.0f, 55.0f / 255.0f, 100.0f / 255.0f, 1.0f);
	v4SkyColor = CVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	float fReflection = 0.8f;
	float fTransparency = 0.7f;
	float fFrenel = 0.75f;
	float fAttenuation = 0.9f;
	v4SeaParameters = CVECTOR4(fAttenuation, fReflection, fTransparency, 0.0f);

	fAmp1 = _fAmp1 = 8.0f;
	fAnimSpeed1 = 6.0f;
	fScale1 = 0.5f;
	vMoveSpeed1 = CVECTOR(5.0f, 0.0f, 0.0f);

	fAmp2 = _fAmp2 = 1.0f;
	fAnimSpeed2 = 3.0f;
	fScale2 = 2.0f;
	vMoveSpeed2 = CVECTOR(0.0f, 0.0f, 2.0f);

	fBumpScale = 0.1f;
	fBumpSpeed = 1.0f;
	fGridStep = 0.07f;
	fLodScale = 0.5f;

	fPosShift = 1.2f;
	
	Realize(0);

	return true;
}

long SEA::VisCode(const CVECTOR & vP)
{
	long vc = 0;

	/*
	CVECTOR v = vP - vCamPos;

	CVECTOR vp1 = CVECTOR(pFrustumPlanes[0].Nx, pFrustumPlanes[0].Ny, pFrustumPlanes[0].Nz);
	CVECTOR vp2 = CVECTOR(pFrustumPlanes[1].Nx, pFrustumPlanes[1].Ny, pFrustumPlanes[1].Nz);
	CVECTOR vp3 = CVECTOR(pFrustumPlanes[2].Nx, pFrustumPlanes[2].Ny, pFrustumPlanes[2].Nz);
	CVECTOR vp4 = CVECTOR(pFrustumPlanes[3].Nx, pFrustumPlanes[3].Ny, pFrustumPlanes[3].Nz);

	//if((v | pFrustumPlanes[0].n) < 0 ) vc |= 0x10;
	if ((v | vp1) < 0 ) vc |= 0x01;
	if ((v | vp2) < 0 ) vc |= 0x02;
	if ((v | vp3) < 0 ) vc |= 0x04;
	if ((v | vp4) < 0 ) vc |= 0x08;

	*/

	
	//Max - так точность значительно выше, блоки не должны пропадать иногда в далеке
	PLANE * p = &pFrustumPlanes[0];
	if(p->Nx*vP.x + p->Ny*vP.y + p->Nz*vP.z < p->D) vc |= 0x01;
	p = &pFrustumPlanes[1];
	if(p->Nx*vP.x + p->Ny*vP.y + p->Nz*vP.z < p->D) vc |= 0x02;
	p = &pFrustumPlanes[2];
	if(p->Nx*vP.x + p->Ny*vP.y + p->Nz*vP.z < p->D) vc |= 0x04;
	p = &pFrustumPlanes[3];
	if(p->Nx*vP.x + p->Ny*vP.y + p->Nz*vP.z < p->D) vc |= 0x08;

	return vc;
}

bool SEA::isVisibleBBox(const CVECTOR & vCenter, const CVECTOR & v1, const CVECTOR & v2)
{
	/*CVECTOR vc = vCenter - vCamPos;
	CVECTOR vp1 = v1 - vCamPos;
	CVECTOR vp2 = v2 - vCamPos;
	float fR2 = sqrtf(Sqr((vp1.x - vp2.x) * 0.5f) + Sqr((vp1.z - vp2.z)) * 0.5f);*/

	// if sphere not visible - return
//	for (dword i=0; i<dwNumFrustumPlanes; i++)
//		if ((pFrustumPlanes[i].n | vc) - pFrustumPlanes[i].d < -fR2) return false;

	// check box visible
	long vc = 0xFF;
	vc &= VisCode(CVECTOR(v1.x, v1.y, v1.z)); if (vc == 0) return true;
	vc &= VisCode(CVECTOR(v1.x, v2.y, v1.z)); 
	vc &= VisCode(CVECTOR(v2.x, v1.y, v1.z)); 
	vc &= VisCode(CVECTOR(v2.x, v2.y, v1.z)); if (vc == 0) return true;
	vc &= VisCode(CVECTOR(v1.x, v1.y, v2.z)); 
	vc &= VisCode(CVECTOR(v1.x, v2.y, v2.z)); 
	vc &= VisCode(CVECTOR(v2.x, v1.y, v2.z)); 
	vc &= VisCode(CVECTOR(v2.x, v2.y, v2.z)); 

	return vc == 0;
}

inline float SEA::CalcLod(const float & x, const float & y, const float & z)
{
	return Sqr(x - vCamPos.x) + /*Sqr((y - vCamPos.y)) + */Sqr(z - vCamPos.z);
}

void SEA::CalculateLOD(const CVECTOR & v1, const CVECTOR & v2, long & iMaxLOD, long & iMinLOD)
{
	float	fCur, fMax, fMin;

	fCur = CalcLod(v1.x, vSeaCenterPos.y, v1.z); fMax = fCur;
	fCur = CalcLod(v2.x, vSeaCenterPos.y, v1.z); if (fCur > fMax) fMax = fCur;
	fCur = CalcLod(v1.x, vSeaCenterPos.y, v2.z); if (fCur > fMax) fMax = fCur;
	fCur = CalcLod(v2.x, vSeaCenterPos.y, v2.z); if (fCur > fMax) fMax = fCur;

	if (vCamPos.x < v1.x)
	{
		if (vCamPos.z < v1.z)
			fMin = CalcLod(v1.x, vSeaCenterPos.y, v1.z);
		else
		if (vCamPos.z > v2.z)
			fMin = CalcLod(v1.x, vSeaCenterPos.y, v2.z);
		else
			fMin = CalcLod(v1.x, vSeaCenterPos.y, vCamPos.z);
	}
	else
	if (vCamPos.x > v2.x)
	{
		if (vCamPos.z < v1.z)
			fMin = CalcLod(v2.x, vSeaCenterPos.y, v1.z);
		else
		if (vCamPos.z > v2.z)
			fMin = CalcLod(v2.x, vSeaCenterPos.y, v2.z);
		else
			fMin = CalcLod(v2.x, vSeaCenterPos.y, vCamPos.z);
	}
	else
	{
		if (vCamPos.z < v1.z)
			fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, v1.z);
		else
		if (vCamPos.z > v2.z)
			fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, v2.z);
		else
			fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, vCamPos.z);
	}

	iMaxLOD = int(0.5f * logf(fLodScale * fMax) / logf(2)); 
	if (iMaxLOD < 4) iMaxLOD = 4;
	iMinLOD = int(0.5f * logf(fLodScale * fMin) / logf(2)); 
	if (iMinLOD < 4) iMinLOD = 4;
}

void SEA::AddBlock(long iTX, long iTY, long iSize, long iLOD)
{
	SeaBlock * pB = &aBlocks[aBlocks.Add()];

	pB->iTX = iTX;		
	pB->iTY = iTY;
	pB->iSize = iSize;
	pB->iLOD = iLOD;

	pB->bInProgress = false;
	pB->bDone = false;

	pB->iSize0 = iSize >> iLOD;
}

void SEA::BuildTree(long iTX, long iTY, long iLev)
{
	long	iMaxLOD, iMinLOD;
	long	iSize = long(dwMaxDim >> iLev);
	float	fGSize = fGridStep * iSize;

	CVECTOR v1 = vSeaCenterPos + CVECTOR(iTX * fGSize, -fMaxSeaHeight / 2.0f, iTY * fGSize);
	CVECTOR v2 = v1 + CVECTOR(fGSize, fMaxSeaHeight, fGSize);

	if (!isVisibleBBox(vSeaCenterPos, v1, v2)) return;

	CalculateLOD(v1, v2, iMaxLOD, iMinLOD);

	if (iSize <= long(dwMinDim) || iMaxLOD - iMinLOD <= 1)
	{
		AddBlock(iTX, iTY, iSize, iMinLOD);
		return;
	}

	iTX *= 2; 
	iTY *= 2; 
	iLev++;

	BuildTree(iTX    , iTY    , iLev);
	BuildTree(iTX + 1, iTY    , iLev);
	BuildTree(iTX    , iTY + 1, iLev);
	BuildTree(iTX + 1, iTY + 1, iLev);
}

inline void PrefetchNTA(dword dwAddress)
{
	/*_asm
	{
		mov	eax, dwAddress
		and esi, ~15d
		//add esi, 128d
		prefetchnta [esi]
	}*/
}

// INTEL COMMENT:
// This version of the function takes in 4 at a time, to take advantage of SSE.
// In particular I have converted the normalise and square roots over to SSE.
void SEA::SSE_WaveXZ(SeaVertex * * pArray)
{
	CVECTOR vNormal[4];
	float nY1[4];
	float nY2[4];
	bool bDistancePass = false;

	for( int i=0; i<4; i++ )
	{
		long iX11, iX12, iX21, iX22, iY11, iY12, iY21, iY22;

		float fDistance = Sqr(pArray[i]->vPos.x - vCamPos.x) + Sqr(pArray[i]->vPos.z - vCamPos.z);
		if( fDistance > 1600.0f * 1600.0f )
		{
			vNormal[i].x = 0.0f;
			nY1[i] = 1.0f;
			nY2[i] = 1.0f;
			vNormal[i].y = 1.0f;
			vNormal[i].z = 0.0f;
			pArray[i]->vPos.y = 0.0f;
			continue;
		}

		bDistancePass = true;

		float x1 = (pArray[i]->vPos.x + vMove1.x) * fScale1;
		float z1 = (pArray[i]->vPos.z + vMove1.z) * fScale1;
		iX11 = ffloor(x1 + 0.0f), iX12 = iX11 + 1; iY11 = ffloor(z1 + 0.0f), iY12 = iY11 + 1; 
		float fX1 = (x1 - iX11);
		float fZ1 = (z1 - iY11);
		iX11 &= (XWIDTH-1); iX12 &= (XWIDTH-1); iY11 &= (XWIDTH-1); iY12 &= (XWIDTH-1);

		float x2 = (pArray[i]->vPos.x + vMove2.x) * fScale2;
		float z2 = (pArray[i]->vPos.z + vMove2.z) * fScale2;
		iX21 = ffloor(x2 + 0.0f), iX22 = iX21 + 1; iY21 = ffloor(z2 + 0.0f), iY22 = iY21 + 1; 
		float fX2 = (x2 - iX21);
		float fZ2 = (z2 - iY21);
		iX21 &= (XWIDTH-1); iX22 &= (XWIDTH-1); iY21 &= (XWIDTH-1); iY22 &= (XWIDTH-1);

		float a1, a2, a3, a4;

		a1 = pSeaFrame1[iX11 + iY11 * XWIDTH]; a2 = pSeaFrame1[iX12 + iY11 * XWIDTH];
		a3 = pSeaFrame1[iX11 + iY12 * XWIDTH]; a4 = pSeaFrame1[iX12 + iY12 * XWIDTH];
		pArray[i]->vPos.y = fAmp1 * (a1 + fX1 * (a2 - a1) + fZ1 * (a3 - a1) + fX1 * fZ1 * (a4 + a1 - a2 - a3));

		a1 = pSeaFrame2[iX21 + iY21 * XWIDTH]; a2 = pSeaFrame2[iX22 + iY21 * XWIDTH];
		a3 = pSeaFrame2[iX21 + iY22 * XWIDTH]; a4 = pSeaFrame2[iX22 + iY22 * XWIDTH];
		pArray[i]->vPos.y += fAmp2 * (a1 + fX2 * (a2 - a1) + fZ2 * (a3 - a1) + fX2 * fZ2 * (a4 + a1 - a2 - a3));

		float nx1, nx2, nx3, nx4, nz1, nz2, nz3, nz4;

		nx1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 0]; nz1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 1];
		nx2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 0]; nz2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 1];
		nx3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 0]; nz3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 1];
		nx4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 0]; nz4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 1];

		float nX1 = (nx1 + fX1 * (nx2 - nx1) + fZ1 * (nx3 - nx1) + fX1 * fZ1 * (nx4 + nx1 - nx2 - nx3));
		float nZ1 = (nz1 + fX1 * (nz2 - nz1) + fZ1 * (nz3 - nz1) + fX1 * fZ1 * (nz4 + nz1 - nz2 - nz3));

		nx1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 0]; nz1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 1];
		nx2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 0]; nz2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 1];
		nx3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 0]; nz3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 1];
		nx4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 0]; nz4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 1];

		float nX2 = (nx1 + fX2 * (nx2 - nx1) + fZ2 * (nx3 - nx1) + fX2 * fZ2 * (nx4 + nx1 - nx2 - nx3));
		float nZ2 = (nz1 + fX2 * (nz2 - nz1) + fZ2 * (nz3 - nz1) + fX2 * fZ2 * (nz4 + nz1 - nz2 - nz3));

		nY1[i] = 1.0f - (Sqr(nX1) + Sqr(nZ1));
		nY2[i] = 1.0f - (Sqr(nX2) + Sqr(nZ2));

		vNormal[i].x = fScale1 * fAmp1 * nX1 + fScale2 * fAmp2 * nX2;
		vNormal[i].z = fScale1 * fAmp1 * nZ1 + fScale2 * fAmp2 * nZ2;
	}

	if( bDistancePass )
	{
		// INTEL COMMENT:
		// xmm variables.
		__m128 m128X, m128Y, m128Z;
		__m128 m128NX, m128NY, m128NZ;
		__m128 m128Y1, m128Y2;
		__m128 m128SY1, m128SY2;

		// INTEL COMMENT:
		// Gather the Y's.
		SSE_GatherFourFloats( &nY1[0], &nY1[1], &nY1[2], &nY1[3], &m128Y1 ); 
		SSE_GatherFourFloats( &nY2[0], &nY2[1], &nY2[2], &nY2[3], &m128Y2 ); 
		m128SY1 = _mm_sqrt_ps( m128Y1 );
		m128SY2 = _mm_sqrt_ps( m128Y2 );
		m128Y = _mm_add_ps( m128SY1, m128SY2 );

		// INTEL COMMENT:
		// Gather the X's and Z's.
		SSE_GatherFourFloats( &vNormal[0].x, &vNormal[1].x, &vNormal[2].x, &vNormal[3].x, &m128X ); 
		SSE_GatherFourFloats( &vNormal[0].z, &vNormal[1].z, &vNormal[2].z, &vNormal[3].z, &m128Z );

		// INTEL COMMENT:
		// Normalise.
		SSE_Normalise( m128X, m128Y, m128Z, &m128NX, &m128NY, &m128NZ );

		// INTEL COMMENT:
		// Scatter back out.
		SSE_ScatterFourFloats( &pArray[0]->vNormal.x, &pArray[1]->vNormal.x, &pArray[2]->vNormal.x, &pArray[3]->vNormal.x, m128NX ); 
		SSE_ScatterFourFloats( &pArray[0]->vNormal.y, &pArray[1]->vNormal.y, &pArray[2]->vNormal.y, &pArray[3]->vNormal.y, m128NY ); 
		SSE_ScatterFourFloats( &pArray[0]->vNormal.z, &pArray[1]->vNormal.z, &pArray[2]->vNormal.z, &pArray[3]->vNormal.z, m128NZ ); 
	}
	else
	{
		pArray[0]->vNormal = vNormal[0]; 
		pArray[1]->vNormal = vNormal[1]; 
		pArray[2]->vNormal = vNormal[2]; 
		pArray[3]->vNormal = vNormal[3]; 
	}
}

float __fastcall SEA::WaveXZ(float x, float z, CVECTOR * pNormal)
{
	long iX11, iX12, iX21, iX22, iY11, iY12, iY21, iY22;

	float fDistance = Sqr(x - vCamPos.x) + Sqr(z - vCamPos.z);
	if (fDistance > 1600.0f * 1600.0f)
	{
		if (pNormal) *pNormal = CVECTOR(0.0f, 1.0f, 0.0f);
		return 0.0f;
	}

	float x1 = (x + vMove1.x) * fScale1;
	float z1 = (z + vMove1.z) * fScale1;
	iX11 = ffloor(x1 + 0.0f), iX12 = iX11 + 1; iY11 = ffloor(z1 + 0.0f), iY12 = iY11 + 1; 
	float fX1 = (x1 - iX11);
	float fZ1 = (z1 - iY11);
	iX11 &= (XWIDTH-1); iX12 &= (XWIDTH-1); iY11 &= (XWIDTH-1); iY12 &= (XWIDTH-1);

	float x2 = (x + vMove2.x) * fScale2;
	float z2 = (z + vMove2.z) * fScale2;
	iX21 = ffloor(x2 + 0.0f), iX22 = iX21 + 1; iY21 = ffloor(z2 + 0.0f), iY22 = iY21 + 1; 
	float fX2 = (x2 - iX21);
	float fZ2 = (z2 - iY21);
	iX21 &= (XWIDTH-1); iX22 &= (XWIDTH-1); iY21 &= (XWIDTH-1); iY22 &= (XWIDTH-1);

	float a1, a2, a3, a4;

	a1 = pSeaFrame1[iX11 + iY11 * XWIDTH]; a2 = pSeaFrame1[iX12 + iY11 * XWIDTH];
	a3 = pSeaFrame1[iX11 + iY12 * XWIDTH]; a4 = pSeaFrame1[iX12 + iY12 * XWIDTH];
	float fRes = fAmp1 * (a1 + fX1 * (a2 - a1) + fZ1 * (a3 - a1) + fX1 * fZ1 * (a4 + a1 - a2 - a3));

	a1 = pSeaFrame2[iX21 + iY21 * XWIDTH]; a2 = pSeaFrame2[iX22 + iY21 * XWIDTH];
	a3 = pSeaFrame2[iX21 + iY22 * XWIDTH]; a4 = pSeaFrame2[iX22 + iY22 * XWIDTH];
	fRes += fAmp2 * (a1 + fX2 * (a2 - a1) + fZ2 * (a3 - a1) + fX2 * fZ2 * (a4 + a1 - a2 - a3));

	float nx1, nx2, nx3, nx4, nz1, nz2, nz3, nz4;

	nx1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 0]; nz1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 1];
	nx2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 0]; nz2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 1];
	nx3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 0]; nz3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 1];
	nx4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 0]; nz4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 1];

	float nX1 = (nx1 + fX1 * (nx2 - nx1) + fZ1 * (nx3 - nx1) + fX1 * fZ1 * (nx4 + nx1 - nx2 - nx3));
	float nZ1 = (nz1 + fX1 * (nz2 - nz1) + fZ1 * (nz3 - nz1) + fX1 * fZ1 * (nz4 + nz1 - nz2 - nz3));

	nx1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 0]; nz1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 1];
	nx2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 0]; nz2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 1];
	nx3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 0]; nz3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 1];
	nx4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 0]; nz4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 1];

	float nX2 = (nx1 + fX2 * (nx2 - nx1) + fZ2 * (nx3 - nx1) + fX2 * fZ2 * (nx4 + nx1 - nx2 - nx3));
	float nZ2 = (nz1 + fX2 * (nz2 - nz1) + fZ2 * (nz3 - nz1) + fX2 * fZ2 * (nz4 + nz1 - nz2 - nz3));
	//float nX2 = 0.0f;
	//float nZ2 = 0.0f;

	/*float fDistance = sqrt_ss(Sqr(x - vCamPos.x) + Sqr(fRes - vCamPos.y) + Sqr(z - vCamPos.z));
	{
		float distance_mul = 1.0f - fDistance / 1900.0f;
		if (distance_mul < 0.0f) distance_mul = 0.0f;

		fRes *= distance_mul;

		nX1 *= distance_mul;
		nZ1 *= distance_mul;

		nX2 *= distance_mul;
		nZ2 *= distance_mul;
	}*/

	if (pNormal)
	{
		/*pNormal->x = 0.0f;
		pNormal->y = 1.0f;
		pNormal->z = 0.0f;*/
		float nY1 = sqrtf(1.0f - (Sqr(nX1) + Sqr(nZ1)));
		float nY2 = sqrtf(1.0f - (Sqr(nX2) + Sqr(nZ2)));

		CVECTOR vNormal;

		vNormal.x = fScale1 * fAmp1 * nX1 + fScale2 * fAmp2 * nX2;
		vNormal.z = fScale1 * fAmp1 * nZ1 + fScale2 * fAmp2 * nZ2;
		vNormal.y = nY1 + nY2;

		//vNormal.x += 2.0f * vNormal.x;
		//vNormal.z += 2.0f * vNormal.z;
		vNormal = !vNormal;

		*pNormal = vNormal;
	}

	return fRes;
}

void SEA::PrepareIndicesForBlock(dword dwBlockIndex)
{
	SeaBlock * pB = &aBlocks[dwBlockIndex];

	float	fStep = fGridStep * float(1 << pB->iLOD);
	float	fSize = fGridStep * pB->iSize;
	long	x, y, size0 = pB->iSize >> pB->iLOD;

	pB->iIStart = iIStart;

	float x1 = float(pB->iTX * pB->iSize) * fGridStep;
	float y1 = float(pB->iTY * pB->iSize) * fGridStep;
	float x2 = x1 + float(size0) * fStep;
	float y2 = y1 + float(size0) * fStep;

	pB->iX1 = fftoi(x1 / fGridStep); 
	pB->iX2 = fftoi(x2 / fGridStep); 
	pB->iY1 = fftoi(y1 / fGridStep); 
	pB->iY2 = fftoi(y2 / fGridStep); 

	// analyse 
	long i, j;
	for (i=0; i<long(dwBlockIndex); i++) //if (i == 6)
	{
		SeaBlock * pB2 = &aBlocks[i];

		// Test Up & Down
		bool bTestedUp = pB->iY1 == pB2->iY2;
		bool bTestedDown = pB->iY2 == pB2->iY1;

		//if (!(GetAsyncKeyState('5')<0))
		if (bTestedUp || bTestedDown)
		{
			long iAddSrc = pB2->iIStart + ((bTestedUp) ? (pB2->iSize0 + 1) * pB2->iSize0 : 0);
			long iAddDst = pB->iIStart + ((bTestedUp) ? 0 : (pB->iSize0 + 1) * pB->iSize0);

			if ((pB->iX1 >= pB2->iX1 && pB->iX2 <= pB2->iX2) || (pB->iX1 <= pB2->iX1 && pB->iX2 >= pB2->iX2))
			{
				long iMinX = Max(pB->iX1, pB2->iX1);
				long iMaxX = Min(pB->iX2, pB2->iX2);
				
				long iStartDstX = pB->iSize0 * (iMinX - pB->iX1) / (pB->iX2 - pB->iX1);
				long iStartSrcX = pB2->iSize0 * (iMinX - pB2->iX1) / (pB2->iX2 - pB2->iX1);

				long iEndDstX = pB->iSize0 * (iMaxX - pB->iX1) / (pB->iX2 - pB->iX1);
				long iEndSrcX = pB2->iSize0 * (iMaxX - pB2->iX1) / (pB2->iX2 - pB2->iX1);

				if (pB->iLOD == pB2->iLOD)
					for (j=iStartDstX; j<=iEndDstX; j++) 
						pIndices[iAddDst + j] = pIndices[iAddSrc + iStartSrcX + (j - iStartDstX)];
				else
					for (j=iStartDstX; j<=iEndDstX; j++) 
						pIndices[iAddDst + j] = pIndices[iAddSrc + iStartSrcX + (j - iStartDstX) / 2];

				continue;
			}
		}

		// Test Left & Right
		bool bTestedLeft = pB->iX1 == pB2->iX2;
		bool bTestedRight = pB->iX2 == pB2->iX1;
		//if ((GetAsyncKeyState('6')<0))
		if (bTestedLeft || bTestedRight)
		{
			long iAddSrc = pB2->iIStart + ((bTestedLeft) ? (pB2->iSize0) : 0);
			long iAddDst = pB->iIStart + ((bTestedLeft) ? 0 : (pB->iSize0));

			if ((pB->iY1 >= pB2->iY1 && pB->iY2 <= pB2->iY2) || (pB->iY1 <= pB2->iY1 && pB->iY2 >= pB2->iY2))
			{
				long iMinY = Max(pB->iY1, pB2->iY1);
				long iMaxY = Min(pB->iY2, pB2->iY2);
				
				long iStartDstY = pB->iSize0 * (iMinY - pB->iY1) / (pB->iY2 - pB->iY1);
				long iStartSrcY = pB2->iSize0 * (iMinY - pB2->iY1) / (pB2->iY2 - pB2->iY1);

				long iEndDstY = pB->iSize0 * (iMaxY - pB->iY1) / (pB->iY2 - pB->iY1);
				long iEndSrcY = pB2->iSize0 * (iMaxY - pB2->iY1) / (pB2->iY2 - pB2->iY1);

				if (pB->iLOD == pB2->iLOD)
					for (j=iStartDstY; j<=iEndDstY; j++) 
						pIndices[iAddDst + j * (pB->iSize0 + 1)] = pIndices[iAddSrc + (iStartSrcY + j - iStartDstY) * (pB2->iSize0 + 1)];
				else
					for (j=iStartDstY; j<=iEndDstY; j++) 
						pIndices[iAddDst + j * (pB->iSize0 + 1)] = pIndices[iAddSrc + (iStartSrcY + (j - iStartDstY) / 2) * (pB2->iSize0 + 1)];

				continue;
			}
		}
	}

	long iIFirst = 1000000, iILast = -1000000;

	for (y=0; y<=size0; y++)
		for (x=0; x<=size0; x++)
		{
			if (pIndices[iIStart] != dword(-1))
			{
				iIStart++;
				continue;
			}

			if (iVStart < iIFirst) iIFirst = iVStart;
			if (iVStart > iILast) iILast = iVStart;

			pIndices[iIStart++] = dword(iVStart);
			iVStart++;
		}

	pB->iIFirst = iIFirst;
	pB->iILast = iILast;

	// setup triangles
	long yy, dyy = size0 + 1;
	for(y=0, yy = 0; y<size0; y++, yy += dyy)
		for(x=0; x<size0; x++) 
		{
			// first triangle
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy + dyy + 1)];
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy + 1)];
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy)];
			// second triangle
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy + dyy)];
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy + dyy + 1)];
			*pTriangles++ = (word)pIndices[pB->iIStart + dword(x + yy)];
			
			iTStart += 2;
		}
}

void SEA::SSE_WaveXZBlock(SeaBlock * pB)
{
	//return;
	if (!pB) return;

	SeaVertex	* vTmp[4];
	SeaVertex	vFake;

	vFake.vPos.x = vCamPos.x + 1e+5f;
	vFake.vPos.z = vCamPos.z + 1e+5f;

	float	cx, cz, fStep = fGridStep * float(1 << pB->iLOD);
	float	fSize = fGridStep * pB->iSize;
	long	x, y, size0 = pB->iSize >> pB->iLOD;
	
	float x1 = float(pB->iTX * pB->iSize) * fGridStep;
	float y1 = float(pB->iTY * pB->iSize) * fGridStep;
	float x2 = x1 + float(size0) * fStep;
	float y2 = y1 + float(size0) * fStep;

	pB->iX1 = fftoi(x1 / fGridStep); 
	pB->iX2 = fftoi(x2 / fGridStep); 
	pB->iY1 = fftoi(y1 / fGridStep); 
	pB->iY2 = fftoi(y2 / fGridStep); 

	x1 += vSeaCenterPos.x;	x2 += vSeaCenterPos.x;
	y1 += vSeaCenterPos.z;	y2 += vSeaCenterPos.z;

	//CVECTOR vNormal, vTmp;
	long iCurrentV = 0;

	long iIStart1 = pB->iIStart;
	long iIFirst = pB->iIFirst;
	long iILast = pB->iILast;

	// calculate
	for (cz = y1, y=0; y<=size0; y++, cz+=fStep)
	{
		for (cx = x1, x=0; x<=size0; x++, cx+=fStep)
		{
			dword dwVIndex = pIndices[iIStart1];

			if (long(dwVIndex) < iIFirst || long(dwVIndex) > iILast)
			{
				iIStart1++;
				continue;
			}

			vTmp[iCurrentV] = &pVSea[dwVIndex];
			vTmp[iCurrentV]->vPos.x = cx;
			vTmp[iCurrentV]->vPos.z = cz;
			
			iCurrentV++;

			if (iCurrentV == 4)
			{
				SSE_WaveXZ(&vTmp[0]);
				iCurrentV = 0;

				for (long i=0; i<4; i++)
				{
					vTmp[i]->tu = (vTmp[i]->vPos.x - vTmp[i]->vNormal.x * 5.3f) * fBumpScale;
					vTmp[i]->tv = (vTmp[i]->vPos.z - vTmp[i]->vNormal.z * 5.3f) * fBumpScale;
					vTmp[i]->vPos.x -= vTmp[i]->vNormal.x * fPosShift * 3.0f + vWorldOffset.x;
					vTmp[i]->vPos.z -= vTmp[i]->vNormal.z * fPosShift * 3.0f + vWorldOffset.z;
				}
			}
			
			/*vTmp.y = WaveXZ(cx, cz, &vNormal);

			vTmp.x = cx - vNormal.x * fPosShift * 3.0f;
			vTmp.z = cz - vNormal.z * fPosShift * 3.0f;

			pVSea[dwVIndex].vPos = vTmp;
			pVSea[dwVIndex].vNormal = vNormal;
			pVSea[dwVIndex].tu = (cx - vNormal.x * 5.3f) * fBumpScale;
			pVSea[dwVIndex].tv = (cz - vNormal.z * 5.3f) * fBumpScale;*/

			iIStart1++;
		}
	}

	if (iCurrentV != 0) 
	{
		for (long i=iCurrentV; i<4; i++) vTmp[i] = &vFake;

		SSE_WaveXZ(&vTmp[0]);
		for (long i=0; i<iCurrentV; i++)
		{
			vTmp[i]->tu = (vTmp[i]->vPos.x - vTmp[i]->vNormal.x * 5.3f) * fBumpScale;
			vTmp[i]->tv = (vTmp[i]->vPos.z - vTmp[i]->vNormal.z * 5.3f) * fBumpScale;

			vTmp[i]->vPos.x -= vTmp[i]->vNormal.x * fPosShift * 3.0f + vWorldOffset.x;
			vTmp[i]->vPos.z -= vTmp[i]->vNormal.z * fPosShift * 3.0f + vWorldOffset.z;
		}
	}

	pB->bDone = true;

	EnterCriticalSection(&cs1);
	iBlocksDoneNum++;
	LeaveCriticalSection(&cs1);
}

void SEA::WaveXZBlock(SeaBlock * pB)
{
	//return;
	if (!pB) return;

	float	cx, cz, fStep = fGridStep * float(1 << pB->iLOD);
	float	fSize = fGridStep * pB->iSize;
	long	x, y, size0 = pB->iSize >> pB->iLOD;
	
	float x1 = float(pB->iTX * pB->iSize) * fGridStep;
	float y1 = float(pB->iTY * pB->iSize) * fGridStep;
	float x2 = x1 + float(size0) * fStep;
	float y2 = y1 + float(size0) * fStep;

	pB->iX1 = fftoi(x1 / fGridStep); 
	pB->iX2 = fftoi(x2 / fGridStep); 
	pB->iY1 = fftoi(y1 / fGridStep); 
	pB->iY2 = fftoi(y2 / fGridStep); 

	x1 += vSeaCenterPos.x;	x2 += vSeaCenterPos.x;
	y1 += vSeaCenterPos.z;	y2 += vSeaCenterPos.z;

	CVECTOR vNormal, vTmp;

	long iIStart1 = pB->iIStart;
	long iIFirst = pB->iIFirst;
	long iILast = pB->iILast;

	// calculate
	for (cz = y1, y=0; y<=size0; y++, cz+=fStep)
	{
		for (cx = x1, x=0; x<=size0; x++, cx+=fStep)
		{
			dword dwVIndex = pIndices[iIStart1];

			if (long(dwVIndex) < iIFirst || long(dwVIndex) > iILast)
			{
				iIStart1++;
				continue;
			}
			
			vTmp.y = WaveXZ(cx, cz, &vNormal);

			vTmp.x = cx - vNormal.x * fPosShift * 3.0f - vWorldOffset.x;
			vTmp.z = cz - vNormal.z * fPosShift * 3.0f - vWorldOffset.z;

			pVSea[dwVIndex].vPos = vTmp;
			pVSea[dwVIndex].vNormal = vNormal;
			pVSea[dwVIndex].tu = (cx - vNormal.x * 5.3f) * fBumpScale;
			pVSea[dwVIndex].tv = (cz - vNormal.z * 5.3f) * fBumpScale;

			iIStart1++;
		}
	}

	pB->bDone = true;

	EnterCriticalSection(&cs1);
	iBlocksDoneNum++;
	LeaveCriticalSection(&cs1);
}


SEA::SeaBlock * SEA::GetUndoneBlock()
{
	SeaBlock * pB = null;
	for (long i=0; i<aBlocks; i++) if (!aBlocks[i].bInProgress)
	{
		pB = &aBlocks[i];
		pB->bInProgress = true;
		return pB;
	}
	return pB;
}

dword SEA::ThreadExecute(long iThreadIndex)
{
	HANDLE hHandles[] = {SEA::pSea->hEventCalcMaps, SEA::pSea->aEventCalcBlock[iThreadIndex]};

	while (true)
	{
		dword dwValue = WaitForMultipleObjects(ARRSIZE(hHandles), hHandles, false, INFINITE);

		if (dwValue >= WAIT_OBJECT_0 && dwValue < WAIT_OBJECT_0 + ARRSIZE(hHandles))
		{
			HANDLE hValue = hHandles[dwValue - WAIT_OBJECT_0];
			
			if (hValue == SEA::pSea->hEventCalcMaps)
			{
			}

			if (hValue == SEA::pSea->aEventCalcBlock[iThreadIndex])
			{
				while (true)
				{
					EnterCriticalSection(&SEA::pSea->cs);
					SeaBlock * pB = SEA::pSea->GetUndoneBlock();
					LeaveCriticalSection(&SEA::pSea->cs);

					if (!pB) break;

					//if (SEA::bIntel && SEA::bSSE) // fix AMD
					if (SEA::bSSE) SEA::pSea->SSE_WaveXZBlock(pB); else SEA::pSea->WaveXZBlock(pB);

					SEA::pSea->iB2++;
					SEA::pSea->aThreadsTest[iThreadIndex]++;
				}
			}
		}
	}
}

void SEA::CalculateNormalMap(float fFrame, float fAmplitude, float * pfOut, array<dword*> & aFrames)
{
	long iFrame1 = fftol(fFrame) % aFrames.Len();
	long iFrame2 = (iFrame1 + 1) % aFrames.Len();

	float fDelta = fFrame - iFrame1;

	dword * pB1 = aFrames[iFrame1];
	dword * pB2 = aFrames[iFrame2];

	for (long y=0; y<YWIDTH; y++)
		for (long x=0; x<XWIDTH; x++)
		{
			dword dw1 = pB1[x + y * XWIDTH];
			dword dw2 = pB2[x + y * XWIDTH];
			float nx1 = float(short(dw1)) / 32767.5f;
			float nx2 = float(short(dw2)) / 32767.5f;
			float nz1 = float(short(dw1 >> 0x10)) / 32767.5f;
			float nz2 = float(short(dw2 >> 0x10)) / 32767.5f;

			pfOut[2 * (x + y * XWIDTH) + 0] = (nx1 + (nx2 - nx1) * fDelta);
			pfOut[2 * (x + y * XWIDTH) + 1] = (nz1 + (nz2 - nz1) * fDelta);
		}
}

void SEA::CalculateHeightMap(float fFrame, float fAmplitude, float * pfOut, array<byte*> & aFrames)
{
	long iFrame1 = fftol(fFrame) % aFrames.Len();
	long iFrame2 = (iFrame1 + 1) % aFrames.Len();

	float fDelta = fFrame - iFrame1;

	byte * pB1 = aFrames[iFrame1];
	byte * pB2 = aFrames[iFrame2];

	for (long y=0; y<YWIDTH; y++)
		for (long x=0; x<XWIDTH; x++)
		{
			float f1 = pB1[x + y * XWIDTH];
			float f2 = pB2[x + y * XWIDTH];

			pfOut[x + y * XWIDTH] = fAmplitude * (f1 + (f2 - f1) * fDelta);
		}
}

void SEA::Realize(dword dwDeltaTime)
{
	static float fTmp = 0.0f;

	if (api->Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 && api->Controls->GetDebugAsyncKeyState('S') < 0)
	{
		if (bTempFullMode)
		{
			bTempFullMode = false;
			fGridStep = fTempGridStep;
			fLodScale = fTempLodScale;
		}
		else
		{
			bTempFullMode = true;
			fTempGridStep = fGridStep;
			fTempLodScale = fLodScale;

			fGridStep = 0.07f;
			fLodScale = 0.5f;
		}

		Sleep(100);
	}

	if (bStop) return;

	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	vMove1 += fDeltaTime * vMoveSpeed1;
	vMove2 += fDeltaTime * vMoveSpeed2;

	fFrame1 += fDeltaTime * fAnimSpeed1;
	fFrame2 +=  fDeltaTime * fAnimSpeed2;
	while (fFrame1 >= FRAMES) fFrame1 -= FRAMES;
	while (fFrame2 >= FRAMES) fFrame2 -= FRAMES;

	//pSeaParameters->SetTexture(0, pAnimTexture);

	dword dwTotalRDTSC;
	RDTSC_B(dwTotalRDTSC);
	
	CMatrix mView, mIView;
	Render().GetTransform(D3DTS_VIEW, mView);
	mIView = mView; mIView.Transposition();

	vCamPos = mIView.Pos();
#ifdef OLD_WORLD_POS
	vWorldOffset = 0.0f;
#else
	vWorldOffset = vCamPos;
	vWorldOffset.y = 0.f;
#endif

	bUnderSea = false;

	if (vCamPos.y < 0.0f)
		bUnderSea = true;
	
	if (vCamPos.y < WaveXZ(vCamPos.x, vCamPos.z && bStarted))
		bUnderSea = true;

	if (!bUnderSea) bUnderSeaStarted = false;

	if (bSimpleSea)
	{
		EnvMap_Render2();
		SunRoad_Render2();
	}
	else
	{
		EnvMap_Render();
		SunRoad_Render();
	}

	pFrustumPlanes = Render().GetPlanes();

	float fBlockSize = 256.0f * fGridStep;
	long iNumBlocks = (long)dwMaxDim / (256 * 2);
	vSeaCenterPos = CVECTOR(fBlockSize * (long(vCamPos.x / fBlockSize) - iNumBlocks), fMaxSeaHeight * 0.5f, fBlockSize * (long(vCamPos.z / fBlockSize) - iNumBlocks) );

	iB1 = 0;
	iB2 = 0;

	iVStart = 0;
	iTStart = 0;
	iIStart = 0;

	if (!pVolumeTexture && aBumpMaps.Size())
	{
		IDirect3DSurface8	* pFace;

		static float fBumpMapFrame = 0.0f;
		fBumpMapFrame += float(fDeltaTime) * fBumpSpeed * 48.0f;

		dword dw1 = long(fBumpMapFrame) % aBumpMaps.Size();
		dword dw2 = long(fBumpMapFrame + 1.0f) % aBumpMaps.Size();

		float fAlpha = 255.0f * (fBumpMapFrame - float(long(fBumpMapFrame)));
		Render().SetRenderState(D3DRS_TEXTUREFACTOR, ARGB(fAlpha, 0, 0, 0));

		Render().SetTexture(0, aBumpMaps[dw1]);
		Render().SetTexture(1, aBumpMaps[dw2]);

		Render().EndScene();
		Render().PushRenderTarget();
		for (dword i=0; i<MIPSLVLS; i++)
		{
			HRESULT hr = pRenderTargetBumpMap->GetSurfaceLevel(i, &pFace);
			Render().SetRenderTarget(pFace, null);
			pFace->Release();
			float w, h;
			w = h = float(XWIDTH >> i);

			Render().BeginScene();

			RS_SPRITE spr[4];
			FillSpriteVertex(spr[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
			FillSpriteVertex(spr[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
			FillSpriteVertex(spr[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
			FillSpriteVertex(spr[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);

			Render().DrawSprites(spr, 1, "bump_interpolate");
	
			Render().EndScene();
		}
		Render().PopRenderTarget();
		Render().BeginScene();
	}

	memset(pIndices, 0xFF, NUM_VERTEXS * sizeof(pIndices[0]) * 3);

	dword dwX;
	RDTSC_B(dwX);
	//SetEvent(hEventCalcMaps);
	CalculateHeightMap(fFrame1, 1.0f / 255.0f, pSeaFrame1, aBumps);
	CalculateNormalMap(fFrame1, 1.0f / 255.0f, pSeaNormalsFrame1, aNormals);

	CalculateHeightMap(fFrame2, 1.0f / 255.0f, pSeaFrame2, aBumps);
	CalculateNormalMap(fFrame2, 1.0f / 255.0f, pSeaNormalsFrame2, aNormals);
	RDTSC_E(dwX);
	//api->Trace("dwX = %d", dwX);

	aBlocks.Empty();
	BuildTree(0, 0, 0);
	aBlocks.QSort(SeaBlock::QSort);

	dword	i;
	long	iNumVPoints = 0;
	for (i=0; i<aBlocks(); i++) 
	{
		iNumVPoints += aBlocks[i].iSize0 * aBlocks[i].iSize0;
		if (iNumVPoints >= NUM_VERTEXS)
		{
			aBlocks.DelRange(i, aBlocks.Last());
			break;
		}
	}

	SeaVertex * pVSea2 = (SeaVertex*)Render().LockVertexBuffer(iVSeaBuffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
	pTriangles = (word*)Render().LockIndexBuffer(iISeaBuffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);

	for (i=0; i<aBlocks(); i++) PrepareIndicesForBlock(i);
	//for (i=0; i<aBlocks(); i++) SetBlock(i);

	iBlocksDoneNum = 0;

	bool bHT = false;

	dword dwBlockRDTSC;
	RDTSC_B(dwBlockRDTSC);

	if (bHyperThreading)			// P4 / PRESCOTT Version
	{
		bHT = true;

		for (long i=0; i<aEventCalcBlock; i++)
		{
			aThreadsTest[i] = 0;
			SetEvent(aEventCalcBlock[i]);
		}

		while (true)
		{
			EnterCriticalSection(&cs);
			SeaBlock * pB = GetUndoneBlock();
			LeaveCriticalSection(&cs);
			if (!pB) break;
			//if (bIntel && bSSE) // fix AMD
			if (bSSE) SSE_WaveXZBlock(pB); else WaveXZBlock(pB);
			iB1++;
		}

		while (iBlocksDoneNum < aBlocks)
			_mm_pause();
	}
	else
	{
		while (true)
		{
			SeaBlock * pB = GetUndoneBlock();
			if (!pB) break;
			//if (bIntel && bSSE) // fix AMD
			if (bSSE) SSE_WaveXZBlock(pB); else WaveXZBlock(pB);
			iB1++;
		}
	}
	RDTSC_E(dwBlockRDTSC);

	if (iVStart && iTStart)
		memcpy(pVSea2, pVSea, iVStart * sizeof(SeaVertex));

	Render().UnLockVertexBuffer(iVSeaBuffer);
	Render().UnLockIndexBuffer(iISeaBuffer);

	if (iVStart && iTStart)
	{
		CMatrix mWorldView, mWorldViewProj;

		CMatrix mView = Render().GetView();
		CMatrix mWorld; mWorld.SetIdentity();// = Render().GetWorld();
		CMatrix mProjection = Render().GetProjection();

#ifndef OLD_WORLD_POS
		mView.MoveInversePosition(-vWorldOffset.x,0.f,-vWorldOffset.z);
#endif

		mWorldView.EqMultiply(mWorld, mView);
		mWorldViewProj.EqMultiply(mWorldView, mProjection);

		mWorldViewProj.Transposition4x4();

		fTmp += fDeltaTime * fBumpSpeed;
		while (fTmp >= 1.0f) fTmp -= 1.0f;

		Render().SetVertexShaderConstant(GC_CONSTANT, &CVECTOR4(0.0f, 1.0f, 0.5f, -0.04f), 1);
		Render().SetVertexShaderConstant(GC_CONSTANT2, &CVECTOR4(2.0f, -1.0f, 0.00036621652552071f, (bFogEnable) ? fFogSeaDensity : 0.0f), 1);
		Render().SetVertexShaderConstant(GC_SHADOW_CONST1, &CVECTOR4(fFoamV, fFoamK, fFoamUV, 6.0f), 1);
		Render().SetVertexShaderConstant(GC_ANIMATION, &CVECTOR4(fTmp, fTmp, fTmp, fTmp), 1);
		Render().SetVertexShaderConstant(GC_CAMERA_POS, &CVECTOR4(vCamPos.x-vWorldOffset.x, vCamPos.y, vCamPos.z-vWorldOffset.z, 1.0f), 1);
		Render().SetVertexShaderConstant(GC_MTX_WVP, &mWorldViewProj, 4);

		Render().SetVertexShaderConstant(GC_FREE, &v4SeaParameters, 1);
		Render().SetVertexShaderConstant(GC_FREE + 1, &v4SeaColor, 1);
		Render().SetVertexShaderConstant(GC_FREE + 2, &v4SkyColor, 1);

		Render().SetVertexShaderConstant(GC_FREE + 5, &CVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), 1);
		Render().SetVertexShaderConstant(GC_FREE + 6, &CVECTOR4(fFrenel, 1.0f, 0.5f, 1.0f), 1);		// Frenel K, Frenel Max
		Render().SetVertexShaderConstant(GC_FREE + 7, &CVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), 1);

		CVECTOR vTmp = !CVECTOR(0.0f, 1.0f, 0.0f);
		Render().SetVertexShaderConstant(GC_FREE + 30, &CVECTOR4(vTmp.x, vTmp.y, vTmp.z, 1.0f), 1);

		if (bSimpleSea)
		{
			Render().SetVertexShaderConstant(GC_FREE + 8, &mTexProjection, 4);			// Matrix!!

			//Render().SetTexture(0, pVolumeTexture);
			Render().SetTexture(0, (pVolumeTexture) ? (IDirect3DBaseTexture8*)pVolumeTexture : (IDirect3DBaseTexture8*)pRenderTargetBumpMap);
			Render().SetTexture(1, pReflection);
			//Render().SetTexture(2, pVolumeTexture);
			Render().SetTexture(2, (pVolumeTexture) ? (IDirect3DBaseTexture8*)pVolumeTexture : (IDirect3DBaseTexture8*)pRenderTargetBumpMap);
			Render().SetTexture(3, pReflectionSunroad);

			Render().SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(0.08f));
			Render().SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
			Render().SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
			Render().SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(0.08f));

			Render().SetTextureStageState(3, D3DTSS_BUMPENVMAT00, F2DW(0.05f));
			Render().SetTextureStageState(3, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
			Render().SetTextureStageState(3, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
			Render().SetTextureStageState(3, D3DTSS_BUMPENVMAT11, F2DW(0.05f));

			Render().DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0, iVStart, 0, iTStart, "Sea3");
		}
		else
		{
			D3DCAPS8 d3dCaps;
			Render().GetDeviceCaps(&d3dCaps);
			dword dwPSVersionLo = LOBYTE(d3dCaps.PixelShaderVersion);
			dword dwPSVersionHi = HIBYTE(d3dCaps.PixelShaderVersion);

			Render().SetVertexShaderConstant(GC_FREE + 8, &CMatrix(0.0f, 0.0f, PId2), 4);			// Matrix!!

			Render().SetTexture(0, (pVolumeTexture) ? (IDirect3DBaseTexture8*)pVolumeTexture : (IDirect3DBaseTexture8*)pRenderTargetBumpMap);
			Render().SetTexture(3, pEnvMap);
			Render().DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0, iVStart, 0, iTStart, "Sea2");

			if (fFoamK > 0.0f && bFoamEnable && bIniFoamEnable && dwPSVersionHi >= 1 && dwPSVersionLo >= 4)
			{
				//Render sea foam
				Render().SetPixelShaderConstant(0, &CVECTOR4(fFoamTextureDisturb, 0.0f, 0.0f, 0.0f), 1);

				Render().TextureSet(0, iFoamTexture);
				Render().SetTexture(4, (pVolumeTexture) ? (IDirect3DBaseTexture8*)pVolumeTexture : (IDirect3DBaseTexture8*)pRenderTargetBumpMap);
				Render().DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0, iVStart, 0, iTStart, "Foam_14");
			}

			Render().SetTexture(0, (pVolumeTexture) ? (IDirect3DBaseTexture8*)pVolumeTexture : (IDirect3DBaseTexture8*)pRenderTargetBumpMap);
			Render().SetTexture(3, pSunRoadMap);
			Render().DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0, iVStart, 0, iTStart, "Sea2_SunRoad");
		}
	}

	RDTSC_E(dwTotalRDTSC);
	//Render().Print(50, 300, "Total ticks with rendering %d", /*iVStart, iTStart, */dwTotalRDTSC);
	/*Render().Print(50, 320, "calc blk%s: %d", (bHT) ? " (HT)" : "", dwBlockRDTSC);
	Render().Print(50, 340, "Blocks in 1st thread: %d", iB1);
	for (long i=0; i<aThreadsTest; i++)
		Render().Print(50, 360 + 20 * i, "Blocks in thread %d: %d", i + 1, aThreadsTest[i]);
	*/
	//Render().Print(30, 140, "rdtsc = %d", dwBlockRDTSC);
	//Render().Print(30, 160, "Intel CPU: %s, SSE: %s, HyperThreading: %s", (bIntel) ? "Yes" : "No", (bSSE) ? "On" : "Off", (bHyperThreading) ? "On" : "Off");

	/*D3DVIEWPORT8 vp; Render().GetViewport(&vp);
	float w = 256;
	float h = 256;
	RS_SPRITE spr[4];
	FillSpriteVertex(spr[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
	FillSpriteVertex(spr[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
	FillSpriteVertex(spr[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
	FillSpriteVertex(spr[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);
	//Render().SetTexture(0, pReflection);
	Render().SetTexture(0, pReflectionSunroad);
	Render().DrawSprites(spr, 1, "Telescope");*/

	if (bUnderSea && bUnderSeaEnable)
	{
		CVECTOR d(20.0f, 5.0f, 20.0f);
		CVECTOR d2(54.0f, 10.0f, 54.0f);
		if (!bUnderSeaStarted)
		{
			// делаем кучку планктона первый раз
			aSeaTrash.Empty();
			for (long i=0; i<1000; i++)
			{
				SeaTrash & st = aSeaTrash[aSeaTrash.Add()];
				st.vPos = vCamPos + CVECTOR(RRnd(-d.x, d.x), RRnd(-d.y, d.y), RRnd(-d.z, d.z));
				st.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
				st.dwSubTexture = rand() % 4;
				st.fSize = RRnd(0.01f, 0.05f);
				st.fTime = float(i) * 0.02f;
			}

			// делаем кучку световых столбов первый раз
			aSeaLights.Empty();
			for (long i=0; i<50; i++)
			{
				SeaLight & sl = aSeaLights[aSeaLights.Add()];
				sl.vPos = vCamPos + CVECTOR(RRnd(-d2.x, d2.x), RRnd(-d2.y, d2.y), RRnd(-d2.z, d2.z));
				sl.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
				sl.dwSubTexture = rand() % 4;
				sl.fTime = float(i);
			}
		}

		// добавляем 1 плактонинку каждую секунду
		fLastTrashTime += fDeltaTime;
		while (fLastTrashTime >= 0.03f)
		{
			fLastTrashTime -= 0.03f;

			SeaTrash & st = aSeaTrash[aSeaTrash.Add()];
			st.vPos = vCamPos + CVECTOR(RRnd(-d.x, d.x), RRnd(-d.y, d.y), RRnd(-d.z, d.z));
			st.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
			st.dwSubTexture = rand() % 4;
			st.fTime = 20.0f;
			st.fSize = RRnd(0.01f, 0.05f);
		}

		// Уничтожаем старые плактонинки
		for (long i=0; i<aSeaTrash.Len(); i++)
		{
			aSeaTrash[i].fTime -= fDeltaTime;
			if (aSeaTrash[i].fTime <= 0.0f)
			{
				aSeaTrash.ExtractNoShift(i);
				i--;
				continue;
			}
			aSeaTrash[i].vPos += aSeaTrash[i].vSpeed * fDeltaTime;
		}
		// Рисуем планктон
		aTrashRects.Empty();
		for (long i=0; i<aSeaTrash.Len(); i++)
		{
			RS_RECT & r = aTrashRects[aTrashRects.Add()];

			float fAlpha = 1.0f;
			if (aSeaTrash[i].fTime >= 45.0f)
				fAlpha = 1.0f - (aSeaTrash[i].fTime - 45.0f) / 5.0f;
			if (aSeaTrash[i].fTime <= 5.0f)
				fAlpha = 1.0f - (5.0f - aSeaTrash[i].fTime) / 5.0f;

			r.vPos = aSeaTrash[i].vPos;
			r.fSize = aSeaTrash[i].fSize;
			r.fAngle = 0.0f;
			r.dwColor = ARGB(fAlpha * 255.0f, 255, 255, 255);
			r.dwSubTexture = aSeaTrash[i].dwSubTexture;
		}

		if (aTrashRects.Len())
		{
			Render().TextureSet(0, iSeaTrashTexture);
			Render().DrawRects(&aTrashRects[0], aTrashRects.Size(), "seatrash", 2, 2);
		}

		// Рисуем стаи рыб

		// Рисуем световые столбы
		// добавляем 1 световой столб каждую секунду
		fLastLightTime += fDeltaTime;
		while (fLastLightTime >= 1.0f)
		{
			fLastLightTime -= 1.0f;

			SeaLight & sl = aSeaLights[aSeaLights.Add()];
			sl.vPos = vCamPos + CVECTOR(RRnd(-d2.x, d2.x), RRnd(-d2.y, d2.y), RRnd(-d2.z, d2.z));
			sl.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
			sl.dwSubTexture = rand() % 4;
			sl.fTime = 50.0f;
		}

		// Уничтожаем старые световые столбики
		for (long i=0; i<aSeaLights.Len(); i++)
		{
			aSeaLights[i].fTime -= fDeltaTime;
			if (aSeaLights[i].fTime <= 0.0f)
			{
				aSeaLights.ExtractNoShift(i);
				i--;
				continue;
			}
			aSeaLights[i].vPos += aSeaLights[i].vSpeed * fDeltaTime;
		}
		
		float fAlphaK = (vCamPos.y < 0.0f) ? Max(0.0f, 1.0f + vCamPos.y / 30.0f) : 1.0f;

		// Рисуем световые столбики
		aLightsRects.Empty();
		for (long i=0; i<aSeaLights.Len(); i++)
		{
			RS_RECT & r = aLightsRects[aLightsRects.Add()];

			float fAlpha = 1.0f;
			if (aSeaLights[i].fTime >= 45.0f)
				fAlpha = 1.0f - (aSeaLights[i].fTime - 45.0f) / 5.0f;
			if (aSeaLights[i].fTime <= 5.0f)
				fAlpha = 1.0f - (5.0f - aSeaLights[i].fTime) / 5.0f;

			CVECTOR v = aSeaLights[i].vPos;
			r.vPos = CVECTOR(v.x, WaveXZ(v.x, v.z) - 5.0f, v.z);
			r.fSize = 20.0f;
			r.fAngle = 0.0f;
			r.dwColor = ARGB(0.07f * fAlphaK * fAlpha * 255.0f, 227, 245, 153);
			r.dwSubTexture = aSeaLights[i].dwSubTexture;
		}

		if (aLightsRects.Len())
		{
			Render().TextureSet(0, iSeaLightTexture);
			Render().DrawRects(&aLightsRects[0], aLightsRects.Size(), "seatrash", 2, 2, 0.5f);
		}

		bUnderSeaStarted = true;
	}


	bStarted = true;
}

float SEA::Trace(const CVECTOR & vSrc, const CVECTOR & vDst)
{
	long iNumTests = 5;
	float fRes = 2.0f;
	float fDV = 1.0f / float(iNumTests - 1);

	if (vSrc.y > fMaxSeaHeight && vDst.y > fMaxSeaHeight) return 2.0f;

	for (long i=0; i<iNumTests; i++)
	{
		CVECTOR vTemp = vSrc + float(i) * fDV * (vDst - vSrc);
		float fWaveY = WaveXZ(vTemp.x, vTemp.z, null);

		if (fWaveY > vTemp.y) return float(i) * fDV;
	}

	return 2.0f;
}

float SEA::Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst) 
{ 
	float fRes = Trace(vSrc, vDst);
	
	if (fRes <= 1.0f)
	{
		CVECTOR vTemp = vSrc + fRes * (vDst - vSrc);
		float fTmpY = WaveXZ(vTemp.x, vTemp.z, null);
		api->Event(BALL_WATER_HIT, "lfff", iBallOwner, vTemp.x, fTmpY, vTemp.z);
	}

	return fRes; 
}

dword SEA::AttributeChanged(ATTRIBUTES * pAttribute)
{
	ATTRIBUTES * pParent = pAttribute->GetParent();
	ATTRIBUTES * pParent2 = (pParent) ? pParent->GetParent() : null;

	char * sName = pAttribute->GetThisName();
	char * sValue = pAttribute->GetThisAttr();

	if (*pParent == "isDone")
	{
		Realize(0);
	}

	if (*pParent == "Sea2")
	{
		if (*pAttribute == "WaterColor")	{ v4SeaColor = COLOR2VECTOR4(pAttribute->GetAttributeAsDword()); return 0; }
		if (*pAttribute == "SkyColor")		{ v4SkyColor = COLOR2VECTOR4(pAttribute->GetAttributeAsDword()); return 0; }
		
		if (*pAttribute == "Amp1")			{ _fAmp1 = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "AnimSpeed1")	{ fAnimSpeed1 = pAttribute->GetAttributeAsFloat(); return 0; }
 		if (*pAttribute == "Scale1")		{ fScale1 = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "MoveSpeed1")	{ sscanf(pAttribute->GetThisAttr(), "%f, %f, %f", &vMoveSpeed1.x, &vMoveSpeed1.y, &vMoveSpeed1.z); return 0; }

		if (*pAttribute == "Amp2")			{ _fAmp2 = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "AnimSpeed2")	{ fAnimSpeed2 = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Scale2")		{ fScale2 = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "MoveSpeed2")	{ sscanf(pAttribute->GetThisAttr(), "%f, %f, %f", &vMoveSpeed2.x, &vMoveSpeed2.y, &vMoveSpeed2.z); return 0; }

		if (*pAttribute == "PosShift")		{ fPosShift = pAttribute->GetAttributeAsFloat(); return 0; }

		if (*pAttribute == "BumpScale")		{ fBumpScale = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "BumpSpeed")		{ fBumpSpeed = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "LodScale")		{ fLodScale = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "GridStep")		{ fGridStep = pAttribute->GetAttributeAsFloat(); return 0; }

		if (*pAttribute == "Reflection")	{ v4SeaParameters.y = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Transparency")	{ v4SeaParameters.z = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Attenuation")	{ v4SeaParameters.x = pAttribute->GetAttributeAsFloat(); return 0; }

		if (*pAttribute == "Frenel")		{ fFrenel = pAttribute->GetAttributeAsFloat(); return 0; }

		if (*pAttribute == "FoamV")				{ fFoamV = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "FoamK")				{ fFoamK = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "FoamUV")			{ fFoamUV = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "FoamTexDisturb")	{ fFoamTextureDisturb = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "FoamEnable")		{ bFoamEnable = pAttribute->GetAttributeAsDword() != 0; return 0; }

		if (*pAttribute == "SimpleSea")		
		{ 
			bSimpleSea = pAttribute->GetAttributeAsDword() != 0; 
			BuildVolumeTexture();
			return 0; 
		}
		return 0;
	}

	if (*pParent == "fog")
	{
		if (*pAttribute == "Enable")	{ bFogEnable = pAttribute->GetAttributeAsDword()!=0; return 0; }
		if (*pAttribute == "Start")		{ fFogStartDistance = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Color")		{ vFogColor = (1.0f / 255.0f) * COLOR2VECTOR(pAttribute->GetAttributeAsDword()); return 0; }
		if (*pAttribute == "SeaDensity"){ fFogSeaDensity = pAttribute->GetAttributeAsFloat(); return 0; }
		return 0;
	}

	if (*pAttribute == "Stop")
	{
		bStop = AttributesPointer->GetAttributeAsDword("Stop") != 0;
	}

	if (*pAttribute == "UnderWater")
	{
		bUnderSeaEnable = AttributesPointer->GetAttributeAsDword("UnderWater") != 0;
	}

	if (*pAttribute == "MaxSeaHeight")
	{
		fMaxSeaHeight = AttributesPointer->GetAttributeAsFloat("MaxSeaHeight", 50.0f);

		float fScale = (fMaxSeaHeight >= _fAmp1 + _fAmp2) ? 1.0f : fMaxSeaHeight / (_fAmp1 + _fAmp2);

		fAmp1 = _fAmp1 * fScale;
		fAmp2 = _fAmp2 * fScale;
	}

	return 0;
}