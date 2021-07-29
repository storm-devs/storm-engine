#include "SEA_AI.h"
#include "AIGroup.h"
#include "shared/messages.h"

CREATE_CLASS(SEA_AI)

CREATE_CLASS(AIFort)

CREATE_CLASS(AIBalls)

SEA_AI::SEA_AI()
{
    bFirstInit = true;
}

SEA_AI::~SEA_AI()
{
    for (uint32_t i = 0; i < AIGroup::AIGroups.size(); i++)
        STORM_DELETE(AIGroup::AIGroups[i]);
    AIGroup::AIGroups.clear();
    AIShip::AIShips.clear();
    Helper.Uninit();
}

bool SEA_AI::Init()
{
    Helper.SetDevice();
    return true;
}

void SEA_AI::SetDevice()
{
}

uint64_t dwRDTSC;

void SEA_AI::Execute(uint32_t Delta_Time)
{
    RDTSC_B(dwRDTSC);
    const auto fDeltaTime = 0.001f * static_cast<float>(Delta_Time);

    if (bFirstInit)
    {
        Helper.CalculateRelations();
        bFirstInit = false;
    }

    for (uint32_t i = 0; i < AIGroup::AIGroups.size(); i++)
    {
        AIGroup::AIGroups[i]->Execute(fDeltaTime);
    }

    RDTSC_E(dwRDTSC);
}

extern uint32_t dwTotal;

void SEA_AI::Realize(uint32_t Delta_Time)
{
    const auto fDeltaTime = 0.001f * static_cast<float>(Delta_Time);
    for (uint32_t i = 0; i < AIGroup::AIGroups.size(); i++)
    {
        AIGroup::AIGroups[i]->Realize(fDeltaTime);
    }
    // core.Trace("%d",dwTotal);
    // AIHelper::pRS->Print(0,90,"%d",dwRDTSC);//dwTotal);
}

bool SEA_AI::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool SEA_AI::LoadState(ENTITY_STATE *state)
{
    return true;
}

void SEA_AI::ProcessMessage(uint32_t iMsg, uint32_t wParam, uint32_t lParam)
{
}

