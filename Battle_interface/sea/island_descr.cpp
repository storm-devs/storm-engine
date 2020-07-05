#include "island_descr.h"
#include "..\..\common_h\model.h"

ISLAND_DESCRIBER::ISLAND_DESCRIBER()
{
	m_bYesIsland = false;
	m_pIslandAttributes = NULL;
	m_pLocators = NULL;
	m_nLocators = 0;
}

ISLAND_DESCRIBER::~ISLAND_DESCRIBER()
{
	ReleaseAll();
}

void ISLAND_DESCRIBER::ReleaseAll()
{
	PTR_DELETE(m_pLocators);
	m_nLocators = 0;
	m_pIslandAttributes = NULL;
	m_bYesIsland = false;
}

void ISLAND_DESCRIBER::SetIsland(ATTRIBUTES * pAIsland)
{
	if(pAIsland==NULL) return;
	m_bYesIsland = true;
	m_pIslandAttributes = pAIsland;
	// создать список локаторов
	ATTRIBUTES * pA = pAIsland->GetAttributeClass("reload");
	if(pA==NULL) return;
	if( (m_nLocators=pA->GetAttributesNum()) == 0 ) return;
	if( (m_pLocators=NEW LOCATOR_DESCR[m_nLocators])==NULL )
	{
		_THROW("Can`t allocate memory");
	}
	// пройтись по всем локаторам
	ATTRIBUTES * pATmp;
	for(int i=0; i<m_nLocators; i++)
	{
		m_pLocators[i].pchr_note = null;
		if( (pATmp=m_pLocators[i].pA=pA->GetAttributeClass(i))==null ) continue;
		m_pLocators[i].pchr_note = pATmp->GetAttribute("labelLoc");
		m_pLocators[i].x = pATmp->GetAttributeAsFloat("x",0.f);
		m_pLocators[i].z = pATmp->GetAttributeAsFloat("z",0.f);
		m_pLocators[i].r = pATmp->GetAttributeAsFloat("radius",0.f);
		m_pLocators[i].locatorType = ISLAND_LOCATOR_LAND;
		m_pLocators[i].relation = BI_RELATION_NEUTRAL;
		m_pLocators[i].picIdx = -1;
		m_pLocators[i].texIdx = -1;
		m_pLocators[i].bDiseased = false;
		VDATA * pvdat = api->Event("evntGetLandData","a",pATmp);
		if(pvdat) {
			long lTmp;
			if( pvdat->Get(lTmp,0) )
				if( lTmp==0 ) m_pLocators[i].locatorType = ISLAND_LOCATOR_LAND;
				else if( lTmp==1 ) m_pLocators[i].locatorType = ISLAND_LOCATOR_FORT;
				else m_pLocators[i].locatorType = ISLAND_LOCATOR_TOWN;
			if( pvdat->Get(lTmp,1) )	m_pLocators[i].relation = lTmp;
			if( pvdat->Get(lTmp,2) )	m_pLocators[i].texIdx = lTmp;
			if( pvdat->Get(lTmp,3) )	m_pLocators[i].picIdx = lTmp;
			if( pvdat->Get(lTmp,4) )	m_pLocators[i].selPicIdx = lTmp;
			if( pvdat->Get(lTmp,5) )	m_pLocators[i].bDiseased = (lTmp!=0);
			m_pLocators[i].pchr_note = pATmp->GetAttribute("labelLoc");
		}
	}
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstLocator()
{
	m_nFindType=ISLAND_FIND_LOCATOR;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstLand()
{
	m_nFindType=ISLAND_FIND_LAND;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstFort()
{
	m_nFindType=ISLAND_FIND_FORT;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstFriendFort()
{
	m_nFindType=ISLAND_FIND_FRIENDFORT;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstNeutralFort()
{
	m_nFindType=ISLAND_FIND_NEUTRALFORT;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstEnemyFort()
{
	m_nFindType=ISLAND_FIND_ENEMYFORT;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetFirstBrokenFort()
{
	m_nFindType=ISLAND_FIND_BROKENFORT;
	m_lastFindIdx=0;
	if(m_nLocators==0 && m_pLocators==NULL) return NULL;
	return FindLocator(m_pLocators,m_nLocators);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::GetNext()
{
	if(m_lastFindIdx<0 || m_lastFindIdx>=m_nLocators) return NULL;
	assert(m_pLocators!=NULL);
	return FindLocator(&m_pLocators[m_lastFindIdx],m_nLocators-m_lastFindIdx);
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::FindLocator(LOCATOR_DESCR * p,int nMax)
{
	assert(p!=NULL);
	if(nMax<1) return NULL;
	int i;

	switch(m_nFindType)
	{
	case ISLAND_FIND_LOCATOR:
		m_lastFindIdx++;
		return &p[0];
	case ISLAND_FIND_LAND:
		for(i=0;i<nMax;i++)
			if(p[i].locatorType==ISLAND_LOCATOR_LAND)
			{
				m_lastFindIdx+=i+1;
				return &p[i];
			}
		break;
	default:
		for(i=0;i<nMax;i++)
			if(p[i].locatorType==ISLAND_LOCATOR_FORT)
				switch(m_nFindType)
			{
				case ISLAND_FIND_FORT:
					m_lastFindIdx+=i+1;
					return &p[i];
				case ISLAND_FIND_FRIENDFORT:
					if(p[i].relation==BI_RELATION_FRIEND)
					{
						m_lastFindIdx+=i+1;
						return &p[i];
					}
					break;
				case ISLAND_FIND_NEUTRALFORT:
					if(p[i].relation==BI_RELATION_NEUTRAL)
					{
						m_lastFindIdx+=i+1;
						return &p[i];
					}
					break;
				case ISLAND_FIND_ENEMYFORT:
					if(p[i].relation==BI_RELATION_ENEMY)
					{
						m_lastFindIdx+=i+1;
						return &p[i];
					}
					break;
				case ISLAND_FIND_BROKENFORT:
					m_lastFindIdx+=i+1;
					return &p[i];
			}
		break;
	}
	// ничего не нашли
	m_lastFindIdx+=nMax;
	return NULL;
}

ISLAND_DESCRIBER::LOCATOR_DESCR * ISLAND_DESCRIBER::FindLocatorByName(char * name)
{
	if(name==NULL) return NULL;
	for(int i=0; i<m_nLocators; i++)
	{
		if( m_pLocators[i].pA==NULL ) continue;
		char * curName = m_pLocators[i].pA->GetAttribute("name");
		if(curName!=NULL && stricmp(name,curName)==0) return &m_pLocators[i];
	}
	return NULL;
}

void ISLAND_DESCRIBER::Refresh()
{
	if(m_pLocators==null) return;
	for(int i=0; i<m_nLocators; i++)
	{
		if( m_pLocators[i].locatorType == ISLAND_LOCATOR_FORT )
		{
			m_pLocators[i].relation = GetVDATALong(api->Event(BI_EVENT_GET_FORT_RELATION,"a",m_pLocators[i].pA),BI_RELATION_NEUTRAL);
		}
	}
}
