//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    NPCaracter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "np_character.h"
#include "characters_groups.h"
#include "defines.h"
#include "shared/messages.h"

//============================================================================================

// Goto
#define NPC_STOP_GOTODIST 0.3f // Distance in meters at which it is considered that the character has reached the point

// Follow
#define NPC_STOP_DIST_NPC 1.5f  // Distance to the character, at which it is necessary to stop and stand
#define NPC_START_DIST_NPC 2.5f // Distance to the character at which to start moving
#define NPC_RUN_DIST_NPC 10.0f  // Distance to the character, at which to run
#define NPC_WALK_DIST_NPC 4.0f  // Distance to the character, at which running turns into walking

// Escape
#define NPC_STOP_ESCAPE 20.0f // Distance at which to stop the escape

// Fight
#define NPC_FIGHT_STOP_DIST 2.0f  // Distance to the character at which to stop
#define NPC_FIGHT_GO_DIST 2.4f    // Distance to the character, at which to start moving towards the goal
#define NPC_FIGHT_RUN_DIST 4.0f   // Distance to the character, at which to switch to running mode
#define NPC_FIGHT_WALK_DIST 2.5f  // Distance to the character, at which to switch to walking mode
#define NPC_FIGHT_FIRE_DIST 25.0f // Distance to the character, at which you can shoot

// ============================================================================================
// Construction, destruction
// ============================================================================================

NPCharacter::NPCharacter() : taskstack{}
{
    charactersGroups = {};
    fMusketerTime = {};
    stackPointer = 0;
    task.task = npct_stay;
    lastSetTask = npct_unknow;
    CmdStay();
    // Fight system
    fightLevel = 0.0f;
    // Attacks
    attackCur = 0.5f; // k3 fix 0.05f;
    attackPrbFast = 0.2f;
    attackPrbForce = 0.5f;
    attackPrbRound = 0.1f;
    attackPrbBreak = 0.1f;
    attackPrbFeint = 0.1f;
    // Defence
    defenceCur = 0.05f;
    blockTime = 0.0f;
    defencePrbBlock = 0.9f;
    defencePrbParry = 0.1f;
    isRecoilEnable = true;
    // Shooting
    fireCur = 0.0f;
    isFireEnable = true;
    // The current state of the enemy
    enemyFgtType = fgt_none;
    isFgtChanged = false;
    fightTick = 0.0f;
    wantToFire = false;
    wantToAttack = false;
    wantToDefence = false;

    // fMusketerTime = 2.0f;
    bMusketer = false;
    fMusketerDistance = 20.0f;
    bMusketerNoMove = false;
    fMusketerFireTime = 0.0f;
    fMusketerCheckFireTime = 5.0f;

    bTryAnyTarget = false;
}

NPCharacter::~NPCharacter()
{
}

bool NPCharacter::PostInit()
{
    charactersGroups = EntityManager::GetEntityId("CharactersGroups");
    float tmp;
    int32_t tmpBool;
    VDATA *vd;
    // Attack parameters
    vd = core.Event("NPC_Event_GetAttackActive", "i", GetId());
    tmp = attackCur;
    if (vd && vd->Get(tmp))
        attackCur = tmp;
    vd = core.Event("NPC_Event_GetAttackWeightFast", "i", GetId());
    tmp = attackPrbFast;
    if (vd && vd->Get(tmp))
        attackPrbFast = tmp;
    vd = core.Event("NPC_Event_GetAttackWeightForce", "i", GetId());
    tmp = attackPrbForce;
    if (vd && vd->Get(tmp))
        attackPrbForce = tmp;
    vd = core.Event("NPC_Event_GetAttackWeightRound", "i", GetId());
    tmp = attackPrbRound;
    if (vd && vd->Get(tmp))
        attackPrbRound = tmp;
    vd = core.Event("NPC_Event_GetAttackWeightBreak", "i", GetId());
    tmp = attackPrbBreak;
    if (vd && vd->Get(tmp))
        attackPrbBreak = tmp;
    vd = core.Event("NPC_Event_GetAttackWeightFeint", "i", GetId());
    tmp = attackPrbFeint;
    if (vd && vd->Get(tmp))
        attackPrbFeint = tmp;
    // Defence parameters
    vd = core.Event("NPC_Event_GetDefenceActive", "i", GetId());
    tmp = defenceCur;
    if (vd && vd->Get(tmp))
        defenceCur = tmp;
    vd = core.Event("NPC_Event_GetDefenceWeightBlock", "i", GetId());
    tmp = defencePrbBlock;
    if (vd && vd->Get(tmp))
        defencePrbBlock = tmp;
    vd = core.Event("NPC_Event_GetDefenceWeightParry", "i", GetId());
    tmp = defencePrbParry;
    if (vd && vd->Get(tmp))
        defencePrbParry = tmp;
    vd = core.Event("NPC_Event_EnableRecoil", "i", GetId());
    tmpBool = isRecoilEnable;
    if (vd && vd->Get(tmpBool))
        isRecoilEnable = tmpBool != 0;
    // Shooting parameters
    vd = core.Event("NPC_Event_GetFireActive", "i", GetId());
    tmp = fireCur;
    if (vd && vd->Get(tmp))
        fireCur = tmp;
    vd = core.Event("NPC_Event_EnableFire", "i", GetId());
    tmpBool = isFireEnable;
    if (vd && vd->Get(tmpBool))
        isFireEnable = tmpBool != 0;
    vd = core.Event("NPC_Event_StunChance", "i", GetId());
    int32_t tmpInt;
    if (vd && vd->Get(tmpInt))
        stunChance = tmpInt;

    // Parameter normalization
    if (attackCur < 0.0f)
        attackCur = 0.0f;
    if (attackCur > 1000.0f)
        attackCur = 1000.0f;
    if (defenceCur < 0.0f)
        defenceCur = 0.0f;
    if (defenceCur > 1000.0f)
        defenceCur = 1000.0f;
    auto p = attackPrbFast + attackPrbForce + attackPrbRound + attackPrbBreak + attackPrbFeint;
    if (p > 0.0f)
    {
        p = 1.0f / p;
        attackPrbFast *= p;
        attackPrbForce *= p;
        attackPrbRound *= p;
        attackPrbBreak *= p;
        attackPrbFeint *= p;
    }
    p = defencePrbBlock + defencePrbParry;
    if (p > 0.0f)
    {
        p = 1.0f / p;
        defencePrbBlock *= p;
        defencePrbParry *= p;
    }
    return true;
}

