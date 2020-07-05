
#ifndef _Blood_H_
#define _Blood_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"
#include "..\common_h\defines.h"
#include "..\common_h\templates\array.h"

#define MAX_BLOOD_TRIANGLES			10000
#define ON_LIVETIME_BLOOD_TRIANGLES	(MAX_BLOOD_TRIANGLES*3/4)
#define MAX_CLIPPING_TRIANGLES		64

class Blood : public ENTITY
{
	struct BloodVertex
	{
		CVECTOR pos;
		dword dwCol;
		float u,v;
	};
	struct BloodTriangle
	{
		BloodVertex v[3];
	};
	struct BloodInfo
	{
		long nStartIdx;
		long nIdxQ;
		float fLiveTime;
		CVECTOR cpos;
	};
	struct ClipTriangle
	{
		CVECTOR v[3];
	};
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Blood();
	virtual ~Blood();

	//Инициализация
	bool Init();
	//Работа
	void Execute(dword delta_time);
	void Realize(dword delta_time);
	//
	dword _cdecl ProcessMessage(MESSAGE &message);

	static bool AddClipPoligon(const CVECTOR * v, long nv);

protected:
	VDX8RENDER* pRS;
	COLLIDE* pCol;
	long texID;

	BloodTriangle pvBloodT[MAX_BLOOD_TRIANGLES];
	long nStartT;
	long nUsedTQ;

	array<BloodInfo> aBlood;

	array<ENTITY_ID> aModels;

	static ClipTriangle clipT[MAX_CLIPPING_TRIANGLES];
	static long nClipTQ;
	static CVECTOR normal;

	void AddBlood(const CVECTOR& pos);
	void BuildBloodDataByCollision(const CVECTOR& cpos);
	void SetVertexByPos(BloodVertex& v, const CVECTOR& pos, const CVECTOR& vc, float fU0, float fV0);
	long CheckBloodQuantityInRadius(const CVECTOR& cpos, float fDist, long nLimitQ);
};

#endif

