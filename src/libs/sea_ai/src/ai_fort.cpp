#include "ai_fort.h"
#include "shared/messages.h"

AIFort *AIFort::pAIFort = nullptr;

AIFort::AIFort()
{
    dtFiredTimer.Setup(FRAND(1.0f), 1.0f);
    pLastTraceFort = nullptr;
    pShipsLights = nullptr;
    pAIFort = this;
}

AIFort::~AIFort()
{
    pAIFort = nullptr;
    for (auto &aFort : aForts)
        STORM_DELETE(aFort);
}

bool AIFort::Init()
{
    return true;
}

void AIFort::SetDevice()
{
}

float AIFort::GetSpeedV0(uint32_t dwFortIndex)
{
    ATTRIBUTES *pACannon, *pACharacter;
    pACharacter = aForts[dwFortIndex]->GetACharacter();
    pACannon = pACharacter->FindAClass(pACharacter, "Ship.Cannons");
    Assert(pACannon);
    return pACannon->GetAttributeAsFloat("SpeedV0");
}

// float fAngF = 0.0f;
// float yyy = 0.0f;

void AIFort::Execute(uint32_t Delta_Time)
{
    /*CVECTOR vSrc = CVECTOR(-10000.0f * sinf(fAngF), yyy, -10000.0f * cosf(fAngF));
    CVECTOR vDst = -CVECTOR(vSrc.x, yyy, vSrc.z);
    vDst.y = yyy;
    Trace(vSrc, vDst);*/
    fMinCannonDamageDistance = AttributesPointer->GetAttributeAsFloat("MinCannonDamageDistance");

    const auto fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;
    if (!aForts.size())
        return;

    const auto bFiredTimer = dtFiredTimer.Update(fDeltaTime);

    for (uint32_t k = 0; k < aForts.size(); k++)
    {
        auto *pF = aForts[k];
        if (pF->isDead())
            continue;
        if (!pF->isNormalMode())
            continue;

        auto fSpeedV0 = 0.0f;
        auto dwCurrentCannonType = 0xFFFFFFFF;
        const auto iMax = pF->GetAllCannonsNum(); // boal fix
        for (uint32_t i = 0; i < iMax; i++)
        {
            auto *pC = pF->GetCannon(i);

            const auto dwNewCurrentCannonType = pF->GetCannonType(i);

            // update cannons parameters
            if (dwCurrentCannonType != dwNewCurrentCannonType)
            {
                dwCurrentCannonType = dwNewCurrentCannonType;
                auto *pACannons = pF->GetACharacter()->FindAClass(pF->GetACharacter(), "Ship.Cannons");
                if (!pACannons)
                    pACannons = pF->GetACharacter()->CreateSubAClass(pF->GetACharacter(), "Ship.Cannons");
                Assert(pACannons);
                pACannons->SetAttributeUseDword("type", dwCurrentCannonType);
                core.Event(CANNON_RECALCULATE_PARAMETERS, "l", GetIndex(pF->GetACharacter()));

                fSpeedV0 = GetSpeedV0(k);
            }

            if (!pC->isFired() && pC->isReady2Fire() && bFiredTimer)
            {
                AIShip *pFireAIShip = nullptr;
                auto fMinDistance = 1e10f;
                auto vCPos = pC->GetPos();
                const auto fMaxFireDistance = AICannon::CalcMaxFireDistance(vCPos.y, fSpeedV0, 0.35f); // FIX-ME
                for (auto &j : AIShip::AIShips)
                    if (!j->isDead() &&
                        Helper.isEnemy(pF->GetACharacter(), j->GetACharacter()))
                    {
                        const auto fDistance = j->GetDistance(vCPos);
                        if (fDistance > fMaxFireDistance)
                            continue;
                        if (fDistance < fMinDistance)
                        {
                            fMinDistance = fDistance;
                            pFireAIShip = j;
                        }
                    }
                if (pFireAIShip)
                    pC->Fire(fSpeedV0, pFireAIShip->GetPos());
            }
            pC->Execute(fDeltaTime);
        }
    }
}

