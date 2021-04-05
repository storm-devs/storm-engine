#include "ISLAND.h"
#include "../../Shared/messages.h"
#include "../../Shared/sea_ai/Script_defines.h"
#include "Foam.h"
#include "Weather_Base.h"
#include "filesystem.h"
#include "inlines.h"
#include "tga.h"
#include <cstdio>

#define TGA_DATA_CHUNK 0xC001F00D

#define HMAP_EMPTY 0
#define HMAP_START 2.0f
#define HMAP_NUMBERS (255.0f - HMAP_START)
#define HMAP_MAXHEIGHT -20.0f

#define SEA_BED_NODE_NAME "seabed"

#define DMAP_SIZE 2048

ISLAND::ISLAND()
{
    bForeignModels = false;
    pRS = nullptr;
    pGS = nullptr;
    pDepthMap = nullptr;
    pShadowMap = nullptr;
    bDrawReflections = false;

    fCurrentImmersion = 0.0f;

    ZERO(AIFortEID);
}

ISLAND::~ISLAND()
{
    Uninit();
}

void ISLAND::Uninit()
{
    for (uint32_t i = 0; i < aSpheres.size(); i++)
        EntityManager::EraseEntity(aSpheres[i]);
    aSpheres.clear();
    STORM_DELETE(pDepthMap);
    STORM_DELETE(pShadowMap);

    if (!bForeignModels)
    {
        EntityManager::EraseEntity(model_id);
        EntityManager::EraseEntity(seabed_id);
    }
}

bool ISLAND::Init()
{
    // EntityManager::AddToLayer("system_messages", GetId(), 1);
    SetDevice();

    // calc optimization
    for (uint32_t i = 0; i < 256; i++)
    {
        fDepthHeight[i] = static_cast<float>(HMAP_MAXHEIGHT / HMAP_NUMBERS * (static_cast<float>(i) - HMAP_START));
        if (i == HMAP_EMPTY)
            fDepthHeight[i] = HMAP_MAXHEIGHT;
    }

    return true;
}

void ISLAND::SetDevice()
{
    // core.LayerCreate("island_trace", true, false);

    pCollide = static_cast<COLLIDE *>(core.CreateService("COLL"));
    Assert(pCollide);
    pRS = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    Assert(pRS);
    pGS = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    Assert(pGS);
}

bool bView = false;

