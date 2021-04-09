#pragma once

#include <vector>

#define TSTACK_BUFFER_BLOCK_SIZE 128
#define TSTACK_BUFFER_LIMIT 65535

class S_TSTACK
{
    std::vector<char> pStackData;
    unsigned long Buffer_size;
    unsigned long Data_num;
    unsigned long Unit_size;

  public:
    S_TSTACK();
    ~S_TSTACK();
    void SetUnitSize(unsigned long unit_size);
    void Release();
    bool Push(char *pUnit);
    bool Pop(char *pUnit);
    bool Read(char *pUnit);
};
