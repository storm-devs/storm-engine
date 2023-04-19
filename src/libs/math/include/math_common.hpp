#pragma once

#include <cmath>

#ifdef SQR
#undef SQR
#endif

#define SQR(x) ::storm::Sqr(x)
#define FRAND(x) ::storm::RandomFloat(x)

namespace storm {

constexpr const float kRandMaxFloat = static_cast<float>(RAND_MAX);

inline float RandomFloat(const float x) {
    return x * static_cast<float>(rand()) / storm::kRandMaxFloat;
}

template<typename T>
inline T Sqr(const T x) {
    return x * x;
}

} // namespace storm