void ISLAND::Realize(uint32_t Delta_Time)
{
    uint32_t dwAmbient, dwAmbientOld;

    if (bForeignModels)
        return;

    auto *pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id));
    Assert(pModel);

    uint32_t bFogEnable;
    uint32_t bLighting;
    pRS->GetRenderState(D3DRS_FOGENABLE, &bFogEnable);
    pRS->GetRenderState(D3DRS_AMBIENT, &dwAmbientOld);
    pRS->GetRenderState(D3DRS_LIGHTING, &bLighting);

    pRS->SetRenderState(D3DRS_LIGHTING, false);
    dwAmbient = dwAmbientOld & 0xFF;

    CVECTOR vCamPos, vCamAng;
    float fOldNear, fOldFar, fPerspective;
    pRS->GetCamera(vCamPos, vCamAng, fPerspective);
    pRS->GetNearFarPlane(fOldNear, fOldFar);

    auto fRadius = sqrtf(Sqr(vBoxSize.x / 2.0f) + Sqr(vBoxSize.z / 2.0f));
    auto fCamDistance = sqrtf(~(vCamPos - vBoxCenter));
    auto fMaxDistance = fCamDistance + fRadius;

    fCurrentImmersion = 0.0f;
    if (fCamDistance > fImmersionDistance)
        fCurrentImmersion = (fCamDistance / fImmersionDistance - 1.0f) * fImmersionDepth;

    CMatrix mTemp;
    mTemp.BuildPosition(0.0f, -fCurrentImmersion, 0.0f);
    pModel->mtx = mIslandOld * mTemp;

    float fOldFogDensity;
    float fIslandFogDensity;

    fIslandFogDensity = AttributesPointer->GetAttributeAsFloat("FogDensity", 0.0f);

    if (aForts.size() && !AIFortEID) //~!@
    {
        AIFortEID = EntityManager::GetEntityId("AIFort");
    }

    pRS->GetRenderState(D3DRS_FOGDENSITY, (uint32_t *)&fOldFogDensity);
    pRS->SetRenderState(D3DRS_FOGDENSITY, F2DW(fIslandFogDensity));
    long j;
    for (j = static_cast<long>(fMaxDistance / fOldFar); j >= 0; j--)
    {
        if (j != 0)
            pRS->SetRenderState(D3DRS_ZWRITEENABLE, false);
        pRS->SetNearFarPlane((j == 0) ? fOldNear : fOldFar * static_cast<float>(j),
                             fOldFar * static_cast<float>(j + 1));
        pModel->ProcessStage(Stage::realize, Delta_Time);
        pRS->SetRenderState(D3DRS_LIGHTING, true);
        D3DLIGHT9 lt, ltold;
        ZERO(lt);
        lt.Type = D3DLIGHT_POINT;
        lt.Diffuse.a = 0.0f;
        lt.Diffuse.r = 1.0f;
        lt.Diffuse.g = 1.0f;
        lt.Diffuse.b = 1.0;
        lt.Ambient.r = 1.0f;
        lt.Ambient.g = 1.0f;
        lt.Ambient.b = 1.0f;
        lt.Specular.r = 1.0f;
        lt.Specular.g = 1.0f;
        lt.Specular.b = 1.0f;
        lt.Position.x = 0.0f;
        lt.Position.y = 0.0f;
        lt.Position.z = 0.0f;
        lt.Attenuation0 = 1.0f;
        lt.Range = 1e9f;
        pRS->GetLight(0, &ltold);
        pRS->SetLight(0, &lt);
        for (uint32_t k = 0; k < aForts.size(); k++)
        {
            auto *const ent = EntityManager::GetEntityPointer(aForts[k]);
            auto mOld = static_cast<MODEL *>(ent)->mtx;
            static_cast<MODEL *>(ent)->mtx = mOld * mTemp;

            core.Send_Message(AIFortEID, "li", AI_MESSAGE_FORT_SET_LIGHTS, aForts[k]);
            static_cast<Entity *>(ent)->ProcessStage(Stage::realize, Delta_Time);
            core.Send_Message(AIFortEID, "li", AI_MESSAGE_FORT_UNSET_LIGHTS, aForts[k]);

            static_cast<MODEL *>(ent)->mtx = mOld;
        }
        pRS->SetLight(0, &ltold);
        pRS->SetRenderState(D3DRS_LIGHTING, false);
        pRS->SetRenderState(D3DRS_ZWRITEENABLE, true);
    }
    pRS->SetRenderState(D3DRS_FOGDENSITY, F2DW(fOldFogDensity));

    pRS->SetNearFarPlane(fOldNear, fOldFar / 2.0f);
    if (!bDrawReflections)
    {
        pRS->SetRenderState(D3DRS_FOGENABLE, false);
        // pRS->SetRenderState(D3DRS_AMBIENT, RGB(dwAmbient/4,dwAmbient/4,dwAmbient/4));

        auto *pSeaBed = static_cast<MODEL *>(EntityManager::GetEntityPointer(seabed_id));
        if (pSeaBed)
            pSeaBed->ProcessStage(Stage::realize, Delta_Time);
    }

    pRS->SetNearFarPlane(fOldNear, fOldFar);

    pRS->SetRenderState(D3DRS_LIGHTING, false);
    pRS->SetRenderState(D3DRS_FOGENABLE, bFogEnable);
    pRS->SetRenderState(D3DRS_AMBIENT, dwAmbientOld);
    pRS->SetRenderState(D3DRS_LIGHTING, bLighting);

    uint32_t i;
    for (i = 0; i < aSpheres.size(); i++)
    {
        auto *pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(aSpheres[i]));
        auto vPos = AIPath.GetPointPos(i);
        if (pModel)
            pModel->mtx.BuildPosition(vPos.x, 5.0f, vPos.z);
    }

    if (core.Controls->GetDebugAsyncKeyState('O') < 0)
        bView ^= 1;
    if (bView)
    {
        std::vector<RS_LINE> aLines;
        for (i = 0; i < AIPath.GetNumEdges(); i++)
        {
            AIFlowGraph::edge_t *pE = AIPath.GetEdge(i);
            aLines.push_back(RS_LINE{AIPath.GetPointPos(pE->dw1), 0xFFFFFF});
            aLines.push_back(RS_LINE{AIPath.GetPointPos(pE->dw2), 0xFFFFFF});
        }
        CMatrix m;
        pRS->SetTransform(D3DTS_WORLD, m);
        pRS->DrawLines(&aLines[0], aLines.size() / 2, "Line");
    }
}

bool ISLAND::GetDepth(FRECT *pRect, float *fMinH, float *fMaxH)
{
    return false;
}

uint64_t ISLAND::ProcessMessage(MESSAGE &message)
{
    entid_t eID;
    char str[256], idstr[256];
    switch (message.Long())
    {
    case MSG_ISLAND_ADD_FORT:
        aForts.push_back(message.EntityID());
        break;
    case MSG_LOCATION_ADD_MODEL:
        eID = message.EntityID();
        message.String(sizeof(idstr) - 1, idstr);
        message.String(sizeof(str) - 1, str);
        AddLocationModel(eID, idstr, str);
        break;
    case MSG_ISLAND_LOAD_GEO: // from sea
        message.String(sizeof(cFoamDir) - 1, cFoamDir);
        message.String(sizeof(cModelsDir) - 1, cModelsDir);
        message.String(sizeof(cModelsID) - 1, cModelsID);
        Mount(cModelsID, cModelsDir, nullptr);
        CreateHeightMap(cFoamDir, cModelsID);
        // CreateShadowMap(cModelsDir, cModelsID);
        break;
    case MSG_ISLAND_START: // from location
        CreateHeightMap(cModelsDir, cModelsID);
        // CreateShadowMap(cModelsDir, cModelsID);
        break;
    case MSG_SEA_REFLECTION_DRAW:
        bDrawReflections = true;
        Realize(0);
        bDrawReflections = false;
        break;
    case MSG_MODEL_SET_MAX_VIEW_DIST:
        core.Send_Message(model_id, "lf", MSG_MODEL_SET_MAX_VIEW_DIST, message.Float());
        break;
    }
    return 1;
}

