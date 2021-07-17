#include "s_eventtab.h"
#define HASHT_INDEX(x) (uint8_t)(x >> 24)
#define HASHT_CODE(x) (x & 0xffffff)
#define HASH2INDEX(x) (uint8_t)(x & 0x2f)

S_EVENTTAB::S_EVENTTAB()
{
    for (uint32_t n = 0; n < HASHTABLE_SIZE; n++)
    {
        Buffer_size[n] = 0;
        Event_num[n] = 0;
    }
}

S_EVENTTAB::~S_EVENTTAB()
{
    Release();
}

void S_EVENTTAB::Clear()
{
    for (uint32_t i = 0; i < HASHTABLE_SIZE; i++)
    {
        for (uint32_t n = 0; n < Event_num[i]; n++)
        {
            for (uint32_t m = 0; m < pTable[i][n].elements; m++)
            {
                if (!pTable[i][n].pFuncInfo[m].bStatic)
                    pTable[i][n].pFuncInfo[m].status = FSTATUS_DELETED;
            }

            // if(pTable[n].pFuncInfo) delete pTable[n].pFuncInfo;
            // if(pTable[n].name) delete pTable[n].name;
        }
        // delete pTable; pTable = 0;
        // Buffer_size = 0;
        // Event_num = 0;
    }
    ProcessFrame();
}

void S_EVENTTAB::Release()
{
    for (uint32_t i = 0; i < HASHTABLE_SIZE; i++)
    {
        for (uint32_t n = 0; n < Event_num[i]; n++)
        {
            delete pTable[i][n].name;
        }

        Buffer_size[i] = 0;
        Event_num[i] = 0;
    }
}

bool S_EVENTTAB::GetEvent(EVENTINFO &ei, uint32_t event_code)
{
    const auto ti = HASHT_INDEX(event_code);
    const auto tc = HASHT_CODE(event_code);
    if (tc >= Event_num[ti])
        return false;
    ei = pTable[ti][tc];
    return true;
}

uint32_t S_EVENTTAB::AddEventHandler(const char *event_name, uint32_t func_code, uint32_t func_segment_id, long flag,
                                     bool bStatic)
{
    uint32_t i;

    const auto hash = MakeHashValue(event_name);

    const auto ti = HASH2INDEX(hash);

    for (uint32_t n = 0; n < Event_num[ti]; n++)
    {
        if (pTable[ti][n].hash == hash)
        {
            if (_stricmp(event_name, pTable[ti][n].name) != 0)
                continue;
            // event already in list
            for (i = 0; i < pTable[ti][n].elements; i++)
            {
                // event handler function already set
                if (pTable[ti][n].pFuncInfo[i].func_code == func_code)
                {
                    /*if(pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED)
                    {
                      trace("pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED : %s",pTable[ti][n].name);
                    }*/
                    // return n;
                    pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;

                    return (((ti << 24) & 0xff000000) | (n & 0xffffff));
                }
            }
            // add function
            i = pTable[ti][n].elements;
            pTable[ti][n].elements++;
            pTable[ti][n].pFuncInfo.resize(pTable[ti][n].elements);

            pTable[ti][n].pFuncInfo[i].func_code = func_code;
            pTable[ti][n].pFuncInfo[i].segment_id = func_segment_id;
            if (flag)
                pTable[ti][n].pFuncInfo[i].status = FSTATUS_NEW;
            else
                pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;
            pTable[ti][n].pFuncInfo[i].bStatic = bStatic;
            // return n;
            return (((ti << 24) & 0xff000000) | (n & 0xffffff));
        }
    }

    // add new event
    if (Event_num[ti] >= Buffer_size[ti])
    {
        Buffer_size[ti] += BUFFER_BLOCK_SIZE;
        pTable[ti].resize(Buffer_size[ti]);
    }

    pTable[ti][Event_num[ti]].elements = 1;
    pTable[ti][Event_num[ti]].hash = hash;
    pTable[ti][Event_num[ti]].name = nullptr;

    pTable[ti][Event_num[ti]].pFuncInfo.push_back(EVENT_FUNC_INFO{});
    pTable[ti][Event_num[ti]].pFuncInfo[0].func_code = func_code;
    pTable[ti][Event_num[ti]].pFuncInfo[0].segment_id = func_segment_id;
    if (flag)
        pTable[ti][Event_num[ti]].pFuncInfo[0].status = FSTATUS_NEW;
    else
        pTable[ti][Event_num[ti]].pFuncInfo[0].status = FSTATUS_NORMAL;
    pTable[ti][Event_num[ti]].pFuncInfo[0].bStatic = bStatic;

    if constexpr (true) // bKeepName)
    {
        if (event_name)
        {
            const auto len = strlen(event_name) + 1;
            pTable[ti][Event_num[ti]].name = new char[len];
            memcpy(pTable[ti][Event_num[ti]].name, event_name, len);
        }
    }
    Event_num[ti]++;
    // return (Event_num[ti] - 1);
    return (((ti << 24) & 0xff000000) | ((Event_num[ti] - 1) & 0xffffff));
}

