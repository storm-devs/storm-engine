#include "s_tstack.h"
#include <memory.h>

#include <stdexcept>

S_TSTACK::S_TSTACK()
{
    Unit_size = 0;
    Buffer_size = 0;
    Data_num = 0;
}

void S_TSTACK::SetUnitSize(unsigned long unit_size)
{
    Unit_size = unit_size;
}

S_TSTACK::~S_TSTACK()
{
    Release();
}

void S_TSTACK::Release()
{
    Buffer_size = 0;
    Data_num = 0;
}

bool S_TSTACK::Push(char *pUnit)
{
    if (pUnit == nullptr)
        return false;
    if (Data_num > TSTACK_BUFFER_LIMIT)
        throw std::runtime_error("stack overflow");
    if (Data_num >= Buffer_size)
    {
        Buffer_size += TSTACK_BUFFER_BLOCK_SIZE;
        pStackData.resize(Buffer_size * Unit_size);
    }
    memcpy(pStackData.data() + Data_num * Unit_size, pUnit, Unit_size);
    Data_num++;
    return true;
}

bool S_TSTACK::Pop(char *pUnit)
{
    if (pUnit == nullptr)
        throw std::runtime_error("invalid 'pUnit'");
    if (Data_num == 0)
        return false;
    Data_num--;
    memcpy(pUnit, pStackData.data() + Data_num * Unit_size, Unit_size);
    return true;
}

bool S_TSTACK::Read(char *pUnit)
{
    if (pUnit == nullptr)
        throw std::runtime_error("invalid 'pUnit'");
    if (Data_num == 0)
        return false;
    memcpy(pUnit, pStackData.data() + (Data_num - 1) * Unit_size, Unit_size);
    return true;
}
