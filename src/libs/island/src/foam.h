#pragma once

#include "ai_flow_graph.h"
#include "island_base.h"
#include "collide.h"
#include "model.h"
#include "sea_base.h"
#include "vma.hpp"

enum FOAMTYPE
{
    FOAM_TYPE_1 = 1,
    FOAM_TYPE_2 = 2,

    FOAM_MODE_FORCEDWORD = 0x7FFFFFFF
};

class CoastFoam : public Entity
{
  public:
    CoastFoam();
    ~CoastFoam();

    bool Init();
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    uint32_t AttributeChanged(ATTRIBUTES *pA);

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
    struct FoamVertex
    {
        CVECTOR vPos;
        uint32_t dwColor;
        float tu, tv;
    };

    struct FoamPart
    {
        CVECTOR v[2];
    };

    struct SubPart
    {
        // float tv;
        float fPos;
        float fSpeed;
        // float fA;
    };

    struct WorkPart
    {
        SubPart p[8];
        CVECTOR v[2];
        float tu;
        // FOAMMODE    Mode;
    };

    struct Foam
    {
        Foam()
        {
            iTexture = -1;
        }

        std::vector<FoamPart> aFoamParts;
        std::vector<WorkPart> aWorkParts;
        float fMove[2];
        float fSX[2];
        float fSpeed[2], fBraking[2];
        int32_t iMode[2];
        int32_t iTexture;
        std::string sTexture;
        int32_t iNumFoams;

        FOAMTYPE Type;

        float fAlphaColor[2], fAlpha[2], fAlphaMin, fAlphaMax;
        float fSpeedMin, fSpeedMax;
        float fBrakingMin, fBrakingMax;
        float fAppearMin, fAppearMax;
        float fTexScaleX;
    };

    int32_t iEditFoam, iEditFoamPart, iEditFoamVertex;
    bool bEditMode, bMoved, bCanEdit;
    float fCursorPosX, fCursorPosY;
    int32_t iFoamTexture, iCursorTex;
    SEA_BASE *pSea;
    std::vector<Foam *> aFoams;
    int32_t iVBuffer, iIBuffer;
    PLANE *pFrustumPlanes;

    float fMaxFoamDistance, fFoamDeltaY;
    int32_t iFoamDivides;

    uint32_t dwNumPenasExecuted;

    VDX9RENDER *rs;

    void ExtractRay(const D3DVIEWPORT9 &viewport, float fCursorX, float fCursorY, CVECTOR &raystart, CVECTOR &rayend);

    bool IsClipped(Foam *pF);

    void Save();
    void Load();
    void clear();

    void InitNewFoam(Foam *pF);
    void RecalculateFoam(int32_t iFoam);
    void ExecuteFoamType2(Foam *pF, float fDeltaTime);
    void ExecuteFoamType1(Foam *pF, float fDeltaTime);
};
