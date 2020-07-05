//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocatorArray
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "LocatorArray.h"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

LocatorArray::LocatorArray(const char * groupName)
{
	locator = null;
	numLocators = 0;
	locatorNames = null;
	bytesInLNArray = 0;
	if(groupName)
	{
		long l = strlen(groupName) + 1;	
		group = NEW char[l];
		memcpy(group, groupName, l);
	}else{
		group = NEW char[1];
		group[0] = 0;
	}
	hash = CalcHashString(group);
	isVisible = false;
	radius = 0.0f;
	kViewRadius = -1.0f;
}

LocatorArray::~LocatorArray()
{
	if(group) delete group;
	if(locator) delete locator;
	if(locatorNames) delete locatorNames;
}

//============================================================================================
//Работа с массивом
//============================================================================================

//Добавить локатор
void LocatorArray::AddLocator(CMatrix & mtx, const char * name)
{
	locator = (LocatorInfro *)RESIZE(locator, (numLocators + 1)*sizeof(LocatorInfro));
	locator[numLocators].mtx = mtx;
	locator[numLocators].radius = -1.0f;
	long slen = 0;
	if(name && name[0])
	{
		slen = strlen(name) + 1;
		locatorNames = (char *)RESIZE(locatorNames, bytesInLNArray + slen);
		locator[numLocators].name = bytesInLNArray;
		memcpy(locatorNames + bytesInLNArray, name, slen);
		locator[numLocators].hash = CalcHashString(locatorNames + bytesInLNArray);
		bytesInLNArray += slen;
	}else locator[numLocators].name = -1;
	numLocators++;
}

//Изменить матрицу локатора
void LocatorArray::SetNewMatrix(long locIndex, CMatrix & mtx)
{
	if(locIndex < 0 || locIndex >= numLocators) return;
	locator[locIndex].mtx = mtx;
}

//Найти ближайший локатор
float LocatorArray::FindNearesLocator(float x, float y, float z, long * locIndex)
{
	if(locIndex) *locIndex = -1;
	float dist = 1000000000.0f;
	CVECTOR v(x, y, z);
	for(long i = 0; i < numLocators; i++)
	{
		float d = ~(locator[i].mtx.Pos() - v);
		if(dist > d)
		{
			if(locIndex) *locIndex = i;
			dist = d;
		}
	}
	return dist;
}

//Найти ближайший локатор по цилиндру
long LocatorArray::FindNearesLocatorCl(float x, float y, float z, float height2, float & dist)
{
	long locIndex = -1;
	for(long i = 0; i < numLocators; i++)
	{
		//if(fabsf(y - locator[i].mtx.Pos().y) > height2) continue;
		float r = GetLocatorRadius(i);
		
		if(fabsf(y - locator[i].mtx.Pos().y) > r) continue;
		
		if(r <= 0.0f) continue;
		float vx = locator[i].mtx.Pos().x - x;
		float vz = locator[i].mtx.Pos().z - z;
		float d = vx*vx + vz*vz;
		if(r*r <= d) continue;
		if(locIndex >= 0)
		{	
			if(d < dist)
			{
				locIndex = i;
				dist = d;
			}
		}else{
			locIndex = i;
			dist = d;
		}
	}
	return locIndex;
}

//Найти локатор по имени
long LocatorArray::FindByName(const char * locName)
{
	if(!locName) return -1;
	long hash = CalcHashString(locName);
	for(long i = 0; i < numLocators; i++)
	{
		if(locator[i].name >= 0)
		{
			if(locator[i].hash == hash)
				if(stricmp(locatorNames + locator[i].name, locName) == 0)
				{
					return i;
				}
		}
	}
	return -1;
}

long LocatorArray::CalcHashString(const char * str)
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

//Сравнить имена групп
bool LocatorArray::CompareGroup(const char * groupName, long ghash)
{
	if(hash != ghash) return false;
	if(!groupName) return group[0] == 0;
	return stricmp(group, groupName) == 0;
}



