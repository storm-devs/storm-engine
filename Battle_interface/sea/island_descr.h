#ifndef _ISLAND_DESCR_H_
#define _ISLAND_DESCR_H_

#include "..\defines.h"

// тип локации - места высадки
#define ISLAND_LOCATOR_LAND		1
#define ISLAND_LOCATOR_FORT		2
#define ISLAND_LOCATOR_TOWN		3
// тип поиска локаторов
#define ISLAND_FIND_LOCATOR		1
#define ISLAND_FIND_LAND		2
#define ISLAND_FIND_FORT		3
#define ISLAND_FIND_FRIENDFORT	4
#define ISLAND_FIND_ENEMYFORT	5
#define ISLAND_FIND_NEUTRALFORT	6
#define ISLAND_FIND_BROKENFORT	7

class ISLAND_DESCRIBER
{
public:
	ISLAND_DESCRIBER();
	~ISLAND_DESCRIBER();

	struct LOCATOR_DESCR
	{
		int		locatorType;
		int		relation;
		ATTRIBUTES * pA;
		float x,z,r;
		char *	pchr_note;
		int		picIdx;
		int		selPicIdx;
		int		texIdx;
		long	characterIndex;
		bool	bDiseased;
	};

	void ReleaseAll();
	bool YesIsland() {return m_bYesIsland;}
	void SetIsland(ATTRIBUTES * pAIsland);
	LOCATOR_DESCR * GetFirstLocator();
	LOCATOR_DESCR * GetFirstLand();
	LOCATOR_DESCR * GetFirstFort();
	LOCATOR_DESCR * GetFirstFriendFort();
	LOCATOR_DESCR * GetFirstNeutralFort();
	LOCATOR_DESCR * GetFirstEnemyFort();
	LOCATOR_DESCR * GetFirstBrokenFort();
	LOCATOR_DESCR * GetNext();
	LOCATOR_DESCR * FindLocatorByName(char * name);

	void Refresh();

protected:
	LOCATOR_DESCR * FindLocator(LOCATOR_DESCR * p,int nMax);

	bool			m_bYesIsland;
	ATTRIBUTES *	m_pIslandAttributes;
	LOCATOR_DESCR * m_pLocators;
	int				m_nLocators;
	// find data
	int				m_lastFindIdx;
	int				m_nFindType;
};

extern ISLAND_DESCRIBER g_IslandDescr;

#endif
