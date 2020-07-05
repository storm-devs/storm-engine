#ifndef _TCARCASS_H_
#define _TCARCASS_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\CVECTOR.h"
#include "..\common_h\MATRIX.h"
#include "TIVBufferManager.h"

#define MAX_MEASURE_POINTS 10
#define MAX_LEVELS		   20
#define CARCASS_VERTEX_FORMAT (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)

#define BOUND_UPPER(a,b) if ((a) > (b)) (a) = (b);
//--------------------------------------------------------------------
struct tCarcassVertex
{
	CVECTOR pos;
	DWORD color;
	float u,v;
};

struct tMeasure
{
	float deltaPointX[MAX_MEASURE_POINTS];
	float deltaPointY[MAX_MEASURE_POINTS];
	int pointsCount;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TCarcass  
{
public:
	TCarcass(int _levelsCount, int _measurePointsCount, VDX8RENDER *_renderer, bool _normalsInverted = false);
	virtual ~TCarcass();

	void InitCircleMeasure(float _d, float _kx, float _ky);
	bool Initialize();
	void Uninitialize();
	void Execute(dword _dTime, CMatrix &_mtx, const CVECTOR *_starts);
	void Realize(const char *_technique);

	void SetSpeed(float _uSpeed, float _vSpeed, float _speedA);
private:
	void RebuildLevels(tCarcassVertex *_vBuffer, bool _firstDraw, dword _dTime);
	void RebuildIndexes(WORD *_iBuffer);

	bool     normalsInverted;
	tMeasure measure;
	CVECTOR  levelStarts[MAX_LEVELS];
	int		 levelsCount;
	CMatrix  sceneMatrix;
	long	 ivElementIndex;
	bool	 indexesCreated;
	dword    time;

	float uSpeed, vSpeed, speedA;
	VDX8RENDER *renderer;
	int iBuffer, vBuffer;
};

#endif
