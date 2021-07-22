#pragma once

#include "Island_Base.h"
#include "dx9render.h"
#include "ship_base.h"
#include <vector>

#define D3DTLVERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

using D3DTLVERTEX = struct
{
    CVECTOR vPos;
    float fRHW;
    uint32_t dwDiffuse;
};

struct TOUCH_SHIP
{
    entid_t eID;                  // ship id
    CVECTOR vContourTemp[2][128]; // temporary storage / must be deleted
    TOUCH_PARAMS TP[2];           // touch params    for collision
    SHIP_BASE *pShip;             // ship pointer
    CVECTOR vKickPoint;           // temp
    CVECTOR vKickAngle;           // temp
    CVECTOR vContour[128];        // initial contour // must be dynamic
    CVECTOR vContourRect[4];      // rect contour
    long iNumVContour;            // num points in contour
};

class TOUCH : public Entity
{
  protected:
    VDX9RENDER *pRS;
    ISLAND_BASE *pIslandBase;

    bool bUseTouch;

    float fScale;
    float fCollisionDepth;

    uint32_t dwDeltaTime;
    long iDeltaTime;

    TOUCH_SHIP *pShips[256];
    long iNumShips;

    BOOL BuildContour(long ship_idx);
    BOOL IsPointInContour(CVECTOR *vP, CVECTOR *vContour, long numvcontour);

    bool IsSinked(long iIndex);

    float Touch(long idx, long skip_idx, CVECTOR *vPos, CVECTOR *vAng, float fPower, float fSlide);
    BOOL FakeTouch();
    BOOL IsIntersectShipsRects(long idx1, long idx2);
    BOOL IsIntersectShipsReal(long idx1, long cidx, CVECTOR *vPos, CVECTOR *vAng, CVECTOR *vRecoil, float *fPower,
                              float *fSlide);
    void GetLineABC(CVECTOR &v1, CVECTOR &v2, float &A, float &B, float &C);
    CVECTOR GetLineIntersectPoint(CVECTOR &v1, CVECTOR &v2, CVECTOR &o1, CVECTOR &o2);
    long ProcessImpulse(long iOurIdx, CVECTOR vPos, CVECTOR vDir, float fPowerApplied);
    long GetTouchPoint(long iIdx, const CVECTOR &vPos);

    void DrawLine(std::vector<RS_LINE2D> &aLines, float x1, float y1, float x2, float y2, uint32_t color);
    void DrawShips();
    CVECTOR GetPoint(float x, float y, float xx, float yy, float xscale, float yscale, float fCos, float fSin,
                     POINT ss);

    void SetDevices();

  public:
    ~TOUCH() override;
    TOUCH();
    void LoadServices();
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;

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
};
