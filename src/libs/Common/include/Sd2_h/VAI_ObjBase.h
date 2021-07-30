#pragma once

#include "model.h"
#include "CannonTrace.h"

class CSaveLoad;

class VAI_OBJBASE : public CANNON_TRACE_BASE
{
  private:
    ATTRIBUTES *pACharacter;

  public:
    ~VAI_OBJBASE() override{};

    virtual void SetACharacter(ATTRIBUTES *pAP)
    {
        pACharacter = pAP;
    };

    virtual ATTRIBUTES *GetACharacter()
    {
        return pACharacter;
    };

    virtual bool Mount(ATTRIBUTES *) = 0;

    virtual void SetPos(const Vector &vNewPos) = 0;

    virtual Vector GetPos() const = 0;
    virtual Vector GetAng() const = 0;
    virtual Matrix *GetMatrix() = 0;
    virtual Vector GetBoxsize() const = 0;

    virtual MODEL *GetModel() const = 0;
    virtual entid_t GetModelEID() const = 0;

    virtual void Save(CSaveLoad *pSL) = 0;
    virtual void Load(CSaveLoad *pSL) = 0;

    virtual void Fire(const Vector &vPos) = 0;
};