inline float ISLAND::GetShadowTemp(long iX, long iZ)
{
    if (iX >= 0 && iX < DMAP_SIZE && iZ >= 0 && iZ < DMAP_SIZE)
        return static_cast<float>(mzShadow.Get(iX, iZ)) / 255.0f;
    return 1.0f;
}

bool ISLAND::GetShadow(float x, float z, float *fRes)
{
    const float fX = (x - vBoxCenter.x) / fShadowMapStep;
    const float fZ = (z - vBoxCenter.z) / fShadowMapStep;

    *fRes = GetShadowTemp(ftoi(fX) + mzShadow.GetSizeX() / 2, ftoi(fZ) + mzShadow.GetSizeX() / 2);

    return true;
}

void ISLAND::AddLocationModel(entid_t eid, char *pIDStr, char *pDir)
{
    Assert(pDir && pIDStr);
    bForeignModels = true;
    strcpy_s(cModelsDir, pDir);
    strcpy_s(cModelsID, pIDStr);
    EntityManager::AddToLayer(ISLAND_TRACE, eid, 10);
}

inline float ISLAND::GetDepthNoCheck(uint32_t iX, uint32_t iZ)
{
    // if (!mzDepth.isLoaded()) return HMAP_MAXHEIGHT;

    /*BYTE byHeight = mzDepth.Get(iX,iZ);
    if (byHeight == HMAP_EMPTY) return HMAP_MAXHEIGHT;
    float fHeight = float(HMAP_MAXHEIGHT / HMAP_NUMBERS * (float(byHeight) - HMAP_START) );
    return fHeight;*/

    return fDepthHeight[mzDepth.Get(iX, iZ)];
}

inline float ISLAND::GetDepthCheck(uint32_t iX, uint32_t iZ)
{
    // if (iX>=iDMapSize) iX = iDMapSize - 1;
    // if (iZ>=iDMapSize) iZ = iDMapSize - 1;
    return GetDepthNoCheck(iX, iZ);
}

bool ISLAND::Check2DBoxDepth(CVECTOR vPos, CVECTOR vSize, float fAngY, float fMinDepth)
{
    // if (!mzDepth.isLoaded()) return false;

    const float fCos = cosf(fAngY), fSin = sinf(fAngY);
    for (float z = -vSize.z / 2.0f; z < vSize.z / 2.0f; z += fStepDZ)
        for (float x = -vSize.x / 2.0f; x < vSize.x / 2.0f; x += fStepDX)
        {
            float fDepth, xx = x, zz = z;
            RotateAroundY(xx, zz, fCos, fSin);
            GetDepthFast(vPos.x + xx, vPos.z + zz, &fDepth);
            if (fDepth > fMinDepth)
                return true;
        }
    return false;
}

bool ISLAND::GetDepthFast(float x, float z, float *fRes)
{
    // if (!mzDepth.isLoaded()) { if (fRes) *fRes = -50.0f; return false; }
    x -= vBoxCenter.x;
    z -= vBoxCenter.z;
    if (fabsf(x) >= vRealBoxSize.x || fabsf(z) >= vRealBoxSize.z)
    // if (x < -vRealBoxSize.x || z < -vRealBoxSize.z || x > vRealBoxSize.x || z > vRealBoxSize.z)
    {
        *fRes = -50.0f;
        return false;
    }

    const float fX = (x * fStep1divDX) + static_cast<float>(iDMapSize >> 1);
    const float fZ = (z * fStep1divDZ) + static_cast<float>(iDMapSize >> 1);

    *fRes = GetDepthNoCheck(ftoi(fX), ftoi(fZ));
    return true;
}

bool ISLAND::GetDepth(float x, float z, float *fRes)
{
    // if (!mzDepth.isLoaded()) {if (fRes) *fRes = -50.0f;    return false; }
    x -= vBoxCenter.x;
    z -= vBoxCenter.z;
    if (fabsf(x) >= vRealBoxSize.x || fabsf(z) >= vRealBoxSize.z)
    // if (x < -vRealBoxSize.x || z < -vRealBoxSize.z || x > vRealBoxSize.x || z > vRealBoxSize.z)
    {
        *fRes = -50.0f;
        return false;
    }

    const float fX = (x * fStep1divDX) + static_cast<float>(iDMapSize >> 1);
    const float fZ = (z * fStep1divDZ) + static_cast<float>(iDMapSize >> 1);

    *fRes = GetDepthNoCheck(ftoi(fX), ftoi(fZ));

    return true;
}

bool ISLAND::ActivateCamomileTrace(CVECTOR &vSrc)
{
    const float fRadius = 100.0f;
    const long iNumPetal = 8;
    long iNumInner = 0;

    for (long i = 0; i < iNumPetal; i++)
    {
        TRIANGLE trg;
        CVECTOR vDst, vCross;
        float fAng, fCos, fSin, fRes;

        fAng = static_cast<float>(i) / static_cast<float>(iNumPetal) * PIm2;
        fCos = cosf(fAng);
        fSin = sinf(fAng);

        vDst = vSrc + CVECTOR(fCos * fRadius, 0.0f, fSin * fRadius);
        fRes = Trace(vSrc, vDst);
        if (fRes > 1.0f)
            continue;
        auto *pEnt = static_cast<MODEL *>(EntityManager::GetEntityPointer(pCollide->GetObjectID()));
        Assert(pEnt);
        pEnt->GetCollideTriangle(trg);
        vCross = !((trg.vrt[1] - trg.vrt[0]) ^ (trg.vrt[2] - trg.vrt[0]));
        fRes = vCross | (!(vDst - vSrc));
        if (fRes > 0.0f)
            iNumInner++;
        if (iNumInner > 1)
            return true;
    }

    return false;
}

