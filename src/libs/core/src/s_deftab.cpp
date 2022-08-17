#include "s_deftab.h"

#include "string_compare.hpp"

#define DTMAKEHASHINDEX(x) (x & 0xff)

S_DEFTAB::S_DEFTAB()
{
    Buffer_size = 0;
    Def_num = 0;
    for (uint32_t n = 0; n < DTHASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
    }
}

S_DEFTAB::~S_DEFTAB()
{
    Release();
}

void S_DEFTAB::Release()
{
    uint32_t n;
    for (n = 0; n < Def_num; n++)
    {
        delete[] pTable[n].name;
        if (pTable[n].deftype == STRING)
        {
            if (pTable[n].data4b != 0)
                delete ((char *)pTable[n].data4b);
        }
    }
    pTable.clear();
    Buffer_size = 0;
    Def_num = 0;
    for (n = 0; n < DTHASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
    }
}

bool S_DEFTAB::GetDef(DEFINFO &di, uint32_t def_code)
{
    if (def_code >= Def_num)
        return false;
    if (pTable[def_code].segment_id == INVALID_SEGMENT_ID)
        return false;
    di = pTable[def_code];
    return true;
}

bool S_DEFTAB::GetDefX(DEFINFO &di, uint32_t def_code)
{
    if (def_code >= Def_num)
        return false;
    di = pTable[def_code];
    return true;
}

uint32_t S_DEFTAB::AddDef(DEFINFO &di)
{
    uint32_t n;

    if (di.name == nullptr)
        return INVALID_DEF_CODE;
    const auto hash = MakeHashValue(di.name);

    for (n = 0; n < Def_num; n++)
    {
        if (pTable[n].hash == hash)
            if (storm::iEquals(pTable[n].name, di.name))
            {
                // define with such name already registred,
                if (pTable[n].segment_id == INVALID_SEGMENT_ID)
                {
                    // but not used
                    pTable[n].data4b = di.data4b;
                    pTable[n].deftype = di.deftype;
                    pTable[n].segment_id = di.segment_id;
                    UpdateHashTable(n, hash, true);

                    return n;
                }
                // and already exist
                // this is 'define redefinition' error
                return INVALID_DEF_CODE;
            }
    }
    // function not found, add anew one
    // adjust buffer size
    if (Def_num >= Buffer_size)
    {
        Buffer_size += DEF_BUFFER_BLOCK_SIZE;
        pTable.resize(Buffer_size);
    }
    pTable[n].data4b = di.data4b;
    pTable[n].deftype = di.deftype;
    pTable[n].segment_id = di.segment_id;
    // pTable[Def_num] = di;
    pTable[Def_num].hash = hash;
    pTable[Def_num].name = nullptr;
    UpdateHashTable(Def_num, hash, true);
    if constexpr (true) // bKeepName)
    {
        if (di.name)
        {
            const auto len = strlen(di.name) + 1;
            pTable[Def_num].name = new char[len];
            memcpy(pTable[Def_num].name, di.name, len);
        }
    }
    Def_num++;

    return (Def_num - 1);
}

uint32_t S_DEFTAB::MakeHashValue(const char *string)
{
    uint32_t hval = 0;
    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A'; // case independent
        hval = (hval << 4) + static_cast<uint32_t>(v);
        const uint32_t g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

void S_DEFTAB::InvalidateBySegmentID(uint32_t segment_id)
{
    for (uint32_t n = 0; n < Def_num; n++)
    {
        if (pTable[n].segment_id != segment_id)
            continue;
        UpdateHashTable(n, pTable[n].hash, false);
        pTable[n].segment_id = INVALID_SEGMENT_ID;
        if (pTable[n].deftype == STRING)
        {
            if (pTable[n].data4b)
            {
                delete ((char *)pTable[n].data4b);
            }
            pTable[n].data4b = 0;
        }
    }
}

uint32_t S_DEFTAB::FindDef(const char *def_name)
{
    if (def_name == nullptr)
        return INVALID_DEF_CODE;
    const auto hash = MakeHashValue(def_name);

    const auto hash_index = DTMAKEHASHINDEX(hash);
    for (uint32_t n = 0; n < HashLine[hash_index].nNumElements; n++)
    {
        const auto ni = HashLine[hash_index].pElements[n];
        if (pTable[ni].hash == hash) // return n;
            if (storm::iEquals(pTable[ni].name, def_name))
                return ni;
    }
    return INVALID_DEF_CODE;

    /*
    for(n=0;n<Def_num;n++)
    {
      if(pTable[n].hash == hash) return n;
    }
    return INVALID_DEF_CODE;*/
}

void S_DEFTAB::UpdateHashTable(uint32_t code, uint32_t hash, bool in)
{
    const uint32_t hash_index = DTMAKEHASHINDEX(hash);

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
