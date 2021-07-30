#pragma once

#include "collide.h"
#include "dx9render.h"

#include <vector>

#define MAX_BLOOD_TRIANGLES 10000
#define ON_LIVETIME_BLOOD_TRIANGLES (MAX_BLOOD_TRIANGLES * 3 / 4)
#define MAX_CLIPPING_TRIANGLES 64

class Blood : public Entity
{
    struct BloodVertex
    {
        Vector pos;
        uint32_t dwCol;
        float u, v;
    };

    struct BloodTriangle
    {
        BloodVertex v[3];
    };

    struct BloodInfo
    {
        long nStartIdx;
        long nIdxQ;
        float fLiveTime;
        Vector cpos;
    };

    struct ClipTriangle
    {
        Vector v[3];
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Blood();
    virtual ~Blood();

    // Initialization
    bool Init() override;
    // Work
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //
    uint64_t ProcessMessage(MESSAGE &message) override;

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

    static bool AddClipPoligon(const Vector *v, long nv);

  protected:
    VDX9RENDER *pRS;
    COLLIDE *pCol;
    long texID;

    BloodTriangle pvBloodT[MAX_BLOOD_TRIANGLES];
    long nStartT;
    long nUsedTQ;

    std::vector<BloodInfo> aBlood;

    std::vector<entid_t> aModels;

    static ClipTriangle clipT[MAX_CLIPPING_TRIANGLES];
    static long nClipTQ;
    static Vector normal;

    void AddBlood(const Vector &pos);
    void BuildBloodDataByCollision(const Vector &cpos);
    void SetVertexByPos(BloodVertex &v, const Vector &pos, const Vector &vc, float fU0, float fV0);
    long CheckBloodQuantityInRadius(const Vector &cpos, float fDist, long nLimitQ);
};
