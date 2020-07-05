#ifndef AIHELPER_HPP
#define AIHELPER_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\island_base.h"
#include "..\common_h\collide.h"
#include "..\common_h\collide.h"
#include "..\common_h\Island_Base.h"
#include "..\common_h\defines.h"
#include "..\common_h\Character.h"
#include "..\engine\program\sea_ai\Script_Defines.h"
#include "..\engine\program\Events.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\DTimer.h"
#include "..\common_h\SD2_H\SaveLoad.h"

class AIAttributesHolder
{
protected:
	ATTRIBUTES		* pACharacter;
public:
	virtual void		SetACharacter(ATTRIBUTES * pAP) { pACharacter = pAP; };
	virtual ATTRIBUTES	* GetACharacter() const { return pACharacter; };
};

class VAI_INNEROBJ;
class AIHelper
{
public:
	AIHelper();
	~AIHelper();

	static	ATTRIBUTES		* pASeaCameras;
	static	ISLAND_BASE		* pIsland;
	static	VDX8RENDER		* pRS;
	static	COLLIDE			* pCollide;

	static	float			fGravity;

	bool	SetDevice();
	bool	Init();
	bool	Uninit();
	void	AddCharacter(ATTRIBUTES * pACharacter, ATTRIBUTES * pAMainCharacter);
	void	CalculateRelations();

	bool	isFriend(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const;
	bool	isEnemy(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const;
	bool	isNeutral(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const;

	ATTRIBUTES * GetMainCharacter(ATTRIBUTES * pACharacter);

	static	VAI_INNEROBJ	* FindAIInnerObj(ATTRIBUTES * pACharacter);

	dword	GetRelation(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const;
	dword	GetRelationSafe(ATTRIBUTES * pA1, ATTRIBUTES * pA2) const;

	static void	Print(float x, float y, float fScale, char * pFormat, ...);
	static void Print3D(CVECTOR vPos, float dy, float fScale, char * pFormat, ...);

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);

private:
	dword				* pRelations, dwRelationSize;
	array<ATTRIBUTES*>	aCharacters, aMainCharacters;

	dword * GetRelation(dword x, dword y) const;
	dword FindIndex(ATTRIBUTES * pACharacter) const;
};

extern AIHelper	Helper;

#endif