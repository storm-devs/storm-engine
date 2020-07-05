//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	UserData
//--------------------------------------------------------------------------------------------
//	Хранение и доступ к пользовательским данным
//============================================================================================

#include "UserData.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

UserData::UserData()
{
	data = null;
	numData = 0;
	maxData = 0;	
	strings = null;
	numBytes = 0;
	maxBytes = 0;
}

UserData::~UserData()
{
	if(data) delete data;
	if(strings) delete strings;
}

//--------------------------------------------------------------------------------------------
//Работа с данными
//--------------------------------------------------------------------------------------------

//Добавить данные
void UserData::AddData(const char * dname, const char * sdata)
{
	if(!dname || !dname[0]) return;
	//Выделяем памяти
	if(numData >= maxData)
	{
		maxData += 16;
		data = (Data *)RESIZE(data, maxData*sizeof(Data));
	}
	//Длинны строк
	long ldname = strlen(dname) + 1;
	long ldata = strlen(sdata) + 1;
	//Заголовок данных
	data[numData].hashCode = CalcHashString(dname);
	data[numData].name = numBytes;
	data[numData].data = numBytes + ldname;
	numData++;
	//Память для строк
	if(numBytes + ldname + ldata >= maxBytes)
	{
		maxBytes += numBytes + ldname + ldata + 1024;
		strings = (char *)RESIZE(strings, maxBytes);
	}
	//Копируем строки
	memcpy(strings + numBytes, dname, ldname);
	memcpy(strings + numBytes + ldname, sdata, ldata);
	numBytes += ldname + ldata;
}

//Освободить лишнюю память после добавления данных
void UserData::FreeExtra()
{
	if(numData < maxData)
	{
		maxData = numData;
		data = (Data *)RESIZE(data, maxData*sizeof(Data));
	}
	if(numBytes < maxBytes)
	{
		maxBytes = numBytes;
		strings = (char *)RESIZE(strings, maxBytes);
	}	
}

//Найти пользовательские данные
const char * UserData::GetData(const char * dataName)
{
	if(!dataName || !dataName[0]) return null;
	//1 Вычислим хэшь значение строки
	long hash = CalcHashString(dataName);
	//2 Поищем данные с таким названием
	for(long i = 0; i < numData; i++)	
		if(data[i].hashCode == hash)
		{
			if(stricmp(dataName, strings + data[i].name) == 0)
					return strings + data[i].data;
		}	
	return null;
}


//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------

long UserData::CalcHashString(const char * str)
{
  unsigned long hval = 0;
  while(*str != '\0')
  {
	char c = *str++;
	if(c >= 'A' && c <= 'Z') c += 'a' - 'A';
    hval = (hval<<4) + (unsigned long int)c;
    unsigned long g = hval & ((unsigned long int) 0xf << (32 - 4));
    if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
  }
  return long(hval);
}

