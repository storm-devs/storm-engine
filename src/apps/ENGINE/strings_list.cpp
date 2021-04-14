#include "strings_list.h"
#include <cstring>
#include <stdexcept>

STRINGS_LIST::STRINGS_LIST()
{
    List_size = 0;
    Strings = 0;
    String_Table_PTR = nullptr;
    used_data_size = 0;
    for (uint32_t n = 0; n < CACHE_SIZE; n++)
        Cache[n] = INVALID_ORDINAL_NUMBER;
    Cache_Pos = 0;
}

STRINGS_LIST::~STRINGS_LIST()
{
    Release();
}

void STRINGS_LIST::CacheString(uint32_t code)
{
    if (Cache_Pos >= CACHE_SIZE)
        Cache_Pos = 0;
    Cache[Cache_Pos] = code;
    Cache_Pos++;
}

uint32_t STRINGS_LIST::GetStringsCount()
{
    return Strings;
}

char *STRINGS_LIST::GetString(uint32_t code)
{
    if (code >= Strings || String_Table_PTR == nullptr)
        return nullptr;
    return String_Table_PTR[code] + used_data_size + sizeof(uint32_t);
}

bool STRINGS_LIST::AddString(const char *_char_PTR)
{
    // GUARD(STRINGS_LIST::AddString)
    if (_char_PTR == nullptr)
        throw std::runtime_error("zero string");
    const auto hash = MakeHashValue(_char_PTR);
    if (String_Table_PTR == nullptr) // first time
    {
        String_Table_PTR = static_cast<char **>(malloc(sizeof(char *) * SL_BLOCK_SIZE));
        List_size = SL_BLOCK_SIZE;
    }
    if (Strings >= List_size) // enlarge list size
    {
        String_Table_PTR =
            static_cast<char **>(realloc(String_Table_PTR, (List_size + SL_BLOCK_SIZE) * sizeof(char *)));
        List_size = List_size + SL_BLOCK_SIZE;
    }

    const auto len = strlen(_char_PTR) + 1;
    String_Table_PTR[Strings] = static_cast<char *>(new char[len + used_data_size + sizeof(uint32_t)]);
    if (String_Table_PTR[Strings] == nullptr)
        return false;
    memset(String_Table_PTR[Strings], 0, used_data_size + sizeof(uint32_t));
    *((uint32_t *)(String_Table_PTR[Strings])) = hash;
    memcpy(String_Table_PTR[Strings] + used_data_size + sizeof(uint32_t), _char_PTR, len);
    Strings++;
    // UNGUARD
    return true;
}

void STRINGS_LIST::Release()
{
    // GUARD(STRINGS_LIST::Release)
    uint32_t n;
    if (Strings <= 0 || String_Table_PTR == nullptr)
        return;
    for (n = 0; n < Strings; n++)
        delete String_Table_PTR[n];
    free(String_Table_PTR);
    List_size = 0;
    Strings = 0;
    String_Table_PTR = nullptr;
    for (n = 0; n < CACHE_SIZE; n++)
        Cache[n] = INVALID_ORDINAL_NUMBER;
    Cache_Pos = 0;
    // UNGUARD
}

uint32_t STRINGS_LIST::GetStringCode(const char *_char_PTR)
{
    if (Strings <= 0 || String_Table_PTR == nullptr || _char_PTR == nullptr)
    {
        return INVALID_ORDINAL_NUMBER;
    }
    const auto hash = MakeHashValue(_char_PTR);

    /*for(n=0;n<CACHE_SIZE;n++)
    {
      if(Cache[n] == INVALID_ORDINAL_NUMBER) break;
      if(Cache[n] >= Strings) throw std::runtime_error(cache error);
      if(hash == *((DWORD *)String_Table_PTR[Cache[n]]))
      {
        //return Cache[n];
        if(_stricmp(String_Table_PTR[Cache[n]] + used_data_size + sizeof(DWORD),_char_PTR) == 0) return Cache[n];
      }
    }*/

    for (uint32_t n = 0; n < Strings; n++)
    {
        if (hash == *((uint32_t *)String_Table_PTR[n]))
        {
            if (_stricmp(String_Table_PTR[n] + used_data_size + sizeof(uint32_t), _char_PTR) == 0)
            {
                // CacheString(n);
                return n;
            }
        }
    }
    // UNGUARD
    return INVALID_ORDINAL_NUMBER;

    /*

      for(n=0;n<CACHE_SIZE;n++)
      {
        if(Cache[n] == INVALID_ORDINAL_NUMBER) break;
        if(Cache[n] >= Strings) throw std::runtime_error(cache error);
        if(_stricmp(String_Table_PTR[Cache[n]] + used_data_size + sizeof(DWORD),_char_PTR) == 0) return Cache[n];
      }

      for(n = 0; n < Strings; n++)
      {
        if(_stricmp(String_Table_PTR[n] + used_data_size + sizeof(DWORD),_char_PTR) == 0)
        {
          CacheString(n);
          return n;
        }
      }
      //UNGUARD
      return INVALID_ORDINAL_NUMBER;*/
}

bool STRINGS_LIST::AddUnicalString(const char *_char_PTR)
{
    if (GetStringCode(_char_PTR) != INVALID_ORDINAL_NUMBER)
        return false;
    return AddString(_char_PTR);
}

bool STRINGS_LIST::GetStringData(uint32_t code, void *data_PTR)
{
    if (code >= Strings || String_Table_PTR == nullptr || data_PTR == nullptr)
        return false;
    memcpy(data_PTR, String_Table_PTR[code] + sizeof(uint32_t), used_data_size);
    return true;
}

bool STRINGS_LIST::SetStringData(uint32_t code, void *data_PTR)
{
    // GUARD(STRINGS_LIST::SetStringData)
    if (code >= Strings || String_Table_PTR == nullptr || data_PTR == nullptr)
        return false;
    memcpy(String_Table_PTR[code] + sizeof(uint32_t), data_PTR, used_data_size);
    // UNGUARD
    return true;
}

void STRINGS_LIST::SetStringDataSize(uint32_t _size)
{
    if (used_data_size == _size)
        return;
    Release();
    used_data_size = _size;
}

void STRINGS_LIST::DeleteString(uint32_t code)
{
    uint32_t n;
    if (code >= Strings || String_Table_PTR == nullptr)
        return;
    delete String_Table_PTR[code];
    if (code == (Strings - 1))
    {
        Strings--;
        for (n = 0; n < CACHE_SIZE; n++)
            Cache[n] = INVALID_ORDINAL_NUMBER;
        Cache_Pos = 0;
        return;
    }
    memcpy(&String_Table_PTR[code], &String_Table_PTR[code + 1], (Strings - code - 1) * sizeof(char *));
    Strings--;
    for (n = 0; n < CACHE_SIZE; n++)
        Cache[n] = INVALID_ORDINAL_NUMBER;
    Cache_Pos = 0;
}

uint32_t STRINGS_LIST::MakeHashValue(const char *string)
{
    uint32_t hval = 0;

    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A';

        hval = (hval << 4) + static_cast<unsigned long>(v);
        const uint32_t g = hval & (static_cast<unsigned long>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}
