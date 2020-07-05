#ifndef NETLIGHTPILLAR_HPP
#define NETLIGHTPILLAR_HPP

#include "..\..\common.h"

#define NUM_SEGMENTS	64

class NetLightPillar : public ENTITY
{
public:
	NetLightPillar();
	virtual ~NetLightPillar();

	bool	Init();
	void	Realize(dword DeltaTime);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);

private:
	struct vertex_t
	{
		CVECTOR vPos;
		dword	dwColor;
	};

	vertex_t	Vertex[NUM_SEGMENTS * 2];
	CVECTOR		vPos;
	dword		dwColor1, dwColor2;
	float		fRadius, fHeight;
};

#endif