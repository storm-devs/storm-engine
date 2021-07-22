//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Player
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Player.h"

#include "Location.h"
#include "LocationCamera.h"
#include "collide.h"
#include "shared/messages.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

Player::Player()
{
    lastChange = 10.0f;
    task.task = npct_none;
    activatedDialog = false;
    CmdNone();
    isSpecialMode = false;
    isSetBlock = false;
    kSMReload = 1.0f;
    locCam = nullptr;
    shootgunMode = false;
    // shootgunMode = false;
}

Player::~Player()
{
    entid_t peid;
    if (peid = EntityManager::GetEntityId("ShootGunParticles"))
        EntityManager::EraseEntity(peid);
}

bool Player::PostInit()
{
    auto *const location = GetLocation();
    if (!location->supervisor.player)
        location->supervisor.player = this;
    baterfl = EntityManager::GetEntityId("Animals");
    return NPCharacter::PostInit();
}

void Player::Reset()
{
    if (fgtCurType == fgt_block && !isSetBlock)
        fgtCurType = fgt_none;
    NPCharacter::Reset();
}

// Move the character to the desired position
void Player::Move(float dltTime)
{
    kSMReload += dltTime * 0.7f;
    if (kSMReload > 1.0f)
        kSMReload = 1.0f;
    if (!locCam)
    {
        entid_t lcam;
        if (lcam = EntityManager::GetEntityId("LocationCamera"))
        {
            locCam = static_cast<LocationCamera *>(EntityManager::GetEntityPointer(lcam));
        }
    }
    else
    {
        locCam->LockFPMode(shootgunMode);
    }
    // tuner.isVisible = !shootgunMode;

    const auto oldSGMode = shootgunMode;
    shootgunMode = false;
    auto *vd = core.Event("EventSGMode", nullptr);
    if (vd)
    {
        long data = 0;
        if (vd->Get(data))
            shootgunMode = (data != 0);
    }
    if (oldSGMode != shootgunMode)
    {
        if (shootgunMode)
        {
            core.Send_Message(effects, "s", "SGInited");
            tuner.alpha = 0.0f;
            tuner.camAlpha = 0.0f;
            isFight = true;
        }
        else
        {
            core.Send_Message(effects, "s", "SGRelease");
            tuner.alpha = 1.0f;
            tuner.camAlpha = 1.0f;
            isFight = false;
        }
    }

    /*
    if(GetAsyncKeyState(VK_SPACE) < 0)
    {
      impulse.y += 5.0f;
      impulse.y *= 2;
    }*/

    if (task.task == npct_none)
    {
        isEnableJump = true;
        //------------------------------------
        Rotate(dltTime);
        //------------------------------------
        if (lastChange > 0.3f)
        {
            // lastChange = 0.0f;
            SetRunMode(IsRunMode(dltTime));
            if (GoForward(dltTime))
            {
                StartMove(false);
                StrafeWhenMove(dltTime);
            }
            else if (GoBack(dltTime))
            {
                if (!IsFight())
                {
                    StartMove(true);
                    StrafeWhenMove(dltTime);
                }
                else
                {
                    Recoil();
                }
            }
            else
            {
                StopMove();
                StrafeWhenStop(dltTime);
            }
        }
        else
            lastChange += dltTime;
        //------------------------------------
        if (!shootgunMode)
        {
            if (IsChangeFightMode())
            {
                isSetBlock = false;
                SetFightMode(!IsFight());
            }
            if (IsFight())
            {
                if (IsDoBlock())
                {
                    // boal -->
                    if (fgtCurType == fgt_attack_fast || fgtCurType == fgt_attack_force ||
                        fgtCurType == fgt_attack_round || fgtCurType == fgt_attack_break ||
                        fgtCurType == fgt_attack_feint || fgtCurType == fgt_parry)
                    {
                        StopFightAnimation();
                    }
                    // boal <--
                    Block();
                }
                else if (IsDoParry())
                {
                    // boal -->
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak ||
                        fgtCurType == fgt_hit_attack)
                    {
                        StopFightAnimation();
                    }
                    // boal <--
                    Parry();
                }
                else if (IsDoAttackForce())
                {
                    // boal -->
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak)
                    {
                        StopFightAnimation();
                    }
                    // boal <--
                    Attack(FindAttackCharacter(), fgt_attack_force);
                }
                else if (IsDoAttackFast())
                {
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak)
                    {
                        StopFightAnimation();
                    }
                    Attack(FindAttackCharacter(), fgt_attack_fast);
                }
                else if (IsDoAttackRound())
                {
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak ||
                        fgtCurType == fgt_hit_attack)
                    {
                        StopFightAnimation();
                    }
                    Attack(FindAttackCharacter(), fgt_attack_round);
                }
                else if (IsDoAttackBreak())
                {
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak)
                    {
                        StopFightAnimation();
                    }
                    Attack(FindAttackCharacter(), fgt_attack_break);
                }
                else if (IsDoAttackFeint())
                {
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak ||
                        fgtCurType == fgt_hit_attack)
                    {
                        StopFightAnimation();
                    }
                    Attack(FindAttackCharacter(), fgt_attack_feint);
                }
                else if (IsFire())
                {
                    if (fgtCurType == fgt_block || fgtCurType == fgt_blockhit || fgtCurType == fgt_blockbreak ||
                        fgtCurType == fgt_hit_attack)
                    {
                        StopFightAnimation();
                    }
                    Fire();
                }
            }
        }
        else
        {
            tuner.alpha = 0.0f;
            tuner.camAlpha = 0.0f;
            isFight = true;
            if (kSMReload >= 1.0f)
                if (IsFire())
                    FireFromShootgun();
        }
        //------------------------------------
    }
    else
        isEnableJump = false;
    NPCharacter::Move(dltTime);
}