uint32_t NPCharacter::ChlProcessMessage(int32_t messageID, MESSAGE &message)
{
    char buf[128];
    switch (messageID)
    {
    case MSG_NPCHARACTER_SETTASK: {
        const std::string &buf = message.String();
        return SetNewTask(GetTaskID(buf.c_str()), message);
    }
    case MSG_NPCHARACTER_PUSHTASK:
        return PushTask();
    case MSG_NPCHARACTER_POPTASK:
        return PopTask();
    case MSG_CHARACTER_SETFTGLEVEL:
        fightLevel = message.Float();
        if (fightLevel < 0.0f)
            fightLevel = 0.0f;
        if (fightLevel > 1.0f)
            fightLevel = 1.0f;
        return 1;
    case MSG_NPCHARACTER_GETTASK:
        auto *v = message.ScriptVariablePointer();
        if (!v)
            return 0;
        v->Set((char *)GetTaskName(task.task));
        return 1;
    }
    return 0;
}

// Move the character to the desired position
void NPCharacter::Move(float dltTime)
{
    // process the task
    switch (task.task)
    {
    case npct_none:
        break;
    case npct_stay:
        break;
    case npct_gotopoint:
        break;
    case npct_followcharacter:
        break;
    case npct_fight:
        break;
    case npct_escape:
        break;
    case npct_dead:
        break;
    }
    AICharacter::Move(dltTime);
}

// Update character position
void NPCharacter::Update(float dltTime)
{
    bMusketer = AttributesPointer->GetAttributeAsDword("isMusketer", 0) != 0;
    fMusketerDistance = AttributesPointer->GetAttributeAsFloat("MusketerDistance", 20.0f);
    bMusketerNoMove = fMusketerDistance <= 0.0f;

    AICharacter::bMusketer = bMusketer;
    AICharacter::bMusketerNoMove = bMusketerNoMove;

    fMusketerFireTime -= dltTime;
    fMusketerCheckFireTime -= dltTime;
    if (fMusketerFireTime <= 0.0f)
        fMusketerFireTime = 0.0f;

    switch (task.task)
    {
    case npct_none:
        break;
    case npct_stay:
        SetFightMode(false);
        break;
    case npct_gotopoint:
        break;
    case npct_followcharacter:
        UpdateFollowCharacter(dltTime);
        break;
    case npct_fight:
        UpdateFightCharacter(dltTime);
        break;
    case npct_escape:
        UpdateEscapeCharacter(dltTime);
        break;
    case npct_dead:
        break;
    }
    AICharacter::Update(dltTime);
    auto *const location = GetLocation();
    // write debug information
    if (location->IsDebugView())
    {
        const auto isDebugEx = location->IsExDebugView();
        // if(AttributesPointer && AttributesPointer->GetAttributeAsDword("hideInfo", 0)) return;
        const auto rad = 25.0f;
        const char *id = nullptr;
        if (AttributesPointer)
            id = AttributesPointer->GetAttribute("id");
        if (!id)
            id = "<none>";
        const char *fid = nullptr;
        auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(task.target));
        if (chr)
        {
            if (chr->AttributesPointer)
                fid = chr->AttributesPointer->GetAttribute("id");
        }
        if (!fid)
            fid = "<none>";
        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, 3, 1.0f, 0xffffff, 0.5f, "id(%s)", id);
        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, 2, 1.0f, 0xffffff, 0.5f, "task(%s<-%s)",
                        GetTaskName(task.task), GetTaskName(lastSetTask));
        if (isDebugEx)
            location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, 1, 1.0f, 0xffffff, 0.5f, "target(%s)", fid);
        if (isDebugEx)
            location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, 0, 1.0f, 0xffffff, 0.5f, "cmd(%s)",
                            GetCommandName(command.cmd));
        if (isDebugEx)
            location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, -1, 1.0f, 0xffffff, 0.5f,
                            "pos(%.2f, %.2f, %.2f)", curPos.x, curPos.y, curPos.z);
        if (isDebugEx)
            location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, -2, 1.0f, 0xffffff, 0.5f, "isJump = %s",
                            isJump ? "true" : "false");
        int32_t line = 4;
        if (isDebugEx && command.cmd == aicmd_gotopoint)
            location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f,
                            "goto(%.2f, %.2f, %.2f)", command.pnt.x, command.pnt.y, command.pnt.z);
        if (isDebugEx && AttributesPointer)
        {
            auto *atr = AttributesPointer->FindAClass(AttributesPointer, "chr_ai.tmpl");
            if (atr)
            {
                for (int32_t i = atr->GetAttributesNum() - 1; i >= 0; i--)
                {
                    fid = atr->GetAttributeName(i);
                    id = atr->GetAttribute(i);
                    if (fid && id)
                        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f,
                                        "tmpl.%s(%s)", fid, id);
                }
                id = atr->GetThisAttr();
                if (id)
                    location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f, "tmpl(%s)",
                                    id);
            }
            atr = AttributesPointer->FindAClass(AttributesPointer, "chr_ai.type");
            if (atr)
            {
                for (int32_t i = atr->GetAttributesNum() - 1; i >= 0; i--)
                {
                    fid = atr->GetAttributeName(i);
                    id = atr->GetAttribute(i);
                    if (fid && id)
                        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f,
                                        "type.%s(%s)", fid, id);
                }
                id = atr->GetThisAttr();
                if (id)
                    location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f, "type(%s)",
                                    id);
            }
            atr = AttributesPointer->FindAClass(AttributesPointer, "chr_ai.group");
            if (atr)
            {
                id = atr->GetThisAttr();
                if (!id)
                    id = "";
                location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f,
                                "group(\"%s\")", id);
            }
        }
        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f, "wantToAttack = %s",
                        wantToAttack ? "true" : "false");
        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f, "wantToDefence = %s",
                        wantToDefence ? "true" : "false");
        location->Print(curPos + CVECTOR(0.0f, height, 0.0f), rad, line++, 1.0f, 0xffffff, 0.5f, "wantToFire = %s)",
                        wantToFire ? "true" : "false");
    }
}

