#ifndef _SCRIPT_LIBRIARY_TEST_H_
#define _SCRIPT_LIBRIARY_TEST_H_
#include "script_libriary.h"

class SCRIPT_LIBRIARY_TEST : public SCRIPT_LIBRIARY
{
  public:
    SCRIPT_LIBRIARY_TEST(){};
    ~SCRIPT_LIBRIARY_TEST(){};
    virtual bool Init();
};
#endif