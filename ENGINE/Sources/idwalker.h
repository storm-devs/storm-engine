#ifndef _IDWALKER_H_
#define _IDWALKER_H_

#include "..\..\common_h\vidwalker.h"


class IDWALKER : VIDWALKER
{
	void * pLayer;
public:
	 IDWALKER(){pInterface = null; pLayer = null;Search_Link = null;};
	~IDWALKER(){if(pInterface) pInterface->Invalidate();};
	ENTITY_ID * GetID();
	ENTITY_ID * GetIDNext();
	void Invalidate();
	void SetLayer(void * pl){pLayer = pl;};
	void SetInterface(void * pi){pInterface = (VIDWALKER *)pi;};
	VIDWALKER * pInterface;
	LPBYTE Search_Link;
	bool Validate() { return true; };
};

class TIDWALKER : public VIDWALKER
{
	IDWALKER * pReal;
	bool Invalide;
public:
	TIDWALKER(void * preal)
	{
		pReal = (IDWALKER*)preal; 
		Invalide = false;
	};
	~TIDWALKER()
	{
		pReal->Invalidate();
	};
	ENTITY_ID * GetID()
	{ 
		if(Invalide) return 0;
		return pReal->GetID();
	};
	ENTITY_ID * GetIDNext()
	{
		if(Invalide) return 0;
		return pReal->GetIDNext();
	};
	void Invalidate()
	{
		Invalide = true;
	}
	bool Validate() { return !Invalide; };
};



#endif