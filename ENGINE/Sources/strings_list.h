#ifndef _STRINGS_LIST_H_
#define _STRINGS_LIST_H_

//-------------------------------------------------------------------------------------------------
// Simple class for creating and keeping in memory strings list
// Also, each string can has its own data. Amount of memory for this data you can assign 
// using SetStringDataSize(..) function. When you changing the size for this data, all items in list 
// cleared. This data can be set or retrivied

#include "common_h.h"

#define SL_BLOCK_SIZE			128	// initiate number of strings
#define INVALID_ORDINAL_NUMBER	0xffffffff
#define CACHE_SIZE				8

class STRINGS_LIST
{
protected:
	
	dword List_size;
	dword Strings;
	char * * String_Table_PTR;		
	dword used_data_size;
	dword Cache[CACHE_SIZE];
	dword Cache_Pos;
public:

	STRINGS_LIST();
	~STRINGS_LIST();

	dword  GetStringsCount();
	char * GetString(dword code);
	dword  GetStringCode(char * _char_PTR);
	bool   AddString(char * _char_PTR);
	bool   AddUnicalString(char * _char_PTR);
	void   Release();
	void   DeleteString(dword code);

	bool GetStringData(dword code,void * data_PTR);
	bool SetStringData(dword code,void * data_PTR);
	void SetStringDataSize(dword _size);

	void CacheString(dword code);
	dword MakeHashValue(const char * string);
};

#endif