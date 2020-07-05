#include <stdio.h>
#include "ibattle.h"
#include "battle_land.h"

#include "..\engine\program\battle_interface\msg_control.h"
#include "ships_list.h"
#include "island_descr.h"
#include "LogAndAction.h"
#include "IBoardingStatus.h"
#include "spyglass.h"
#include "ShipPointer.h"
#include "ActivePerkShower.h"
#include "battle_shipsign.h"

INTERFACE_FUNCTION
CREATE_CLASS(BATTLE_INTERFACE)
CREATE_CLASS(ILogAndActions)
CREATE_CLASS(IBoardingStatus)
CREATE_CLASS(BATTLE_LAND_INTERFACE)
CREATE_CLASS(ISPYGLASS)
CREATE_CLASS(SHIPPOINTER)
CREATE_CLASS(ActivePerkShower)

ENTITY_ID idBattleInterface;
SHIP_DESCRIBE_LIST g_ShipList;
ISLAND_DESCRIBER g_IslandDescr;

DWORD tmExecute,tmRealize;

#define MINIMAP_ZOOM_IN			"MiniMapZoomIn"
#define MINIMAP_ZOOM_OUT		"MiniMapZoomOut"

DWORD g_dwBlinkColor;

BATTLE_INTERFACE::BATTLE_INTERFACE()
{
	g_IslandDescr.ReleaseAll();
	g_ShipList.ReleaseAll();
	m_pMessageIcons = null;
	m_pShipIcon = null;
	rs = NULL;
	idBattleInterface = GetID();
	m_bShowCommandMenu = true;
	m_bShowBattleNavigator = true;
	m_bYesShowAll = false;
	m_bVisible = true;
	m_fBlinkSpeed = 0.f;
	m_fCurBlinkTime = 0.f;
	m_bNeedIslandSet = true;
	m_bMyShipView = false;
}

BATTLE_INTERFACE::~BATTLE_INTERFACE()
{
	DELETE(m_pMessageIcons);
	DELETE( m_pShipIcon );
}

