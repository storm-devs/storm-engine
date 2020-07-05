#ifndef SHIP_TRAIL_HPP
#define	SHIP_TRAIL_HPP

#include "..\..\common.h"

#define NUM_SHIP_TRAIL_SEGMENTS		3		// -2 only

typedef struct
{
	float	fA[4],fB[4],fC[4];
	CVECTOR	vPos;
	CVECTOR	v[4];
	float	A,B,C;
	float	fSpeed;		// speed in born
	float	fWidth;		// current width
	float	fAngY;		// angle 
	float	fTime;		// life time for this trail (in seconds)
	float	fVStart;	// tv coordinate start
	float	fVEnd;		// tv coordinate end
	float	fVPos;		// current v position for this trail part
} shiptrail_t;

struct seaship_t
{
	ENTITY_ID				eID;
	float					fSpeed;
	float					fLen, fWidth;		// minimal len and width
	CVECTOR					vPos, vCurPos;		// last and current position
	dword					dwNumTrails;
	shiptrail_t				* pShipTrail;
	float					fShipTrailVSize;	// full length of ship trail
	FRECT					fRect;				// rect of all trail
	bool					bFirstInfo, bUse;
};

#endif