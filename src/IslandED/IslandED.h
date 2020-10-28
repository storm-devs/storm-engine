#ifndef IN_GAME_UTILS_ISLAND_ED_HPP
#define IN_GAME_UTILS_ISLAND_ED_HPP

#include "..\Sea_AI\AIFlowGraph.h"
#include "common_defines.h"
#include "dx8render.h"
#include "templates\array.h"
#include "templates\string.h"
#include "tga.h"
#include "vmodule_api.h"

#define INVALID_POINT_INDEX 0xFFFFFFFF

struct spoint
{
    CVECTOR vPos;
    dword dwIdx;
};

class IslandED : public ENTITY
{
  public:
    IslandED();
    ~IslandED();

    bool Init();
    void Execute(dword dwDeltaTime);
    void Realize(dword dwDeltaTime);

  private:
    AIFlowGraph::Path *pPath;
    AIFlowGraph Graph;

    array<spoint> aPoints;
    bool bCursorVisible, bCanDoPoint, bCanDoGraf;
    bool bPathFirstPnt, bCanDoPath;

    INIFILE *pIni;

    bool bFirstExecute;
    bool bTrace;
    CVECTOR pTrace1, pTrace2;
    CVECTOR vBoxCenter, vBoxSize;

    dword dwFirstPointIdx;
    RECT SR;
    float fCenterX, fCenterY, fX, fY, fScale;
    long iLockX, iLockY;
    bool bLoaded;
    TGA_H Header;
    BYTE *pRaw;
    IDirect3DTexture8 *pIslandTexture;
    VDX8RENDER *pRS;

    dword GetNearestPointIdx(CVECTOR vSrc);
    float Trace(spoint *p1, spoint *p2);
    bool CheckIslandPoint(CVECTOR &vPos);
    void OpenNewFile();
    CVECTOR GetScreenPos(CVECTOR &vRealPos);
    CVECTOR GetRealPos(CVECTOR &vScrPos);
};

#endif