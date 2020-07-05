#ifndef _VPARTICLE_SYSTEM_H_
#define _VPARTICLE_SYSTEM_H_

//#include <windows.h>
#include "..\common_h\service.h"

class VPARTICLE_SYSTEM
{
public:
	VPARTICLE_SYSTEM() {};
	virtual ~VPARTICLE_SYSTEM() {};
	virtual void	Stop() = 0;
	virtual void	SetEmitter(CVECTOR p, CVECTOR a) = 0;
	virtual void	LinkToObject(ENTITY_ID id, CVECTOR _LinkPos) = 0;
	virtual void	SetDelay(long _delay) = 0;
	virtual void	SetLifeTime(dword time) = 0;
	virtual void	StopEmitter(){};
	virtual	void	Pause(bool _bPause){};

};

#endif
