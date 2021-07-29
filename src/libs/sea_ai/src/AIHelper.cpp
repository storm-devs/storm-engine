#include "AIHelper.h"
#include "AIFort.h"
#include "shared/events.h"

AIHelper Helper;

VDX9RENDER *AIHelper::pRS = nullptr;
ISLAND_BASE *AIHelper::pIsland = nullptr;
COLLIDE *AIHelper::pCollide = nullptr;
ATTRIBUTES *AIHelper::pASeaCameras = nullptr;

float AIHelper::fGravity = 9.81f;

AIHelper::AIHelper()
    : dwRelationSize(0)
{
    pRelations = nullptr;
}

AIHelper::~AIHelper()
{
    Uninit();
}

bool AIHelper::Uninit()
{
    // pRS = null;
    pIsland = nullptr;
    // pCollide = null;
    pASeaCameras = nullptr;
    aCharacters.clear();
    aMainCharacters.clear();
    dwRelationSize = 0;
    STORM_DELETE(pRelations);
    return true;
}

bool AIHelper::SetDevice()
{
    pRS = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    Assert(pRS);
    pCollide = static_cast<COLLIDE *>(core.CreateService("COLL"));
    Assert(pCollide);

    return true;
}

bool AIHelper::Init() const
{
    pIsland = static_cast<ISLAND_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("island")));

    return true;
}

ATTRIBUTES *AIHelper::GetMainCharacter(ATTRIBUTES *pACharacter)
{
    /*uint32_t dwIdx = aCharacters.Find(pACharacter);
    if (dwIdx != INVALID_ARRAY_INDEX)
      return aMainCharacters[dwIdx];
    return null;*/
    const auto it = std::find(aCharacters.begin(), aCharacters.end(), pACharacter); //~!~
    return it != aCharacters.end() ? aMainCharacters[it - aCharacters.begin()] : nullptr;
}

void AIHelper::AddCharacter(ATTRIBUTES *pACharacter, ATTRIBUTES *pAMainCharacter)
{
    // uint32_t dwIdx = aCharacters.Find(pACharacter);
    // if (dwIdx != INVALID_ARRAY_INDEX)
    //{
    //    aMainCharacters[dwIdx] = pAMainCharacter;
    //    return;
    //}
    const auto it = std::find(aCharacters.begin(), aCharacters.end(), pACharacter); //~!~
    if (it != aCharacters.end())
    {
        aMainCharacters[it - aCharacters.begin()] = pAMainCharacter;
        return;
    }
    aCharacters.push_back(pACharacter);
    aMainCharacters.push_back(pAMainCharacter);
}

void AIHelper::CalculateRelations()
{
    uint32_t x, y;

    STORM_DELETE(pRelations);
    dwRelationSize = aCharacters.size();
    pRelations = new uint32_t[SQR(dwRelationSize)];
    for (y = 0; y < dwRelationSize; y++)
        for (x = 0; x < dwRelationSize; x++)
            if (x != y)
            {
                auto *pData =
                    core.Event(GET_RELATION_EVENT, "ll", GetIndex(aMainCharacters[y]), GetIndex(aMainCharacters[x]));
                Assert(pData);
                *GetRelation(y, x) = static_cast<uint32_t>(pData->GetLong());
            }
            else
            {
                *GetRelation(y, x) = RELATION_FRIEND;
            }
}

uint32_t AIHelper::FindIndex(ATTRIBUTES *pACharacter) const
{
    const auto it = std::find(aCharacters.begin(), aCharacters.end(), pACharacter);
    return it != aCharacters.end() ? it - aCharacters.begin() : -1;
}

uint32_t *AIHelper::GetRelation(uint32_t x, uint32_t y) const
{
    Assert(x < dwRelationSize && y < dwRelationSize);
    return &pRelations[x + y * dwRelationSize];
}

