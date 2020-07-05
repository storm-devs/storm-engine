#include <assert.h>
#include "ships_list.h"
#include "battle_command.h"

#define LOADING_MODE_MY_SHIP		0
#define LOADING_MODE_ENEMY_SHIP		1

class TMP_LONG_STACK
{
	long sizeIncr;
	long *ldat;
	long datsize;
	long curidx;
	long defReturn;
public:
	TMP_LONG_STACK() {sizeIncr=100;ldat=null;datsize=0;curidx=0;defReturn=-1;}
	~TMP_LONG_STACK() {if(ldat!=null) delete ldat; ldat=null;}
	void Push(long data);
	long Pop();
	long GetFore();
	void SetSizeIncrement(long sizeinc) {sizeIncr=sizeinc;}
	void SetDefReturn(long defRet) {defReturn=defRet;}
};
void TMP_LONG_STACK::Push(long data)
{
	if(curidx>=datsize)
	{
		if(sizeIncr<=0)
		{
			api->Trace("WARNING! push for TMP_LONG_STACK impossible - array grid <= 0");
			return;
		}
		long *pold = ldat;
		ldat = NEW long[datsize+sizeIncr];
		if(ldat==null)	{_THROW("allocate memory error");}
		if(pold!=null)
		{
			memcpy(ldat,pold,sizeof(long)*datsize);
			delete pold;
		}
		datsize += sizeIncr;
	}
	ldat[curidx++] = data;
}
long TMP_LONG_STACK::GetFore()
{
	if(ldat==null || curidx<=0)
	{
		api->Trace("WARNING! GetFore from TMP_LONG_STACK is empty");
		return defReturn;
	}
	long retVal = ldat[0];
	curidx--;
	if(curidx==0)
	{
		delete ldat;
		ldat = null;
		datsize = 0;
	}
	else	for(int i=0; i<curidx; i++)	ldat[i] = ldat[i+1];
	return retVal;
}
long TMP_LONG_STACK::Pop()
{
	if(ldat==null || curidx<=0)
	{
		api->Trace("WARNING! pop from TMP_LONG_STACK is empty");
		return defReturn;
	}
	long retVal = ldat[--curidx];
	if(curidx==0)
	{
		delete ldat;
		ldat = null;
		datsize = 0;
	}
	return retVal;
}

void SetNLongData(VDATA * pvd, int n, ...)
{
	va_list args;
	va_start(args,n);

	for(int i=0; i<n; i++)
	{
		long * pIVal = va_arg(args,long*);
		if(!pIVal) break;
		long nDefVal = va_arg(args,long);
		if(pvd) pvd->Get(*pIVal,i);
		else *pIVal = nDefVal;
	}

	va_end(args);
}

SHIP_DESCRIBE_LIST::SHIP_DESCRIBE_LIST()
{
	root = NULL;
	mainCharacter = NULL;
	pMainShipAttr = NULL;
}

SHIP_DESCRIBE_LIST::~SHIP_DESCRIBE_LIST()
{
	ReleaseAll();
}

void SHIP_DESCRIBE_LIST::ShipSink(long charIdx)
{
	SHIP_DESCR * sd = FindShip(charIdx);
	if(sd==NULL) return;
	sd->isDead = true;
}

void SHIP_DESCRIBE_LIST::Release(long charIdx)
{
	if(charIdx == -1L) return;
	if(root==NULL) return;

	SHIP_DESCR * sd = root;
	if(root->characterIndex == charIdx)
	{
		root = root->next;
		delete sd;
		return;
	}

	SHIP_DESCR * pr = root->next;
	while(pr!=NULL)
	{
		if(pr->characterIndex == charIdx)
		{
			sd->next = pr->next;
			delete pr;
			return;
		}
		sd=pr; pr=pr->next;
	}

	mainCharacter = NULL;
	pMainShipAttr = NULL;
}

void SHIP_DESCRIBE_LIST::Add(long chIdx, ATTRIBUTES * pChAttr, ATTRIBUTES * pShipAttr, bool bMyShip,long relation)
{
	assert(pChAttr!=NULL);
	assert(pShipAttr!=NULL);
	SHIP_DESCR * pr = NEW SHIP_DESCR;
	if(pr==NULL)
	{
		_THROW("Can`t allocate memory");
	}
	pr->characterIndex = chIdx;
	pr->maxCrew = pShipAttr->GetAttributeAsDword("MaxCrew");
	pr->maxHP = pShipAttr->GetAttributeAsDword("HP");
	pr->maxSP = pShipAttr->GetAttributeAsDword("SP");
	pr->isMyShip = bMyShip;
	pr->relation = relation;
	pr->isDead = false;

	ATTRIBUTES * pAttr = pChAttr->GetAttributeClass("Ship");
	assert(pAttr!=NULL);
	pr->pAttr = pAttr;
	long lTmp;
	SetNLongData( api->Event(BI_EVENT_GET_DATA,"ll",BIDT_SHIPPICTURE,chIdx), 4, &pr->pictureNum,0, &pr->selectPictureNum,0, &pr->textureNum,-1, &lTmp,0 );//&pr->isDead,false );
	pr->isDead = lTmp!=0;

	pr->next = NULL;

	// find this ship
	ENTITY_ID ei;
	if(_CORE_API->FindClass(&ei,"ship",0)) do
	{
		VAI_OBJBASE * vob = (VAI_OBJBASE*)_CORE_API->GetEntityPointer(&ei);
		if(vob==NULL) continue;
		ATTRIBUTES *pA = vob->GetACharacter();
		if((long)pA->GetAttributeAsDword("index")==chIdx)
		{
			pr->pShip = vob;
			break;
		}
	} while(_CORE_API->FindClassNext(&ei));

	SHIP_DESCR * ptmp = root;
	if(ptmp==NULL)	root = pr;
	else
	{
		while(ptmp->next!=NULL) ptmp = ptmp->next;
		ptmp->next = pr;
	}

	if(pChAttr->GetAttributeAsDword("MainCharacter",0)!=0)
	{
		mainCharacter = pr;
		pMainShipAttr = pShipAttr;
	}
}

void SHIP_DESCRIBE_LIST::ReleaseAll()
{
	while(root!=NULL)
	{
		SHIP_DESCR * ptmp = root;
		root = root->next;
		delete ptmp;
	}
	root = NULL;
	mainCharacter = NULL;
	pMainShipAttr = NULL;
}

SHIP_DESCRIBE_LIST::SHIP_DESCR * SHIP_DESCRIBE_LIST::FindShip(long idxCharacter)
{
	for(SHIP_DESCR *ptmp=root; ptmp!=NULL; ptmp=ptmp->next)
		if(ptmp->characterIndex==idxCharacter)
			return ptmp;
	return NULL;
}

void SHIP_DESCRIBE_LIST::Refresh()
{
	ReleaseAll();

	TMP_LONG_STACK tls;
	ENTITY_ID ei;
//	tls.Push(-1);

	if(api->FindClass(&ei,"ship",0)) do
	{
		VAI_OBJBASE * vob = (VAI_OBJBASE*)_CORE_API->GetEntityPointer(&ei);
		if(vob==NULL) continue;
		ATTRIBUTES * pA= vob->GetACharacter();
		if(pA==NULL) continue;
		tls.Push((long)pA->GetAttributeAsDword("index"));
	} while(api->FindClassNext(&ei));
	tls.Push(-1);

	for(long chrIdx = tls.GetFore(); chrIdx>=0; chrIdx = tls.GetFore())
	{
		api->Event("BI_CallUpdateShip","l",chrIdx);
	}

	BATTLE_COMMAND::m_bMakeModeUpdate = true;
}