void AIFort::Realize(uint32_t Delta_Time)
{
    /*entid_t eidIsland;
    float fCurrentImmersion = 0.0f;
    if (core.FindClass(&eidIsland, "Island", 0))
    {
      ISLAND_BASE * pIsland = (ISLAND_BASE *)core.GetEntityPointer(eidIsland);
      fCurrentImmersion = pIsland->GetCurrentImmersion();
    }*/

    /*for (uint32_t k=0; k<aForts.size(); k++)
    {
      CMatrix mTemp;
      mTemp.BuildPosition(0.0f, -fCurrentImmersion, 0.0f);
      aForts[k]->GetModel()->mtx = aForts[k]->mOldMatrix * mTemp;

      pShipsLights->SetLights(&aForts[k]->tmpObject);
      aForts[k]->GetModel()->Realize(Delta_Time);
      pShipsLights->UnSetLights(&aForts[k]->tmpObject);

      aForts[k]->GetModel()->mtx = aForts[k]->mOldMatrix;
    }*/

    /*   // boal del_cheat
  #ifndef XBOX
    if (core.Controls->GetDebugAsyncKeyState('X') < 0)
    {
      CMatrix mI; mI.SetIdentity();
      AIHelper::pRS->SetTransform(D3DTS_WORLD, mI);

      int32_t iNumCannons = 0;
      for (uint32_t k=0; k<aForts.size(); k++)
      {
        AI_FORT    * pF = aForts[k];
        for (uint32_t i=0; i<pF->GetAllCannonsNum(); i++)
        {
          AICannon * pC = pF->GetCannon(i);
          CVECTOR vPos = pC->GetPos();
          CMatrix mRot(0.0f, pC->GetDirY(), 0.0f);
          AIHelper::pRS->DrawVector(vPos, vPos + mRot * CVECTOR(0.0f, 0.0f, 3.0f), 0xFFFFFFFF);
          AIHelper::pRS->DrawSphere(vPos, 0.3f, 0xFF00FF00);
          iNumCannons++;
        }
      }
    }
  #endif
  */
}

bool AIFort::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool AIFort::LoadState(ENTITY_STATE *state)
{
    return true;
}

bool AIFort::AddFort(ATTRIBUTES *pIslandAP, ATTRIBUTES *pFortLabelAP, ATTRIBUTES *pFortCharacter, entid_t eidModel,
                     entid_t eidBlot)
{
    Assert(pFortLabelAP);

    auto *pFortAP = pFortLabelAP->FindAClass(pFortLabelAP, "fort");
    Assert(pFortAP);
    auto *const pModelAP = pFortAP->FindAClass(pFortAP, "model");
    Assert(pModelAP);
    auto *const pLocatorsAP = pFortAP->FindAClass(pFortAP, "locators");
    Assert(pLocatorsAP);
    auto *const pModelsDirAP = pIslandAP->FindAClass(pIslandAP, "filespath.models");
    Assert(pModelsDirAP);

    const char *pModelName = pModelAP->GetThisAttr();
    Assert(pModelName);
    const char *pLocatorsName = pLocatorsAP->GetThisAttr();
    Assert(pLocatorsName);
    const char *pModelsDir = pModelsDirAP->GetThisAttr();
    Assert(pModelsDir);

    auto *pFort = new AI_FORT(pFortLabelAP);
    pFort->SetACharacter(pFortCharacter);
    pFort->SetModelEID(eidModel);
    pFort->SetBlotEID(eidBlot);
    // pFort->pFortLabelAP = pFortLabelAP;
    aForts.push_back(pFort);

    auto *pACannonsType1 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.1");
    Assert(pACannonsType1);
    auto *pACannonsType2 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.2");
    Assert(pACannonsType2);
    auto *pACannonsType3 = pFortCharacter->FindAClass(pFortCharacter, "Fort.Cannons.Type.3");
    Assert(pACannonsType3);
    pFort->dwCannonType = pACannonsType1->GetAttributeAsDword();
    pFort->dwCulverinType = pACannonsType2->GetAttributeAsDword();
    pFort->dwMortarType = pACannonsType3->GetAttributeAsDword();

    pFort->mOldMatrix = pFort->GetModel()->mtx;

    ScanFortForCannons(pFort, pModelsDir, pLocatorsName);

    auto *pALights = pFortCharacter->FindAClass(GetACharacter(), "ship.lights");
    auto *pAFlares = pFortCharacter->FindAClass(GetACharacter(), "ship.flares");

    const auto bLights = (pALights) ? pALights->GetAttributeAsDword() != 0 : false;
    const auto bFlares = (pAFlares) ? pAFlares->GetAttributeAsDword() != 0 : false;

    const auto eidTmp = core.GetEntityId("shiplights");
    pShipsLights = static_cast<IShipLights *>(core.GetEntityPointer(eidTmp));
    Assert(pShipsLights);

    pShipsLights->AddLights(&pFort->tmpObject, pFort->GetModel(), bLights, bFlares);
    pShipsLights->ProcessStage(Stage::execute, 0);

    core.Event(FORT_CREATE, "al", pFortCharacter, pFort->GetAllCannonsNum());

    return true;
}

