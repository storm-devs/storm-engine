
#ifndef VBLAST_H
#define VBLAST_H

#include "..\common_h\collide.h"
#include "..\common_h\cvector.h"

class VBLAST_PACK : public ENTITY
	{
	public:
	virtual void CreateBlast(CVECTOR pos, CVECTOR dir) = 0;
	};

class VBLAST : public COLLISION_OBJECT
	{
	public:
	virtual void CreateBlast() = 0;
	};
	
#endif