uint32_t S_EVENTTAB::MakeHashValue(const char *string)
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

bool S_EVENTTAB::DelEventHandler(const char *event_name, uint32_t func_code)
{
    if (event_name == nullptr)
        return false;
    const auto hash = MakeHashValue(event_name);

    const auto ti = HASH2INDEX(hash);

    for (uint32_t n = 0; n < Event_num[ti]; n++)
    {
        if (pTable[ti][n].hash == hash)
            if (_stricmp(pTable[ti][n].name, event_name) == 0)
            {
                return DelEventHandler(ti, n, func_code);
                // return;
            }
    }
    return false;
}

void S_EVENTTAB::SetStatus(const char *event_name, uint32_t func_code, uint32_t status)
{
    if (event_name == nullptr)
        return;

    const auto hash = MakeHashValue(event_name);
    const auto ti = HASH2INDEX(hash);

    for (uint32_t n = 0; n < Event_num[ti]; n++)
    {
        if (pTable[ti][n].hash == hash)
            if (_stricmp(pTable[ti][n].name, event_name) == 0)
            {
                for (uint32_t i = 0; i < pTable[ti][n].elements; i++)
                {
                    if (pTable[ti][n].pFuncInfo[i].func_code == func_code)
                    {
                        pTable[ti][n].pFuncInfo[i].status = status;
                        return;
                    }
                }
            }
    }
}

bool S_EVENTTAB::DelEventHandler(uint8_t ti, uint32_t event_code, uint32_t func_code, bool bDelStatic)
{
    if (!bDelStatic)
    {
        if (pTable[ti][event_code].pFuncInfo[func_code].bStatic)
        {
            return false;
        }
    }

    for (auto n = func_code; n < (pTable[ti][event_code].elements - 1); n++)
    {
        pTable[ti][event_code].pFuncInfo[n] = pTable[ti][event_code].pFuncInfo[n + 1];
    }
    pTable[ti][event_code].elements--;
    pTable[ti][event_code].pFuncInfo.resize(pTable[ti][event_code].elements);
    return true;
}

void S_EVENTTAB::InvalidateBySegmentID(uint32_t segment_id)
{
    for (uint32_t ti = 0; ti < HASHTABLE_SIZE; ti++)
    {
        for (uint32_t n = 0; n < Event_num[ti]; n++)
        {
            for (uint32_t i = 0; i < pTable[ti][n].elements; i++)
            {
                if (pTable[ti][n].pFuncInfo[i].segment_id == segment_id)
                {
                    if (DelEventHandler(static_cast<uint8_t>(ti), n, i, true))
                        i = 0;
                }
            }
        }
    }
}

uint32_t S_EVENTTAB::FindEvent(const char *event_name)
{
    if (event_name == nullptr)
        return INVALID_EVENT_CODE;
    const auto hash = MakeHashValue(event_name);
    const auto ti = HASH2INDEX(hash);
    for (uint32_t n = 0; n < Event_num[ti]; n++)
    {
        if (pTable[ti][n].hash == hash)
            if (_stricmp(pTable[ti][n].name, event_name) == 0)
                return (((ti << 24) & 0xff000000) | (n & 0xffffff));
    }
    return INVALID_EVENT_CODE;
}

void S_EVENTTAB::ProcessFrame()
{
    for (uint32_t ti = 0; ti < HASHTABLE_SIZE; ti++)
        for (uint32_t n = 0; n < Event_num[ti]; n++)
        {
            // delete old handlers
            for (uint32_t i = 0; i < pTable[ti][n].elements; i++)
            {
                if (pTable[ti][n].pFuncInfo[i].status == FSTATUS_DELETED)
                {
                    DelEventHandler(static_cast<uint8_t>(ti), n, i);
                    i = 0;
                }
                else
                    pTable[ti][n].pFuncInfo[i].status = FSTATUS_NORMAL;
            }
        }
}
