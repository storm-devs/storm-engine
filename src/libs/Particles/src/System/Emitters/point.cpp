#include "point.h"

// Constructor / destructor
PointEmitter::PointEmitter(ParticleSystem *pSystem) : BaseEmitter(pSystem)
{
}

PointEmitter::~PointEmitter()
{
}

// Get a position for the emission of new particles
Vector PointEmitter::GetNewParticlePosition(float DeltaTime)
{
    // TODO: should emit at the starting position of the track ...
    return Vector(0.0f);
}
