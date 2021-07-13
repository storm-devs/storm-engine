#pragma once

#include "../Cvector.h"
#include "../Matrix.h"
#include "../model.h"
#include "../vmodule_api.h"
#include "CannonTrace.h"

class CSaveLoad;

class VAI_OBJBASE : public CANNON_TRACE_BASE
{
  private:
    ATTRIBUTES *pACharacter;

  public:
    virtual ~VAI_OBJBASE(){};

    virtual void SetACharacter(ATTRIBUTES *pAP)
    {
        pACharacter = pAP;
    };
    virtual ATTRIBUTES *GetACharacter()
    {
        return pACharacter;
    };

    virtual bool Mount(ATTRIBUTES *) = 0;

    virtual void SetPos(const CVECTOR &vNewPos) = 0;

    virtual CVECTOR GetPos() const = 0;
    virtual CVECTOR GetAng() const = 0;
    virtual CMatrix *GetMatrix() = 0;
    virtual CVECTOR GetBoxsize() const = 0;

    virtual MODEL *GetModel() const = 0;
    virtual entid_t GetModelEID() const = 0;

    virtual void Save(CSaveLoad *pSL) = 0;
    virtual void Load(CSaveLoad *pSL) = 0;

    virtual void Fire(const CVECTOR &vPos) = 0;
};
