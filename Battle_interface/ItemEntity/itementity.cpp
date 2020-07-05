#include "itementity.h"
#include "..\utils.h"
#include "..\..\common_h\model.h"

ItemEntity::ItemEntity()
{
	SetBeginData();
}

ItemEntity::~ItemEntity()
{
	Release();
}

bool ItemEntity::Init()
{
	if( !ReadAndCreate() ) return false;
	return true;
}

void ItemEntity::Realize(dword delta_time)
{
	if( m_bVisible && m_pModel )
		if( m_bTieToLocator )
			DrawIntoLocator();
		else
			m_pModel->Realize( delta_time );
}

dword _cdecl ItemEntity::ProcessMessage(MESSAGE & message)
{
	long nMsgCod = message.Long();
	switch( nMsgCod )
	{
	case 0: // Tie item to moving locator
		{
			DeleteParticle();
			ENTITY_ID mdlEID = GetModelEIDFromCharacterEID( message.EntityID() );
			char locName[255];
			message.String( sizeof(locName)-1, locName );
			m_bTieToLocator = TieToLocator( mdlEID, locName );
		}
	break;

	case 1: // Untie item from moving locator and return to his place
		UnTieFromLocator();
		CreateParticle();
	break;

	case 2: // Turn off item show
		if( m_bVisible ) {
			UnTieFromLocator();
			m_bVisible = false;
			DeleteParticle();
		}
	break;

	case 3: // turn on item show
		m_bVisible = true;
		CreateParticle();
	break;

	case 4: // replace item to other position
		m_mtxpos.Pos().x = message.Float();
		m_mtxpos.Pos().y = message.Float();
		m_mtxpos.Pos().z = message.Float();
		m_mtxpos.Vx().x = message.Float();
		m_mtxpos.Vx().y = message.Float();
		m_mtxpos.Vx().z = message.Float();
		m_mtxpos.Vy().x = message.Float();
		m_mtxpos.Vy().y = message.Float();
		m_mtxpos.Vy().z = message.Float();
		m_mtxpos.Vz().x = message.Float();
		m_mtxpos.Vz().y = message.Float();
		m_mtxpos.Vz().z = message.Float();
		SetModelToPosition(m_mtxpos);
		DeleteParticle();
		CreateParticle();
	break;

	case 5:
		{
			DeleteParticle();
			ENTITY_ID mdlID = GetModelEIDFromCharacterEID( message.EntityID() );
			ENTITY_ID mdlToTieID = GetModelEIDFromCharacterEID( message.EntityID() );
			char sLocName[255];			message.String( sizeof(sLocName)-1, sLocName );
			char sStartEvntName[255];	message.String( sizeof(sStartEvntName)-1, sStartEvntName );
			char sEndEvntName[255];		message.String( sizeof(sEndEvntName)-1, sEndEvntName );
			SetEventListener(mdlID, mdlToTieID,sLocName, sStartEvntName,sEndEvntName);
		}
	break;
	}
	return -1;
}

bool ItemEntity::ReadAndCreate()
{
	BIUtils::ReadVectorFormAttr(AttributesPointer,"pos",m_mtxpos.Pos(),CVECTOR(0.f));
	BIUtils::ReadVectorFormAttr(AttributesPointer,"pos.vx",m_mtxpos.Vx(),CVECTOR(0.f));
	BIUtils::ReadVectorFormAttr(AttributesPointer,"pos.vy",m_mtxpos.Vy(),CVECTOR(0.f));
	BIUtils::ReadVectorFormAttr(AttributesPointer,"pos.vz",m_mtxpos.Vz(),CVECTOR(0.f));
	char* pcModelName = BIUtils::GetStringFromAttr(AttributesPointer,"model","");
	char* pcTechnique = BIUtils::GetStringFromAttr(AttributesPointer,"technique","");
	if( pcModelName ) {
		if( api->CreateEntity(&m_eidModel,"modelr") ) {
			api->Send_Message(m_eidModel,"ls",MSG_MODEL_LOAD_GEO,pcModelName);
			m_pModel = (MODEL*)api->GetEntityPointer(&m_eidModel);
			SetModelToPosition(m_mtxpos);
			SetTechnique( pcTechnique );
		}
	}
	m_bVisible = m_pModel!=0;
	if( m_bVisible )
		CreateParticle();
	return true;
}

void ItemEntity::SetBeginData()
{
	m_bVisible = false;
	m_pModel = 0;
	m_bTieToLocator = false;
	m_pParticle = 0;
}

void ItemEntity::Release()
{
	if( m_pModel ) {
		api->DeleteEntity( m_eidModel );
		m_pModel = 0;
	}
	DeleteParticle();
}

void ItemEntity::SetModelToPosition(const CMatrix& mtx)
{
	if( m_pModel ) {
		api->Send_Message(m_eidModel,"lffffffffffff",MSG_MODEL_SET_POSITION,
			m_mtxpos.Pos().x, m_mtxpos.Pos().y, m_mtxpos.Pos().z,
			m_mtxpos.Vx().x, m_mtxpos.Vx().y, m_mtxpos.Vx().z,
			m_mtxpos.Vy().x, m_mtxpos.Vy().y, m_mtxpos.Vy().z,
			m_mtxpos.Vz().x, m_mtxpos.Vz().y, m_mtxpos.Vz().z );
	}
}

void ItemEntity::SetTechnique( const char* pcTechnique )
{
	if( m_pModel ) {
		NODE* pRootNod = m_pModel->GetNode(0);
		if( pRootNod ) {
			pRootNod->SetTechnique( pcTechnique );
		}
	}
}

