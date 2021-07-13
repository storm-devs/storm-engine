#pragma once

#include "Matrix.h"
#include "Sd2_h/CannonTrace.h"
#include "blast.h"
#include "dx9render.h"
#include "geometry.h"
#include "vmodule_api.h"

typedef struct
{
    CVECTOR pos;
    CVECTOR ang;
    CVECTOR ang_speed;
    CVECTOR dir;
    GEOS *geo;
    float speed;
    bool bDouble;
    bool bEffect;
} GEOPARTICLE;

class BLAST : public Entity
{
    VDX9RENDER *rs;
    VGEOMETRY *gs;
    std::vector<GEOPARTICLE> Item;
    CANNON_TRACE_BASE *pSea;
    entid_t sea_eid;
    float AngleDeviation;
    uint32_t ItemsNum;
    CMatrix Center;
    entid_t Splash;

  public:
    ~BLAST();
    BLAST();
    bool Init();
    uint64_t ProcessMessage(MESSAGE &message);
    uint32_t AttributeChanged(ATTRIBUTES *);
    void Realize(uint32_t Delta_Time);
    void ProcessTime(uint32_t Delta_Time);
    void SetBlastCenter(CVECTOR pos, CVECTOR ang);
    void AddGeometry(char *name, long num);
    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        // case Stage::execute:
        //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }
};