bool BATTLE_INTERFACE::Init()
{
	if( (rs=(VDX8RENDER *)_CORE_API->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}

	LoadIniFile();

	return true;
}

void BATTLE_INTERFACE::Execute(dword delta_time)
{
    _asm rdtsc _asm mov tmExecute,eax
	m_fCurBlinkTime += delta_time*m_fBlinkSpeed;
	if(m_fCurBlinkTime>PIm2) m_fCurBlinkTime-=PIm2;
	g_dwBlinkColor = (DWORD)(255*fabsf(sinf(m_fCurBlinkTime)))&0xFF;
	CheckSeaState();
	if(m_bYesShowAll && m_bVisible)
	{
		CONTROL_STATE cs;

		if(m_bShowBattleNavigator)
		{
			api->Controls->GetControlState(MINIMAP_ZOOM_IN,cs);
			if(cs.state == CST_ACTIVE)	BattleNavigator.DecrementScale();
			api->Controls->GetControlState(MINIMAP_ZOOM_OUT,cs);
			if(cs.state == CST_ACTIVE)	BattleNavigator.IncrementScale();
		}

		if( m_bShowCommandMenu && m_pShipIcon )
		{
			if( !m_pShipIcon->IsActive() )
			{
				api->Controls->GetControlState(BI_COMMANDS_ACTIVATE,cs);
				if( cs.state == CST_ACTIVATED )
				{
					m_pShipIcon->SetActive( true );
				}
			}
			else
			{
				m_pShipIcon->MakeControl();
			}
			/*if( CommandMenu.GetComState() != 0 )	api->Controls->GetControlState(BI_COMMANDS_CONFIRM,cs);
			else	api->Controls->GetControlState(BI_COMMANDS_ACTIVATE,cs);
			if(cs.state == CST_ACTIVATED)
				CommandMenu.DoControl(BI_MSG_COMMAND_ACTIVATE,-1);

			api->Controls->GetControlState(BI_COMMANDS_RIGHTSTEP,cs);
			if(cs.state == CST_ACTIVATED)
				CommandMenu.DoControl(BI_MSG_COMMAND_RIGHT,-1);

			api->Controls->GetControlState(BI_COMMANDS_LEFTSTEP,cs);
			if(cs.state == CST_ACTIVATED)
				CommandMenu.DoControl(BI_MSG_COMMAND_LEFT,-1);

			api->Controls->GetControlState(BI_COMMANDS_CANCEL,cs);
			if(cs.state == CST_ACTIVATED)
				CommandMenu.DoControl(BI_MSG_COMMAND_DEACTIVATE,-1);

			CommandMenu.Update();*/
		}
		if(m_bShowBattleNavigator)	BattleNavigator.Update();

		if(m_pMessageIcons)
		{
			//if(CommandMenu.GetComState()==0)
			if( !m_pShipIcon || !m_pShipIcon->IsActive() )
			{
				if(!m_bMyShipView) api->Event("blieGetMsgIconRoot");
				m_bMyShipView = true;
			}
			else
			{
				if(m_bMyShipView) m_pMessageIcons->SetShowMsg(false);
				m_bMyShipView = false;
			}
		}
	}
	if(m_pMessageIcons) m_pMessageIcons->Update(delta_time);
	/*if(AttributesPointer!=null)
		AttributesPointer->SetAttributeUseDword("ComState",CommandMenu.GetComState());*/
    _asm rdtsc  _asm sub eax,tmExecute _asm mov tmExecute,eax
}

void BATTLE_INTERFACE::Realize(dword delta_time)
{
#ifndef _XBOX
		//if(GetAsyncKeyState('K')<0) return;
#endif
    _asm rdtsc _asm mov tmRealize,eax
	if(m_bNeedIslandSet)
	{
		api->Trace("Island Set");
		BattleNavigator.SetIsland();
		m_bNeedIslandSet = false;
	}

	if(m_bYesShowAll)
	{
		if(m_bVisible)
		{
			if( m_bShowCommandMenu ) m_pShipIcon->Draw();
			//if(m_bShowCommandMenu) CommandMenu.Draw();
			if( m_bShowBattleNavigator ) BattleNavigator.Draw();
			if( m_pMessageIcons ) m_pMessageIcons->Draw();
		}
	}
	else
	{
		m_bYesShowAll=true;
	}

    _asm rdtsc  _asm sub eax,tmRealize _asm mov tmRealize,eax
	//rs->Print(0,0,"Realize=%ld, Execute=%ld",tmRealize,tmExecute);
}

void BATTLE_INTERFACE::LoadIniFile()
{
    GUARD(BATTLE_INTERFACE::LoadIniFile)

	m_fBlinkSpeed = .003f;
	if(AttributesPointer!=null)
		m_fBlinkSpeed = AttributesPointer->GetAttributeAsFloat("blindSpeed",m_fBlinkSpeed);
	m_fCurBlinkTime = 0;

	//CommandMenu.Init(GetID(),rs);
	BattleNavigator.Init(rs);

	ATTRIBUTES * pA = null;
	if(AttributesPointer!=null)
	{
		m_bShowCommandMenu = AttributesPointer->GetAttributeAsDword("ShowCommands",1) != 0;
		m_bShowBattleNavigator = AttributesPointer->GetAttributeAsDword("ShowNavigator",1) != 0;
		pA = AttributesPointer->FindAClass(AttributesPointer,"MessageIcons");
	}

	DELETE( m_pMessageIcons );
	m_pMessageIcons = NEW MESSAGE_ICONS;
	if(m_pMessageIcons==NULL) {
		THROW("allocate memory error");
	}
	if(m_pMessageIcons)	m_pMessageIcons->InitData(GetID(),rs,pA);

	DELETE( m_pShipIcon );
	m_pShipIcon = NEW BIShipIcon( GetID(), rs );
	Assert( m_pShipIcon );
	m_pShipIcon->Init( AttributesPointer, AttributesPointer ? AttributesPointer->GetAttributeClass("ShipIcon") : null );

    UNGUARD
}

dword BATTLE_INTERFACE::AttributeChanged(ATTRIBUTES * pAttr)
{
	return 0;
}

dword _cdecl BATTLE_INTERFACE::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
	case BI_MSG_SET_ISLAND:
		m_bNeedIslandSet = true;
		g_IslandDescr.SetIsland(message.AttributePointer());
		m_bMyShipView = false;
		break;
	case BI_IN_CREATE_SHIP: // "laall"
		{
			long chIdx = message.Long();
			ATTRIBUTES * pChAttr = message.AttributePointer();
			ATTRIBUTES * pShipAttr = message.AttributePointer();
			bool bMyShip = (message.Long() != 0L);
			long relation = message.Long();
			g_ShipList.Add(chIdx,pChAttr,pShipAttr,bMyShip,relation);
			if( m_pShipIcon ) m_pShipIcon->SetUpdate();
		}
		//CommandMenu.SetMode(-1,-1,-1,-1);
		break;
	case BI_IN_DELETE_SHIP: // "ll"
		g_ShipList.Release(message.Long());
		//CommandMenu.SetMode(-1,-1,-1,-1);
		if( m_pShipIcon ) m_pShipIcon->SetUpdate();
		break;
	case BI_IN_DEAD_SHIP: // "l"
   		g_ShipList.ShipSink(message.Long());
		//CommandMenu.SetMode(-1,-1,-1,-1);
		if( m_pShipIcon ) m_pShipIcon->SetUpdate();
		break;
	case BI_IN_SET_COMMAND_MODE: // "lllll"
		{
			long comMode = message.Long();
			long startTextureNumber = message.Long();
			long startPictureNumber = message.Long();
			long characterNum = message.Long();
			long commandState = message.Long();
			//CommandMenu.SetMode(comMode,startTextureNumber,startPictureNumber,characterNum, commandState);
		}
		break;
	case BI_SET_VISIBLE: // "ll"
		m_bVisible = (0!=message.Long());
		break;
	case BI_SET_ISLAND_LOCATORS: // "la"
		g_IslandDescr.SetIsland(message.AttributePointer());
		break;
	case BI_MSG_NOT_ENOUGH_BALLS_FLAG: // "l"
		BattleNavigator.SetEnoughBalls(message.Long()!=0);
		break;
	case BI_MSG_REFRESH:
		g_ShipList.Refresh();
		g_IslandDescr.Refresh();
		if( m_pShipIcon ) m_pShipIcon->SetUpdate();
		break;

	case BI_MSG_SET_MSG_ICONS:
		if(m_pMessageIcons)
		{
			EnableMessageIcons(message.ScriptVariablePointer());
		}
		break;

	case BI_MSG_ADD_NEWTEXTURE:
		{
			char param[256];	message.String(sizeof(param)-1,param);
			int hQ = message.Long();
			int vQ = message.Long();
			return m_pShipIcon->AddTexture( param,hQ,vQ );
			//return CommandMenu.AddTexture(param, hQ, vQ);
		}
		break;
	}
	return 0;
}

