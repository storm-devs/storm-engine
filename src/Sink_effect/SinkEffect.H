#ifndef _SINKEFFECT_H_
#define _SINKEFFECT_H_

#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"
#include "..\common_h\geometry.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\CVECTOR.h"
#include "..\common_h\dx8render.h"
#include "SinkSplashDefines.h"
#include "TSink.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class SINKEFFECT: public ENTITY
{
public:
	SINKEFFECT();
	virtual ~SINKEFFECT();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);

private:
	void InitializeSinks();
	TSink *TryToAddSink(const CVECTOR &_pos, float _r);

	TSink    sinks[MAX_SINKS];
	VDX8RENDER *renderer;
	SEA_BASE *sea;
};

#endif