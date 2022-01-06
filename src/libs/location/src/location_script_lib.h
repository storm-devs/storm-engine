#pragma once

#include "script_libriary.h"

class ScriptLocationLibrary : public SCRIPT_LIBRIARY
{
  public:
    ScriptLocationLibrary(){};

    ~ScriptLocationLibrary() override{};

    bool Init() override;
};
