#include "strings_list.h"
#include "..\..\common_h\exs.h"
#include "..\..\common_h\memop.h"

STRINGS_LIST::STRINGS_LIST()
{
	List_size = 0;
	Strings = 0;
	String_Table_PTR = null;
	used_data_size = 0;
	dword n;
	for(n=0;n<CACHE_SIZE;n++) Cache[n] = INVALID_ORDINAL_NUMBER;
	Cache_Pos = 0;
}

STRINGS_LIST::~STRINGS_LIST()
{
	Release();
}

void STRINGS_LIST::CacheString(dword code)
{
	if(Cache_Pos >= CACHE_SIZE) Cache_Pos = 0;
	Cache[Cache_Pos] = code;	
	Cache_Pos++;
}

dword STRINGS_LIST::GetStringsCount() { return Strings; }

char * STRINGS_LIST::GetString(dword code)
{
	if(code >= Strings || String_Table_PTR == null) return null;
	return String_Table_PTR[code] + used_data_size + sizeof(DWORD);
}


bool STRINGS_LIST::AddString(char * _char_PTR)
{
	GUARD(STRINGS_LIST::AddString)
	if(_char_PTR == null) _THROW(zero string);
	DWORD hash;
	hash = MakeHashValue(_char_PTR);	
	if(String_Table_PTR == null)	// first time
	{

		String_Table_PTR = (char * *)NEW char[sizeof(char*)*SL_BLOCK_SIZE];
		List_size = SL_BLOCK_SIZE;
	}
	if(Strings >= List_size)		// enlarge list size
	{
		String_Table_PTR = (char **)RESIZE(String_Table_PTR,(List_size + SL_BLOCK_SIZE)*sizeof(char*));
		List_size = List_size + SL_BLOCK_SIZE;
	}
	

	String_Table_PTR[Strings] = (char *)NEW char[strlen(_char_PTR) + 1 + used_data_size + sizeof(DWORD)];
	if(String_Table_PTR[Strings] == null) return false;
	memset(String_Table_PTR[Strings],0,used_data_size + sizeof(DWORD));
	*((DWORD *)(String_Table_PTR[Strings])) = hash;
	strcpy(String_Table_PTR[Strings] + used_data_size + sizeof(DWORD),_char_PTR);
	Strings++;
	UNGUARD
	return true;
}

void STRINGS_LIST::Release()
{
	GUARD(STRINGS_LIST::Release)
	dword n;
	if(Strings <= 0 || String_Table_PTR == null) return;
	for(n = 0; n < Strings; n++) delete String_Table_PTR[n];
	delete String_Table_PTR;
	List_size = 0;
	Strings = 0;
	String_Table_PTR = null;		
	for(n=0;n<CACHE_SIZE;n++) Cache[n] = INVALID_ORDINAL_NUMBER;
	Cache_Pos = 0;
	UNGUARD
}

dword STRINGS_LIST::GetStringCode(char * _char_PTR)
{
	GUARD(STRINGS_LIST::GetStringCode)
	dword n;
	DWORD hash;

	if(Strings <= 0 || String_Table_PTR == null  || _char_PTR == null)
	{
		return INVALID_ORDINAL_NUMBER;
	}
	hash = MakeHashValue(_char_PTR);

	/*for(n=0;n<CACHE_SIZE;n++) 
	{
		if(Cache[n] == INVALID_ORDINAL_NUMBER) break;
		if(Cache[n] >= Strings) _THROW(cache error);
		if(hash == *((DWORD *)String_Table_PTR[Cache[n]]))
		{
			//return Cache[n];
			if(stricmp(String_Table_PTR[Cache[n]] + used_data_size + sizeof(DWORD),_char_PTR) == 0) return Cache[n];
		}
	}*/
	
	for(n = 0; n < Strings; n++) 
	{
		if(hash == *((DWORD *)String_Table_PTR[n]))
		{
			if(stricmp(String_Table_PTR[n] + used_data_size + sizeof(DWORD),_char_PTR) == 0) 
			{
				//CacheString(n);
				return n;
			}
		}
	}
	UNGUARD
	return INVALID_ORDINAL_NUMBER;

/*	
	
	for(n=0;n<CACHE_SIZE;n++) 
	{
		if(Cache[n] == INVALID_ORDINAL_NUMBER) break;
		if(Cache[n] >= Strings) _THROW(cache error);
		if(stricmp(String_Table_PTR[Cache[n]] + used_data_size + sizeof(DWORD),_char_PTR) == 0) return Cache[n];
	}
	
	for(n = 0; n < Strings; n++) 
	{
		if(stricmp(String_Table_PTR[n] + used_data_size + sizeof(DWORD),_char_PTR) == 0) 
		{
			CacheString(n);
			return n;
		}
	}
	UNGUARD
	return INVALID_ORDINAL_NUMBER;*/
}

bool STRINGS_LIST::AddUnicalString(char * _char_PTR)
{
	if(GetStringCode(_char_PTR) != INVALID_ORDINAL_NUMBER) return false;
	return AddString(_char_PTR);
}

bool STRINGS_LIST::GetStringData(dword code,void * data_PTR)
{
	if(code >= Strings || String_Table_PTR == null || data_PTR == null) return false;
	memcpy(data_PTR,String_Table_PTR[code] + sizeof(DWORD),used_data_size);
	return true;
}

bool STRINGS_LIST::SetStringData(dword code,void * data_PTR)
{
	GUARD(STRINGS_LIST::SetStringData)
	if(code >= Strings || String_Table_PTR == null || data_PTR == null) return false;
	memcpy(String_Table_PTR[code] + sizeof(DWORD),data_PTR,used_data_size);
	UNGUARD
	return true;
}

void STRINGS_LIST::SetStringDataSize(dword _size)
{
	if(used_data_size == _size) return;
	Release();
	used_data_size = _size;
}

void STRINGS_LIST::DeleteString(dword code)
{
	dword n;
	if(code >= Strings || String_Table_PTR == null) return;
	delete String_Table_PTR[code];
	if(code == (Strings - 1))
	{
		Strings--;
		for(n=0;n<CACHE_SIZE;n++) Cache[n] = INVALID_ORDINAL_NUMBER;
		Cache_Pos = 0;
		return;
	}
	memcpy(&String_Table_PTR[code],&String_Table_PTR[code + 1],(Strings - code - 1)*sizeof(char *));
	Strings--;
	for(n=0;n<CACHE_SIZE;n++) Cache[n] = INVALID_ORDINAL_NUMBER;
	Cache_Pos = 0;
}

dword STRINGS_LIST::MakeHashValue(const char * string)
{
  dword hval = 0;
  dword g;
  char v;

  while(*string != 0)
  {
	v = *string++;
	if ('A' <= v && v <= 'Z') v += 'a' - 'A';

    hval = (hval<<4) + (unsigned long int)v;
    g = hval & ((unsigned long int) 0xf << (32 - 4));
    if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
  }
  return hval;
}
