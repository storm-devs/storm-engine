

#include "LocationScriptLib.h"
#include "..\common_h\dx8render.h"
#include "Fader.h"

//============================================================================================

struct LocationFindCacheElement
{
	LocationFindCacheElement()
	{
		name = null;
		size = 0;
		max = 0;
		index = -1;
		use = 0;
	};

	~LocationFindCacheElement()
	{
		if(name) delete name;
	};

	inline long Cmp(const LocationFindCacheElement & v)
	{
		if(v.size != size) return false;
		if(stricmp(v.name, name) == 0) return true;
		return false;
	};

	inline void Set(const char * str)
	{
		size = strlen(str) + 1;
		if(size > max)
		{
			max = (size + 15) & ~15;
			if(name) delete name;
			name = new char[max];
		}
		strcpy(name, str);
	};

	char * name;
	long size;
	long max;	
	long index;
	long use;
};

LocationFindCacheElement charactersFindCache[16];
LocationFindCacheElement locationsFindCache[8];
LocationFindCacheElement charactersFindBuf;
dword locationFindCodeID = -1;


inline bool CheckID(VDATA * vd, const char * id, bool & res)
{
	res = false;
	if(!vd || !id || !id[0]) return false;
	ATTRIBUTES * a = vd->GetAClass();
	if(!a) return false;
	a = a->GetAttributeClassByCode(locationFindCodeID);
	if(!a) return true;
	char * attr = a->GetThisAttr();
	if(!attr) return true;
	res = stricmp(attr, id) == 0;
	return true;
}

void slAddToCache(LocationFindCacheElement * element, long size, const char * name, long index)
{
	Assert(name);
	Assert(name[0]);
	//Ищем ячейку для записи
	for(long i = 0, j = 0, min = element[i].use; i < size; i++)
	{
		if(element[i].index < 0){ j = i; break; }
		if(element[i].use < min){ j = i; min = element[i].use; }
	}
	element[j].index = index;
	element[j].use = 16;
	element[j].Set(name);
}