void ISLAND::CalcBoxParameters(CVECTOR &_vBoxCenter, CVECTOR &_vBoxSize)
{
    GEOS::INFO ginfo;
    float x1 = 1e+8f, x2 = -1e+8f, z1 = 1e+8f, z2 = -1e+8f;

    const auto its = EntityManager::GetEntityIdIterators(ISLAND_TRACE);
    for (auto it = its.first; it != its.second; ++it)
    {
        MODEL *pM = static_cast<MODEL *>(EntityManager::GetEntityPointer(it->second));
        if (pM == nullptr)
            continue;

        uint32_t i = 0;
        while (true)
        {
            NODE *pN = pM->GetNode(i);
            i++;
            if (!pN)
                break;
            pN->geo->GetInfo(ginfo);
            CVECTOR vGlobPos = pN->glob_mtx.Pos();
            const CVECTOR vBC = vGlobPos + CVECTOR(ginfo.boxcenter.x, 0.0f, ginfo.boxcenter.z);
            const CVECTOR vBS = CVECTOR(ginfo.boxsize.x, 0.0f, ginfo.boxsize.z) / 2.0f;
            if (vBC.x - vBS.x < x1)
                x1 = vBC.x - vBS.x;
            if (vBC.x + vBS.x > x2)
                x2 = vBC.x + vBS.x;
            if (vBC.z - vBS.z < z1)
                z1 = vBC.z - vBS.z;
            if (vBC.z + vBS.z > z2)
                z2 = vBC.z + vBS.z;
        }
    }
    _vBoxCenter = CVECTOR((x1 + x2) / 2.0f, 0.0f, (z1 + z2) / 2.0f);
    _vBoxSize = CVECTOR(x2 - x1, 0.0f, z2 - z1);
}

void ISLAND::CreateDirectories(char *pDir)
{
    char sCurDir[256], sTemp[256];

    fio->_GetCurrentDirectory(sizeof(sCurDir), sCurDir);
    if (strlen(sCurDir) && sCurDir[strlen(sCurDir) - 1] != '\\')
        strcat_s(sCurDir, "\\");

    char *pLast, *pStr;

    pLast = pStr = pDir;
    while (true)
    {
        pStr = strchr(pStr, '\\');
        if (!pStr)
            break;
        strncpy_s(sTemp, pLast, pStr - pLast);
        sTemp[pStr - pLast] = '\0';
        pLast = ++pStr;
        strcat_s(sCurDir, sTemp);
        strcat_s(sCurDir, "\\");
        BOOL bOk = fio->_CreateDirectory(sCurDir, nullptr);
    }
}