void AIFort::AddFortHit(int32_t iCharacterIndex, CVECTOR &vHitPos)
{
    uint32_t i, j, iMax;
    for (i = 0; i < aForts.size(); i++)
    {
        auto *pF = aForts[i];

        iMax = pF->GetAllCannonsNum(); // boal fix
        for (j = 0; j < iMax; j++)
        {
            auto *pC = pF->GetCannon(j);

            if (pC->isDamaged())
                continue;

            auto vPos = pC->GetPos();
            const auto fDistance = sqrtf(~(vPos - vHitPos));
            if (fDistance > fMinCannonDamageDistance)
                continue;

            // VDATA * pVData = core.Event(FORT_CANNON_DAMAGE, "llallfffff", iCharacterIndex,
            // GetIndex(pF->GetACharacter()), pF->pFortLabelAP, pF->GetAllCannonsNum(), pF->GetDamagedCannonsNum(),
            // vPos.x, vPos.y, vPos.z, fDistance, pC->GetDamage()); Assert(pVData);
            auto *pVData = core.Event(FORT_CANNON_DAMAGE, "llallfffff", iCharacterIndex, GetIndex(pF->GetACharacter()),
                                      pF->pFortLabelAP, iMax, pF->GetDamagedCannonsNum(), vPos.x, vPos.y, vPos.z,
                                      fDistance, pC->GetDamage());
            Assert(pVData);

            pC->SetDamage(pVData->GetFloat());
        }
    }
}

AIFort::AI_FORT *AIFort::FindFort(entid_t eidModel)
{
    for (auto &aFort : aForts)
    {
        if (core.GetEntityPointer(aFort->GetModelEID()) == core.GetEntityPointer(eidModel))
            return aFort;
    }
    return nullptr;
}

uint64_t AIFort::ProcessMessage(MESSAGE &message)
{
    CVECTOR vHit;
    int32_t iCharacterIndex;
    entid_t eidFortModel, eidBlot;
    ATTRIBUTES *pFortAPLabel, *pCharacter, *pIslandAP;
    AI_FORT *pFort = nullptr;

    switch (message.Long())
    {
    case AI_MESSAGE_FORT_HIT:
        iCharacterIndex = message.Long();
        vHit.x = message.Float();
        vHit.y = message.Float();
        vHit.z = message.Float();
        AddFortHit(iCharacterIndex, vHit);
        break;
    case AI_MESSAGE_ADD_FORT:
        pIslandAP = message.AttributePointer();
        pFortAPLabel = message.AttributePointer();
        pCharacter = message.AttributePointer();
        eidFortModel = message.EntityID();
        eidBlot = message.EntityID();
        AddFort(pIslandAP, pFortAPLabel, pCharacter, eidFortModel, eidBlot);
        Helper.AddCharacter(pCharacter, pCharacter);
        break;
    case AI_MESSAGE_FORT_SET_LIGHTS:
        pFort = FindFort(message.EntityID());
        if (pShipsLights && pFort)
            pShipsLights->SetLights(&pFort->tmpObject);
        break;
    case AI_MESSAGE_FORT_UNSET_LIGHTS:
        pFort = FindFort(message.EntityID());
        if (pShipsLights && pFort)
            pShipsLights->UnSetLights(&pFort->tmpObject);
        break;
    }
    return 0;
}