// ============================================================================================
// Tasks
// ============================================================================================

void NPCharacter::SetEscapeTask(Character *c)
{
    if (bMusketerNoMove)
        return;
    if (task.task == npct_runtopoint)
        return;

    task.task = npct_runtopoint;
    lastSetTask = npct_runtopoint;
    memset(&task.target, 0, sizeof(task.target));
    // task.target = c->GetId();
    /*SetFightMode(false);
    SetRunMode(true);
    task.target = c->GetId();*/

    CVECTOR vOurPos, vPos;
    c->GetPosition(vPos);
    GetPosition(vOurPos);

    auto fCurDist = sqrtf(~(vOurPos - vPos));
    const CVECTOR vDir = !(vOurPos - vPos);

    SetFightMode(false);
    SetRunMode(true);
    for (int32_t i = 0; i < 32; i++)
    {
        CMatrix mRot;
        mRot.BuildRotateY(static_cast<float>(i / 32.0f) * PIm2);
        CVECTOR vDir1 = mRot * vDir;

        task.to = vPos + vDir1 * (fMusketerDistance * 0.8f);
        if (CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST * 2.0f))
            break;
    }
    /*task.to.x = vPos.x;
    task.to.y = vPos.y;
    task.to.z = vPos.z - fMusketerDistance * 0.8f;*/
}

// Set a new task
bool NPCharacter::SetNewTask(NPCTask tsk, MESSAGE &message)
{
    if (bMusketerNoMove)
    {
        if (tsk == npct_gotopoint)
            return true;
        if (tsk == npct_runtopoint)
            return true;
        if (tsk == npct_followcharacter)
            return true;
    }

    task.task = tsk;
    lastSetTask = tsk;
    memset(&task.target, 0, sizeof(task.target));
    switch (tsk)
    {
    case npct_none:
        // SetRunMode(false);
        // SetFightMode(false);
        CmdStay();
        return CmdNone();
    case npct_stay:
        SetFightMode(false);
        return CmdStay();
    case npct_gotopoint:
        SetFightMode(false);
        SetRunMode(false);
        task.to.x = message.Float();
        task.to.y = message.Float();
        task.to.z = message.Float();
        return CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST);
    case npct_runtopoint:
        SetFightMode(false);
        SetRunMode(true);
        task.to.x = message.Float();
        task.to.y = message.Float();
        task.to.z = message.Float();
        return CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST * 2.0f);
    case npct_followcharacter:
        SetFightMode(false);
        task.target = message.EntityID();
        return InitFollowChartacter(task.target);
    case npct_fight:
        task.target = message.EntityID();
        return InitFightChartacter(task.target);
    case npct_escape:
        if (bMusketerNoMove)
            return true;
        SetFightMode(false);
        SetRunMode(true);
        task.target = message.EntityID();
        return (EntityManager::GetEntityPointer(task.target) != nullptr);
    case npct_dead:
        CmdStay();
        Dead();
        return true;
    }
    return false;
}

bool NPCharacter::InitFollowChartacter(entid_t eid)
{
    task.target = eid;
    auto c = static_cast<Character *>(EntityManager::GetEntityPointer(eid));
    if (c)
    {
        const char *id = c->AttributesPointer->GetAttribute("id");
    }
    task.isFollowInit = 0;
    return true;
}

