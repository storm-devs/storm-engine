#include "BackgroundShipTask.h"
#include <stdio.h>

INTERFACE_FUNCTION
CREATE_CLASS(BackgroundShipTask)

BackgroundShipTask::BackgroundShipTask()
{
	m_nCharacterIndex = -1;
	m_bTaskPresent = false;
	m_pDateAttribute = null;
	m_bGoToNextData = true;
}

BackgroundShipTask::~BackgroundShipTask()
{
}

bool BackgroundShipTask::Init()
{
	return true;
}

void BackgroundShipTask::Execute(dword delta_time)
{
	Update();
}

dword _cdecl BackgroundShipTask::ProcessMessage(MESSAGE & message)
{
	long nCode = message.Long();
	switch( nCode )
	{
	case 0: // инициализация "llla": msg_code, char_index, task_day, Environment.date
		{
			m_nCharacterIndex = message.Long();
			long nTaskDay = message.Long();
			m_pDateAttribute = message.AttributePointer();
			m_bTaskPresent = true;
			m_bGoToNextData = true;
			SetStartTiming( nTaskDay );
		}
	break;

	case 1: // Перечитывание параметров "la": msg_code, Environment.date
		{
			ReadParametersFromAttributes();
			m_pDateAttribute = message.AttributePointer();
			m_bGoToNextData = true;
		}
	break;

	case 2: // Досрочное завершения задания "l": msg_code
		{
			m_bGoToNextData = false;
		}
	break;
	}
	return 0;
}

dword BackgroundShipTask::AttributeChanged(ATTRIBUTES * pAttr)
{
	return 0;
}

void BackgroundShipTask::Update()
{
	// нет задачи - нечего выполнять
	if( !m_bTaskPresent ) return;

	// получаем текущую дату и время
	UpdateCurrentDate();

	// время на выполнение задания вышло
	if( m_CurrentDate >= m_EndDate )
	{
		if( CheckBattleResult() ) // последняя проверка ()
		{ // успех
			api->Event( "evCompanionTaskSuccess", "l", m_nCharacterIndex );
		}
		else
		{ // проигрыш
			api->Event( "evCompanionTaskFault", "l", m_nCharacterIndex );
		}
		ResetTask();
		return;
	}

	// пришло время очередной проверки на бой
	if( m_CurrentDate >= m_RecalculateDate )
	{
		if( CheckBattleResult() ) // последняя проверка ()
		{ // успех
			api->Event( "evCompanionTaskSuccess", "l", m_nCharacterIndex );
			if( m_bGoToNextData )
				SetNextCheckingTime();
			else 
				ResetTask();
		}
		else
		{ // проигрыш
			api->Event( "evCompanionTaskFault", "l", m_nCharacterIndex );
			ResetTask();
		}
	}
}

void BackgroundShipTask::UpdateCurrentDate()
{
	if( m_pDateAttribute )
	{
		m_CurrentDate.min = m_pDateAttribute->GetAttributeAsDword( "min", 0 );
		m_CurrentDate.hour = m_pDateAttribute->GetAttributeAsDword( "hour", 0 );
		m_CurrentDate.day = m_pDateAttribute->GetAttributeAsDword( "day", 1 );
		m_CurrentDate.month = m_pDateAttribute->GetAttributeAsDword( "month", 1 );
		m_CurrentDate.year = m_pDateAttribute->GetAttributeAsDword( "year", 0 );
	}
}

bool BackgroundShipTask::CheckBattleResult()
{
	VDATA* pVDat = api->Event( "evCheckBattleResult", "l", m_nCharacterIndex );
	long n = 0;
	if( pVDat ) n = pVDat->GetLong();
	return (n!=0); // успех битвы
}

void BackgroundShipTask::ResetTask()
{
	m_bTaskPresent = false;
	m_nCharacterIndex = -1;
}

void BackgroundShipTask::SetStartTiming( long nDayQuantity )
{
	UpdateCurrentDate();
	m_StartDate = m_CurrentDate;
	m_EndDate = m_CurrentDate;
	m_EndDate.AddDay( nDayQuantity );
	m_RecalculateDate = m_CurrentDate;
	SetNextCheckingTime();
}

void BackgroundShipTask::SetNextCheckingTime()
{
	// следующая проверка будет через 10 дней
	m_RecalculateDate.AddDay( 10 );
}

void BackgroundShipTask::WriteDataToAttribute()
{
	ATTRIBUTES* pARoot = AttributesPointer;
	if( !pARoot ) return;
	ATTRIBUTES* pATmp = pARoot->CreateSubAClass( pARoot, "startdate" );
	if( pATmp ) m_StartDate.WriteDataToAttributeClass( pATmp );
	pATmp = pARoot->CreateSubAClass( pARoot, "enddate" );
	if( pATmp ) m_EndDate.WriteDataToAttributeClass( pATmp );
}

void BackgroundShipTask::ReadParametersFromAttributes()
{
	ATTRIBUTES* pARoot = AttributesPointer;
	if( !pARoot ) return;
	m_bTaskPresent = true;

	ATTRIBUTES* pATmp = pARoot->GetAttributeClass( "startdate" );
	if( pATmp ) m_StartDate.ReadDataFromAttributeClass( pATmp );
	pATmp = pARoot->GetAttributeClass( "enddate" );
	if( pATmp ) m_EndDate.ReadDataFromAttributeClass( pATmp );

	m_nCharacterIndex = pARoot->GetAttributeAsDword( "character", -1 );
}