uint32_t AIFort::AttributeChanged(ATTRIBUTES *pAttribute)
{
    return 0;
}

bool AIFort::Mount(ATTRIBUTES *pAttribute)
{
    return true;
}

bool AIFort::ScanFortForCannons(AI_FORT *pFort, const char *pModelsDir, const char *pLocatorsName) const
{
    entid_t model_id;
    GEOS::LABEL label;
    GEOS::INFO info;
    NODE *pNode;
    // std::string        sLocatorsName;

    const auto path = std::filesystem::path() / pModelsDir / pLocatorsName;
    const auto pathStr = path.string();
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~
    // sLocatorsName.Format("%s\\%s", pModelsDir, pLocatorsName);
    model_id = core.CreateEntity("MODELR");
    core.Send_Message(model_id, "ls", MSG_MODEL_LOAD_GEO, (char *)pathStr.c_str());

    auto *pModel = static_cast<MODEL *>(core.GetEntityPointer(model_id));
    Assert(pModel);

    // search and add cannons & culverins
    uint32_t dwIdx = 0;
    while (pNode = pModel->GetNode(dwIdx))
    {
        pNode->geo->GetInfo(info);
        for (int32_t i = 0; i < info.nlabels; i++)
        {
            pNode->geo->GetLabel(i, label);
            AICannon *pCannon = nullptr;

            if (storm::iEquals(label.name, "cannon", 6) && pFort->dwCannonType != -1)
            {
                pFort->aCannons.push_back(AICannon{});
                pCannon = &pFort->aCannons.back();
                // pCannon = &pFort->aCannons[pFort->aCannons.Add()];
            }
            if (storm::iEquals(label.name, "culverin", 8) && pFort->dwCulverinType != -1)
            {
                pFort->aCulverins.push_back(AICannon{});
                pCannon = &pFort->aCulverins.back();
                // pCannon = &pFort->aCulverins[pFort->aCulverins.Add()];
            }
            if (storm::iEquals(label.name, "mortar", 6) && pFort->dwMortarType != -1)
            {
                pFort->aMortars.push_back(AICannon{});
                pCannon = &pFort->aMortars.back();
                // pCannon = &pFort->aMortars[pFort->aMortars.Add()];
                pCannon->SetType(AICannon::CANNONTYPE::CANNONTYPE_MORTAR);
            }

            if (pCannon)
            {
                pCannon->Init(pFort, GetId(), label);
                pCannon->QuickRecharge(true);
            }
        }
        dwIdx++;
    }

    core.EraseEntity(model_id);
    return true;
}

bool AIFort::isAttack(AIGroup *pGroup)
{
    return false;
}

bool AIFort::isDead()
{
    return false;
}

float AIFort::GetPower()
{
    return 10000.0f;
}

ATTRIBUTES *AIFort::GetACharacter()
{
    if (pLastTraceFort)
        return pLastTraceFort->GetACharacter();
    return nullptr;
}

float AIFort::Trace(const CVECTOR &vSrc, const CVECTOR &vDst)
{
    pLastTraceFort = nullptr;

    auto fBestRes = 2.0f;
    auto fBestDistance = 1e10f;
    for (uint32_t i = 0; i < GetNumForts(); i++)
    {
        auto *pModel = static_cast<MODEL *>(core.GetEntityPointer(GetFort(i)->GetModelEID()));
        Assert(pModel);
        const auto fRes = pModel->Trace(vSrc, vDst);
        if (fRes > 1.0f)
            continue;
        const auto fDistance = fRes * sqrtf(~(vDst - vSrc));
        if (fDistance < fBestDistance)
        {
            pLastTraceFort = GetFort(i);
            fBestDistance = fDistance;
            fBestRes = fRes;
        }
    }

    return fBestRes;
}

