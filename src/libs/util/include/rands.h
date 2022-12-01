#pragma once

#include <cstdlib>

// returns random float
inline float rand(float r)
{
    return r * ((float)rand() / (float)RAND_MAX);
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