// ************************************************************************
// *  Author : Vano, 1861; Description : Sea/Ocean/Water/Lake/River/pool  *
// ************************************************************************
#ifndef NEW_SEA_HPP
#define NEW_SEA_HPP

#include "..\common_h\dx8render.h"
#include "..\common_h\cvector4.h"
#include "..\common_h\templates.h"
#include "..\common_h\Sea_Base.h"

#include "..\common_h\defines.h"

#include "..\common_h\Intel.h"

class SEA : public SEA_BASE
{
private:
	struct SeaVertex
	{
		CVECTOR		vPos;
		CVECTOR		vNormal;
		float		tu, tv;
	};

	struct SeaBlock
	{
		long iX1, iX2, iY1, iY2;			// result rectangle(in units)
		long iSize0;						// 

		long iTX, iTY;
		long iSize;
		long iLOD;
		long iIStart;
		long iIFirst, iILast;

		bool bInProgress, bDone;

		static bool QSort(const SeaBlock & b1, const SeaBlock & b2) { return (b1.iLOD > b2.iLOD); }
		inline bool operator < (const SeaBlock & B) const { return (B.iLOD < iLOD); };
	};

	struct SeaTrash
	{
		CVECTOR		vPos;
		CVECTOR		vSpeed;
		dword		dwSubTexture;
		float		fTime;
		float		fSize;
	};

	struct SeaLight
	{
		CVECTOR		vPos;
		CVECTOR		vSpeed;
		dword		dwSubTexture;
		float		fTime;
	};

	static SEA		* pSea;

	dword			dwMaxDim, dwMinDim;
	float			fMaxSeaHeight;
	float			fGridStep;
	float			fLodScale;
	float			fBumpScale, fBumpSpeed;
	CVECTOR			vMoveSpeed1, vMoveSpeed2;

	CVECTOR			vCamPos, vSeaCenterPos, vWorldOffset;

	static bool		bIntel, bSSE, bDisableSSE;

	bool			bStarted;
	bool			bUnderSea;
	bool			bUnderSeaEnable;
	float			bUnderSeaStarted;

	long					iVSeaBuffer, iISeaBuffer;

	IDirect3DVolumeTexture8	* pVolumeTexture;
	IDirect3DCubeTexture8	* pEnvMap, * pSunRoadMap;
	IDirect3DSurface8		* pZStencil;

	IDirect3DTexture8		* pReflection, * pReflectionSunroad;
	IDirect3DSurface8		* pReflectionSurfaceDepth;

	bool		bSimpleSea;

	SeaVertex	* pVSea;
	word		* pTriangles;
	dword		* pIndices;
	long		iVStart, iTStart, iIStart;

	PLANE		* pFrustumPlanes;
	dword		dwNumFrustumPlanes;

	long				iSeaTrashTexture;
	float				fLastTrashTime;
	array<SeaTrash>		aSeaTrash;
	array<RS_RECT>		aTrashRects;

	long				iSeaLightTexture;
	array<SeaLight>		aSeaLights;
	float				fLastLightTime;
	array<RS_RECT>		aLightsRects;

	array<dword*>		aNormals;
	array<byte*>		aBumps;
	array<SeaBlock>		aBlocks;

	CVECTOR4		v4SeaColor, v4SkyColor, v4SeaParameters;

	array<IDirect3DTexture8*>	aBumpMaps;
	IDirect3DTexture8			* pRenderTargetBumpMap;

	float * pSeaFrame1, * pSeaFrame2, * pSeaNormalsFrame1, * pSeaNormalsFrame2;

	CVECTOR vMove1, vMove2;
	float fFrame1, fFrame2;
	float fScale1, fScale2;
	float fAmp1, fAmp2;
	float _fAmp1, _fAmp2;
	float fAnimSpeed1, fAnimSpeed2;
	float fPosShift;
	float fFrenel;

	bool bStop;

	long iB1, iB2;

	float	fFogSeaDensity, fFogStartDistance;
	CVECTOR vFogColor;
	bool	bFogEnable;

	bool	bTempFullMode;
	float	fTempGridStep, fTempLodScale;

	bool	bIniFoamEnable, bFoamEnable;
	float	fFoamV, fFoamK, fFoamUV, fFoamTextureDisturb;
	long	iFoamTexture;

	CMatrix	mTexProjection;

	void				SSE_WaveXZ(SeaVertex * * pArray);
	float __fastcall	WaveXZ(float x, float z, CVECTOR * pNormal = null);

	void			AddBlock(long iTX, long iTY, long iSize, long iLOD);
	void			BuildTree(long iTX, long iTY, long iLev);
	void			SetBlock(dword dwBlockIndex);

	bool			isVisibleBBox(const CVECTOR & vCenter, const CVECTOR & v1, const CVECTOR & v2);
	void			CalculateLOD(const CVECTOR & v1, const CVECTOR & v2, long & iMaxLOD, long & iMinLOD);
	inline float	CalcLod(const float & x, const float & y, const float & z);
	void			WaveXZBlock(SeaBlock * pB);
	void			SSE_WaveXZBlock(SeaBlock * pB);
	SeaBlock *		GetUndoneBlock();
	void			PrepareIndicesForBlock(dword dwBlockIndex);

	long			VisCode(const CVECTOR & vP);

	void			CalculateHeightMap(float fFrame, float fAmplitude, float * pfOut, array<byte*> & aFrames);
	void			CalculateNormalMap(float fFrame, float fAmplitude, float * pfOut, array<dword*> & aFrames);

	bool			SunRoad_Render2();
	bool			EnvMap_Render2();
	bool			SunRoad_Render();
	bool			EnvMap_Render();
	void			EnvMap_GetSideMatrix(D3DCUBEMAP_FACES Face, CMatrix & mView);

	// HyperThreading 
	Intel				intel;
	HANDLE				hEventCalcMaps;
	array<HANDLE>		aEventCalcBlock;
	array<long>			aThreadsTest;
	array<HANDLE>		aThreads;
	bool				bHyperThreading;
	CRITICAL_SECTION	cs, cs1;
	long				iBlocksDoneNum;

	static dword	ThreadExecute(long iThreadIndex);
	void __declspec(dllexport) __cdecl SFLB_CreateBuffers();
public:

	SEA();
	virtual ~SEA();

	bool	Init();
	void	BuildVolumeTexture();
	void 	Realize(dword dwDeltaTime);

	float Trace(const CVECTOR & vSrc, const CVECTOR & vDst);
	virtual float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst);

	virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) { return false; }
	virtual const char *GetCollideMaterialName() { return null; };
	virtual bool GetCollideTriangle(TRIANGLE &triangle) { return false; }

	bool EditMode_Update();

	dword AttributeChanged(ATTRIBUTES * pAttribute);

	//bool			Init();
	//void			Realize(dword Delta_Time);
	//void			Execute(dword Delta_Time);
	//dword _cdecl	ProcessMessage(MESSAGE & message);
};

#endif