bool NPCharacter::InitFightChartacter(entid_t eid)
{
    enemyFgtType = fgt_none;
    isFgtChanged = false;
    task.target = eid;
    task.isFollowInit = 0;
    return true;
}

// ============================================================================================
// Executing tasks
// ============================================================================================

// Completing the task of following a character
void NPCharacter::UpdateFollowCharacter(float dltTime)
{
    // goal
    auto *c = static_cast<NPCharacter *>(EntityManager::GetEntityPointer(task.target));
    if (!c || c->deadName != nullptr || c->liveValue < 0)
    {
        const NPCTask tsk = task.task;
        task.task = npct_none;
        CmdStay();
        FailureCommand(tsk);
        return;
    }
    // check the distance to the target
    const float dst = ~(c->curPos - curPos);
    if (task.isFollowInit == 0)
    {
        task.isFollowInit = 1;
        if (dst <= NPC_START_DIST_NPC * NPC_START_DIST_NPC)
            EndGotoCommand();
    }
    if (command.cmd != aicmd_gotopoint)
    {
        SetRunMode(false);
        if (dst > NPC_START_DIST_NPC * NPC_START_DIST_NPC)
        {
            CmdGotoPoint(c->curPos.x, c->curPos.y, c->curPos.z, NPC_STOP_DIST_NPC, c->currentNode, false);
            core.Event("Location_CharacterFollowGo", "si", GetTaskName(npct_followcharacter), GetId());
        }
    }
    else
    {
        if (dst > NPC_RUN_DIST_NPC * NPC_RUN_DIST_NPC)
            SetRunMode(true);
        if (dst < NPC_WALK_DIST_NPC * NPC_WALK_DIST_NPC)
            SetRunMode(false);
        CmdGotoPoint(c->curPos.x, c->curPos.y, c->curPos.z, NPC_STOP_DIST_NPC, c->currentNode, false);
    }
    SetExCharacter(c);
}

// Execution of the escape task
void NPCharacter::UpdateEscapeCharacter(float dltTime)
{
    // The character we are running from
    auto *c = static_cast<NPCharacter *>(EntityManager::GetEntityPointer(task.target));
    if (!c || c->deadName != nullptr || c->liveValue < 0)
    {
        const NPCTask tsk = task.task;
        task.task = npct_none;
        CmdStay();
        FailureCommand(tsk);
        return;
    }
    // leave along the radial line
    const float fDist = (bMusketer) ? fMusketerDistance * 0.7f : NPC_STOP_ESCAPE;
    CmdEscape(c->curPos.x, c->curPos.y, c->curPos.z, fDist);
    if (isSlide)
    {
        NPCTask tsk = task.task;
        task.task = npct_none;
        CmdStay();
        SetRunMode(false);
        core.Event("Location_CharacterEscapeSlide", "si", GetTaskName(npct_escape), GetId());
    }
}