uint32_t AIHelper::GetRelationSafe(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const
{
    Assert(pA1 && pA2);
    if (dwRelationSize == 0)
        return RELATION_NEUTRAL;
    const auto dw1 = FindIndex(pA1);
    if (dw1 == INVALID_ARRAY_INDEX)
        return RELATION_NEUTRAL;
    const auto dw2 = FindIndex(pA2);
    if (dw2 == INVALID_ARRAY_INDEX)
        return RELATION_NEUTRAL;
    return *GetRelation(dw1, dw2);
}

uint32_t AIHelper::GetRelation(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const
{
    Assert(pA1 && pA2);
    const auto dw1 = FindIndex(pA1);
    Assert(dw1 != INVALID_ARRAY_INDEX);
    const auto dw2 = FindIndex(pA2);
    Assert(dw2 != INVALID_ARRAY_INDEX);
    return *GetRelation(dw1, dw2);
}

bool AIHelper::isFriend(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const
{
    return GetRelation(pA1, pA2) == RELATION_FRIEND;
}

bool AIHelper::isEnemy(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const
{
    return GetRelation(pA1, pA2) == RELATION_ENEMY;
}

bool AIHelper::isNeutral(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const
{
    return GetRelation(pA1, pA2) == RELATION_NEUTRAL;
}

VAI_INNEROBJ *AIHelper::FindAIInnerObj(ATTRIBUTES *pACharacter)
{
    VAI_INNEROBJ *pObj = nullptr;
    if (AIFort::pAIFort)
        pObj = AIFort::pAIFort->FindFort(pACharacter);
    if (!pObj)
        pObj = AIShip::FindShip(pACharacter);
    return pObj;
}

void AIHelper::Print(float x, float y, float fScale, const char *pFormat, ...)
{
    char cBuffer[2048];

    va_list args;
    va_start(args, pFormat);
    _vsnprintf_s(cBuffer, sizeof(cBuffer), pFormat, args);
    va_end(args);

    pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, PR_ALIGN_CENTER, false, fScale, 0, 0, static_cast<long>(x),
                  static_cast<long>(y), cBuffer);
}

void AIHelper::Print3D(CVECTOR vPos, float dy, float fScale, const char *pFormat, ...)
{
    CMatrix mtx, view, prj;
    char Buff_4k[2048];
    D3DVIEWPORT9 vp;
    MTX_PRJ_VECTOR vrt;

    pRS->GetTransform(D3DTS_VIEW, view);
    pRS->GetTransform(D3DTS_PROJECTION, prj);
    mtx.EqMultiply(view, prj);
    view.Transposition();

    pRS->GetViewport(&vp);
    auto fWidth = vp.Width * 0.5f;
    auto fHeight = vp.Height * 0.5f;

    mtx.Projection(&vPos, &vrt, 1, fWidth, fHeight, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
    vPos = CVECTOR(vrt.x, vrt.y, vrt.z);

    va_list args;
    va_start(args, pFormat);
    _vsnprintf_s(Buff_4k, sizeof(Buff_4k), pFormat, args);
    va_end(args);

    pRS->ExtPrint(FONT_DEFAULT, 0xFFFFFFFF, 0x00000000, PR_ALIGN_CENTER, false, fScale, 0, 0, static_cast<long>(vPos.x),
                  static_cast<long>(vPos.y + dy), Buff_4k);
}

void AIHelper::Save(CSaveLoad *pSL)
{
    pSL->SaveFloat(fGravity);
    pSL->SaveDword(dwRelationSize);
    pSL->SaveBuffer((const char *)pRelations, dwRelationSize * dwRelationSize * sizeof(uint32_t));

    pSL->SaveAPointer("seacameras", pASeaCameras);

    pSL->SaveDword(aCharacters.size());
    for (uint32_t i = 0; i < aCharacters.size(); i++)
        pSL->SaveAPointer("character", aCharacters[i]);

    pSL->SaveDword(aMainCharacters.size());
    for (uint32_t i = 0; i < aMainCharacters.size(); i++)
        pSL->SaveAPointer("character", aMainCharacters[i]);
}

void AIHelper::Load(CSaveLoad *pSL)
{
    fGravity = pSL->LoadFloat();
    dwRelationSize = pSL->LoadDword();
    pSL->LoadBuffer((char **)&pRelations);

    pASeaCameras = pSL->LoadAPointer("seacameras");

    auto dwNum = pSL->LoadDword();
    for (uint32_t i = 0; i < dwNum; i++)
        aCharacters.push_back(pSL->LoadAPointer("character"));

    dwNum = pSL->LoadDword();
    for (uint32_t i = 0; i < dwNum; i++)
        aMainCharacters.push_back(pSL->LoadAPointer("character"));
}