uint64_t SEA_AI::ProcessMessage(MESSAGE &message)
{
    auto iCode = message.Long();
    switch (iCode)
    {
    case AI_MESSAGE_UNLOAD: {
        uint32_t i;
        for (i = 0; i < AIGroup::AIGroups.size(); i++)
            AIGroup::AIGroups[i]->Unload();
        for (i = 0; i < AIShip::AIShips.size(); i++)
            AIShip::AIShips[i]->Unload();
    }
    break;
    case AI_MESSAGE_CANNON_RELOAD:
        AIShip::ReloadCannons(message.AttributePointer());
        break;
    case AI_MESSAGE_SET_CAMERAS_ATTRIBUTE:
        AIHelper::pASeaCameras = message.AttributePointer();
        break;
    case AI_MESSAGE_ADD_SHIP: {
        auto eidShip = message.EntityID();
        auto *pACharacter = message.AttributePointer();
        auto *pAShip = message.AttributePointer();
        AddShip(eidShip, pACharacter, pAShip);
    }
    break;
    case AI_MESSAGE_GROUP_SET_LOCATION_OTHER_GROUP: {
        const std::string &cGroupName = message.String();
        const std::string &cOtherGroupName = message.String();
        AIGroup::GroupSetLocationNearOtherGroup(cGroupName.c_str(), cOtherGroupName.c_str());
        break;
    }
    case AI_MESSAGE_GROUP_SET_TYPE: {
        const std::string &cGroupName = message.String();
        const std::string &cTemp = message.String();
        AIGroup::GroupSetType(cGroupName.c_str(), cTemp.c_str());
        break;
    }
    case AI_MESSAGE_GROUP_SET_COMMANDER: {
        const std::string &cGroupName = message.String();
        auto *pCharacter = message.AttributePointer();
        AIGroup::GroupSetCommander(cGroupName.c_str(), pCharacter);
    }
    break;
    case AI_MESSAGE_GROUP_GET_ATTACK_HP: {
        const std::string &cGroupName = message.String();
        auto fDistance = message.Float();
        auto *pVData = message.ScriptVariablePointer();
        pVData->Set(AIGroup::GetAttackHP(cGroupName.c_str(), fDistance));
    }
    break;
    case AI_MESSAGE_GROUP_SET_XYZ_AY: {
        float x, y, z, ay;
        const std::string &cGroupName = message.String();
        x = message.Float();
        y = message.Float();
        z = message.Float();
        ay = message.Float();
        AIGroup::SetXYZ_AY(cGroupName.c_str(), CVECTOR(x, y, z), ay);
    }
    break;
    case AI_MESSAGE_SHIP_SET_SAIL_STATE: {
        auto *pCharacter = message.AttributePointer();
        auto fSailState = message.Float();
        auto *pAIShip = AIShip::FindShip(pCharacter);
        if (!pAIShip)
        {
            core.Trace("SeaAI err: SetSailState, can't find ship for character = %s", pCharacter->GetAttribute("id"));
            return 0;
        }
        core.Send_Message(pAIShip->GetShipEID(), "lf", MSG_SHIP_SET_SAIL_STATE, fSailState);
    }
    break;
    case AI_MESSAGE_SHIP_SET_TASK: {
        ATTRIBUTES *pCharacter1, *pCharacter2;
        uint32_t dwCommand = message.Long();
        uint32_t dwTaskPriority = message.Long();
        switch (dwCommand)
        {
        case AITASK_ATTACK:
            pCharacter1 = message.AttributePointer();
            pCharacter2 = message.AttributePointer();
            AIShip::ShipSetAttack(dwTaskPriority, pCharacter1, pCharacter2);
            break;
        case AITASK_RUNAWAY:
            pCharacter1 = message.AttributePointer();
            AIShip::ShipSetRunAway(dwTaskPriority, pCharacter1);
            break;
        case AITASK_MOVE:
            pCharacter1 = message.AttributePointer();
            if (message.GetCurrentFormatType() == 'a')
            {
                pCharacter2 = message.AttributePointer();
                AIShip::ShipSetMove(dwTaskPriority, pCharacter1, pCharacter2);
            }
            else
            {
                CVECTOR vPos = 0.0f;
                vPos.x = message.Float();
                vPos.z = message.Float();
                AIShip::ShipSetMove(dwTaskPriority, pCharacter1, vPos);
            }
            break;
        case AITASK_DRIFT:
            pCharacter1 = message.AttributePointer();
            AIShip::ShipSetDrift(dwTaskPriority, pCharacter1);
            break;
        case AITASK_DEFEND:
            pCharacter1 = message.AttributePointer();
            pCharacter2 = message.AttributePointer();
            AIShip::ShipSetDefend(dwTaskPriority, pCharacter1, pCharacter2);
            break;
        case AITASK_BRANDER:
            pCharacter1 = message.AttributePointer();
            pCharacter2 = message.AttributePointer();
            AIShip::ShipSetBrander(dwTaskPriority, pCharacter1, pCharacter2);
            break;
        case AITASK_ABORDAGE:
            pCharacter1 = message.AttributePointer();
            pCharacter2 = message.AttributePointer();
            AIShip::ShipSetAbordage(dwTaskPriority, pCharacter1, pCharacter2);
            break;
        }
    }
    break;
    case AI_MESSAGE_GROUP_SET_TASK: {
        CVECTOR vPnt;
        char cGroupAttackingName[256];
        uint32_t dwCommand = message.Long();
        switch (dwCommand)
        {
        case AITASK_MOVE: {
            const std::string &cGroupName = message.String();
            vPnt.x = message.Float();
            vPnt.y = message.Float();
            vPnt.z = message.Float();
            AIGroup::GroupSetMove(cGroupName.c_str(), vPnt);
            break;
        }
        case AITASK_ATTACK: {
            const std::string &cGroupName = message.String();
            const std::string &cGroupAttackingName = message.String();
            AIGroup::GroupSetAttack(cGroupName.c_str(), cGroupAttackingName.c_str());
            break;
        }
        case AITASK_RUNAWAY:
            const std::string &cGroupName = message.String();
            AIGroup::GroupSetRunAway(cGroupName.c_str());
            break;
        }
    }
    break;
    case AI_MESSAGE_UPDATE_RELATIONS:
        Helper.CalculateRelations();
        break;
    case AI_MESSAGE_SHIP_GET_ATTACK_HP: {
        auto *pACharacter = message.AttributePointer();
        auto fDistance = message.Float();
        auto *pVD = message.ScriptVariablePointer();
        pVD->Set(AIShip::FindShip(pACharacter)->GetAttackHP(fDistance));
    }
    break;
    case AI_MESSAGE_SHIP_CHANGE_GROUP: {
        auto *pACharacter = message.AttributePointer();
        const std::string &cGroupName = message.String();
        AIGroup::ShipChangeGroup(pACharacter, cGroupName.c_str());
    }
    break;
    case AI_MESSAGE_CANNON_FIRE: {
        auto *pACharacter = message.AttributePointer();
        auto bCameraOutside = (message.Long() == 1);
        AIShip::ShipFire(pACharacter, bCameraOutside);
    }
    break;
    case AI_MESSAGE_SET_OFFICER_2_SHIP: {
        auto *pACharacter1 = message.AttributePointer();
        auto *pACharacter2 = message.AttributePointer();
        AIGroup::SetOfficerCharacter2Ship(pACharacter1, pACharacter2);
    }
    break;
    case AI_MESSAGE_SWAP_SHIPS: {
        auto *pACharacter1 = message.AttributePointer();
        auto *pACharacter2 = message.AttributePointer();
        AIGroup::SwapCharactersShips(pACharacter1, pACharacter2);
    }
    break;
    case AI_MESSAGE_SAIL_2_LOCATOR: {
        float x, y, z, ay;
        x = message.Float();
        y = message.Float();
        z = message.Float();
        ay = message.Float();

        AIGroup::SailMainGroup(CVECTOR(x, y, z), ay, nullptr);
    }
    break;
    case AI_MESSAGE_SAIL_2_CHARACTER: {
        auto *pACharacter = message.AttributePointer();
        auto fDistance = message.Float();
        auto fAngle = message.Float();
        auto *pAIShip = AIShip::FindShip(pACharacter);
        Assert(pAIShip);
        auto vPos = pAIShip->GetPos() + (fDistance * CVECTOR(sinf(fAngle), 0.0f, cosf(fAngle)));

        AIGroup::SailMainGroup(vPos, -10000.0f, pACharacter);
    }
    break;
    case AI_MESSAGE_CHARACTER_DEAD: {
        auto *pAIObj = AIHelper::FindAIInnerObj(message.AttributePointer());
        if (pAIObj)
        {
            pAIObj->SetDead(true);
            if (pAIObj->GetObjType() != AIOBJ_FORT)
                static_cast<AIShip *>(pAIObj)->GetShipBasePointer()->SetDead();
        }
    }
    break;
    case AI_MESSAGE_GET_RELATION: {
        auto *pACharacter1 = message.AttributePointer();
        auto *pACharacter2 = message.AttributePointer();
        auto *pVData = message.ScriptVariablePointer();
        pVData->Set(static_cast<long>(Helper.GetRelationSafe(pACharacter1, pACharacter2)));
    }
    break;
    case AI_MESSAGE_SET_COMPANION_ENEMY: {
        auto *pACharacter = message.AttributePointer();
        SetCompanionEnemy(pACharacter);
    }
    break;
    case AI_MESSAGE_CANNONS_PARAMS:
        AIShipCannonController::fMaxCannonDamageDistance = message.Float();
        AIShipCannonController::fMaxCannonDamageRadiusPoint = message.Float();
        break;
    case AI_MESSAGE_CANNONS_BOOM_CHECK: {
        float fTmpCannonDamage, x, y, z;

        auto *pACharacter = message.AttributePointer();
        fTmpCannonDamage = message.Float();
        x = message.Float();
        y = message.Float();
        z = message.Float();

        auto *pAIShip = AIShip::FindShip(pACharacter);
        Assert(pAIShip);
        pAIShip->GetCannonController()->CheckCannonsBoom(fTmpCannonDamage, CVECTOR(x, y, z));
    }
    break;
        // boal 08.08.06 method of recounting guns on the ship -->
    case AI_MESSAGE_RESEARCH_CANNONS: {
        auto *pACharacter = message.AttributePointer();

        auto *pAIShip = AIShip::FindShip(pACharacter);
        Assert(pAIShip);
        pAIShip->GetCannonController()->ResearchCannons();
    }
    break;
        // boal 08.08.06 method of recounting guns on the ship <--
    case AI_MESSAGE_SEASAVE: //TODO: check these two
    {
        char str[256]{};
        Save(str);
    }
    break;
    case AI_MESSAGE_SEALOAD: {
        char str[256]{};
        Load(str);
    }
    break;
    }
    return 0;
}

