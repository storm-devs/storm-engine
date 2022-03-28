#pragma once

#include "common_camera.h"
#include "island_base.h"
#include "dx9render.h"
#include "sea_base.h"

#define MIN_DIST 70.f
#define MAX_DIST 120.f
#define MIN_XANG PI / 6
#define MAX_XANG (PI / 2 + PI / 12)
#define CAMERA_HEIGHT 1.0f
#define CAMERA_ANGLE PI / 18

class SHIP_CAMERA : public COMMON_CAMERA
{
  private:
    float fMinHeightOnSea, fMaxHeightOnShip;
    float fDistance, fMaxDistance, fMinDistance;
    float fDistanceDlt, fDistanceInertia;
    float fMinAngleX, fMaxAngleX;
    float fAngleXDlt, fAngleXInertia;
    float fAngleYDlt, fAngleYInertia;
    float fSensivityDistance, fSensivityAzimuthAngle, fSensivityHeightAngle;
    float fSensivityHeightAngleOnShip;
    float fInvertMouseX, fInvertMouseY;
    CVECTOR vCenter, vAng = {};
    float fModelAy;

    // camera ellipsoid parameters
    float a = 1e-10f;
    float b = 1e-10f;
    float c = 1e-10f;

  private:
    SEA_BASE *pSea;
    ISLAND_BASE *pIsland;
    int32_t lIlsInitCnt;
    VDX9RENDER *pRS;

    void Move(float fDeltaTime);

    bool Init() override;
    void SetDevices();
    void Execute(uint32_t dwDeltaTime);
    void Realize(uint32_t dwDeltaTime) const;

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
        default: ;
        }
    }

    void SetCharacter(ATTRIBUTES *_pACharacter) override;

    uint32_t AttributeChanged(ATTRIBUTES *) override;
    void ShipsCollision(CVECTOR &pos);
    bool IslandCollision(CVECTOR &pos);

  public:
    SHIP_CAMERA();

    void Save(CSaveLoad *pSL) override;
    void Load(CSaveLoad *pSL) override;
};