void Player::Update(float dltTime)
{
    auto aDialog = false;
    if (task.task == npct_none)
    {
        if (!IsFight())
        {
            CONTROL_STATE cs;
            core.Controls->GetControlState("ChrAction", cs);
            if (cs.state == CST_ACTIVATED)
                aDialog = true;
            if (activatedDialog)
            {
                auto *chr = FindDialogCharacter();
                if (chr)
                {
                    Assert(AttributesPointer);
                    Assert(chr->AttributesPointer);
                    const long first = AttributesPointer->GetAttributeAsDword("index", -1);
                    const long next = chr->AttributesPointer->GetAttributeAsDword("index", -1);
                    if (first >= 0 && next >= 0)
                    {
                        core.Event("dlgReady", "ll", next, first);
                    }
                    else
                    {
                        core.Trace("Incorrect character index! Dialog not activated...");
                    }
                }
            }

            core.Controls->GetControlState("ChrJump", cs);
            if (cs.state == CST_ACTIVATED)
            {
                StartJump();
            }
        }
        else
        {
            if (IsFireFindTarget())
            {
                float kDist;
                auto *c = FindGunTarget(kDist);
                if (c)
                    c->Select();
            }
        }
    }

    // float kDist;
    // Character * c = FindGunTarget(kDist);
    // if(c) c->Select();
    NPCharacter::Update(dltTime);
    activatedDialog = aDialog;
    core.Send_Message(baterfl, "lff", MSG_ANIMALS_BUTTERFLIES_XYZ, curPos.x, curPos.z);
    // go through characters in search of enemies to the player
    if (const auto eid = EntityManager::GetEntityId("CharactersGroups"))
    {
        auto *const location = GetLocation();
        for (long i = 0; i < location->supervisor.numCharacters; i++)
        {
            auto *const chr = location->supervisor.character[i].c;
            if (chr != this && chr)
            {
                chr->isPlayerEnemy = (core.Send_Message(eid, "sii", "IsEnemy", GetId(), chr->GetId()) != 0);
            }
        }
    }
}

// Save parameters
void Player::SetSaveData(ATTRIBUTES *sdata)
{
    if (!sdata)
        return;
    sdata->SetAttributeUseDword("isFight", isFight);
}

// Restore parameters
void Player::GetSaveData(ATTRIBUTES *sdata)
{
    if (!sdata)
        return;
    if (task.task == npct_none)
    {
        SetFightMode(sdata->GetAttributeAsDword("isFight", isFight) != 0, false);
    }
}