void SEA_AI::Save(const char *pStr)
{
    CSaveLoad SL;

    SL.CreateWrite();

    Helper.Save(&SL);
    core.Send_Message(EntityManager::GetEntityId("SEA_CAMERAS"), "lp", AI_MESSAGE_SEASAVE, &SL);

    AIBalls::pAIBalls->Save(&SL);

    SL.SaveDword(AIGroup::AIGroups.size());
    for (uint32_t i = 0; i < AIGroup::AIGroups.size(); i++)
        AIGroup::AIGroups[i]->Save(&SL);

    if (AIFort::pAIFort)
        AIFort::pAIFort->Save(&SL);
}

void SEA_AI::Load(const char *pStr)
{
    Helper.Uninit();

    CSaveLoad SL;

    SL.CreateLoad();

    Helper.Load(&SL);
    core.Send_Message(EntityManager::GetEntityId("SEA_CAMERAS"), "lp", AI_MESSAGE_SEALOAD, &SL);

    AIBalls::pAIBalls->Load(&SL);

    const auto dwNumGroups = SL.LoadDword();
    for (uint32_t i = 0; i < dwNumGroups; i++)
    {
        AIGroup::AIGroups.push_back(new AIGroup());
        AIGroup::AIGroups.back()->Load(&SL);
    }

    if (AIFort::pAIFort)
        AIFort::pAIFort->Load(&SL);
    Helper.Init();
}