ATTRIBUTES * _cdecl GetAttributesFromPath(ATTRIBUTES * pA,...)
{
	va_list arglist;

	char * sName;
	ATTRIBUTES *pTmpAttr = pA;
	va_start( arglist, pA );
	while( (sName=va_arg(arglist,char*))!=NULL )
	{
		if(pTmpAttr==NULL) return NULL;
		pTmpAttr = pTmpAttr->GetAttributeClass(sName);
	}
	va_end(arglist);

	return pTmpAttr;
}

void BATTLE_INTERFACE::CheckSeaState()
{
	SHIP_DESCRIBE_LIST::SHIP_DESCR * main_sd = g_ShipList.GetMainCharacterShip();
	if(main_sd==NULL) return;

	long nReloadTargetIndex=-1;
	float sqrRadius = api->Entity_GetAttributeAsFloat(&idBattleInterface,"boardRadius",0.f);
	sqrRadius *= sqrRadius;
	float minReloadRadius=sqrRadius;
	float sqrFreeDistance = api->Entity_GetAttributeAsFloat(&idBattleInterface,"freeDistance",500.f);
	sqrFreeDistance *= sqrFreeDistance;

	bool bSailTo=false,	bLandTroops=false,	bMap=true;
	bool bAttack=false,	bDefend=false,		bReload=false;

	for(SHIP_DESCRIBE_LIST::SHIP_DESCR * ps = g_ShipList.GetShipRoot(); ps!=NULL; ps=ps->next)
	{
		if(ps==main_sd) continue;
		bSailTo = true;
		if(ps->isMyShip)
		{
			bDefend = true;
			float curRad;
			if( (curRad=~(ps->pShip->GetPos() - main_sd->pShip->GetPos())) < sqrRadius )
			{
				bReload = true;
				if(curRad<minReloadRadius)
				{
					minReloadRadius = curRad;
					nReloadTargetIndex = ps->characterIndex;
				}
			}
		}
		else
		{
			if( ps->relation==BI_RELATION_ENEMY)
			{
				if( !ps->isDead )
				{
					bAttack=true;
					float fdist;
					if( (fdist = ~(ps->pShip->GetPos() - main_sd->pShip->GetPos()))<sqrFreeDistance ) bMap = false;
				}
			}
		}
	}

	if( g_IslandDescr.GetFirstLocator() != null )
		bSailTo = true;

	if( g_IslandDescr.GetFirstEnemyFort() != null )
		bAttack=true;

	api->Event(BI_EVENT_SET_SEA_STATE,"lllllll",bSailTo,bLandTroops,bMap,bAttack,bDefend,bReload,nReloadTargetIndex);
}

