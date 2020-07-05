#ifndef _TSINKSPLASH_H_
#define _TSINKSPLASH_H_

#include "..\common_h\matrix.h"
#include "..\common_h\messages.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\exs.h"
#include "..\common_h\CVECTOR.h"
#include "..\SoundService\VSoundService.h"
#include "seps.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct SINK_VERTEX
{
	CVECTOR pos;
	dword color;
	float tu,tv;
};

class TSinkSplash  
{
public:
	TSinkSplash();
	virtual ~TSinkSplash();

	void Initialize(INIFILE * _ini, SEA_BASE *sea);
	void Release();
	void Start(const CVECTOR &_pos, WORD *_indexes, SINK_VERTEX *_vertexes, long vOffset);
	void Process(dword _dTime, WORD *_indexes, SINK_VERTEX *_vertexes);
	bool Enabled();
	void Reset(WORD *_indexes, SINK_VERTEX *_vertexes);
	void AdditionalRealize(dword _dTime);

private:
	float HeightF(dword _time, float _r, float _k);

	SEPS_PS *ps;
	bool enabled;
	SEA_BASE *sea;
	dword time;

	float distortDivider;
	CVECTOR center,dir;
	float growK;
};

#endif 
