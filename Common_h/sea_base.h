#ifndef SEA_BASE_HPP
#define SEA_BASE_HPP

#include "vmodule_api.h"
#include "sd2_h\CannonTrace.h"

class SEA_BASE : public CANNON_TRACE_BASE
{
public:
	virtual float __fastcall WaveXZ(float x, float z, CVECTOR * vNormal = null) { return 0.0f; };
};

#endif