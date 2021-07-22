#pragma once
#include "script_libriary.h"

class SCRIPT_RIGGING_FILES : public SCRIPT_LIBRIARY
{
  public:
    SCRIPT_RIGGING_FILES(){};

    ~SCRIPT_RIGGING_FILES() override{};
    bool Init() override;
};
