#ifndef _A3DCAPABLE_H_
#define _A3DCAPABLE_H_

#include <dsound.h>
#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
#include "TSoundCore.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class A3DCapable  
{
public:
	A3DCapable();
	A3DCapable(const A3DCapable &_source);
	virtual ~A3DCapable();

	//LPDIRECTSOUNDBUFFER8 CreateBuffer(LPDIRECTSOUND8 _ds8, int _size, const WAVEFORMATEX &_waveFormat);
	void Init();
	bool Play(bool _looped, 
			  const CVECTOR *_startPosition = 0,
			  float _minDistance = -1.0f,
			  float _maxDistance = -1.0f,
			  float _volume      = -1.0f);
	bool Unload();

	bool Set3DParameter(eSoundMessage _message, const void *_op);
	bool Get3DParameter(eSoundMessage _message, void *_op);

protected:
	TSoundCore			  *core;
	LPDIRECTSOUND3DBUFFER ds3DBuffer;

};

#endif // !defined
