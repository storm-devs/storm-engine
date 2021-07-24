#pragma once
#include "script_libriary.h"

class SCRIPT_LIBRIARY_TEST : public SCRIPT_LIBRIARY
{
  public:
    SCRIPT_LIBRIARY_TEST(){};

    ~SCRIPT_LIBRIARY_TEST() override{};
    bool Init() override;
};
