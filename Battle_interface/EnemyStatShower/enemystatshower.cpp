#include "EnemyStatShower.h"
#include "..\utils.h"
#include "..\..\common_h\model.h"

EnemyStatShower::EnemyStatShower()
{
	SetBeginData();
}

EnemyStatShower::~EnemyStatShower()
{
	Release();
}

bool EnemyStatShower::Init()
{
	if( !ReadAndCreate() ) return false;
	return true;
}

void EnemyStatShower::Realize(dword delta_time)
{
}

dword _cdecl EnemyStatShower::ProcessMessage(MESSAGE & message)
{
	long nMsgCod = message.Long();
	switch( nMsgCod )
	{
	case 0: // add character for shower
		{
			//ENTITY_ID mdlEID = GetModelEIDFromCharacterEID( message.EntityID() );
		}
	break;
}
	return -1;
}

bool EnemyStatShower::ReadAndCreate()
{
	//BIUtils::ReadVectorFormAttr(AttributesPointer,"pos",m_mtxpos.Pos(),CVECTOR(0.f));
	return true;
}

void EnemyStatShower::SetBeginData()
{
}

void EnemyStatShower::Release()
{
}

ENTITY_ID EnemyStatShower::GetModelEIDFromCharacterEID(ENTITY_ID& chrEID)
{
	ENTITY_ID eid;
	VDATA* pvdat = (VDATA*)api->GetScriptVariable("g_TmpModelVariable");
	if( pvdat ) {
		api->Send_Message( chrEID, "le", MSG_CHARACTER_GETMODEL, pvdat );
		eid = pvdat->GetEntityID();
	}
	return eid;
}
