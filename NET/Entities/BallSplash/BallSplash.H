#ifndef _BALLSPLASH_H_
#define _BALLSPLASH_H_

#include "..\..\common.h"

/*#include "..\common_h\object.h"
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
#include "..\common_h\dx8render.h"*/
#include "BallSplashDefines.h"
#include "TSplash.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class NetBallSplash: public ENTITY
{
public:
	NetBallSplash();
	virtual ~NetBallSplash();

	virtual bool Init();
	virtual dword _cdecl ProcessMessage(MESSAGE & message);
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);

private:
	void InitializeSplashes();
	TSplash *TryToAddSplash(const CVECTOR &_pos, const CVECTOR &_dir);

	TSplash    splashes[MAX_SPLASHES];
	VDX8RENDER *renderer;
	SEA_BASE   *sea;
};

#endif