float AIFort::Cannon_Trace(int32_t iBallOwner, const CVECTOR &vSrc, const CVECTOR &vDst)
{
    float fBestRes = 2.0;
    for (uint32_t i = 0; i < GetNumForts(); i++)
    {
        auto *pModel = static_cast<MODEL *>(core.GetEntityPointer(GetFort(i)->GetModelEID()));

        const float fRes = pModel->Trace(vSrc, vDst);
        if (fRes < fBestRes)
            fBestRes = fRes;
        if (fRes <= 1.0f)
        {
            const CVECTOR vTemp = vSrc + (vDst - vSrc) * fRes;
            core.Event(BALL_FORT_HIT, "lfff", iBallOwner, vTemp.x, vTemp.y, vTemp.z);

            const CVECTOR vDir = !(vDst - vSrc);
            core.Send_Message(GetFort(i)->GetBlotEID(), "lffffff", MSG_BLOTS_HIT, vTemp.x, vTemp.y, vTemp.z, vDir.x,
                              vDir.y, vDir.z);
        }
    }
    return fBestRes;
}

AIFort::AI_FORT *AIFort::FindFort(ATTRIBUTES *pACharacter)
{
    for (uint32_t i = 0; i < GetNumForts(); i++)
    {
        if (GetFort(i)->GetACharacter() == pACharacter)
            return GetFort(i);
    }
    return nullptr;
}

CVECTOR AIFort::AI_FORT::GetAttackPoint(VAI_INNEROBJ *pObj)
{
    const float fDistance = pObj->GetMaxFireDistance();
    const uint32_t iMax = GetAllCannonsNum(); // boal fix
    for (uint32_t i = 0; i < iMax; i++)
    {
        AICannon *pC = GetCannon(i);
        if (pC->isDamaged())
            continue;

        const CVECTOR vCPos = pC->GetPos();
        const float fDirY = pC->GetDirY();

        const CVECTOR vRes = vCPos + fDistance / 1.4f * CVECTOR(sinf(fDirY), 0.0f, cosf(fDirY));

        // check for ship can place here
        // pObj->

        return (vRes - GetPos());
    }

    return 0.0f;
}

void AIFort::Save(CSaveLoad *pSL)
{
    pSL->SaveFloat(fMinCannonDamageDistance);
    for (auto &aFort : aForts)
        aFort->Save(pSL);
}

void AIFort::Load(CSaveLoad *pSL)
{
    fMinCannonDamageDistance = pSL->LoadFloat();
    for (auto &aFort : aForts)
        aFort->Load(pSL, GetId());
}

void AIFort::AI_FORT::Save(CSaveLoad *pSL)
{
    pSL->SaveVector(vPos);

    uint32_t i;
    for (i = 0; i < aCannons.size(); i++)
        aCannons[i].Save(pSL);
    for (i = 0; i < aCulverins.size(); i++)
        aCulverins[i].Save(pSL);
    for (i = 0; i < aMortars.size(); i++)
        aMortars[i].Save(pSL);
}

void AIFort::AI_FORT::Load(CSaveLoad *pSL, entid_t eid)
{
    vPos = pSL->LoadVector();

    uint32_t i;
    for (i = 0; i < aCannons.size(); i++)
    {
        aCannons[i].Load(pSL, this, eid);
        if (aCannons[i].isDamaged())
        {
            const CVECTOR vPos = aCannons[i].GetPos();
            core.Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
        }
    }

    for (i = 0; i < aCulverins.size(); i++)
    {
        aCulverins[i].Load(pSL, this, eid);
        if (aCulverins[i].isDamaged())
        {
            const CVECTOR vPos = aCulverins[i].GetPos();
            core.Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
        }
    }

    for (i = 0; i < aMortars.size(); i++)
    {
        aMortars[i].Load(pSL, this, eid);
        if (aMortars[i].isDamaged())
        {
            const CVECTOR vPos = aMortars[i].GetPos();
            core.Event(FORT_LOADDMGCANNON, "fff", vPos.x, vPos.y, vPos.z);
        }
    }
}

void AIFort::Fire(const CVECTOR &vPos)
{
    for (auto &aFort : aForts)
        pShipsLights->AddDynamicLights(&aFort->tmpObject, vPos);
}
