#pragma once

namespace storm {

#ifdef _DEBUG
constexpr bool kIsDebug = true;
#else
constexpr bool kIsDebug = false;
#endif

#ifdef STORM_ENABLE_SAFE_MODE
constexpr bool kIsSafeMode = true;
#else
constexpr bool kIsSafeMode = false;
#endif

constexpr bool kValidateCollisionData = kIsSafeMode || kIsDebug;

}