bool ISLAND::CreateShadowMap(char *pDir, char *pName)
{
    const std::string sDir;
    auto *const pWeather =
        static_cast<WEATHER_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("Weather")));
    if (pWeather == nullptr)
    {
        throw std::exception("No found WEATHER entity!");
    }

    const fs::path path = fs::path() / "resource" / "foam" / pDir / AttributesPointer->GetAttribute("LightingPath");
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~
    // sDir.Format("resource\\foam\\%s\\%s\\", pDir, AttributesPointer->GetAttribute("LightingPath")); sDir.CheckPath();
    // sprintf_s(fname, "%s%s.tga", (const char*)sDir.c_str(), pName);
    const std::string fileName = path.string() + pName + ".tga";

    CreateDirectories((char *)sDir.c_str());

    fShadowMapSize = 2.0f * Max(vRealBoxSize.x, vRealBoxSize.z) + 1024.0f;
    fShadowMapStep = fShadowMapSize / DMAP_SIZE;

    if (mzShadow.Load(fileName + ".zap"))
    {
        return true;
    }

    // try to load tga file
    auto fileS = fio->_CreateFile(fileName.c_str(), std::ios::binary | std::ios::in);
    if (fileS.is_open())
    {
        TGA_H tga_head;

        fio->_ReadFile(fileS, &tga_head, sizeof(tga_head));
        const uint32_t dwSize = tga_head.width;
        pShadowMap = new uint8_t[dwSize * dwSize];
        fio->_ReadFile(fileS, pShadowMap, dwSize * dwSize);
        fio->_CloseFile(fileS);

        mzShadow.DoZip(pShadowMap, dwSize);
        mzShadow.Save(fileName + ".zap");
        STORM_DELETE(pShadowMap);
        return true;
    }

    pShadowMap = new uint8_t[DMAP_SIZE * DMAP_SIZE];

    float fX, fZ;
    uint32_t x, z;
    CVECTOR vSun;
    pWeather->GetVector(whv_sun_pos, &vSun); // CVECTOR(15000.0f, 2000.0f, 15000.0f);
    vSun = 100000.0f * !vSun;

    for (fZ = -fShadowMapSize / 2.0f, z = 0; fZ < fShadowMapSize / 2.0f; fZ += fShadowMapStep, z++)
        for (fX = -fShadowMapSize / 2.0f, x = 0; fX < fShadowMapSize / 2.0f; fX += fShadowMapStep, x++)
        {
            // uint32_t x = uint32_t((fX + fShadowMapSize / 2.0f) / fShadowMapStep);
            // uint32_t z = uint32_t((fZ + fShadowMapSize / 2.0f) / fShadowMapStep);
            // if (x == 774) _asm int 3
            CVECTOR vSrc = CVECTOR(fX, 2.0f, fZ) + vBoxCenter;
            CVECTOR vDst = vSrc + vSun;
            float fRes = Trace(vSrc, vDst);
            pShadowMap[x + z * DMAP_SIZE] = 255;
            if (fRes <= 1.0f)
            {
                const float fLen = sqrtf(~(vSun - vSrc)) * fRes;
                const float fValue = Bring2Range(50.0f, 254.0f, 0.0f, 500.0f, fLen);
                pShadowMap[x + z * DMAP_SIZE] = static_cast<uint8_t>(fValue);
            }
            if (GetDepthFast(vSrc.x, vSrc.z, &fRes))
            {
                if (fRes >= -0.001f)
                {
                    pShadowMap[x + z * DMAP_SIZE] = 0;
                }
            }
        }

    SaveTga8((char *)fileName.c_str(), pShadowMap, DMAP_SIZE, DMAP_SIZE);

    Blur8(&pShadowMap, DMAP_SIZE);
    Blur8(&pShadowMap, DMAP_SIZE);
    Blur8(&pShadowMap, DMAP_SIZE);
    Blur8(&pShadowMap, DMAP_SIZE);

    mzShadow.DoZip(pShadowMap, DMAP_SIZE);
    mzShadow.Save(fileName + ".zap");

    STORM_DELETE(pShadowMap);

    return true;
}

void ISLAND::Blur8(uint8_t **pBuffer, uint32_t dwSize)
{
    uint32_t x, z;
    uint8_t *pNewBuffer = new uint8_t[dwSize * dwSize];
    const uint32_t dwMask = dwSize - 1;
    // do blur for shadow map
    for (z = 0; z < dwSize; z++)
        for (x = 0; x < dwSize; x++)
        {
            uint32_t dwRes = (*pBuffer)[z * dwSize + x];
            dwRes += (*pBuffer)[((z + 0) & dwMask) * dwSize + ((x + 1) & dwMask)];
            dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x + 1) & dwMask)];
            dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x + 0) & dwMask)];
            dwRes += (*pBuffer)[((z + 1) & dwMask) * dwSize + ((x - 1) & dwMask)];
            dwRes += (*pBuffer)[((z + 0) & dwMask) * dwSize + ((x - 1) & dwMask)];
            dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x - 1) & dwMask)];
            dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x + 0) & dwMask)];
            dwRes += (*pBuffer)[((z - 1) & dwMask) * dwSize + ((x + 1) & dwMask)];
            dwRes /= 9;

            pNewBuffer[z * dwSize + x] = static_cast<uint8_t>(dwRes);
        }

    STORM_DELETE(*pBuffer);
    *pBuffer = pNewBuffer;
}

