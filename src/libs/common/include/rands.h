#pragma once

#include <random>
#include <climits>

// returns random float
inline float rand(float r)
{
    std::random_device rd;
    return r * ((float)rd() / (float)UINT_MAX);
}

// returns larger half of random float
inline float randUpper(float r)
{
    float half = r / 2.0f;
    return (half + rand(half));
}

// returns random float around 0
inline float randCentered(float r)
{
    float half = r / 2.0f;
    return (rand(r) - half);
}
