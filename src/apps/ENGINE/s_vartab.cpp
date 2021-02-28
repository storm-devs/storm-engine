#include "s_vartab.h"
//#include <string.h>

#define VTMAKEHASHINDEX(x) (x & 0xff)

S_VARTAB::S_VARTAB()
{
    Buffer_size = 0;
    Var_num = 0;
    //    bKeepName = false;
    Global_var_num = 0;
    for (uint32_t n = 0; n < VTHASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
    }
}

S_VARTAB::~S_VARTAB()
{
    Release();
}

void S_VARTAB::Release()
{
    uint32_t n;
    for (n = 0; n < Var_num; n++)
    {
        delete pTable[n].pDClass;
        delete pTable[n].name;
    }
    pTable.clear();
    Buffer_size = 0;
    Var_num = 0;
    for (n = 0; n < VTHASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
        HashLine[n].pElements.clear();
    }
}

bool S_VARTAB::GetVar(VARINFO &vi, uint32_t var_code)
{
    if (var_code >= Var_num)
        return false;
    if (pTable[var_code].segment_id == INVALID_SEGMENT_ID)
        return false;
    vi = pTable[var_code];
    return true;
}

bool S_VARTAB::GetVarX(VARINFO &vi, uint32_t var_code)
{
    if (var_code >= Var_num)
        return false;
    vi = pTable[var_code];
    return true;
}

uint32_t S_VARTAB::AddVar(VARINFO &vi)
{
    if (vi.name == nullptr)
        return INVALID_VAR_CODE;

    // trace("%s : %d",vi.name,(long)vi.bArray);

    const auto hash = MakeHashValue(vi.name);

    for (uint32_t n = 0; n < Var_num; n++)
    {
        if (pTable[n].hash == hash)
            if (_stricmp(pTable[n].name, vi.name) == 0)
            {
                // variable with such name already registred,
                if (pTable[n].segment_id == INVALID_SEGMENT_ID)
                {
                    UpdateHashTable(n, hash, true);
                    // but segment is unloaded
                    pTable[n].segment_id = vi.segment_id;

                    // if the same type and dimension - just return code
                    if (pTable[n].type == vi.type && pTable[n].elements == vi.elements)
                        return n;

                    // name the same, but type or dimension different - recreate (keep old name and hash)
                    pTable[n].elements = vi.elements;
                    pTable[n].type = vi.type;
                    delete pTable[n].pDClass;

                    pTable[n].pDClass = new DATA;
                    pTable[n].pDClass->SetVCompiler(pVCompiler);
                    pTable[n].pDClass->SetType(vi.type, vi.elements);

                    return n;
                }
                // and already exist
                // this is 'double variable name' error
                // (possible becose hash function error), user must rename variable
                return INVALID_VAR_CODE;
            }
    }
    // function not found, add anew one
    // adjust buffer size
    if (Var_num >= Buffer_size)
    {
        Buffer_size += VAR_BUFFER_BLOCK_SIZE;
        pTable.resize(Buffer_size);
    }
    // pTable[Var_num] = vi;
    pTable[Var_num].bArray = vi.bArray;
    pTable[Var_num].segment_id = vi.segment_id;
    pTable[Var_num].elements = vi.elements;
    pTable[Var_num].type = vi.type;
    pTable[Var_num].hash = hash;
    pTable[Var_num].name = nullptr;

    pTable[Var_num].pDClass = new DATA;
    pTable[Var_num].pDClass->SetVCompiler(pVCompiler);
    pTable[Var_num].pDClass->SetType(vi.type, vi.elements);
    pTable[Var_num].pDClass->SetGlobalVarTableIndex(Var_num);

    UpdateHashTable(Var_num, hash, true);

    if constexpr (true) // bKeepName)
    {
        if (vi.name)
        {
            const auto len = strlen(vi.name) + 1;
            pTable[Var_num].name = new char[len];
            memcpy(pTable[Var_num].name, vi.name, len);
        }
    }
    Var_num++;

    return (Var_num - 1);
}

uint32_t S_VARTAB::MakeHashValue(const char *string)
{
    uint32_t hval = 0;
    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A'; // case independent
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

void S_VARTAB::InvalidateBySegmentID(uint32_t segment_id)
{
    for (uint32_t n = 0; n < Var_num; n++)
    {
        if (pTable[n].segment_id != segment_id)
            continue;
        pTable[n].segment_id = INVALID_SEGMENT_ID;
        UpdateHashTable(n, pTable[n].hash, false);
    }
}

uint32_t S_VARTAB::FindVar(const char *var_name)
{
    if (var_name == nullptr)
        return INVALID_VAR_CODE;
    const auto hash = MakeHashValue(var_name);
    const auto hash_index = VTMAKEHASHINDEX(hash);
    for (uint32_t n = 0; n < HashLine[hash_index].nNumElements; n++)
    {
        const auto ni = HashLine[hash_index].pElements[n];
        if (pTable[ni].hash == hash) // return n;
            if (_stricmp(pTable[ni].name, var_name) == 0)
                return ni;
    }
    return INVALID_VAR_CODE;
    /*

    uint32_t n;
    uint32_t hash;
    if(var_name == 0) return INVALID_VAR_CODE;
    hash = MakeHashValue(var_name);
    for(n=0;n<Var_num;n++)
    {
      if(pTable[n].hash == hash) //return n;
      if(_stricmp(pTable[n].name,var_name)==0) return n;
    }
    return INVALID_VAR_CODE;*/
}

bool S_VARTAB::ArraySizeChanged(uint32_t nIndex, uint32_t nNewSize)
{
    if (nIndex >= Var_num)
        return false;
    if (!pTable[nIndex].pDClass->IsArray())
    {
        // trace("ERROR: invalid variable type for 'ChangeArraySize(?)' function");
        return false;
    }
    pTable[nIndex].elements = nNewSize;
    return true;
}

void S_VARTAB::UpdateHashTable(uint32_t code, uint32_t hash, bool in)
{
    const auto hash_index = VTMAKEHASHINDEX(hash);

    for (uint32_t n = 0; n < HashLine[hash_index].nNumElements; n++)
    {
        if (HashLine[hash_index].pElements[n] != code)
            continue;
        if (!in)
        {
            // take element out of list
            HashLine[hash_index].pElements[n] = HashLine[hash_index].pElements[HashLine[hash_index].nNumElements - 1];
            HashLine[hash_index].nNumElements--;
            HashLine[hash_index].pElements.resize(HashLine[hash_index].nNumElements);
            return;
        }
        return;
        // ok, already in list (? possible)
    }
    // not in list - add
    HashLine[hash_index].nNumElements++;
    HashLine[hash_index].pElements.resize(HashLine[hash_index].nNumElements);
    HashLine[hash_index].pElements[HashLine[hash_index].nNumElements - 1] = code;
}
