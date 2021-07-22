#pragma once

#include "data.h"
#include "v_s_stack.h"

#define STACK_BUFFER_BLOCK_SIZE 512
#define STACK_BUFFER_LIMIT 65535

class S_STACK : public VS_STACK
{
    std::vector<DATA *> pStackData;
    uint32_t Buffer_size;
    uint32_t Data_num;
    VIRTUAL_COMPILER *pVCompiler;

  public:
    S_STACK();
    ~S_STACK();
    void Release();
    DATA *Push(DATA *pdataclass = nullptr);
    DATA *Pop();
    DATA *Read(uint32_t offset, uint32_t index);
    DATA *Read();

    uint32_t GetDataNum()
    {
        return Data_num;
    };

    void SetVCompiler(VIRTUAL_COMPILER *pvc)
    {
        pVCompiler = pvc;
    }

    void InvalidateFrom(uint32_t index);
};
