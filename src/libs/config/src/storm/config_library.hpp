#pragma once

#include "script_libriary.h"

namespace storm {

class ConfigLibrary : public SCRIPT_LIBRIARY {
  public:
    bool Init() override;
};

} // namespace storm
