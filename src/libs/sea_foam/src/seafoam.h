#pragma once

#include "seafoam_defines.h"
#include "t_carcass.h"
#include "v_sound_service.h"
#include "dx9render.h"
#include "geos.h"
#include "model.h"
#include "sea_base.h"
#include "seafoam_ps.h"
#include "ship_base.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
struct tFoamEmitter
{
    CVECTOR center[TRACE_STEPS_Y];
    CVECTOR meanPoint;
    float k;
    CVECTOR finalPos;
};

struct tShipFoamInfo
{
    tFoamEmitter hull[2][TRACE_STEPS_Z];
    CVECTOR levelStarts[2][TRACE_STEPS_Z];
    GEOS::INFO hullInfo;
    SHIP_BASE *ship;
    TCarcass *carcass[2];
    SEAFOAM_PS *frontEmitter[3];
    MODEL *shipModel;
    TSD_ID sound;
    bool doSplash;
    bool firstSoundPlay;
    bool enabled;
};

class SEAFOAM : public Entity
{
  public:
    SEAFOAM();
    ~SEAFOAM() override;

    bool Init() override;
    uint32_t AttributeChanged(ATTRIBUTES *pA) override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }

  private:
    void InitializeShipFoam();
    void ReleaseShipFoam();
    void RealizeShipFoam_Particles(tShipFoamInfo &_shipFoamInfo, uint32_t dTime);
    void RealizeShipFoam_Mesh(tShipFoamInfo &_shipFoamInfo, uint32_t dTime);
    void CreateTracePoints(tShipFoamInfo *_shipFoamInfo);
    void InterpolateLeftParticle(tShipFoamInfo &_shipFoamInfo, int z, uint32_t dTime);
    void InterpolateRightParticle(tShipFoamInfo &_shipFoamInfo, int z, uint32_t dTime);
    void AddShip(entid_t pShipEID);

    VDX9RENDER *renderer;
    entid_t seaID;
    SEA_BASE *sea;
    tShipFoamInfo shipFoamInfo[MAX_SHIPS]{};
    int shipsCount;
    std::unique_ptr<INIFILE> psIni;
    int32_t carcassTexture;
    bool isStorm;
    VSoundService *soundService;
};
