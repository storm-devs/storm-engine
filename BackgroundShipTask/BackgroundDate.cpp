#include "BackgroundDate.h"
#include "..\common_h\dx8render.h"
#include <stdio.h>

BackgroundDataDescriber::BackgroundDataDescriber()
{
	year = 0;
	month = 1;
	day = 1;

	hour = 0;
	min = 0;
}

BackgroundDataDescriber::BackgroundDataDescriber( const BackgroundDataDescriber& dd )
{
}

BackgroundDataDescriber::~BackgroundDataDescriber()
{
}

const BackgroundDataDescriber& BackgroundDataDescriber::operator = ( const BackgroundDataDescriber& dd )
{
	return (*this);
}

bool BackgroundDataDescriber::operator == ( const BackgroundDataDescriber& dd )
{
	if( year != dd.year || month != dd.month || day != dd.day ||
		hour != dd.hour || min != dd.min ) return false;
	return true;
}

bool BackgroundDataDescriber::operator < ( const BackgroundDataDescriber& dd )
{
	if( year < dd.year ) return true;
	if( year > dd.year ) return false;

	if( month < dd.month ) return true;
	if( month > dd.month ) return false;

	if( day < dd.day ) return true;
	if( day > dd.day ) return false;

	if( hour < dd.hour ) return true;
	if( hour > dd.hour ) return false;

	if( min < dd.min ) return true;
	return false;
}

bool BackgroundDataDescriber::operator <= ( const BackgroundDataDescriber& dd )
{
	return !( (*this) > dd );
}

bool BackgroundDataDescriber::operator > ( const BackgroundDataDescriber& dd )
{
	if( year > dd.year ) return true;
	if( year < dd.year ) return false;

	if( month > dd.month ) return true;
	if( month < dd.month ) return false;

	if( day > dd.day ) return true;
	if( day < dd.day ) return false;

	if( hour > dd.hour ) return true;
	if( hour < dd.hour ) return false;

	if( min > dd.min ) return true;
	return false;
}

bool BackgroundDataDescriber::operator >= ( const BackgroundDataDescriber& dd )
{
	return !( (*this) < dd );
}

void BackgroundDataDescriber::AddMinutes( long nQuantity )
{
	min += nQuantity;
	long nAddHours = 0;
	// проверка на время больше часа
	if( min >= 60 ) {
		nAddHours = min / 60;
		min -= nAddHours * 60;
	}
	// и на откат назад
	if( min < 0 ) {
		nAddHours = (min-59) / 60;
		min -= nAddHours * 60;
	}
	// полученные на смещение часы присоединяем к имеющимся
	if( nAddHours != 0 )
		AddHour( nAddHours );
}

void BackgroundDataDescriber::AddHour( long nQuantity )
{
	hour += nQuantity;
	long nAddDays = 0;
	// проверка на время больше суток
	if( hour >= 24 ) {
		nAddDays = hour / 24;
		hour -= nAddDays * 24;
	}
	// и на откат назад
	if( hour < 0 ) {
		nAddDays = (hour-23) / 24;
		hour -= nAddDays * 24;
	}
	// полученные на смещение часы присоединяем к имеющимся
	if( nAddDays != 0 )
		AddDay( nAddDays );
}

void BackgroundDataDescriber::AddDay( long nQuantity )
{
	day += nQuantity;
	if( day > 0 )
	{ // дней вперед
		// проверка на день больше месяца
		while( true )
		{
			bool bRightDay = true;
			switch( month )
			{
			//----------------------------------------------------
			case 1: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 2: // 28/29 day (february with normal/leap year)
				if( (year%4) == 0 ) {
					// високосный год
					if( day > 29 ) { day -= 29; bRightDay = false; }
				} else {
					// не високосный год
					if( day > 28 ) { day -= 28; bRightDay = false; }
				}
			break;
			//----------------------------------------------------
			case 3: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 4: // 30 day
				if( day > 30 ) { day -= 30; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 5: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 6: // 30 day
				if( day > 30 ) { day -= 30; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 7: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 8: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 9: // 30 day
				if( day > 30 ) { day -= 30; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 10: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 11: // 30 day
				if( day > 30 ) { day -= 30; bRightDay = false; }
			break;
			//----------------------------------------------------
			case 12: // 31 day
				if( day > 31 ) { day -= 31; bRightDay = false; }
			break;
			//----------------------------------------------------
			default:
				api->Trace("Warning! BackgroundShipTask::ChangeDate() - have wrong month.");
			}
			if( bRightDay ) break;
			else AddMonth( 1 );
		}
	}
	else
	{ // дней назад
		while( day < 1 )
		{
			AddMonth( -1 ); // проматываем месяц назад
			switch( month )
			{
			case 1: day += 31; break;
			case 2: // 28/29 day (february with normal/leap year)
				if( (year%4) == 0 ) day += 29;
				else day += 28;
				break;
			case 3: day += 31; break;
			case 4: day += 30; break;
			case 5: day += 31; break;
			case 6: day += 30; break;
			case 7: day += 31; break;
			case 8: day += 31; break;
			case 9: day += 30; break;
			case 10: day += 31; break;
			case 11: day += 30; break;
			case 12: day += 31; break;
			default:
				api->Trace("Warning! BackgroundShipTask::ChangeDate() - have wrong month.");
			}
		}
	}
}

void BackgroundDataDescriber::AddMonth( long nQuantity )
{
	month += nQuantity;
	long nAddYear = 0;

	if( month > 12 ) {
		nAddYear = (month-1) / 12;
		month -= nAddYear * 12;
	}
	if( month < 1 )
	{
		nAddYear = (month-12) / 12;
		month -= nAddYear * 12;
	}
	if( nAddYear != 0 )
		AddYears( nAddYear );
}

void BackgroundDataDescriber::AddYears( long nQuantity )
{
	year += nQuantity;
}

void BackgroundDataDescriber::WriteDataToAttributeClass( ATTRIBUTES* pA )
{
	if( !pA ) return;
	pA->SetAttributeUseDword( "min", min );
	pA->SetAttributeUseDword( "hour", hour );
	pA->SetAttributeUseDword( "day", day );
	pA->SetAttributeUseDword( "month", month );
	pA->SetAttributeUseDword( "year", year );
}

void BackgroundDataDescriber::ReadDataFromAttributeClass( ATTRIBUTES* pA )
{
	if( !pA ) return;
	min = pA->GetAttributeAsDword( "min", 0 );
	hour = pA->GetAttributeAsDword( "hour", 0 );
	day = pA->GetAttributeAsDword( "day", 1 );
	month = pA->GetAttributeAsDword( "month", 1 );
	year = pA->GetAttributeAsDword( "year", 0 );
}
