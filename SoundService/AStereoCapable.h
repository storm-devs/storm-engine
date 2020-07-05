#ifndef _ASTEREOCAPABLE_H_
#define _ASTEREOCAPABLE_H_

#include <dsound.h>
#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
#include "TSoundCore.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class AStereoCapable  
{
public:
	AStereoCapable();
	virtual ~AStereoCapable();
	//bool CreateBuffer(LPDIRECTSOUND8 _ds8, int _size, const WAVEFORMATEX &_waveFormat);
	bool Unload();

	virtual bool Play(bool _looped = true, float _volume = -1.f, dword _time = 0);
	virtual bool Stop();
	void SetVolume (float _volume);

protected:
	TSoundCore			  *core;
};

#endif // !defined