void Player::Rotate(float dltTime)
{
    CONTROL_STATE cs;
    if (!isSpecialMode)
    {
        auto a = GetRotateH() * 0.01f;
        if (a > 0.3f)
            a = 0.3f;
        if (a < -0.3f)
            a = -0.3f;
        Turn(GetAY() + a);
    }
    else
    {
        if (!lockRotate)
        {
            const auto dx = GetRotateH() * 0.067f;
            core.Controls->GetControlState("ChrTurnV", cs);
            auto dz = cs.fValue * 0.067f;
            if (core.Controls->GetControlState("ChrTurnV1", cs))
                dz += cs.fValue * 0.067f;
            if (core.Controls->GetControlState("ChrTurnV2", cs))
                dz += cs.fValue * 0.067f;
            if (dx * dx + dz * dz > 0.1f)
            {
                // Rotate the vector relative to the camera
                CMatrix mtx;
                auto *const location = GetLocation();
                location->GetRS()->GetTransform(D3DTS_VIEW, mtx);
                mtx.Transposition3X3();
                mtx.Vy() = CVECTOR(0.0f, 1.0f, 0.0f);
                mtx.Vx().y = 0.0f;
                mtx.Vz().y = 0.0f;
                mtx.Vx() = !CVECTOR(mtx.Vx());
                mtx.Vz() = !CVECTOR(mtx.Vz());
                mtx.Pos() = 0.0f;
                const auto res = mtx * CVECTOR(dx, 0.0f, dz);
                Turn(res.x, res.z);
            }
        }
    }
}

bool Player::GoForward(float dltTime)
{
    CONTROL_STATE cs;
    if (!isSpecialMode)
    {
        auto res = false;
        core.Controls->GetControlState("ChrForward", cs);
        if (cs.lValue != 0)
            res = true;
        if (core.Controls->GetControlState("ChrForward1", cs) && cs.lValue != 0)
            res = true;
        if (core.Controls->GetControlState("ChrForward2", cs) && cs.lValue != 0)
            res = true;
        return res;
    }
    const auto dx = GetRotateH() * 0.067f;
    core.Controls->GetControlState("ChrTurnV", cs);
    auto dz = cs.fValue * 0.067f;
    if (core.Controls->GetControlState("ChrTurnV1", cs))
        dz += cs.fValue * 0.067f;
    if (core.Controls->GetControlState("ChrTurnV2", cs))
        dz += cs.fValue * 0.067f;
    if (IsMove())
    {
        return dx * dx + dz * dz > 0.1f;
    }
    return dx * dx + dz * dz > 0.2f;
}

bool Player::GoBack(float dltTime) const
{
    if (!isSpecialMode)
    {
        CONTROL_STATE cs;
        auto res = false;
        core.Controls->GetControlState("ChrBackward", cs);
        if (cs.lValue != 0)
            res = true;
        if (core.Controls->GetControlState("ChrBackward1", cs) && cs.lValue != 0)
            res = true;
        if (core.Controls->GetControlState("ChrBackward2", cs) && cs.lValue != 0)
            res = true;
        return res;
    }
    return false;
}

bool Player::IsRunMode(float dltTime)
{
    CONTROL_STATE cs;
    if (!isSpecialMode)
    {
        core.Controls->GetControlState("ChrRun", cs);
        return !(cs.lValue != 0);
    }
    const auto dx = GetRotateH() * 0.067f;
    core.Controls->GetControlState("ChrTurnV", cs);
    auto dz = cs.fValue * 0.067f;
    if (core.Controls->GetControlState("ChrTurnV1", cs))
        dz += cs.fValue * 0.067f;
    if (core.Controls->GetControlState("ChrTurnV2", cs))
        dz += cs.fValue * 0.067f;
    if (IsMove() && IsRun())
    {
        return dx * dx + dz * dz > 0.4f;
    }
    return dx * dx + dz * dz > 0.6f;
}

void Player::StrafeWhenMove(float dltTime)
{
    strafeMove = 0.0f;

    // ----------------------------------------------------------------------------------------
    // If you need to turn off strafe - uncomment return
    // stopstrafe
    // ----------------------------------------------------------------------------------------

    // return;

    if (!isSpecialMode)
    {
        CONTROL_STATE cs;
        if (core.Controls->GetControlState("ChrStrafeLeft", cs))
        {
            if (cs.lValue != 0)
            {
                strafeMove += -1.0f;
            }
        }
        if (core.Controls->GetControlState("ChrStrafeRight", cs))
        {
            if (cs.lValue != 0)
            {
                strafeMove += 1.0f;
            }
        }
    }
}

void Player::StrafeWhenStop(float dltTime)
{
    StrafeWhenMove(dltTime);
    if (IsFight())
    {
        if (strafeMove > 0.5f)
        {
            StrafeRight();
        }
        else if (strafeMove < -0.5f)
        {
            StrafeLeft();
        }
        strafeMove = 0.0f;
    }
    else
    {
        strafeMove = 0.0f;
    }
}

