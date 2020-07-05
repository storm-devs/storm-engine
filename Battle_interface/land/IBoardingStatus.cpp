#include <stdio.h>
#include "..\..\common_h\matrix.h"
#include "..\..\engine\program\battle_interface\log_msg.h"
#include "IBoardingStatus.h"

IBoardingStatus::IBoardingStatus()
{
	rs = NULL;
}

IBoardingStatus::~IBoardingStatus()
{
}

bool IBoardingStatus::Init()
{
	if( (rs=(VDX8RENDER *)_CORE_API->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}

	_CORE_API->SystemMessages(GetID(),true);
	_CORE_API->LayerAdd("realize",GetID(),0xFFFFFFFF);

	return true;
}

dword _cdecl IBoardingStatus::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
	case LOG_INIT_CHARCTER_HP:
		Create();
		break;
	case LOG_SET_CHARCTER_HP:
		{
			float myHP = message.Float();
			float enemyHP = message.Float();
			SetCharactersHP(myHP,enemyHP);
		}
		break;
	}
	return 0;
}

void IBoardingStatus::Realize(dword delta_time)
{
	if(rs==NULL) return;

	rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,BI_COLORONLY_VERTEX_FORMAT,2,m_MyChar,sizeof(BI_COLORONLY_VERTEX),"battle_only_color");
	rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,BI_COLORONLY_VERTEX_FORMAT,2,m_EnemyChar,sizeof(BI_COLORONLY_VERTEX),"battle_only_color");
}

void IBoardingStatus::Create()
{
	// Установить параметры для иконки активного действия
	ATTRIBUTES * pA = api->Entity_GetAttributePointer(&GetID());
	if(pA!=NULL)
	{
		m_myPos.x = (float)pA->GetAttributeAsDword("myLeft",10);
		m_myPos.y = (float)pA->GetAttributeAsDword("myTop",460);
		m_enemyPos.x = (float)pA->GetAttributeAsDword("enemyLeft",10);
		m_enemyPos.y = (float)pA->GetAttributeAsDword("enemyTop",450);
		m_Width = pA->GetAttributeAsDword("width",120);
		m_Height = pA->GetAttributeAsDword("height",8);
		m_myColor = pA->GetAttributeAsDword("myColor",ARGB(255,0,0,128));
		m_enemyColor = pA->GetAttributeAsDword("enemyColor",ARGB(255,128,0,0));
	}
	else
	{
		m_myPos.x = 10.f;
		m_myPos.y = 460.f;
		m_enemyPos.x = 10.f;
		m_enemyPos.y = 450.f;
		m_Width = 120;
		m_Height = 8;
		m_myColor = ARGB(255,0,0,128);
		m_enemyColor = ARGB(255,128,0,0);
	}

	//
	m_MyChar[0].col = m_MyChar[1].col = m_MyChar[2].col = m_MyChar[3].col = m_myColor;
	m_MyChar[0].w = m_MyChar[1].w = m_MyChar[2].w = m_MyChar[3].w = .5f;
	m_MyChar[0].pos.z = m_MyChar[1].pos.z = m_MyChar[2].pos.z = m_MyChar[3].pos.z = 1.f;
	m_MyChar[2].pos.x=m_MyChar[3].pos.x = m_Width+(m_MyChar[0].pos.x=m_MyChar[1].pos.x=m_myPos.x);
	m_MyChar[1].pos.y=m_MyChar[3].pos.y = m_Height+(m_MyChar[0].pos.y=m_MyChar[2].pos.y=m_myPos.y);

	m_EnemyChar[0].col = m_EnemyChar[1].col = m_EnemyChar[2].col = m_EnemyChar[3].col = m_enemyColor;
	m_EnemyChar[0].w = m_EnemyChar[1].w = m_EnemyChar[2].w = m_EnemyChar[3].w = .5f;
	m_EnemyChar[0].pos.z = m_EnemyChar[1].pos.z = m_EnemyChar[2].pos.z = m_EnemyChar[3].pos.z = 1.f;
	m_EnemyChar[2].pos.x=m_EnemyChar[3].pos.x = m_Width+(m_EnemyChar[0].pos.x=m_EnemyChar[1].pos.x=m_enemyPos.x);
	m_EnemyChar[1].pos.y=m_EnemyChar[3].pos.y = m_Height+(m_EnemyChar[0].pos.y=m_EnemyChar[2].pos.y=m_enemyPos.y);
}

void IBoardingStatus::SetCharactersHP(float myHP,float enemyHP)
{
	m_MyChar[2].pos.x = m_MyChar[3].pos.x = m_myPos.x + myHP*m_Width;
	m_EnemyChar[2].pos.x = m_EnemyChar[3].pos.x = m_enemyPos.x + enemyHP*m_Width;
}