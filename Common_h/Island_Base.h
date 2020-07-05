#ifndef ISLAND_BASE_HPP
#define ISLAND_BASE_HPP

#include "..\common_h\d_types.h"
#include "..\common_h\entity.h"
#include "..\common_h\sd2_h\CannonTrace.h"

class ISLAND_BASE : public CANNON_TRACE_BASE
{
public:
	virtual ENTITY_ID GetModelEID() = 0;

	virtual bool Check2DBoxDepth(CVECTOR vPos, CVECTOR vSize, float fAngY, float fMinDepth) = 0;
	virtual bool GetDepth(float x, float z, float * fRes = 0) = 0;
	virtual bool GetDepthFast(float x, float z, float * fRes = 0) = 0;
	virtual bool GetDepth(FRECT * pRect, float * fMinH, float * fMaxH) = 0;

	virtual bool GetShadow(float x, float z, float * fRes = 0) = 0;

	virtual bool GetMovePoint(CVECTOR & vSrc, CVECTOR & vDst, CVECTOR & vRes) = 0;

	virtual ENTITY_ID GetSeabedEID() = 0;

	virtual float GetCurrentImmersion() = 0;
};

#endif