void BATTLE_INTERFACE::EnableMessageIcons(VDATA * pvdat)
{
	if(!m_pMessageIcons) return;
	m_pMessageIcons->SetShowMsg(false);
	if(!pvdat) return;
	m_pMessageIcons->SetShowMsg(true);

	ATTRIBUTES * pAttr[4];
	long pLeft[4];

	int nCommandos = 0;
	//CommandMenu.GetCommandData(nCommandos,pLeft);

	for(int i=0; i<4; i++)
	{
		pAttr[i] = null;
		if(i<nCommandos)	pAttr[i] = pvdat->GetAClass(i);
	}

	m_pMessageIcons->StartData( pAttr, pLeft );
}

DWORD GetIntervalColor(DWORD minV,DWORD maxV, float fpar)
{
	long a = minV>>24L;
	long ad = (long)(maxV>>24L) - a;
	long r = (minV&0xFF0000)>>16;
	long rd = (long)((maxV&0xFF0000)>>16) - r;
	long g = (minV&0xFF00)>>8;
	long gd = (long)((maxV&0xFF00)>>8) - g;
	long b = minV&0xFF;
	long bd = (long)(maxV&0xFF) - b;

	a += long(ad*fpar);
	r += long(rd*fpar);
	g += long(gd*fpar);
	b += long(bd*fpar);

	return ARGB(a,r,g,b);
}

long GetLongFromAttr(ATTRIBUTES * pA, char * name, long defVal)
{
	if(pA==null || name==null) return defVal;
	return pA->GetAttributeAsDword(name,defVal);
}

float GetFloatFromAttr(ATTRIBUTES * pA, char * name, float defVal)
{
	if(pA==null || name==null) return defVal;
	return pA->GetAttributeAsFloat(name,defVal);
}

bool ReadStringFromAttr(ATTRIBUTES * pA, char * name, char * buf, long bufSize, char * defVal)
{
	if(buf==null || bufSize<1) return false;
	buf[0] = 0;
	char * strGet;
	bool bRet = true;
	if( pA==null || (strGet=pA->GetAttribute(name))==null )
	{
		strGet = defVal;
		bRet = false;
	}

	if(strGet!=null)
	{
		if( (int)strlen(strGet)>bufSize-1 )
		{
			strncpy(buf,strGet,bufSize-1);
			buf[bufSize-1] = 0;
		}
		else strcpy(buf,strGet);
	} else bRet = false;

	return bRet;
}

char * GetStringFromAttr(ATTRIBUTES * pA, char * name, char * defVal)
{
	if(pA==null || name==null) return defVal;
	char * aVal = pA->GetAttribute(name);
	if(aVal==null) return defVal;
	return aVal;
}
