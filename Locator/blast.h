#ifndef _BLAST_H_
#define _BLAST_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"
#include "blast.h"
#include "..\common_h\sd2_h\cannontrace.h"

typedef struct
{
	CVECTOR pos;
	CVECTOR ang;
	CVECTOR ang_speed;
	CVECTOR dir;
	GEOS *  geo;
	float speed;
	bool bDouble;
	bool bEffect;
} GEOPARTICLE;

class BLAST : public ENTITY 
{
	VDX8RENDER * rs;
	VGEOMETRY * gs;
	GEOPARTICLE * Item;
	CANNON_TRACE_BASE * pSea;
	ENTITY_ID sea_eid;
	float AngleDeviation;
	dword ItemsNum;
	CMatrix Center;
	ENTITY_ID Splash;
public:
	~BLAST();
	 BLAST();
	bool Init();
	dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES *);
	void Realize(dword Delta_Time);
	void ProcessTime(dword Delta_Time);
	void SetBlastCenter(CVECTOR pos, CVECTOR ang);
	void AddGeometry(char * name, long num);
};
#endif