dword slNativeFastFind(VS_STACK * pS, LocationFindCacheElement * cache, long cacheSize)
{
	//Получить строки
	VDATA * pStr = (VDATA*)pS->Pop();
	char * nm = null;
	if(!pStr->Get(nm)) return IFUNCRESULT_FAILED;
	if(nm) charactersFindBuf.Set(nm); else charactersFindBuf.Set("");
	//Массив персонажей
	VDATA * pArray = (VDATA*)pS->Pop();
	if (!pArray) return IFUNCRESULT_FAILED;
	pArray = (VDATA*)pArray->GetReference();
	if(!pArray) return IFUNCRESULT_FAILED;
	//Возвращаемое значение
	VDATA * pReturn = (VDATA*)pS->Push();
	if (!pReturn) return IFUNCRESULT_FAILED;
	if(!charactersFindBuf.name[0])
	{
		pReturn->Set(-1L);
		return IFUNCRESULT_OK;
	}
	//Снижаем значения использования в кеше
	for(long i = 0; i < cacheSize; i++)
	{
		cache[i].use--;
		if(cache[i].use < 0) cache[i].use = 0;
	}
	//Смотрим в кеше
	bool res;
	for(i = 0; i < cacheSize; i++)
	{
		if(cache[i].index < 0) continue;
		if(!cache[i].Cmp(charactersFindBuf)) continue;
		//Проверяем на правильность кешь-значения
		if(dword(cache[i].index) >= pArray->GetElementsNum())
		{
			cache[i].index = -1;
			continue;
		}
		VDATA * vd = (VDATA *)pArray->GetArrayElement(cache[i].index);
		if(!CheckID(vd, charactersFindBuf.name, res))
		{
			cache[i].index = -1;
			continue;
		}
		if(res)
		{
			//Нашли в кеше, возвращаем
			cache[i].use++;
			pReturn->Set(cache[i].index);
			return IFUNCRESULT_OK;
		}
	}
	//Придётся искать по массиву
	long num = pArray->GetElementsNum();
	for(i = 0; i < num; i++)
	{
		VDATA * vd = (VDATA *)pArray->GetArrayElement(i);
		if(CheckID(vd, charactersFindBuf.name, res))
		{
			if(res)
			{
				slAddToCache(cache, cacheSize, charactersFindBuf.name, i);
				pReturn->Set(i);
				return IFUNCRESULT_OK;
			}
		}
	}
	//
	pReturn->Set(-1L);
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeFindCharacter(VS_STACK * pS)
{
	return slNativeFastFind(pS, charactersFindCache, sizeof(charactersFindCache)/sizeof(LocationFindCacheElement));
}

dword __cdecl slNativeFindLocation(VS_STACK * pS)
{
	return slNativeFastFind(pS, locationsFindCache, sizeof(locationsFindCache)/sizeof(LocationFindCacheElement));
}

dword __cdecl slNativeFindLaodLocation(VS_STACK * pS)
{
	//Возвращаемое значение
	VDATA * pReturn = (VDATA*)pS->Push();
	if (!pReturn) return IFUNCRESULT_FAILED;
	//Ищим локацию
	ENTITY_ID loc;
	if(!api->FindClass(&loc, "Location", 0))
	{
		pReturn->Set(-1L);
		return IFUNCRESULT_OK;
	}
	ENTITY * l = api->GetEntityPointer(&loc);
	if(!l || !l->AttributesPointer)
	{
		pReturn->Set(-1L);
		return IFUNCRESULT_OK;
	}
	long index = l->AttributesPointer->GetAttributeAsDword("index", -1L);
	pReturn->Set(index);
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeSetReloadBackImage(VS_STACK * pS)
{
	//Получить строки
	VDATA * pStr = (VDATA*)pS->Pop();
	char * nm = null;
	if(!pStr->Get(nm)) return IFUNCRESULT_FAILED;
	//Устанавливаем картинку
	VDX8RENDER * rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(rs)
	{
		rs->SetProgressImage(nm);
	}
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeReloadProgressStart(VS_STACK * pS)
{
	VDX8RENDER * rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(rs) rs->StartProgressView();
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeReloadProgressUpdate(VS_STACK * pS)
{
	VDX8RENDER * rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(rs) rs->ProgressView();
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeReloadProgressEnd(VS_STACK * pS)
{
	VDX8RENDER * rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(rs) rs->EndProgressView();
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeSleep(VS_STACK * pS)
{
	//Получить строки
	VDATA * pInt = (VDATA*)pS->Pop();
	long delay = 1;
	if(!pInt || !pInt->Get(delay)) return IFUNCRESULT_FAILED;
	if(delay < 1) delay = 1;
	Sleep(delay);
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeExecuteTechnique(VS_STACK * pS)
{
	//Получить строку
	VDATA * pStr = (VDATA*)pS->Pop();
	char * nm = null;
	if(!pStr->Get(nm)) return IFUNCRESULT_FAILED;
	//Исполить технику
	if(nm && nm[0])
	{
		VDX8RENDER * rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
		rs->TechniqueExecuteStart(nm);
		while(rs->TechniqueExecuteNext());
	}
	return IFUNCRESULT_OK;
}


dword __cdecl slGetNextLineString(VS_STACK * pS)
{
	//Возвращаемое значение
	VDATA * pReturn = (VDATA*)pS->Push();
	if(!pReturn) return IFUNCRESULT_FAILED;
	pReturn->Set("\r\n");
	return IFUNCRESULT_OK;
}

dword __cdecl slNativeSetReloadNextTipsImage(VS_STACK * pS)
{
	if(Fader::numberOfTips <= 0)
	{
		Fader::currentTips = -1;
	}else{
		Fader::currentTips = rand() % Fader::numberOfTips;
	}
	return IFUNCRESULT_OK;
}



//============================================================================================


bool ScriptLocationLibrary::Init()
{
	locationFindCodeID = api->AttributeName2Code("id");

	IFUNCINFO sIFuncInfo;
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncName = "NativeFindCharacter";
	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncAddress = slNativeFindCharacter;
	api->SetScriptFunction(&sIFuncInfo);
	
	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncName = "NativeFindLocation";
	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncAddress = slNativeFindLocation;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "NativeFindLoadCharacter";
	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncAddress = slNativeFindLaodLocation;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncName = "SetReloadProgressBackImage";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeSetReloadBackImage;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "SetReloadNextTipsImage";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeSetReloadNextTipsImage;
	api->SetScriptFunction(&sIFuncInfo);	

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "ReloadProgressStart";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeReloadProgressStart;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "ReloadProgressUpdate";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeReloadProgressUpdate;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "ReloadProgressEnd";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeReloadProgressEnd;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncName = "SystemDelay";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeSleep;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncName = "ExecuteTechnique";
	sIFuncInfo.pReturnValueName = "void";
	sIFuncInfo.pFuncAddress = slNativeExecuteTechnique;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 0;
	sIFuncInfo.pFuncName = "GetNextLineString";
	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncAddress = slGetNextLineString;
	api->SetScriptFunction(&sIFuncInfo);

	return true;
}
