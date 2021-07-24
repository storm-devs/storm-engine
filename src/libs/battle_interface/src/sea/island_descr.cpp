#include "island_descr.h"

#include "core.h"

#include "model.h"
#include "shared/battle_interface/msg_control.h"

ISLAND_DESCRIBER::ISLAND_DESCRIBER() : m_lastFindIdx(0), m_nFindType(0)
{
    m_bYesIsland = false;
    m_pIslandAttributes = nullptr;
    m_pLocators = nullptr;
    m_nLocators = 0;
}

ISLAND_DESCRIBER::~ISLAND_DESCRIBER()
{
    ReleaseAll();
}

void ISLAND_DESCRIBER::ReleaseAll()
{
    STORM_DELETE(m_pLocators);
    m_nLocators = 0;
    m_pIslandAttributes = nullptr;
    m_bYesIsland = false;
}

void ISLAND_DESCRIBER::SetIsland(ATTRIBUTES *pAIsland)
{
    if (pAIsland == nullptr)
        return;
    m_bYesIsland = true;
    m_pIslandAttributes = pAIsland;
    // create a list of locators
    auto *pA = pAIsland->GetAttributeClass("reload");
    if (pA == nullptr)
        return;
    if ((m_nLocators = pA->GetAttributesNum()) == 0)
        return;
    if ((m_pLocators = new LOCATOR_DESCR[m_nLocators]) == nullptr)
    {
        throw std::runtime_error("Can`t allocate memory");
    }
    // go through all locators
    ATTRIBUTES *pATmp;
    for (auto i = 0; i < m_nLocators; i++)
    {
        m_pLocators[i].pchr_note = nullptr;
        if ((pATmp = m_pLocators[i].pA = pA->GetAttributeClass(i)) == nullptr)
            continue;
        m_pLocators[i].pchr_note = pATmp->GetAttribute("labelLoc");
        m_pLocators[i].x = pATmp->GetAttributeAsFloat("x", 0.f);
        m_pLocators[i].z = pATmp->GetAttributeAsFloat("z", 0.f);
        m_pLocators[i].r = pATmp->GetAttributeAsFloat("radius", 0.f);
        m_pLocators[i].locatorType = ISLAND_LOCATOR_LAND;
        m_pLocators[i].relation = BI_RELATION_NEUTRAL;
        m_pLocators[i].picIdx = -1;
        m_pLocators[i].texIdx = -1;
        m_pLocators[i].characterIndex = -1;
        m_pLocators[i].bDiseased = false;
        auto *pvdat = core.Event("evntGetLandData", "a", pATmp);
        if (pvdat)
        {
            long lTmp;
            if (pvdat->Get(lTmp, 0))
                if (lTmp == 0)
                    m_pLocators[i].locatorType = ISLAND_LOCATOR_LAND;
                else if (lTmp == 1)
                    m_pLocators[i].locatorType = ISLAND_LOCATOR_FORT;
                else
                    m_pLocators[i].locatorType = ISLAND_LOCATOR_TOWN;
            if (pvdat->Get(lTmp, 1))
                m_pLocators[i].relation = lTmp;
            if (pvdat->Get(lTmp, 2))
                m_pLocators[i].texIdx = lTmp;
            if (pvdat->Get(lTmp, 3))
                m_pLocators[i].picIdx = lTmp;
            if (pvdat->Get(lTmp, 4))
                m_pLocators[i].selPicIdx = lTmp;
            if (pvdat->Get(lTmp, 5))
                m_pLocators[i].bDiseased = (lTmp != 0);
            m_pLocators[i].pchr_note = pATmp->GetAttribute("labelLoc");
        }
    }
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstLocator()
{
    m_nFindType = ISLAND_FIND_LOCATOR;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstLand()
{
    m_nFindType = ISLAND_FIND_LAND;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstFort()
{
    m_nFindType = ISLAND_FIND_FORT;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstFriendFort()
{
    m_nFindType = ISLAND_FIND_FRIENDFORT;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstNeutralFort()
{
    m_nFindType = ISLAND_FIND_NEUTRALFORT;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstEnemyFort()
{
    m_nFindType = ISLAND_FIND_ENEMYFORT;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetFirstBrokenFort()
{
    m_nFindType = ISLAND_FIND_BROKENFORT;
    m_lastFindIdx = 0;
    if (m_nLocators == 0 && m_pLocators == nullptr)
        return nullptr;
    return FindLocator(m_pLocators, m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::GetNext()
{
    if (m_lastFindIdx < 0 || m_lastFindIdx >= m_nLocators)
        return nullptr;
    assert(m_pLocators != NULL);
    return FindLocator(&m_pLocators[m_lastFindIdx], m_nLocators - m_lastFindIdx);
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::FindLocator(LOCATOR_DESCR *p, size_t nMax)
{
    assert(p != NULL);
    if (nMax < 1)
        return nullptr;
    int i;

    switch (m_nFindType)
    {
    case ISLAND_FIND_LOCATOR:
        m_lastFindIdx++;
        return &p[0];
    case ISLAND_FIND_LAND:
        for (i = 0; i < nMax; i++)
            if (p[i].locatorType == ISLAND_LOCATOR_LAND)
            {
                m_lastFindIdx += i + 1;
                return &p[i];
            }
        break;
    default:
        for (i = 0; i < nMax; i++)
            if (p[i].locatorType == ISLAND_LOCATOR_FORT)
                switch (m_nFindType)
                {
                case ISLAND_FIND_FORT:
                    m_lastFindIdx += i + 1;
                    return &p[i];
                case ISLAND_FIND_FRIENDFORT:
                    if (p[i].relation == BI_RELATION_FRIEND)
                    {
                        m_lastFindIdx += i + 1;
                        return &p[i];
                    }
                    break;
                case ISLAND_FIND_NEUTRALFORT:
                    if (p[i].relation == BI_RELATION_NEUTRAL)
                    {
                        m_lastFindIdx += i + 1;
                        return &p[i];
                    }
                    break;
                case ISLAND_FIND_ENEMYFORT:
                    if (p[i].relation == BI_RELATION_ENEMY)
                    {
                        m_lastFindIdx += i + 1;
                        return &p[i];
                    }
                    break;
                case ISLAND_FIND_BROKENFORT:
                    m_lastFindIdx += i + 1;
                    return &p[i];
                }
        break;
    }
    // found nothing
    m_lastFindIdx += nMax;
    return nullptr;
}

ISLAND_DESCRIBER::LOCATOR_DESCR *ISLAND_DESCRIBER::FindLocatorByName(char *name) const
{
    if (name == nullptr)
        return nullptr;
    for (auto i = 0; i < m_nLocators; i++)
    {
        if (m_pLocators[i].pA == nullptr)
            continue;
        auto *const curName = m_pLocators[i].pA->GetAttribute("name");
        if (curName != nullptr && _stricmp(name, curName) == 0)
            return &m_pLocators[i];
    }
    return nullptr;
}

void ISLAND_DESCRIBER::Refresh() const
{
    if (m_pLocators == nullptr)
        return;
    for (auto i = 0; i < m_nLocators; i++)
    {
        if (m_pLocators[i].locatorType == ISLAND_LOCATOR_FORT)
        {
            m_pLocators[i].relation =
                GetVDATALong(core.Event(BI_EVENT_GET_FORT_RELATION, "a", m_pLocators[i].pA), BI_RELATION_NEUTRAL);
        }
    }
}
