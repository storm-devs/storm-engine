#include "s_functab.h"

/*DWORD _testfunc(VS_STACK * ps)
{
    return 7;
}*/

#define MAKEHASHINDEX(x) (x & 0xff)

S_FUNCTAB::S_FUNCTAB()
{
    Buffer_size = 0;
    Func_num = 0;
    for (uint32_t n = 0; n < HASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
    }
}

S_FUNCTAB::~S_FUNCTAB()
{
    Release();
}

void S_FUNCTAB::Release()
{
    uint32_t n;
    for (n = 0; n < Func_num; n++)
    {
        delete pTable[n].name;
        for (uint32_t i = 0; i < pTable[n].var_num; i++)
        {
            delete pTable[n].pLocal[i].name;
        }
        pTable[n].pLocal.clear();
        delete pTable[n].decl_file_name;
        pTable[n].decl_file_name = nullptr;
    }
    Buffer_size = 0;
    Func_num = 0;
    for (n = 0; n < HASHT_SIZE; n++)
    {
        HashLine[n].nNumElements = 0;
    }
}

bool S_FUNCTAB::GetFunc(FUNCINFO &fi, uint32_t func_code)
{
    if (func_code >= Func_num)
        return false;

    if (pTable[func_code].segment_id == IMPORTED_SEGMENT_ID)
    {
        if (pTable[func_code].pImportedFunc == nullptr)
        {
            return false;
        }
        fi = pTable[func_code];
        return true;
    }
    if (pTable[func_code].offset == INVALID_FUNC_OFFSET)
    {
        return false;
    }
    fi = pTable[func_code];
    return true;
}

bool S_FUNCTAB::GetFuncX(FUNCINFO &fi, uint32_t func_code)
{
    if (func_code >= Func_num)
        return false;
    fi = pTable[func_code];
    return true;
}

uint32_t S_FUNCTAB::AddFunc(const FUNCINFO &fi)
{
    if (fi.name == nullptr)
        return INVALID_FUNC_CODE;
    const auto hash = MakeHashValue(fi.name);
    auto hash_index = MAKEHASHINDEX(hash);

    const auto len = strlen(fi.decl_file_name) + 1;
    for (uint32_t n = 0; n < Func_num; n++)
    {
        if (pTable[n].hash != hash)
            continue;
        if (_stricmp(pTable[n].name, fi.name) != 0)
            continue;

        // function with such name already registred,
        if (pTable[n].offset == INVALID_FUNC_OFFSET)
        {
            // but offset isnt set (function segment unloaded)
            // - set function segment and offset info

            pTable[n].fTimeUsage = 0;
            pTable[n].nNumberOfCalls = 0;
            pTable[n].offset = fi.offset;
            pTable[n].segment_id = fi.segment_id;
            pTable[n].decl_line = fi.decl_line;
            pTable[n].pImportedFunc = fi.pImportedFunc;

            delete pTable[n].decl_file_name;

            // pTable[n].decl_file_name = new char[len];
            // memcpy(pTable[n].decl_file_name, fi.decl_file_name, len);
            pTable[n].decl_file_name = _strdup(fi.decl_file_name);
            pTable[n].code = n;
            UpdateHashTable(n, hash, true);
            return n;
        }
        // and already exist
        // this is 'double function name' error
        // (possible becose hash function error), user must rename function
        return INVALID_FUNC_CODE;
    }
    // function not found, add anew one
    // adjust buffer size
    if (Func_num >= Buffer_size)
    {
        Buffer_size += FUNC_BUFFER_BLOCK_SIZE;
        pTable.resize(Buffer_size);
    }
    pTable[Func_num] = fi;
    pTable[Func_num].hash = hash;
    pTable[Func_num].name = nullptr;
    pTable[Func_num].arguments = fi.arguments;
    pTable[Func_num].ext_args = 0;
    pTable[Func_num].var_num = 0;
    pTable[Func_num].decl_line = fi.decl_line;
    pTable[Func_num].fTimeUsage = 0;
    pTable[Func_num].nNumberOfCalls = 0;

    // pTable[Func_num].decl_file_name = new char[len];
    // memcpy(pTable[Func_num].decl_file_name, fi.decl_file_name, len);
    pTable[Func_num].decl_file_name = _strdup(fi.decl_file_name);

    pTable[Func_num].code = Func_num;
    pTable[Func_num].pImportedFunc = fi.pImportedFunc;

    UpdateHashTable(Func_num, hash, true);

    /*    DWORD a;
      pTable[Func_num].pImportedFunc = testfunc;
      a = pTable[Func_num].pImportedFunc(0);
    */
    if constexpr (true) // bKeepName)
    {
        if (fi.name)
        {
            // const auto len = strlen(fi.name) + 1;
            // pTable[Func_num].name = new char[len];
            // memcpy(pTable[Func_num].name, fi.name, len);
            pTable[Func_num].name = _strdup(fi.name);
        }
    }
    Func_num++;

    return (Func_num - 1);
}

uint32_t S_FUNCTAB::MakeHashValue(const char *string)
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

void S_FUNCTAB::InvalidateBySegmentID(uint32_t segment_id)
{
    for (uint32_t n = 0; n < Func_num; n++)
    {
        if (pTable[n].segment_id != segment_id)
            continue;
        UpdateHashTable(n, pTable[n].hash, false);
        pTable[n].offset = INVALID_FUNC_OFFSET;
        for (uint32_t i = 0; i < pTable[n].var_num; i++)
        {
            delete pTable[n].pLocal[i].name;
        }
        pTable[n].pLocal.clear();
        pTable[n].var_num = 0;
        pTable[n].arguments = 0;
        delete pTable[n].decl_file_name;
        pTable[n].decl_file_name = nullptr;
    }
}

