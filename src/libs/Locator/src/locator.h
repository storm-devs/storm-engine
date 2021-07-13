#pragma once

#include "blast.h"
#include "geometry.h"
#include "vmodule_api.h"

class VDATA;

class LOCATOR : public Entity
{
    enum
    {
        _X_ = 1,
        _Y_ = 2,
        _Z_ = 4,
        _AX_ = 8,
        _AY_ = 16,
        _AZ_ = 32,
        _XYZ_ = _X_ | _Y_ | _Z_
    };
    VGEOMETRY *gs;
    VDX9RENDER *rs;
    GEOS *geo;
    long groupID;
    long stringIndex;
    entid_t ParticlesID;

    void LocateForI(VDATA *pData);
    void LocateForI_L2(ATTRIBUTES *pA, GEOS *g, GEOS::LABEL &label);
    void LocateForI_Locators(ATTRIBUTES *pA, GEOS *geo, long iGroupID, uint32_t dwFlags);

  public:
    ~LOCATOR();
    LOCATOR();
    bool Init();
    uint64_t ProcessMessage(MESSAGE &message);
    uint32_t AttributeChanged(ATTRIBUTES *);
    void ProcessStage(Stage, uint32_t) override
    {
    }
    bool VerifyParticles();
};