bool ISLAND::CreateHeightMap(char *pDir, char *pName)
{
    TGA_H tga_head;
    std::string sDir;
    char str_tmp[256];

    fs::path path = fs::path() / "resource" / "foam" / pDir / pName;
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~
    std::string fileName = path.string() + ".tga";
    std::string iniName = path.string() + ".ini";
    // sDir.Format("resource\\\\foam\\\\%s\\\\", pDir); sDir.CheckPath();
    // sprintf_s(fname, "%s%s.tga", (const char*)sDir.c_str(), pName);
    // sprintf_s(iname, "%s%s.ini", (const char*)sDir.c_str(), pName);

    CreateDirectories((char *)sDir.c_str());

    // calc center and size
    CalcBoxParameters(vBoxCenter, vRealBoxSize);
    vBoxSize = vRealBoxSize + CVECTOR(50.0f, 0.0f, 50.0f);

    rIsland.x1 = vBoxCenter.x - vBoxSize.x / 2.0f;
    rIsland.y1 = vBoxCenter.z - vBoxSize.z / 2.0f;
    rIsland.x2 = vBoxCenter.x + vBoxSize.x / 2.0f;
    rIsland.y2 = vBoxCenter.z + vBoxSize.z / 2.0f;

    bool bLoad = mzDepth.Load(fileName + ".zap");

    if (!bLoad)
    {
        auto fileS = fio->_CreateFile(fileName.c_str(), std::ios::binary | std::ios::in);
        if (fileS.is_open())
        {
            fio->_ReadFile(fileS, &tga_head, sizeof(tga_head));
            iDMapSize = tga_head.width;
            pDepthMap = new uint8_t[iDMapSize * iDMapSize];
            fio->_ReadFile(fileS, pDepthMap, iDMapSize * iDMapSize);
            fio->_CloseFile(fileS);

            mzDepth.DoZip(pDepthMap, iDMapSize);
            mzDepth.Save(fileName + ".zap");
            STORM_DELETE(pDepthMap);
            bLoad = true;
        }
    }

    if (bLoad)
    {
        iDMapSize = mzDepth.GetSizeX();
        for (iDMapSizeShift = 0; iDMapSizeShift < 30; iDMapSizeShift++)
            if (static_cast<uint32_t>(1 << iDMapSizeShift) == iDMapSize)
                break;
        fStepDX = vBoxSize.x / static_cast<float>(iDMapSize);
        fStepDZ = vBoxSize.z / static_cast<float>(iDMapSize);

        fStep1divDX = 1.0f / fStepDX;
        fStep1divDZ = 1.0f / fStepDZ;

        vBoxSize /= 2.0f;
        vRealBoxSize /= 2.0f;

        auto pI = fio->OpenIniFile(iniName.c_str());
        Assert(pI.get());

        CVECTOR vTmpBoxCenter, vTmpBoxSize;
        pI->ReadString("Main", "vBoxCenter", str_tmp, sizeof(str_tmp) - 1, "1.0,1.0,1.0");
        sscanf(str_tmp, "%f,%f,%f", &vTmpBoxCenter.x, &vTmpBoxCenter.y, &vTmpBoxCenter.z);
        pI->ReadString("Main", "vBoxSize", str_tmp, sizeof(str_tmp) - 1, "1.0,1.0,1.0");
        sscanf(str_tmp, "%f,%f,%f", &vTmpBoxSize.x, &vTmpBoxSize.y, &vTmpBoxSize.z);
        if (~(vTmpBoxCenter - vBoxCenter) > 0.1f)
        {
            core.Trace("Island: vBoxCenter not equal, foam error: %s, distance = %.3f", iniName.c_str(),
                       sqrtf(~(vTmpBoxCenter - vBoxCenter)));
            core.Trace("vBoxCenter = %f,%f,%f", vBoxCenter.x, vBoxCenter.y, vBoxCenter.z);
        }
        if (~(vTmpBoxSize - vBoxSize) > 0.1f)
        {
            core.Trace("Island: vBoxSize not equal, foam error: %s", iniName.c_str());
            core.Trace("vBoxSize = %f,%f,%f", vBoxSize.x, vBoxSize.y, vBoxSize.z);
        }

        AIPath.Load(*pI);
        AIPath.BuildTable();

        return true;
    }

    core.Trace("WARN: FOAM: Can't find foam: %s", fileName.c_str());

    long iTestSize = static_cast<long>(vBoxSize.x / 1.5f);
    // fixed maximum depth map to 1024 size!!!!!!!
    iDMapSizeShift = 11;
    // for (iDMapSizeShift=0;iDMapSizeShift<10;iDMapSizeShift++) if ((1<<iDMapSizeShift) >= iTestSize) break;
    iDMapSize = (1 << iDMapSizeShift);

    fStepDX = vBoxSize.x / static_cast<float>(iDMapSize);
    fStepDZ = vBoxSize.z / static_cast<float>(iDMapSize);

    fStep1divDX = 1.0f / fStepDX;
    fStep1divDZ = 1.0f / fStepDZ;

    pDepthMap = new uint8_t[iDMapSize * iDMapSize];

    float fEarthPercent = 0.0f;
    float fX, fZ;

    for (fZ = 0; fZ < static_cast<float>(iDMapSize); fZ += 1.0f)
    {
        if ((static_cast<long>(fZ) & 127) == 127)
            core.Trace("Z = %.0f", fZ);
        for (fX = 0; fX < static_cast<float>(iDMapSize); fX += 1.0f)
        {
            long iIdx = static_cast<long>(fX) + static_cast<long>(fZ) * iDMapSize;
            pDepthMap[iIdx] = 255;
            float fXX = (fX - static_cast<float>(iDMapSize) / 2.0f) * fStepDX;
            float fZZ = (fZ - static_cast<float>(iDMapSize) / 2.0f) * fStepDZ;
            CVECTOR vSrc(fXX, 0.0f, fZZ), vDst(fXX, -500.0f, fZZ + 0.001f);
            vSrc += vBoxCenter;
            vDst += vBoxCenter;
            float fRes = Trace(vSrc, vDst);
            Assert(_isnan(fRes) == false);
            if (fRes <= 1.0f) // island ocean floor exist
            {
                float fHeight = sqrtf(~(fRes * (vDst - vSrc)));
                if (fHeight > -HMAP_MAXHEIGHT)
                {
                    fHeight = -HMAP_MAXHEIGHT;
                }
                // Activate camomile trace!
                if (ActivateCamomileTrace(vSrc))
                    pDepthMap[iIdx] = static_cast<uint8_t>(HMAP_START);
                else
                    pDepthMap[iIdx] = static_cast<unsigned char>(
                        (HMAP_START + static_cast<float>(HMAP_NUMBERS) * fHeight / -HMAP_MAXHEIGHT));
            }
            else // check for up direction
            {
                vSrc = CVECTOR(fXX, 0.0f, fZZ);
                vDst = CVECTOR(fXX, 1500.0f, fZZ + 0.001f);
                vSrc += vBoxCenter;
                vDst += vBoxCenter;
                float fRes = Trace(vSrc, vDst);
                if (fRes <= 1.0f || ActivateCamomileTrace(vSrc))
                {
                    pDepthMap[iIdx] = static_cast<uint8_t>(HMAP_START);
                }
            }
            if (pDepthMap[iIdx] == static_cast<uint8_t>(HMAP_START))
                fEarthPercent += 1.0f;
        }
    }

    vBoxSize /= 2.0f;
    vRealBoxSize /= 2.0f;

    SaveTga8((char *)fileName.c_str(), pDepthMap, iDMapSize, iDMapSize);

    mzDepth.DoZip(pDepthMap, iDMapSize);
    mzDepth.Save(fileName + ".zap");
    STORM_DELETE(pDepthMap);

    auto pI = fio->OpenIniFile(iniName.c_str());
    if (!pI)
    {
        pI = fio->CreateIniFile(iniName.c_str(), false);
        Assert(pI.get());
    }
    char str[512];
    pI->WriteString("Main", "DepthFile", (char *)fileName.c_str());
    sprintf_s(str, "%f,%f,%f", vBoxCenter.x, vBoxCenter.y, vBoxCenter.z);
    pI->WriteString("Main", "vBoxCenter", str);
    sprintf_s(str, "%f,%f,%f", vBoxSize.x, vBoxSize.y, vBoxSize.z);
    pI->WriteString("Main", "vBoxSize", str);

    return true;
}

