#include "BallSplashDefines.h"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float bsrand(float r)
// returns random float
{
    static const float randMax = (float)RAND_MAX;
    return r * (((float)rand()) / randMax);
}

//--------------------------------------------------------------------
float bsrandUpper(float r)
// returns larger half of random float
{
    float half = r / 2.0f;
    return (half + rand(half));
}

//--------------------------------------------------------------------
float bsrandCentered(float r)
// returns random float around 0
{
    float half = r / 2.0f;
    return (rand(r) - half);
}

//--------------------------------------------------------------------