bool Player::IsDoBlock()
{
    //    return true;
    CONTROL_STATE cs;
    core.Controls->GetControlState("ChrBlock", cs);
    if (cs.state == CST_ACTIVATED)
    {
        isSetBlock = true;
    }
    else if (cs.state == CST_INACTIVATED)
    {
        isSetBlock = false;
    }
    else if (cs.state == CST_INACTIVE)
    {
        core.Controls->GetControlState("ChrBlock2", cs);
        if (cs.state == CST_ACTIVATED)
        {
            isSetBlock = true;
        }
        else if (cs.state == CST_INACTIVATED)
        {
            isSetBlock = false;
        }
    }
    if (!IsFight())
    {
        isSetBlock = false;
    }
    return isSetBlock;
}

bool Player::IsDoParry()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrParry", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrParry2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsDoAttackForce()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrAttackForce", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrAttackForce2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsDoAttackFast()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrAttackFast", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrAttackFast2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsDoAttackRound()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrAttackRound", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrAttackRound2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsDoAttackBreak()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrAttackBreak", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrAttackBreak2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsDoAttackFeint()
{
    CONTROL_STATE cs;
    auto bPressed = false;
    core.Controls->GetControlState("ChrAttackFient", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    core.Controls->GetControlState("ChrAttackFient2", cs);
    if (cs.state == CST_ACTIVATED)
        bPressed = true;
    return bPressed;
}

bool Player::IsFire()
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("ChrFire", cs);
    return (cs.state == CST_ACTIVATED);
}

bool Player::IsChangeFightMode()
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("ChrFightMode", cs);
    return (cs.state == CST_ACTIVATED);
}

// Find an attacking enemy
Player *Player::FindAttackCharacter()
{
    auto *const location = GetLocation();
    // Find the surrounding characters
    static Supervisor::FindCharacter fndCharacter[MAX_CHARACTERS];
    static long num = 0;
    if (!location->supervisor.FindCharacters(fndCharacter, num, this, CHARACTER_ATTACK_DIST * 1.1f))
        return nullptr;
    // Choosing the best
    float minDst;
    long task = -1;
    auto isFgt = false;
    const auto isEnemy = false;
    auto enemyCos = -1.0f;
    const auto cdx = sinf(ay);
    const auto cdz = cosf(ay);
    long j = -1;
    for (long i = 0; i < num; i++)
    {
        // Character
        auto &fc = fndCharacter[i];
        // do not look at non-combatants
        // if(!fc.c->IsFight()) continue;
        auto *chr = static_cast<Player *>(fc.c);
        if (chr == this)
            continue;
        // skip the dead
        if (chr->liveValue < 0 || chr->deadName)
            continue;
        // filter out uninteresting
        if (isEnemy) // ~!~
        {
            if (chr->task.task != npct_fight || EntityManager::GetEntityPointer(chr->task.target) != this)
                continue;
        }
        if (isFgt)
        {
            if (!chr->isFight)
                continue;
        }
        else
        {
            if (chr->isFight)
            {
                j = -1;
                isFgt = true;
            }
        }
        /*
                if(chr->task.task == npct_fight &&
                    EntityManager::GetEntityPointer(chr->task.target) == this)
                {
                    j = -1;
                    isEnemy = true;
                }
                */
        // skip non-enemies
        if constexpr (!isEnemy) // ~!~
        {
            if (const auto eid = EntityManager::GetEntityId("CharactersGroups"))
            {
                if (!core.Send_Message(eid, "sii", "IsEnemy", GetId(), chr->GetId()))
                    continue;
            }
        }
        // This bastard climbs on us
        if (j >= 0)
        {
            auto cs = -1.0f;
            if (fc.d2 > 0.0f)
                cs = (fc.dx * cdx + fc.dz * cdz) / sqrtf(fc.d2);
            if (cs > enemyCos)
            {
                j = i;
                minDst = fc.d2;
                enemyCos = cs;
            }
        }
        else
        {
            j = i;
            minDst = fc.d2;
            if (fc.d2 > 0.0f)
                enemyCos = (fc.dx * cdx + fc.dz * cdz) / sqrtf(fc.d2);
            else
                enemyCos = -1.0f;
        }
    }
    if (j >= 0)
        return static_cast<Player *>(fndCharacter[j].c);
    return nullptr;
}