bool ItemEntity::TieToLocator(ENTITY_ID& mdlEID, const char* pcLocName)
{
	m_eidTieModel = mdlEID;
	m_sTieLocName = pcLocName;
	MODEL* pMdl = (MODEL*)api->GetEntityPointer( &mdlEID );
	if( pMdl ) {
		m_pMdlNode = pMdl->GetNode(0);
		if( m_pMdlNode )
			return true;
	}
	return false;
}

void ItemEntity::UnTieFromLocator()
{
	if( m_bTieToLocator ) {
		m_bTieToLocator = false;
		SetModelToPosition(m_mtxpos);
	}
}

void ItemEntity::EndEventProcess()
{
	UnTieFromLocator();
	if( !AttributesPointer ) return;
	if( AttributesPointer->GetAttributeAsDword("hide_after_using",0)!=0 ) {
		m_bVisible = false;
		AttributesPointer->SetAttributeUseDword("hide",1);
	}
}

void ItemEntity::DrawIntoLocator()
{
	MODEL* pMdl = (MODEL*)api->GetEntityPointer(&m_eidTieModel);
	if( !pMdl ) {
		UnTieFromLocator();
		return;
	}

	CMatrix perMtx;
	long sti;
	if( m_pModel )
	{
		sti = -1;
		long idLoc = m_pMdlNode->geo->FindName(m_sTieLocName);

		if((sti = m_pMdlNode->geo->FindLabelN(sti+1, idLoc))>-1)
		{
			Animation *ani = pMdl->GetAnimation();
			CMatrix *bones = &ani->GetAnimationMatrix(0);

			GEOS::LABEL lb;
			m_pMdlNode->geo->GetLabel(sti, lb);
			CMatrix mt;
			mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
			mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
			mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
			mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

			CMatrix mbn = mt * bones[lb.bones[0]];
			mbn.Pos().x *= -1.0f;
			mbn.Vx().x *= -1.0f;
			mbn.Vy().x *= -1.0f;
			mbn.Vz().x *= -1.0f;

			CMatrix scl;
			scl.Vx().x = -1.0f;
			scl.Vy().y = 1.0f;
			scl.Vz().z = 1.0f;
			mbn.EqMultiply(scl, CMatrix(mbn));

			perMtx = mbn * pMdl->mtx;
		}

		m_pModel->mtx = perMtx;
		m_pModel->Realize(0);
	}
}

ENTITY_ID ItemEntity::GetModelEIDFromCharacterEID(ENTITY_ID& chrEID)
{
	ENTITY_ID eid;
	VDATA* pvdat = (VDATA*)api->GetScriptVariable("g_TmpModelVariable");
	if( pvdat ) {
		api->Send_Message( chrEID, "le", MSG_CHARACTER_GETMODEL, pvdat );
		eid = pvdat->GetEntityID();
	}
	return eid;
}

void ItemEntity::SetEventListener(ENTITY_ID& mdlEID, ENTITY_ID& mdlToTieEID,const char* pcLocName, const char* pcStartEvent,const char* pcEndEvent )
{
	MODEL* pMdl = (MODEL*)api->GetEntityPointer(&mdlEID);
	if( !pMdl ) return;
	Animation * a = pMdl->GetAnimation();
	if( a )
	{
		m_eventListener.item = this;
		m_eventListener.m_bStartWaiting = true;
		m_eventListener.m_eidListenedModel = mdlEID;
		m_eventListener.m_eidToTieModel = mdlToTieEID;
		m_eventListener.m_sToTieLocator = pcLocName;
		m_eventListener.m_sStartEvent = pcStartEvent;
		m_eventListener.m_sEndEvent = pcEndEvent;
		a->SetEventListener( &m_eventListener );
		a->SetEvent( ae_end, 0, &m_eventListener );
	}
}

void ItemEntity::EventListener::Event(Animation * animation, long playerIndex, const char * eventName)
{
	if( m_bStartWaiting && m_sStartEvent==eventName ) {
		m_bStartWaiting = false;
		item->m_bTieToLocator = item->TieToLocator( m_eidToTieModel, m_sToTieLocator );
		return;
	}
	if( !m_bStartWaiting && m_sEndEvent==eventName ) {
		MODEL* pMdl = (MODEL*)api->GetEntityPointer(&m_eidListenedModel);
		if( pMdl ) {
			Animation * a = pMdl->GetAnimation();
			if( a ) a->SetEventListener(0);
		}
		item->EndEventProcess();
		return;
	}
}

bool ItemEntity::CreateParticle()
{
	DeleteParticle();

	if( m_bVisible ) {
		char* pcParticleName = BIUtils::GetStringFromAttr(AttributesPointer,"particle","");
		if( pcParticleName && pcParticleName[0] )
		{
			ENTITY_ID eidParticle;
			if( api->FindClass(&eidParticle, "particles", 0) )
			{
				CVECTOR vPos = m_mtxpos.Pos();
				m_pParticle = (VPARTICLE_SYSTEM*)api->Send_Message(eidParticle, "lsffffffl", PS_CREATE_RIC, pcParticleName, vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
				return true;
			}
		}
	}
	return false;
}

void ItemEntity::DeleteParticle()
{
	if( m_pParticle ) {
		ENTITY_ID eidParticle;
		if( api->FindClass(&eidParticle, "particles", 0) )
		{
			if( api->Send_Message(eidParticle, "ll", PS_VALIDATE_PARTICLE, (long)m_pParticle) )
				m_pParticle->Pause(true);
		}
		m_pParticle = 0;
	}
}