// Combat mission execution
void NPCharacter::UpdateFightCharacter(float dltTime)
{
    // fMusketerTime -= dltTime;

    SetFightMode(true);
    // goal
    auto *c = static_cast<NPCharacter *>(EntityManager::GetEntityPointer(task.target));
    if (!c || c->deadName != nullptr || c->liveValue < 0 || c == this)
    {
        const NPCTask tsk = task.task;
        task.task = npct_none;
        CmdStay();
        FailureCommand(tsk);
        return;
    }
    // Deciding what to do
    fightTick -= dltTime;
    if (fightTick <= 0.0f)
    {
        fightTick = 0.2f; // k3 fix 0.5f;
        FightTick();
    }
    // check the distance to the target
    const float dst = ~(c->curPos - curPos);
    // Switching to combat mode
    if (dst > NPC_FIGHT_RUN_DIST * NPC_FIGHT_RUN_DIST)
    {
        SetRunMode(true);
    }
    else if (dst < NPC_FIGHT_WALK_DIST * NPC_FIGHT_WALK_DIST)
    {
        SetRunMode(false);
    }
    if (task.isFollowInit == 0)
    {
        task.isFollowInit = 1;
        if (dst < NPC_STOP_DIST_NPC * NPC_STOP_DIST_NPC)
            EndGotoCommand();
    }
    // Navigation
    bool isProcessWait = false;

    bool bCurrentActionIsFire = false;
    if (fgtCurType == fgt_none && priorityAction.name && shot.name)
    {
        if (storm::iEquals(priorityAction.name, shot.name))
        {
            bCurrentActionIsFire = true;
        }
    }

    const bool bGunLoaded = IsGunLoad();

    if (fgtSetType == fgt_fire || fMusketerFireTime > 0.0f)
        bCurrentActionIsFire = true;

    bool bTryFire = true;
    if (bMusketer && /*fMusketerTime <= 0.0f &&*/ fMusketerDistance > 0.0f && !bCurrentActionIsFire && !bGunLoaded)
    {
        if (sqrt(dst) < fMusketerDistance * 0.6f)
        {
            bTryFire = false;
            // fMusketerTime = 3.0f;
            SetEscapeTask(c);
        }
        // else
        // CmdStay();
    }

    float kdst;

    const bool bVisTarget = VisibleTest(c);
    const bool bFarTarget = dst > NPC_FIGHT_FIRE_DIST * NPC_FIGHT_FIRE_DIST;
    Character *target = FindGunTarget(kdst, true);

    if (bVisTarget && !bFarTarget) // if the target is visible, then try to shoot it
        bTryAnyTarget = false;

    // Once every 5 seconds, check if we can shoot, if not, then move in a circle from the target
    if (bMusketer && bTryFire && fMusketerCheckFireTime <= 0.0f)
    {
        fMusketerCheckFireTime = 6.0f;

        // if the main target is not visible and there are no other visible targets, then try to bypass
        if (!bVisTarget && !target)
        {
            bTryFire = false;

            task.task = npct_runtopoint;
            lastSetTask = npct_runtopoint;
            memset(&task.target, 0, sizeof(task.target));
            // task.target = c->GetId();

            CVECTOR vOurPos, vPos;
            c->GetPosition(vPos);
            GetPosition(vOurPos);

            const float fCurDist = sqrtf(~(vOurPos - vPos));
            const CVECTOR vDir = !(vOurPos - vPos);

            SetFightMode(false);
            SetRunMode(true);
            for (int32_t i = 1; i < 9; i++)
            {
                CMatrix mRot;
                mRot.BuildRotateY(static_cast<float>(i / 9.0f) * PIm2);
                CVECTOR vDir1 = mRot * vDir;

                task.to = vPos + vDir1 * fCurDist;
                if (CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST * 2.0f))
                    break;
            }
        }
        else // if there is a target, but it is different, then try to shoot it
        {
            bTryAnyTarget = true;
        }
    }

    // if (bTryFire)
    {
        if (command.cmd != aicmd_gotopoint && !bCurrentActionIsFire)
        {
            const float fDist = (bMusketer && !bMusketerNoMove) ? fMusketerDistance * 0.9f : NPC_FIGHT_GO_DIST;
            const float fDistTo = (bMusketer && !bMusketerNoMove) ? fMusketerDistance * 0.7f : NPC_FIGHT_GO_DIST;
            if (dst > fDist * fDist)
            {
                // must come closer
                CmdGotoPoint(c->curPos.x, c->curPos.y, c->curPos.z, fDistTo, c->currentNode, false);
                core.Event("Location_CharacterFightGo", "si", GetTaskName(npct_followcharacter), GetId());
            } // else{
            // at war
            // }
        } // else
        // {
        // specify the location of the target
        // CmdGotoPoint(c->curPos.x, c->curPos.y, c->curPos.z, NPC_FIGHT_STOP_DIST, c->currentNode, false);
        // check the possibility of a shot

        double dx = c->curPos.x - curPos.x;
        double dz = c->curPos.z - curPos.z;
        double l = dx * dx + dz * dz;

        // trying to run away
        /*if (bMusketer && fMusketerTime <= 0.0f && fMusketerDistance >= 0.0f && sqrt(l) < fMusketerDistance * 0.6f)
        {
          bTryFire = false;
          fMusketerTime = 3.0f;
          SetEscapeTask(c);
        }*/
        bool bFired = false;
        if (bTryFire && wantToFire && isFireEnable)
        {
            if (dst < NPC_FIGHT_FIRE_DIST * NPC_FIGHT_FIRE_DIST &&
                (dst > NPC_FIGHT_GO_DIST * NPC_FIGHT_GO_DIST || fightLevel > 0.6f))
            {
                if (IsGunLoad())
                {
                    if (l > 0.3)
                    {
                        l = 1.0 / sqrt(l);
                        dx *= l;
                        dz *= l;
                        auto ang = static_cast<float>(acos(dz));
                        if (dx < 0)
                            ang = -ang;
                        if (dx * sinf(ay) + dz * cosf(ay) > 0.65f)
                        {
                            // Determine the current goal
                            const float _ay = ay;
                            ay = ang;
                            float kdst;
                            Character *target = FindGunTarget(kdst, CheckShotOnlyEnemyTest(), true);
                            // Character * target = FindGunTarget(kdst, true);
                            ay = _ay;
                            // if((target == c) || (target && bTryAnyTarget))
                            if (target)
                            {
                                Turn(ang);
                                Fire();
                                bFired = true;
                                fMusketerFireTime = 1.5f;
                            }
                        }
                    }
                }
            }
        }

        if (!bFired && dst <= NPC_FIGHT_GO_DIST * NPC_FIGHT_GO_DIST)
        {
            if (fgtCurType == fgt_none && priorityAction.name && shot.name)
                ;
            else
                DoFightAction(dltTime, c);
        }

        if (bMusketer && command.cmd == aicmd_stay)
        {
            Turn(c->curPos.x - curPos.x, c->curPos.z - curPos.z);
        }
        //}
    }
    SetExCharacter(c);
    if (fgtCurType == fgt_none && priorityAction.name && shot.name)
    {
        if (storm::iEquals(priorityAction.name, shot.name))
        {
            float kdst;
            auto target = static_cast<NPCharacter *>(FindGunTarget(kdst, true));
            if (target)
            {
                if (target != c)
                {
                    const float tdx = curPos.x - target->curPos.x;
                    const float tdz = curPos.z - target->curPos.z;
                    const float edx = curPos.x - c->curPos.x;
                    const float edz = curPos.z - c->curPos.z;
                    if (edz * tdx + edx * tdz < 0.0f)
                    {
                        Turn(ay - 0.1f);
                    }
                    else
                    {
                        Turn(ay + 0.1f);
                    }
                }
                else
                {
                    Turn(c->curPos.x - curPos.x, c->curPos.z - curPos.z);
                }
            }
            else
            {
                Turn(c->curPos.x - curPos.x, c->curPos.z - curPos.z);
            }
        }
    }
}

