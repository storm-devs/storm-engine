// ************************************************************************
// *  Author : Vano, 1861; Description : Sea/Ocean/Water/Lake/River/pool  *
// ************************************************************************
#ifndef NEW_SEA_HPP
#define NEW_SEA_HPP

#include "..\..\common.h"

#include "..\..\..\engine\modules\techniques\weather\shaders\reflect.h"
#include "..\..\..\common_h\Light.h"

class NetSea : public SEA_BASE
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
		long iVStart, iTStart, iIStart;

		bool bInProgress, bDone;

		static bool QSort(const SeaBlock & b1, const SeaBlock & b2) { return (b1.iLOD > b2.iLOD); }
		inline bool operator < (const SeaBlock & B) const { return (B.iLOD < iLOD); };
	};

	dword			dwMaxDim, dwMinDim;
	float			fMaxSeaHeight;
	float			fGridStep;
	float			fLodScale;
	float			fBumpScale, fBumpSpeed;
	CVECTOR			vMoveSpeed1, vMoveSpeed2;

	CVECTOR			vCamPos, vSeaCenterPos;

	long					iVSeaBuffer, iISeaBuffer;

	IDirect3DVolumeTexture9	* pVolumeTexture;
	IDirect3DCubeTexture9	* pEnvMap, * pSunRoadMap;
	IDirect3DSurface9		* pZStencil;

	IDirect3DTexture9		* pReflection, * pReflectionSunroad;
	IDirect3DSurface9		* pReflectionSurfaceDepth;

	bool		bSimpleSea;
	CMatrix		mTexProjection;

	SeaVertex	* pVSea;
	word		* pTriangles;
	dword		* pIndices;
	long		iVStart, iTStart, iIStart;

	PLANE		* pFrustumPlanes;
	dword		dwNumFrustumPlanes;

	array<dword*>		aNormals;
	array<byte*>		aBumps;
	array<SeaBlock>		aBlocks;

	CVECTOR4		v4SeaColor, v4SkyColor, v4SeaParameters;

	float * pSeaFrame1, * pSeaFrame2, * pSeaNormalsFrame1, * pSeaNormalsFrame2;

	CVECTOR vMove1, vMove2;
	float	fFrame1, fFrame2;
	float	fScale1, fScale2;
	float	fAmp1, fAmp2;
	float	_fAmp1, _fAmp2;
	float	fAnimSpeed1, fAnimSpeed2;
	float	fPosShift;
	float	fFrenel;

	long	iLastServerTime;
	long	iLastClientTime;

	long	iB1, iB2;

	float	fCurrentDeltaTime;

	float	fFogSeaDensity, fFogStartDistance;
	CVECTOR vFogColor;
	bool	bFogEnable;

	float __fastcall	WaveXZ(float x, float z, CVECTOR * pNormal = null);

	void			AddBlock(long iTX, long iTY, long iSize, long iLOD);
	void			BuildTree(long iTX, long iTY, long iLev);
	void			SetBlock(dword dwBlockIndex);

	bool			isVisibleBBox(const CVECTOR & vCenter, const CVECTOR & v1, const CVECTOR & v2);
	void			CalculateLOD(const CVECTOR & v1, const CVECTOR & v2, long & iMaxLOD, long & iMinLOD);
	inline float	CalcLod(const float & x, const float & y, const float & z);
	void			WaveXZBlock(SeaBlock * pB);
	SeaBlock *		GetUndoneBlock();

	long			VisCode(const CVECTOR & vP);

	void			CalculateHeightMap(float fFrame, float fAmplitude, float * pfOut, array<byte*> & aFrames);
	void			CalculateNormalMap(float fFrame, float fAmplitude, float * pfOut, array<dword*> & aFrames);

	bool			SunRoad_Render();
	bool			EnvMap_Render();
	bool			SunRoad_Render2();
	bool			EnvMap_Render2();
	void			EnvMap_GetSideMatrix(D3DCUBEMAP_FACES Face, CMatrix & mView);

public:

	NetSea();
	virtual ~NetSea();

	bool	Init();
	void	BuildVolumeTexture();
	void 	Realize(dword dwDeltaTime);
	void	Execute(dword dwDeltaTime);

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