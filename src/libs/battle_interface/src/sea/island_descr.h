#pragma once

#include "../bi_defines.h"

// location type - drop-off points
#define ISLAND_LOCATOR_LAND 1
#define ISLAND_LOCATOR_FORT 2
#define ISLAND_LOCATOR_TOWN 3
// locator search type
#define ISLAND_FIND_LOCATOR 1
#define ISLAND_FIND_LAND 2
#define ISLAND_FIND_FORT 3
#define ISLAND_FIND_FRIENDFORT 4
#define ISLAND_FIND_ENEMYFORT 5
#define ISLAND_FIND_NEUTRALFORT 6
#define ISLAND_FIND_BROKENFORT 7

class ISLAND_DESCRIBER
{
  public:
    ISLAND_DESCRIBER();
    ~ISLAND_DESCRIBER();

    struct LOCATOR_DESCR
    {
        int locatorType;
        int relation;
        ATTRIBUTES *pA;
        float x, z, r;
        const char *pchr_note;
        int picIdx;
        int selPicIdx;
        int texIdx;
        int32_t characterIndex;
        bool bDiseased;
    };

    void ReleaseAll();

    bool YesIsland() const
    {
        return m_bYesIsland;
    }

    void SetIsland(ATTRIBUTES *pAIsland);
    LOCATOR_DESCR *GetFirstLocator();
    LOCATOR_DESCR *GetFirstLand();
    LOCATOR_DESCR *GetFirstFort();
    LOCATOR_DESCR *GetFirstFriendFort();
    LOCATOR_DESCR *GetFirstNeutralFort();
    LOCATOR_DESCR *GetFirstEnemyFort();
    LOCATOR_DESCR *GetFirstBrokenFort();
    LOCATOR_DESCR *GetNext();
    LOCATOR_DESCR *FindLocatorByName(char *name) const;

    void Refresh() const;

  protected:
    LOCATOR_DESCR *FindLocator(LOCATOR_DESCR *p, size_t nMax);

    bool m_bYesIsland;
    ATTRIBUTES *m_pIslandAttributes;
    LOCATOR_DESCR *m_pLocators;
    size_t m_nLocators;
    // find data
    size_t m_lastFindIdx;
    int m_nFindType;
};

extern ISLAND_DESCRIBER g_IslandDescr;
