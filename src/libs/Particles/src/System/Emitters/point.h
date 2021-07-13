#pragma once

#include "base.h"

class PointEmitter : public BaseEmitter
{
  protected:
    // Get a position for emission of new particles
    Vector GetNewParticlePosition(float DeltaTime) override;

  public:
    // Constructor / destructor
    PointEmitter(ParticleSystem *pSystem);
    virtual ~PointEmitter();
};
