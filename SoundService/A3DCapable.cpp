#include "..\common_h\exs.h"
//#include <dxerr8.h>
#include "A3DCapable.h"
#include "VSound.h"

//--------------------------------------------------------------------
A3DCapable::A3DCapable()
	:ds3DBuffer(0)
{

}

//--------------------------------------------------------------------
A3DCapable::~A3DCapable()
{

}

//--------------------------------------------------------------------
bool A3DCapable::Unload()
{
	GUARD(A3DCapable::Unload)

	if (ds3DBuffer)
	{
		ds3DBuffer->Release();
		ds3DBuffer = 0;
	}

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool A3DCapable::Play(bool _looped, const CVECTOR *_startPosition, float _minDistance, float _maxDistance, float _volume)
{
	GUARD(A3DCapable::Unload)

	if (!core->dsBuffer)
		return false;

	if (_startPosition)
		Set3DParameter(SM_POSITION, _startPosition);
	if (_minDistance >= 0.0f)
		Set3DParameter(SM_MIN_DISTANCE, &_minDistance);
	if (_maxDistance >= 0.0f)
		Set3DParameter(SM_MAX_DISTANCE, &_maxDistance);
	if (_volume != -1.f)
		core->SetVolume(_volume);

	SOUND_TRACE("3D - play started");
	if (_looped)
		return (core->dsBuffer->Play(0, 0, DSBPLAY_LOOPING) == DS_OK);
	else
		return (core->dsBuffer->Play(0, 0, 0) == DS_OK);


	UNGUARD
}

//--------------------------------------------------------------------
bool A3DCapable::Set3DParameter(eSoundMessage _message, const void *_op)
{
	GUARD(A3DCapable::Set3DParameter)

#ifndef _XBOX
	if (!ds3DBuffer)
		return false;

	float io;
	switch (_message)
	{
	case SM_MAX_DISTANCE:
		io = *((float *) _op);
		return (ds3DBuffer->SetMaxDistance((D3DVALUE) io, DS3D_DEFERRED) == DS_OK);
	case SM_MIN_DISTANCE:
		io = *((float *) _op);
		return (ds3DBuffer->SetMinDistance((D3DVALUE) io, DS3D_DEFERRED) == DS_OK);
	case SM_POSITION:
		return (ds3DBuffer->SetPosition(((const CVECTOR *) _op)->x, 
										((const CVECTOR *) _op)->y, 
										((const CVECTOR *) _op)->z, 
										DS3D_DEFERRED) == DS_OK);
	}
#else
	if (!core->dsBuffer)
		return false;

	switch (_message)
	{
	case SM_MAX_DISTANCE:
		return (core->dsBuffer->SetMaxDistance(*((const D3DVALUE *) _op), DS3D_DEFERRED) == DS_OK);
	case SM_MIN_DISTANCE:
		return (core->dsBuffer->SetMinDistance(*((const D3DVALUE *) _op), DS3D_DEFERRED) == DS_OK);
	case SM_POSITION:
		return (core->dsBuffer->SetPosition(((const CVECTOR *) _op)->x, 
						  			  ((const CVECTOR *) _op)->y, 
									  ((const CVECTOR *) _op)->z, 
										DS3D_DEFERRED) == DS_OK);
	}
#endif
	return false;
	UNGUARD
}

//--------------------------------------------------------------------
bool A3DCapable::Get3DParameter(eSoundMessage _message, void *_op)
{
	GUARD(A3DCapable::Set3DParameter)

	if (!ds3DBuffer)
		return false;

	switch (_message)
	{
	case SM_MAX_DISTANCE:
		return false;
	case SM_MIN_DISTANCE:
		return false;
	case SM_POSITION:
		return false;
	//case SM_CONE_ANGELS:
	//case SM_CONE_OUTSIDE_VOLUME:
	//case SM_VELOCITY:
	}

	return 0;
	UNGUARD
}

//--------------------------------------------------------------------
A3DCapable::A3DCapable(const A3DCapable &_source)
{
	//
}

//--------------------------------------------------------------------
void A3DCapable::Init()
{
	#ifndef _XBOX
		if (!ds3DBuffer)
			core->dsBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void **) &ds3DBuffer);
	#endif
}