//============================================================================================
// Fight AI
//============================================================================================

// Combat behavior
void NPCharacter::DoFightAction(float dltTime, NPCharacter *enemy)
{
    // Analyzing the change in enemy actions
    if (enemy->fgtCurType != enemyFgtType)
    {
        enemyFgtType = enemy->fgtCurType;
        isFgtChanged = true;
    }
    // look at the current action
    if (blockTime > 0.0f)
    {
        Block();
        blockTime -= dltTime;
        return;
    }
    switch (fgtCurType)
    {
    case fgt_none:
        // free
        DoFightActionAnalysisNone(dltTime, enemy);
        break;
    }
    // Simulating slow turns
    Turn(enemy->curPos.x - curPos.x, enemy->curPos.z - curPos.z);
    isFgtChanged = false;
}

// Combat behavior - idle
void NPCharacter::DoFightActionAnalysisNone(float dltTime, NPCharacter *enemy)
{
    if (!enemy)
        return;
    // look at our desires
    const bool wishAttact = wantToAttack;
    const bool wishDefence = wantToDefence;
    // If we don't want anything, save resources and do nothing else
    if (!(wishAttact | wishDefence))
        return;
    // get the target selection mode for the attack
    int32_t isAdaptive = true;
    VDATA *vd = core.Event("NPC_Event_AdaptiveTargetSelect", "i", GetId());
    if (vd)
        vd->Get(isAdaptive);
    // Correcting taking into account the presence of groups
    auto chrGroup = static_cast<CharactersGroups *>(EntityManager::GetEntityPointer(charactersGroups));
    if (!chrGroup)
        isAdaptive = false;
    // If we want to hit and the mode is not adaptive, then just hit
    if (wishAttact && !isAdaptive)
    {
        DoFightAttack(enemy, 1, false);
        return;
    }
    // collect everyone around
    auto *const location = GetLocation();
    auto fndCharacter = location->supervisor.FindCharacters(this, CHARACTER_ATTACK_DIST, 0.0f, 0.01f, 0.0f, false);
    if (fndCharacter.empty())
        return;
    // our group
    const int32_t grpIndex = chrGroup->FindGroupIndex(group);
    // Enemy table
    auto enemies = std::vector<EnemyState>();
    // Our direction
    const CVECTOR dir(sinf(ay), 0.0f, cosf(ay));
    // Calculating enemies
    bool isFreeBack = isRecoilEnable;
    static const float backAng = -cosf(45.0f * (3.1415926535f / 180.0f));
    for (size_t i = 0; i < fndCharacter.size(); i++)
    {
        // Character
        Supervisor::FindCharacter &fc = fndCharacter[i];
        auto chr = static_cast<NPCharacter *>(fc.c);
        if (chr->liveValue < 0 || chr->deadName || fc.d2 <= 0.0f || chr == this)
            continue;
        fc.d2 = sqrtf(fc.d2);
        if (isFreeBack)
        {
            const float cs = dir.x * fc.dx + dir.z * fc.dz;
            if (cs < 0.0f)
            {
                if (-cs >= backAng * fc.d2)
                {
                    isFreeBack = false;
                }
            }
        }
        // if(!chr->isFight) continue;
        // Belligerent group
        const int32_t grp = chrGroup->FindGroupIndex(chr->group);
        if (grp < 0 || grpIndex < 0)
            return;
        // The attitude of his group to our
        if (chrGroup->FindRelation(grpIndex, grp).curState != CharactersGroups::rs_enemy)
            continue;
        // This is the enemy
        auto &found_enemy = enemies.emplace_back();
        found_enemy.chr = chr;
        // Where the enemy is looking and location relative to us
        if (fc.d2 > 0.0f)
        {
            found_enemy.look = -(sinf(chr->ay) * fc.dx + cosf(chr->ay) * fc.dz) / fc.d2;
            found_enemy.dir = (dir.x * fc.dx + dir.z * fc.dz) / fc.d2;
        }
        else
        {
            found_enemy.look = 0.0f;
            found_enemy.dir = 0.0f;
        }
        // If necessary, analyze the goal
        if (wishAttact)
        {
            float hp = 1.0f;
            vd = core.Event("NpcEvtHP", "i", chr->GetId());
            if (vd)
                vd->Get(hp);
            if (hp < 0.0f)
                hp = 0.0f;
            if (hp > 1.0f)
                hp = 1.0f;
            float energy = 1.0f;
            vd = core.Event("NpcEvtEgy", "i", chr->GetId());
            if (vd)
                vd->Get(energy);
            if (energy < 0.0f)
                energy = 0.0f;
            if (energy > 1.0f)
                energy = 1.0f;
            found_enemy.state = 0.1f / (hp + 0.05f) + 0.2f / (energy + 0.5f);
        }
    }
    // If there is no one malicious, do nothing
    if (enemies.empty())
        return;
    // If attack, then choose a suitable target for the attack.
    if (wishAttact)
    {
        float kSel;
        int32_t counter = 0;
        Character *enemy = nullptr;
        for (size_t i = 0; i < enemies.size(); i++)
        {
            EnemyState &es = enemies[i];
            const float k = es.state * 1.0f + (es.dir + 1.0f) * 0.5f;
            if (enemy)
            {
                if (kSel < k)
                {
                    kSel = k;
                    enemy = es.chr;
                }
            }
            else
            {
                kSel = k;
                enemy = es.chr;
            }
            static const float dangerAng = cosf(0.7f * CHARACTER_ATTACK_ANG * (3.1415926535f / 180.0f));
            if (es.look > dangerAng)
                counter++;
        }
        DoFightAttack(enemy, counter, wishDefence);
        return;
    }
    if (!wishDefence)
        return;
    // decide whether need to defend ourselves
    bool isBreakAttack = false;
    int32_t attacked = 0;
    static const float attackAng = cosf(0.5f * CHARACTER_ATTACK_ANG * (3.1415926535f / 180.0f));
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (enemies[i].look >= attackAng)
        {
            auto chr = static_cast<NPCharacter *>(enemies[i].chr);
            if (chr->fgtCurType >= fgt_attack_fast && chr->fgtCurType <= fgt_attack_feintc)
            {
                if (chr->fgtCurType == fgt_attack_break)
                    isBreakAttack = true;
                attacked++;
            }
        }
    }
    // Check the full rebound capability
    CVECTOR p = curPos;
    if (location->GetPtcData().Move(currentNode, curPos + dir * 2.0f, p) >= 0)
    {
        p -= curPos;
        p.y = 0.0f;
        if (~p < 1.8f)
        {
            isFreeBack = false;
        }
    }
    // If there are no attackers, then bounce or evaluate our stupidity to make a block
    if (!attacked)
    {
        return;
    }
    // estimate the probability that we will miss the hit
    if (!wantToDefence)
    {
        return;
    }
    // If the blow is piercing
    if (isBreakAttack)
    {
        // Probability of recognizing a penetrating attack
        const float prbDetect = fightLevel * fightLevel * 0.9f;
        if (PrTest(prbDetect))
        {
            if (!isFreeBack)
            {
                DoFightBlock(true);
            }
            else
            {
                // There is a choice - parry or bounce
                if (PrTest(0.4f + enemies.size() * (0.1f + fightLevel * 0.1f)))
                {
                    Recoil();
                }
                else
                {
                    DoFightBlock(true);
                }
            }
            return;
        }
    }
    // Deciding how to protect yourself
    if (isFreeBack)
    {
        float reciolPrb = attacked * attacked * 0.1f * (fightLevel + 0.3f);
        if (reciolPrb > 0.9f)
            reciolPrb = 0.9f;
        if (PrTest(reciolPrb))
        {
            Recoil();
            return;
        }
    }
    DoFightBlock();
}

