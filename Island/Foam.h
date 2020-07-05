#ifndef _COAST_FOAM_HPP_
#define _COAST_FOAM_HPP_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "..\common_h\Island_base.h"
#include "..\common_h\defines.h"
#include "..\common_h\model.h"
#include "..\common_h\messages.h"
#include "..\common_h\tga.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\sea_ai\AIFlowGraph.h"

enum FOAMTYPE
{
	FOAM_TYPE_1 = 1,
	FOAM_TYPE_2 = 2,

	FOAM_MODE_FORCEDWORD = 0x7FFFFFFF
};

class CoastFoam : public ENTITY 
{
public:
	CoastFoam();
	~CoastFoam();

	bool Init();
	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	dword AttributeChanged(ATTRIBUTES * pA);

private:
	struct FoamVertex
	{
		CVECTOR vPos;
		dword	dwColor;
		float	tu, tv;
	};

	struct FoamPart { CVECTOR v[2]; };

	struct SubPart
	{
		//float tv;
		float fPos;
		float fSpeed;
		//float fA;
	};

	struct WorkPart
	{
		SubPart		p[8];
		CVECTOR		v[2];
		float		tu;
		//FOAMMODE	Mode;
	};

	struct Foam 
	{
		Foam() : aFoamParts(_FL_), aWorkParts(_FL_) { iTexture = -1; }

		array<FoamPart>	aFoamParts;
		array<WorkPart>	aWorkParts;
		float			fMove[2];
		float			fSX[2];
		float			fSpeed[2], fBraking[2];
		long			iMode[2];
		long			iTexture;
		string			sTexture;
		long			iNumFoams;

		FOAMTYPE		Type;

		float			fAlphaColor[2], fAlpha[2], fAlphaMin, fAlphaMax;
		float			fSpeedMin, fSpeedMax;
		float			fBrakingMin, fBrakingMax;
		float			fAppearMin, fAppearMax;
		float			fTexScaleX;
	};

	long			iEditFoam, iEditFoamPart, iEditFoamVertex;
	bool			bEditMode, bMoved, bCanEdit;
	float			fCursorPosX, fCursorPosY;
	long			iFoamTexture, iCursorTex;
	SEA_BASE		* pSea;
	array<Foam*>	aFoams;
	long			iVBuffer, iIBuffer;
	PLANE			* pFrustumPlanes;

	float			fMaxFoamDistance, fFoamDeltaY;
	long			iFoamDivides;

	dword			dwNumPenasExecuted;

	void ExtractRay (const D3DVIEWPORT8 & viewport, float fCursorX, float fCursorY, CVECTOR & raystart, CVECTOR & rayend);

	bool IsClipped(Foam * pF);

	void Save();
	void Load();
	void DelAll();

	void InitNewFoam(Foam * pF);
	void RecalculateFoam(long iFoam);
	void ExecuteFoamType2(Foam * pF, float fDeltaTime);
	void ExecuteFoamType1(Foam * pF, float fDeltaTime);
};

#endif