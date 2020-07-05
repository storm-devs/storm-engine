#ifndef _BI_ENEMY_STAT_SHOWER_H_
#define _BI_ENEMY_STAT_SHOWER_H_

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\defines.h"
#include "..\..\common_h\templates\string.h"
#include "..\..\common_h\animation.h"

class MODEL;
class NODE;

class EnemyStatShower : public ENTITY
{
public:
	EnemyStatShower();
	~EnemyStatShower();

	bool Init();
	void Realize(dword delta_time);
	dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	bool ReadAndCreate();
	void SetBeginData();
	void Release();

	ENTITY_ID GetModelEIDFromCharacterEID(ENTITY_ID& chrEID);

protected: // data
	bool m_bVisible;
	ENTITY_ID m_eidChrEID;
	MODEL* m_pModel;
};

#endif
