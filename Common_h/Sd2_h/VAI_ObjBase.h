#ifndef VAI_BASE_HPP
#define VAI_BASE_HPP

#include "..\vmodule_api.h"
#include "..\matrix.h"
#include "..\model.h"
#include "..\cvector.h"
#include "..\defines.h"
#include "CannonTrace.h"

class CSaveLoad;

class VAI_OBJBASE : public CANNON_TRACE_BASE
{
private:
	ATTRIBUTES	* pACharacter;

public:
	virtual ~VAI_OBJBASE() {};

	virtual void		SetACharacter(ATTRIBUTES * pAP) { pACharacter = pAP; };
	virtual ATTRIBUTES*	GetACharacter() { return pACharacter; };

	virtual bool		Mount(ATTRIBUTES * ) { return false; };

	virtual void		SetPos(CVECTOR & vNewPos) {};

	virtual CVECTOR		GetPos() { return 0.0f; };
	virtual CVECTOR		GetAng() { return 0.0f; };
	virtual CMatrix *	GetMatrix() = 0;
	virtual CVECTOR		GetBoxSize() { return 0.0f; };

	virtual MODEL *		GetModel() = 0;
	virtual ENTITY_ID	GetModelEID() = 0;

	virtual void Save(CSaveLoad * pSL) = 0;
	virtual void Load(CSaveLoad * pSL) = 0;	

	virtual void Fire(const CVECTOR & vPos) = 0;
};

#endif