bool ISLAND::SaveTga8(char *fname, uint8_t *pBuffer, uint32_t dwSizeX, uint32_t dwSizeY)
{
    TGA_H tga_head;

    ZERO(tga_head);

    tga_head.type = 3;
    tga_head.width = static_cast<uint16_t>(dwSizeX);
    tga_head.height = static_cast<uint16_t>(dwSizeY);
    tga_head.bpp = 8;
    tga_head.attr8 = 8;

    auto fileS = fio->_CreateFile(fname, std::ios::binary | std::ios::out);
    if (!fileS.is_open())
    {
        core.Trace("Island: Can't create island file! %s", fname);
        return false;
    }
    fio->_WriteFile(fileS, &tga_head, sizeof(tga_head));
    fio->_WriteFile(fileS, pBuffer, dwSizeX * dwSizeY);
    fio->_CloseFile(fileS);

    return true;
}

bool ISLAND::Mount(char *fname, char *fdir, entid_t *eID)
{
    // std::string        sRealFileName;
    // std::string sModelPath, sLightPath;

    Uninit();

    SetName(fname);

    const fs::path path = fs::path() / fdir / fname;
    const std::string pathStr = path.string();
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~
    // sRealFileName.Format("%s\\%s", fdir, fname); sRealFileName.CheckPath();

    model_id = EntityManager::CreateEntity("MODELR");
    core.Send_Message(model_id, "ls", MSG_MODEL_SET_LIGHT_PATH, AttributesPointer->GetAttribute("LightingPath"));
    core.Send_Message(model_id, "ls", MSG_MODEL_LOAD_GEO, (char *)pathStr.c_str());

    // extract subobject(sea_bed) to another model
    auto *pModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(model_id));
    NODE *pNode = pModel->FindNode(SEA_BED_NODE_NAME);
    if (pNode)
        seabed_id = pNode->Unlink2Model();
    else
        core.Trace("Island: island %s has no sea bed, check me!", fname);

    EntityManager::AddToLayer(ISLAND_TRACE, model_id, 10);
    EntityManager::AddToLayer(ISLAND_TRACE, seabed_id, 10);
    EntityManager::AddToLayer(RAIN_DROPS, model_id, 100);

    auto *pSeaBedModel = static_cast<MODEL *>(EntityManager::GetEntityPointer(seabed_id));

    mIslandOld = pModel->mtx;
    if (pSeaBedModel)
        mSeaBedOld = pSeaBedModel->mtx;

    /*sModelPath.Format("islands\\%s\\", fname); sModelPath.CheckPath();
    core.Send_Message(lighter_id, "ss", "ModelsPath", (char*)sModelPath);
    sLightPath.Format("%s", AttributesPointer->GetAttribute("LightingPath")); sLightPath.CheckPath();
    core.Send_Message(lighter_id, "ss", "LightPath", (char*)sLightPath);*/

    const auto lighter_id = EntityManager::GetEntityId("lighter");
    core.Send_Message(lighter_id, "ssi", "AddModel", fname, model_id);
    const std::string sSeaBedName = std::string(fname) + "_seabed";
    core.Send_Message(lighter_id, "ssi", "AddModel", (char *)sSeaBedName.c_str(), seabed_id);

    fImmersionDistance = AttributesPointer->GetAttributeAsFloat("ImmersionDistance", 3000.0f);
    fImmersionDepth = AttributesPointer->GetAttributeAsFloat("ImmersionDepth", 25.0f);

    // CreateHeightMap(fname);

    // load geometry for reflection and add it to reflection layer

    /*for (uint32_t i=0;i<AIPath.GetNumPoints();i++)
    {
      entid_t eid;
      EntityManager::CreateEntity(&eid,"MODELR");
      core.Send_Message(eid,"ls",MSG_MODEL_LOAD_GEO,"mirror");
      EntityManager::AddToLayer("sea_realize",eid,10000);
      aSpheres.Add(eid);
    }*/

    // AIFlowGraph::Path * pPath = FindPath(CVECTOR(-10000.0f,0.0f,-10000.0f),CVECTOR(10000.0f,0.0f,10000.0f));
    // STORM_DELETE(pPath);

    return true;
}

