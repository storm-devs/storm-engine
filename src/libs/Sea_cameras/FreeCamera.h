#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Common_Camera.h"
#include "Island_Base.h"
#include "dx8render.h"
#include "vmodule_api.h"

class FREE_CAMERA : public COMMON_CAMERA
{
  private:
    VDX8RENDER *pRS;
    CVECTOR vPos, vAng;
    float fFov;
    long iLockX, iLockY;

    ISLAND_BASE *pIslandBase;
    BOOL bCameraOnEarth;
    float fCameraOnEarthHeight;

  public:
    FREE_CAMERA();
    virtual ~FREE_CAMERA();

    void SetDevice();
    bool Init();
    void Move(dword DeltaTime);
    void Execute(dword Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};

#endif