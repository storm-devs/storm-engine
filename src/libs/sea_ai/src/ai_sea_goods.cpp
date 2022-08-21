#include "ai_sea_goods.h"
#include "shared/sea_ai/script_defines.h"

CREATE_CLASS(AISeaGoods)

AISeaGoods::AISeaGoods()
    : TmpItem(), fDistanceMultiply(0)
{
    pSea = nullptr;
    pGeoService = nullptr;

    bDeleteGoodAnyway = false;

    dtCheckShips.Setup(FRAND(1.0f), 1.0f);
}

AISeaGoods::~AISeaGoods()
{
    for (auto &aGood : aGoods)
    {
        if (aGood->pGeo)
            pGeoService->DeleteGeometry(aGood->pGeo);
        aGood->sModel.clear();
        aGood->aItems.clear();
        STORM_DELETE(aGood);
    }
    aGoods.clear();
}

bool AISeaGoods::Init()
{
    SetDevice();
    return true;
}

void AISeaGoods::SetDevice()
{
    pGeoService = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    Assert(pGeoService);
}

void AISeaGoods::Execute(uint32_t dwDeltaTime)
{
    const auto fDeltaTime = static_cast<float>(dwDeltaTime) * 0.001f;

    if (!pSea)
        pSea = static_cast<SEA_BASE *>(core.GetEntityPointer(core.GetEntityId("sea")));

    if (!pSea)
        return;

    for (size_t i = 0; i < aGoods.size(); i++)
    {
        for (size_t j = 0; j < aGoods[i]->aItems.size(); j++)
        {
            auto *pI = &aGoods[i]->aItems[j];
            pI->fTime -= fDeltaTime;

            pI->vPos.y = pSea->WaveXZ(pI->vPos.x, pI->vPos.z, &pI->vNormal);

            if (pI->fTime <= 0.0f)
            {
                pI->vPos.y -= fabsf(pI->fTime) * 0.05f;
                if (pI->fTime < -20.0f)
                {
                    aGoods[i]->aItems[j] = aGoods[i]->aItems.back();
                    aGoods[i]->aItems.pop_back();
                    j--;
                    continue;
                }
            }

            if (dtCheckShips.Update(dwDeltaTime))
            {
                aShips.clear();

                // enumerate ships
                auto &&entities = core.GetEntityIds("ship");
                for (auto ent : entities)
                {
                    aShips.push_back(static_cast<SHIP_BASE *>(core.GetEntityPointer(ent)));
                }

                // check ships
                for (auto pS : aShips)
                {
                    auto *pACharacter = pS->GetACharacter();
                    const int iCharacterIndex = GetIndex(pS->GetACharacter());
                    const auto fDistance = sqrtf(~(pS->State.vPos - pI->vPos));
                    if (fDistance <= pS->State.vBoxSize.z * fDistanceMultiply)
                    {
                        auto *pVData = core.Event(SHIP_EAT_SWIM_GOOD, "llsl", iCharacterIndex, pI->iCharIndex,
                                                  pI->sGoodName, pI->iQuantity);
                        if (pVData->GetInt() || bDeleteGoodAnyway)
                        {
                            aGoods[i]->aItems[j] = aGoods[i]->aItems.back();
                            aGoods[i]->aItems.pop_back();
                            j--;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void AISeaGoods::Realize(uint32_t dwDeltaTime)
{
    if (!pSea)
        return;

    AIHelper::pRS->SetRenderState(D3DRS_LIGHTING, true);

    for (auto &aGood : aGoods)
        if (aGood->pGeo)
            for (uint32_t j = 0; j < aGood->aItems.size(); j++)
            {
                auto *const pI = &aGood->aItems[j];

                // set world matrix for item
                CMatrix m;
                m.BuildPosition(pI->vPos.x, pI->vPos.y, pI->vPos.z);

                AIHelper::pRS->SetTransform(D3DTS_WORLD, m);
                aGood->pGeo->Draw((GEOS::PLANE *)AIHelper::pRS->GetPlanes(), 0, nullptr);
            }

    AIHelper::pRS->SetRenderState(D3DRS_LIGHTING, false);
}

uint32_t AISeaGoods::AttributeChanged(ATTRIBUTES *pAttribute)
{
    auto *const pParent = pAttribute->GetParent();

    if (*pAttribute == "Add")
    {
        for (auto &aGood : aGoods)
            if (aGood->sModel == sTmpModel)
            {
                aGood->aItems.push_back(TmpItem);
                return 0;
            }
        auto *pG = new goods_t;
        aGoods.push_back(pG);
        pG->sModel = sTmpModel;
        pG->aItems.push_back(TmpItem);
        pG->pGeo = pGeoService->CreateGeometry((sModelPath + "\\" + sTmpModel).c_str(), nullptr, 0);
        return 0;
    }

    if (*pAttribute == "CharIndex")
    {
        TmpItem.iCharIndex = pAttribute->GetAttributeAsDword();
        return 0;
    }
    if (*pAttribute == "Time")
    {
        TmpItem.fTime = pAttribute->GetAttributeAsFloat();
        return 0;
    }
    if (*pAttribute == "Quantity")
    {
        TmpItem.iQuantity = pAttribute->GetAttributeAsDword();
        return 0;
    }
    if (*pAttribute == "Model")
    {
        sTmpModel = to_string(pAttribute->GetThisAttr());
        return 0;
    }
    if (*pAttribute == "Good")
    {
        strcpy_s(TmpItem.sGoodName, pAttribute->GetThisAttr());
        return 0;
    }

    if (*pParent == "Pos")
    {
        if (*pAttribute == "x")
        {
            TmpItem.vPos.x = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "z")
        {
            TmpItem.vPos.z = pAttribute->GetAttributeAsFloat();
            return 0;
        }
    }

    if (*pAttribute == "ModelsPath")
    {
        sModelPath = to_string(pAttribute->GetThisAttr());
        return 0;
    }
    if (*pAttribute == "DeleteGoodAnyway")
    {
        bDeleteGoodAnyway = pAttribute->GetAttributeAsDword() == 1;
        return 0;
    }
    if (*pAttribute == "DistanceMultiply")
    {
        fDistanceMultiply = pAttribute->GetAttributeAsFloat();
        return 0;
    }

    return 0;
}
