#ifndef _VSTACK_H_
#define _VSTACK_H_

//#include <windows.h>
#include "data.h"
#include "memop.h"

class VSTACK
{
    DATA **pBlock;
    long Block_Num;
    long Block_Size;

  public:
    VSTACK();
    ~VSTACK();
    void Push(DATA *block);
    bool Pop(DATA **pblock);
    bool Read(DATA **pblock);
    void Release();
};
#endif