void S_FUNCTAB::InvalidateFunction(uint32_t nFuncHandle)
{
    if (nFuncHandle < Func_num)
    {
        const auto n = nFuncHandle;
        UpdateHashTable(n, pTable[n].hash, false);
        pTable[n].offset = INVALID_FUNC_OFFSET;
        for (uint32_t i = 0; i < pTable[n].var_num; i++)
        {
            delete pTable[n].pLocal[i].name;
        }
        pTable[n].pLocal.clear();
        // pTable[n].var_num = 0;
        // pTable[n].arguments = 0;
        delete pTable[n].decl_file_name;
        pTable[n].decl_file_name = nullptr;
        pTable[n].pImportedFunc = nullptr;
    }
}

uint32_t S_FUNCTAB::FindFunc(const char *func_name)
{
    if (func_name == nullptr)
        return INVALID_FUNC_CODE;
    const auto hash = MakeHashValue(func_name);

    const auto hash_index = MAKEHASHINDEX(hash);
    const auto nNum = HashLine[hash_index].nNumElements;
    for (uint32_t n = 0; n < nNum; n++)
    {
        const auto ni = HashLine[hash_index].pElements[n];
        if (pTable[ni].hash == hash)
            if (_stricmp(pTable[ni].name, func_name) == 0)
                return ni;
    }

    /*    for(n=0;n<Func_num;n++)
      {
        if(pTable[n].hash == hash)
        if(_stricmp(pTable[n].name,func_name)== 0) return n;
      }
    */
    return INVALID_FUNC_CODE;
}

bool S_FUNCTAB::SetFuncOffset(const char *func_name, uint32_t offset)
{
    const auto n = FindFunc(func_name);
    if (n == INVALID_FUNC_CODE)
        return false;
    pTable[n].offset = offset;
    return true;
    /*
      uint32_t n;
      uint32_t hash;
      hash = MakeHashValue(func_name);
      for(n=0;n<Func_num;n++)
      {
        if(pTable[n].hash == hash)
        {
          pTable[n].offset = offset;
          return true;
        }
      }
      return false;
    */
}

bool S_FUNCTAB::AddFuncVar(uint32_t func_code, LVARINFO &lvi)
{
    if (func_code >= Func_num)
        return false;
    if (lvi.name == nullptr)
        return false;

    const auto hash = MakeHashValue(lvi.name);
    for (uint32_t n = 0; n < pTable[func_code].var_num; n++)
    {
        if (hash == pTable[func_code].pLocal[n].hash) // return false;
            if (_stricmp(pTable[func_code].pLocal[n].name, lvi.name) == 0)
                return false;
    }

    const auto vindex = pTable[func_code].var_num;
    pTable[func_code].var_num++;
    pTable[func_code].pLocal.resize(pTable[func_code].var_num);
    pTable[func_code].pLocal[vindex].name = nullptr;
    if constexpr (true) // bKeepName)
    {
        // const auto len = strlen(lvi.name) + 1;
        // pTable[func_code].pLocal[vindex].name = new char[len];
        // memcpy(pTable[func_code].pLocal[vindex].name, lvi.name, len);
        pTable[func_code].pLocal[vindex].name = _strdup(lvi.name);
    }
    pTable[func_code].pLocal[vindex].bArray = lvi.bArray;
    pTable[func_code].pLocal[vindex].hash = hash;
    pTable[func_code].pLocal[vindex].type = lvi.type;
    pTable[func_code].pLocal[vindex].elements = lvi.elements;
    return true;
}

bool S_FUNCTAB::AddFuncArg(uint32_t func_code, LVARINFO &lvi, bool bExt)
{
    if (func_code >= Func_num)
        return false;
    if (bExt)
    {
        pTable[func_code].ext_args++;
        return true;
    }
    pTable[func_code].arguments++;
    return AddFuncVar(func_code, lvi);
}

uint32_t S_FUNCTAB::FindVar(uint32_t func_code, const char *var_name)
{
    if (var_name == nullptr)
        return INVALID_VAR_CODE;
    if (func_code >= Func_num)
        return INVALID_VAR_CODE;
    const auto hash = MakeHashValue(var_name);
    for (uint32_t n = 0; n < pTable[func_code].var_num; n++)
    {
        if (hash == pTable[func_code].pLocal[n].hash) // return n;
            if (_stricmp(var_name, pTable[func_code].pLocal[n].name) == 0)
                return n;
    }
    return INVALID_VAR_CODE;
}

bool S_FUNCTAB::GetVar(LVARINFO &lvi, uint32_t func_code, uint32_t var_code)
{
    if (func_code >= Func_num)
        return false;
    if (var_code >= pTable[func_code].var_num)
        return false;
    lvi = pTable[func_code].pLocal[var_code];
    return true;
}

void S_FUNCTAB::AddTime(uint32_t func_code, uint64_t time)
{
    if (func_code >= Func_num)
        return;
    pTable[func_code].fTimeUsage += time;
}

void S_FUNCTAB::SetTimeUsage(uint32_t func_code, double f)
{
    if (func_code >= Func_num)
        return;
    pTable[func_code].fTimeUsage = f;
}

void S_FUNCTAB::AddCall(uint32_t func_code)
{
    if (func_code >= Func_num)
        return;
    pTable[func_code].nNumberOfCalls++;
}

void S_FUNCTAB::UpdateHashTable(uint32_t code, uint32_t hash, bool in)
{
    const auto hash_index = MAKEHASHINDEX(hash);

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