void Player::FireFromShootgun()
{
    kSMReload = 0.0f;
    if (const auto peid = EntityManager::GetEntityId("sound"))
    {
        core.Send_Message(peid, "lsllll", MSG_SOUND_PLAY, "OBJECTS\\sgboom.wav", 4, false, false, false);
    }
    // Get the position from where to shoot
    auto dx = sinf(ay);
    auto dz = cosf(ay);
    CMatrix mtx;
    auto *const location = GetLocation();
    location->GetRS()->GetTransform(D3DTS_VIEW, mtx);
    mtx.Transposition();
    const auto src = mtx.Pos() + mtx.Vz() * 0.7f;
    core.Send_Message(effects, "sffffff", "SGFireParticles", src.x, src.y - 0.35f, src.z, mtx.Vz().x, mtx.Vz().y,
                      mtx.Vz().z);

    auto *collide = static_cast<COLLIDE *>(core.CreateService("COLL"));
    if (!collide)
    {
        return;
    }
    struct ChrsDmg
    {
        Character *chr;
        float dmg;
    };
    ChrsDmg chrs[16];
    long numChrs = 0;

    const auto ids = EntityManager::GetEntityIdIterators(SUN_TRACE);
    for (long i = 0; i < 6; i++)
    {
        // Get the position where the buckshot will fall
        const auto r = rand() * 3.0f / RAND_MAX;
        const auto a = rand() * 6.283185307f / (RAND_MAX + 1);
        auto dst = mtx * CVECTOR(r * sinf(a), r * cosf(a), 25.0f);
        if (collide)
        {
            auto id = GetId();
            const auto dist = collide->Trace(ids, src, dst, &id, 0);
            if (dist <= 1.0f && dist > (0.2f / 25.0f))
            {
                auto dir = !(src - dst);
                dst = src + (dst - src) * dist;
                // Got somewhere
                auto *const e = EntityManager::GetEntityPointer(collide->GetObjectID());
                if (e && e != this)
                {
                    long n, nm;
                    for (n = 0, nm = location->supervisor.numCharacters; n < nm; n++)
                    {
                        auto *c = static_cast<Player *>(location->supervisor.character[n].c);
                        if (c->Model() == e)
                        {
                            core.Send_Message(effects, "sffffff", "SGBloodParticles", dst.x, dst.y, dst.z, dir.x, dir.y,
                                              dir.z);
                            c->impulse -= dir * (1.5f + rand() * (1.0f / RAND_MAX));
                            c->impulse.y += 1.5f + rand() * (1.0f / RAND_MAX);
                            long j;
                            for (j = 0; j < numChrs; j++)
                            {
                                if (chrs[j].chr == c)
                                {
                                    chrs[j].dmg *= 2.0f;
                                    break;
                                }
                            }
                            if (j >= numChrs && numChrs < 16)
                            {
                                chrs[numChrs].chr = c;
                                chrs[numChrs++].dmg = 1.0f;
                            }
                            break;
                        }
                    }
                    if (n >= nm)
                        core.Send_Message(effects, "sffffff", "SGEnvParticles", dst.x, dst.y, dst.z, dir.x, dir.y,
                                          dir.z);
                }
                else
                    core.Send_Message(effects, "sffffff", "SGEnvParticles", dst.x, dst.y, dst.z, dir.x, dir.y, dir.z);
            }
        }
    }
    for (long i = 0; i < numChrs; i++)
    {
        core.Event("Location_CharacterSGFire", "iif", GetId(), chrs[i].chr->GetId(), chrs[i].dmg);
    }
}

float Player::GetRotateH()
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("ChrCamSpecMode", cs);
    auto dx = 0.f;
    if (cs.state != CST_ACTIVE)
    {
        core.Controls->GetControlState("ChrTurnH", cs);
        dx = cs.fValue;
    }
    if (core.Controls->GetControlState("ChrTurnH1", cs))
        dx += cs.fValue;
    if (core.Controls->GetControlState("ChrTurnHR", cs))
    {
        if (cs.state == CST_ACTIVE)
            dx += 12.0f;
    }
    if (core.Controls->GetControlState("ChrTurnHL", cs))
    {
        if (cs.state == CST_ACTIVE)
            dx -= 12.0f;
    }
    return dx;
}