uint32_t SEA_AI::AttributeChanged(ATTRIBUTES *pAttribute)
{
    uint32_t i;

    if (*pAttribute == "isDone")
    {
        // delete all old groups and ships
        Helper.Init();

        for (i = 0; i < AIShip::AIShips.size(); i++)
            AIShip::AIShips[i]->CheckStartPosition();
    }

    if (*pAttribute == "DistanceBetweenGroupShips")
    {
        AIGroup::fDistanceBetweenGroupShips = pAttribute->GetAttributeAsFloat();
    }

    return 0;
}

void SEA_AI::AddShip(entid_t eidShip, ATTRIBUTES *pCharacter, ATTRIBUTES *pAShip)
{
    Assert(pCharacter && pAShip);
    auto *pG = pCharacter->FindAClass(pCharacter, "SeaAI.Group");
    Assert(pG);

    // search group
    auto *const pGName = pG->GetAttribute("Name");
    Assert(pGName);

    AIGroup::FindOrCreateGroup(pGName)->AddShip(eidShip, pCharacter, pAShip);
}

void SEA_AI::SetCompanionEnemy(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);

    // delete from old group
    auto *pG = AIGroup::FindGroup(pACharacter);
    Assert(pG);
    auto *const pS = pG->ExtractShip(pACharacter);

    // create and add to new group
    auto *pSeaAIG = pACharacter->FindAClass(pACharacter, "SeaAI.Group");
    Assert(pSeaAIG);
    auto *const pGName = pSeaAIG->GetAttribute("Name");
    Assert(pGName);

    AIGroup::FindOrCreateGroup(pGName)->InsertShip(pS);
}