float ISLAND::Cannon_Trace(long iBallOwner, const CVECTOR &vSrc, const CVECTOR &vDst)
{
    const float fRes = Trace(vSrc, vDst);
    if (fRes <= 1.0f)
    {
        const CVECTOR vTemp = vSrc + fRes * (vDst - vSrc);
        core.Event(BALL_ISLAND_HIT, "lfff", iBallOwner, vTemp.x, vTemp.y, vTemp.z);
    }
    return fRes;
}

float ISLAND::Trace(const CVECTOR &vSrc, const CVECTOR &vDst)
{
    return pCollide->Trace(EntityManager::GetEntityIdIterators(ISLAND_TRACE), vSrc, vDst, nullptr, 0);
}

// Path section
bool ISLAND::GetMovePoint(CVECTOR &vSrc, CVECTOR &vDst, CVECTOR &vRes)
{
    // check for one side
    uint32_t i, j;
    vRes = vDst;
    vSrc.y = vDst.y = 0.1f;

    if ((vSrc.x <= rIsland.x1 && vDst.x <= rIsland.x1) || (vSrc.x >= rIsland.x2 && vDst.x >= rIsland.x2) ||
        (vSrc.z <= rIsland.y1 && vDst.z <= rIsland.y1) || (vSrc.z >= rIsland.y2 && vDst.z >= rIsland.y2))
        return false;

    // one simple trace vSrc - vDst
    if (Trace(vSrc, vDst) >= 1.0f)
        return false;

    CVECTOR vDir = !(vDst - vSrc);

    std::vector<AIFlowGraph::npoint_t> *PointsSrc, *PointsDst;

    PointsSrc = AIPath.GetNearestPoints(vSrc);
    PointsDst = AIPath.GetNearestPoints(vDst);

    const uint32_t dwSizeSrc = ((*PointsSrc).size() > 8) ? 8 : (*PointsSrc).size();
    const uint32_t dwSizeDst = ((*PointsDst).size() > 8) ? 8 : (*PointsDst).size();

    for (i = 0; i < dwSizeDst; i++)
        (*PointsDst)[i].fTemp = Trace(vDst, AIPath.GetPointPos((*PointsDst)[i].dwPnt));

    float fMaxDistance = 1e9f;
    uint32_t dwI = INVALID_ARRAY_INDEX, dwJ;
    for (i = 0; i < dwSizeSrc; i++)
    {
        if (Trace(vSrc, AIPath.GetPointPos((*PointsSrc)[i].dwPnt)) < 1.0f)
            continue;
        const float fDist1 = sqrtf(~(vSrc - AIPath.GetPointPos((*PointsSrc)[i].dwPnt)));
        if (fDist1 < 80.0f)
            continue;
        for (j = 0; j < dwSizeDst; j++)
            if ((*PointsDst)[j].fTemp > 1.0f)
            {
                // if (Trace(vDst,AIPath.GetPointPos((*PointsDst)[j].dwPnt)) < 1.0f) continue;
                const float fDist2 = sqrtf(~(vDst - AIPath.GetPointPos((*PointsDst)[j].dwPnt)));
                const float fDistance = AIPath.GetPathDistance((*PointsSrc)[i].dwPnt, (*PointsDst)[j].dwPnt);
                const float fTotalDist = fDistance + fDist1 + fDist2;
                if (fTotalDist < fMaxDistance && fTotalDist > 0.0f)
                {
                    fMaxDistance = fTotalDist;
                    dwI = i;
                    dwJ = j;
                }
            }
    }

    if (INVALID_ARRAY_INDEX != dwI)
        vRes = AIPath.GetPointPos((*PointsSrc)[dwI].dwPnt);

    STORM_DELETE(PointsSrc);
    STORM_DELETE(PointsDst);

    return true;
}

/*AIFlowGraph::VectorPath    * ISLAND::FindPath(CVECTOR & vSrc, CVECTOR & vDst)
{
    AIFlowGraph::VectorPath        * pVPath;

    // check for free path

    // check for one side
    if ((vSrc.x <= rIsland.x1 && vDst.x <= rIsland.x1) || (vSrc.x >= rIsland.x2 && vDst.x >= rIsland.x2) ||
        (vSrc.z <= rIsland.y1 && vDst.z <= rIsland.y1) || (vSrc.z >= rIsland.y2 && vDst.z >= rIsland.y2))
    {
        return null;
    }
    // search to near points
    std::vector<AIFlowGraph::npoint_t> & Points = AIPath.GetNearestPoints(vSrc);
    uint32_t dwPnt1 = Points[0].dwPnt;
    Points = AIPath.GetNearestPoints(vDst);
    uint32_t dwPnt2 = Points[0].dwPnt;

    pVPath = AIPath.GetVectorPath(dwPnt1,dwPnt2);
    if (!pVPath) return null;

    return pVPath;
}
*/
