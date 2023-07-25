#pragma once

#include "attributes.h"
#include "entity.h"
#include "istring.hpp"

namespace storm
{

bool LoadConfig(ATTRIBUTES *target, const std::string_view &file_path, entid_t entity_id);

} // namespace storm
