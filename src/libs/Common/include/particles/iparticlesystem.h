#pragma once

#include <cstdint>

class IGMXEntity;
class Matrix;

class IParticleSystem
{
  protected:
    virtual ~IParticleSystem(){};

  public:
    // Create / delete
    IParticleSystem(){};
    virtual bool Release() = 0;

    // Update all particles
    virtual uint32_t Execute(float DeltaTime) = 0;

    // Restart particle system
    virtual void Restart(uint32_t RandomSeed) = 0;

    // Pause particle emission
    virtual void PauseEmission(bool bPause) = 0;

    // Find out whether emission is paused or not
    virtual bool IsEmissionPaused() = 0;
    // Set whether it's an auto-delete system or regular ...
    virtual void AutoDelete(bool Enabled) = 0;
    // Find out whether it's an auto-delete system or not
    virtual bool IsAutoDeleted() = 0;
    // Set transformation matrix for the system
    virtual void SetTransform(const Matrix &transform) = 0;
    virtual void GetTransform(Matrix &_matWorld) = 0;

    virtual void Teleport(const Matrix &transform) = 0;

    virtual bool IsAlive() = 0;

    virtual const char *GetName() = 0;

    virtual void Stop() = 0;
};
