#ifndef _SINKEFFECT_H_
#define _SINKEFFECT_H_

#include "object.h"
#include "matrix.h"
#include "animation.h"
#include "messages.h"
#include "geos.h"
#include "geometry.h"
#include "dx8render.h"
#include "ship_base.h"
#include "model.h"
#include "geos.h"
#include "sea_base.h"
#include "CVECTOR.h"
#include "dx8render.h"
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