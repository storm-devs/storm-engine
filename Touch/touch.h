#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "..\common_h\ship_base.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\island_base.h"
#include "..\common_h\templates\array.h"

#define D3DTLVERTEX_FORMAT	(D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

typedef struct
{
	CVECTOR		vPos;
	float		fRHW;
	DWORD		dwDiffuse;
} D3DTLVERTEX;

struct TOUCH_SHIP
{
	ENTITY_ID		eID;					// ship id
	CVECTOR			vContourTemp[2][128];	// temporary storage / must be deleted
	TOUCH_PARAMS	TP[2];					// touch params	for collision
	SHIP_BASE		*pShip;					// ship pointer
	CVECTOR			vKickPoint;				// temp
	CVECTOR			vKickAngle;				// temp
	CVECTOR			vContour[128];			// initial contour // must be dynamic
	CVECTOR			vContourRect[4];		// rect contour
	long			iNumVContour;			// num points in contour
};

class TOUCH : public ENTITY
{
protected:
	VDX8RENDER	* pRS;
	ISLAND_BASE	* pIslandBase;

	bool		bUseTouch;

	float		fScale;
	float		fCollisionDepth;

	dword		dwDeltaTime;
	long		iDeltaTime;

	TOUCH_SHIP	*pShips[256];
	long		iNumShips;

	BOOL		BuildContour(long ship_idx);
	BOOL		IsPointInContour(CVECTOR *vP, CVECTOR *vContour, long numvcontour);

	bool		IsSinked(long iIndex);

	float		Touch(long idx, long skip_idx, CVECTOR *vPos, CVECTOR *vAng, float fPower, float fSlide);
	BOOL		FakeTouch();
	BOOL		IsIntersectShipsRects(long idx1, long idx2);
	BOOL		IsIntersectShipsReal(long idx1, long cidx, CVECTOR *vPos, CVECTOR *vAng, CVECTOR *vRecoil, float *fPower, float *fSlide);
	void		GetLineABC(CVECTOR &v1, CVECTOR &v2, float &A, float &B, float &C);
	CVECTOR		GetLineIntersectPoint(CVECTOR &v1, CVECTOR &v2, CVECTOR &o1, CVECTOR &o2);
	long		ProcessImpulse(long iOurIdx, CVECTOR vPos, CVECTOR vDir, float fPowerApplied);
	long		GetTouchPoint(long iIdx, const CVECTOR &vPos);

	void		DrawLine(array<RS_LINE2D> & aLines, float x1, float y1, float x2, float y2, DWORD color);
	void		DrawShips();
	CVECTOR		GetPoint(float x, float y, float xx, float yy, float xscale, float yscale, float fCos, float fSin, POINT ss);

	void		SetDevices();
public:

	virtual ~TOUCH();
	TOUCH();
	void	LoadServices();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE & message);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
};

#endif