// Combat behavior - attack
void NPCharacter::DoFightAttack(Character *enemy, int32_t enemyCounter, bool wishDefence)
{
    wantToAttack = false;
    // enemy - whom to beat
    // enemyCounter - number of active enemies
    // wishDefence - at the time of the attack there was a desire to defend
    struct AttackAction
    {
        float prb;
        FightAction action;
    };
    AttackAction attack[5];
    int32_t count = 0;
    if (attackPrbFast > 0.0f)
    {
        attack[count].prb = attackPrbFast;
        attack[count].action = fgt_attack_fast;
        count++;
    }
    if (attackPrbForce > 0.0f)
    {
        attack[count].prb = attackPrbForce;
        attack[count].action = fgt_attack_force;
        count++;
    }
    if (attackPrbRound > 0.0f)
    {
        if (enemyCounter > 3 - (wishDefence ? 1 : 0))
        {
            attack[count].prb = attackPrbRound;
            attack[count].action = fgt_attack_round;
            count++;
        }
    }
    if (attackPrbBreak > 0.0f)
    {
        attack[count].prb = attackPrbBreak;
        attack[count].action = fgt_attack_break;
        count++;
    }
    if (attackPrbFeint > 0.0f)
    {
        attack[count].prb = attackPrbFeint;
        attack[count].action = fgt_attack_feint;
        count++;
    }
    float max = 0.0f;
    for (int32_t i = 0; i < count; i++)
    {
        max += attack[i].prb;
    }
    if (count == 0 || max <= 0.0f)
        return;
    const float r = rand() * (max / RAND_MAX);
    float p = 0.0f;
    int32_t i;
    for (i = 0; i < count; i++)
    {
        p += attack[i].prb;
        if (r <= p)
            break;
    }
    if (i >= count)
        i = 0;
    Attack(enemy, attack[i].action);
}

// Combat behavior - block, parry
void NPCharacter::DoFightBlock(bool needParry)
{
    wantToDefence = false;
    const float sum = defencePrbBlock + defencePrbParry;
    if (sum <= 0.0f)
        return;
    const float parryEnergy = GetActEnergy("parry");
    const float used = GetEnergy() - parryEnergy;
    const float minTime = 0.8f + (1.0f - fightLevel) * 0.8f;
    const float maxTime = minTime + (1.1f - fightLevel) * 2.5f;
    blockTime = minTime + rand() * (1.0f / RAND_MAX) * (maxTime - minTime);
    if (used < 0.0f)
    {
        Block();
        return;
    }
    if (needParry)
    {
        Parry();
        return;
    }
    const float noParryThreshold = (fightLevel * 0.3f + rand() * (0.3f / RAND_MAX));
    if (used < noParryThreshold)
    {
        Block();
        return;
    }
    if (rand() * sum * (1.0f / RAND_MAX) <= defencePrbBlock)
    {
        Block();
    }
    else
    {
        Parry();
    }
}

