#pragma once

//-------------------------------------------------------------------------------------------------
// Simple class for creating and keeping in memory strings list
// Also, each string can has its own data. Amount of memory for this data you can assign
// using SetStringDataSize(..) function. When you changing the size for this data, all items in list
// cleared. This data can be set or retrivied

#include <cstdint>

#define SL_BLOCK_SIZE 128 // initiate number of strings
#define INVALID_ORDINAL_NUMBER 0xffffffff
#define CACHE_SIZE 8

class STRINGS_LIST
{
  protected:
    uint32_t List_size;
    uint32_t Strings;
    char **String_Table_PTR;
    uint32_t used_data_size;
    uint32_t Cache[CACHE_SIZE];
    uint32_t Cache_Pos;

  public:
    STRINGS_LIST();
    ~STRINGS_LIST();

    uint32_t GetStringsCount();
    char *GetString(uint32_t code);
    uint32_t GetStringCode(const char *_char_PTR);
    bool AddString(const char *_char_PTR);
    bool AddUnicalString(const char *_char_PTR);
    void Release();
    void DeleteString(uint32_t code);

    bool GetStringData(uint32_t code, void *data_PTR);
    bool SetStringData(uint32_t code, void *data_PTR);
    void SetStringDataSize(uint32_t size);

    void CacheString(uint32_t code);
    uint32_t MakeHashValue(const char *string);
};