// Get energy
float NPCharacter::GetEnergy() const
{
    float energy = 1.0f;
    VDATA *vd = core.Event("NpcEvtEgy", "i", GetId());
    if (vd)
        vd->Get(energy);
    if (energy < 0.0f)
        energy = 0.0f;
    if (energy > 1.0f)
        energy = 1.0f;
    return energy;
}

// Get energy for action
float NPCharacter::GetActEnergy(const char *act) const
{
    VDATA *vd = core.Event("NPC_Event_GetActionEnergy", "is", GetId(), act);
    float energy;
    if (vd && vd->Get(energy))
        return energy;
    return 0.1f;
}

void NPCharacter::HitChild(bool isInBlock)
{
    if (isInBlock)
    {
        blockTime -= fightLevel * 0.01f;
    }
}

//============================================================================================

// Cannot further execute the command
void NPCharacter::FailureCommand()
{
    FailureCommand(task.task);
}

// The character arrived at the point
void NPCharacter::EndGotoCommand()
{
    switch (task.task)
    {
    case npct_gotopoint:
        task.task = npct_none;
        CmdStay();
        SetRunMode(false);
        core.Event("Location_CharacterEndTask", "si", GetTaskName(npct_gotopoint), GetId());
        return;
    case npct_runtopoint:
        task.task = npct_none;
        CmdStay();
        SetRunMode(false);
        core.Event("Location_CharacterEndTask", "si", GetTaskName(npct_runtopoint), GetId());
        return;
    case npct_followcharacter:
        CmdStay();
        core.Event("Location_CharacterFollowStay", "si", GetTaskName(npct_followcharacter), GetId());
        return;
    case npct_fight:
        CmdStay();
        core.Event("Location_CharacterFightStay", "si", GetTaskName(npct_followcharacter), GetId());
    }
}

// The character moved away from the point to the required radius
void NPCharacter::EndEscapeCommand()
{
    task.task = npct_stay;
    SetRunMode(false);
    CmdStay();
    core.Event("Location_CharacterEndTask", "si", GetTaskName(npct_escape), GetId());
}

// colliding with a character too often
void NPCharacter::CollisionThreshold()
{
    core.Event("Location_CharacterColThreshold", "si", GetTaskName(task.task), GetId());
}

// Save task on stack
bool NPCharacter::PushTask()
{
    if (stackPointer >= sizeof(taskstack) / sizeof(Task))
        return false;
    task.isRun = IsRun();
    task.isFight = IsFight();
    taskstack[stackPointer++] = task;
    return true;
}

// Retrieve a task from the stack
bool NPCharacter::PopTask()
{
    if (stackPointer <= 0)
        return false;
    task = taskstack[--stackPointer];
    switch (task.task)
    {
    case npct_none:
        return CmdNone();
    case npct_stay:
        return CmdStay();
    case npct_gotopoint:
        SetRunMode(false);
        return CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST);
    case npct_runtopoint:
        SetRunMode(true);
        return CmdGotoPoint(task.to.x, task.to.y, task.to.z, NPC_STOP_GOTODIST);
    case npct_followcharacter:
        return InitFollowChartacter(task.target);
    case npct_fight:
        return InitFightChartacter(task.target);
        break;
    case npct_escape:
        break;
    case npct_dead:
        CmdStay();
        Dead();
        break;
    }
    SetRunMode(task.isRun != 0);
    SetFightMode(task.isFight != 0);
    return true;
}

// ============================================================================================
// Encapsulation
// ============================================================================================

// Cannot further execute the command
void NPCharacter::FailureCommand(NPCTask task) const
{
    core.Event("Location_CharacterTaskFailure", "si", GetTaskName(task), GetId());
}

// Get task type by name
NPCharacter::NPCTask NPCharacter::GetTaskID(const char *taskName)
{
    if (!taskName || !taskName[0])
        return npct_unknow;
    for (int32_t i = 0; i < npct_max; i++)
    {
        const char *task = GetTaskName(static_cast<NPCTask>(i));
        if (storm::iEquals(task, taskName))
            return static_cast<NPCTask>(i);
    }
    return npct_unknow;
}

// Get task name by type
const char *NPCharacter::GetTaskName(NPCTask t)
{
    switch (t)
    {
    case npct_none: // No task, the character is controlled from the outside
        return "None";
    case npct_stay: // Stand still
        return "Stay";
    case npct_gotopoint: // Go to the point
        return "Goto point";
    case npct_runtopoint: // Run to the point
        return "Runto point";
    case npct_followcharacter: // Follow the character
        return "Follow character";
    case npct_fight: // Fight another character
        return "Fight";
    case npct_escape: // Leave the character
        return "Escape";
    case npct_dead: // Death of a character
        return "Dead";
    }
    return "Unknow task";
}

// Making decisions
void NPCharacter::FightTick()
{
    wantToFire = PrTest(fireCur);
    wantToAttack = PrTest(attackCur);
    wantToDefence = PrTest(defenceCur);
}
