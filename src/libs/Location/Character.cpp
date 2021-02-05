//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Character
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Character.h"
#include "CharactersGroups.h"
#include "Lights.h"

#include "../../Shared/messages.h"
#include "../SoundService/VSoundService.h"
#include "defines.h"
#include "geometry.h"
#include "sea_base.h"

#include "core.h"
#include "vdata.h"

//============================================================================================

#define CHARACTER_WAIT_AFTER_DEAD 6.0f //

#define CHARACTER_COS_STAIR 0.9f //

#define CHARACTER_NORM_TO_FIGHT "Normal to fight"
#define CHARACTER_FIGHT_TO_NORM "Fight to normal"
#define CHARACTER_FIGHT_GUNBELT "To belt"
#define CHARACTER_FIGHT_GUNHAND "To hand"
#define CHARACTER_FIGHT_GUNFIRE "Fire"

#define CHARACTER_FIGHT_FIREDIST 25.0f //метров
#define CHARACTER_FIGHT_FIREANG 10.0f  //градусов

#define CHARACTER_DIALOG_ANG 30.0f //Максимальный угл срабатывания диалога

#define CHARACTER_SEA_NOTRUN 0.2f //Относительная высота при которой персонаж не может бежать
#define CHARACTER_SEA_MOVE 0.6f //Относительная высота при которой начинает сносить персонажа
#define CHARACTER_SEA_SWIM 0.8f //Относительная высота при которой персонаж начинает плыть

#define CHARACTER_HIDE_DIST 0.5f //Растояние до камеры при котором гасить персонажа

#define CHARACTER_JUMPMAXTIME 5.0f //Максимально допустимое время падения при прыжке
#define CHARACTER_MAXJUMPPOINTS (sizeof(jumpTrack) / sizeof(CVECTOR))
#define CHARACTER_JUMP_TIMESTEP (CHARACTER_JUMPMAXTIME / CHARACTER_MAXJUMPPOINTS)

//Виды атак
#define FGT_ATTACK_FAST "fast"
#define FGT_ATTACK_FORCE "force"
#define FGT_ATTACK_ROUND "round"
#define FGT_ATTACK_BREAK "break"
#define FGT_ATTACK_FEINT "feint"
#define FGT_ATTACK_PARRY "parry" // для пробоя "hit_parry"  boal

//Таблица возможности смены одного действия другим
uint8_t Character::fightTbl[fgt_max][fgt_max] = {
    // Set type
    // nn afs afr ard abk aft aftc fr hatk hft hpr hrd hfr bk bkh bbr pry rcl stl str  //Current type
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // fgt_none Нет никакого боевого действия
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_attack_fast Быстрый удар
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_attack_force Сильный удар
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_attack_round Круговой удар
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_attack_break Пробивающий удар
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // fgt_attack_feint Финт - спрециальный обманный удар
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    // fgt_attack_feintc Продолжение финта в случае контратаки
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // fgt_fire Выстрел из пистолета
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // fgt_hit_attack Реакция попадания удара по персонажу вводящая его в stall
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // fgt_hit_feint Реакция от финта вводящая его в stall
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // fgt_hit_parry Реакция от парирования вводящая его в stall
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // fgt_hit_round Реакция отталкивание круговым ударом
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // fgt_hit_fire Реакция от выстрела вводящая его в stall
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, // fgt_block Защита саблей
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, // fgt_blockhit Защита саблей
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // fgt_blockbreak Пробивка блока
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    // fgt_parry Парирование, защитное движение вводящее противника в stall
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_recoil Отскок назад
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_strafe_l Отскок влево
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // fgt_strafe_r Отскок право
};

const char *Character::fightNamesTbl[fgt_max] = {
    "fgt_none",          //Нет никакого боевого действия
    "fgt_attack_fast",   //Быстрый удар
    "fgt_attack_force",  //Сильный удар
    "fgt_attack_round",  //Круговой удар
    "fgt_attack_break",  //Пробивающий удар
    "fgt_attack_feint",  //Финт - спрециальный обманный удар
    "fgt_attack_feintc", //Продолжение финта в случае контратаки
    "fgt_fire",          //Выстрел из пистолета
    "fgt_hit_attack", //Реакция попадания удара по персонажу вводящая его в stall
    "fgt_hit_feint",  //Реакция от финта вводящая его в stall
    "fgt_hit_parry",  //Реакция от парирования вводящая его в stall
    "fgt_hit_round",  //Реакция отталкивание круговым ударом
    "fgt_hit_fire",   //Реакция от выстрела вводящая его в stall
    "fgt_block",      //Защита саблей
    "fgt_blockhit",   //Защита саблей
    "fgt_blockbreak", //Пробивка блока
    "fgt_parry", //Парирование, защитное движение вводящее противника в stall
    "fgt_recoil",   //Отскок назад
    "fgt_strafe_l", //Отскок влево
    "fgt_strafe_r", //Отскок право
};

//============================================================================================

Character::ActionCharacter::ActionCharacter()
{
    name = nullptr;
    tblend = 0.0f;
}

void Character::ActionCharacter::SetName(const char *_name)
{
    if (name)
    delete name;
    name = nullptr;
    if (_name && _name[0])
    {
        const long l = strlen(_name) + 1;
        name = new char[l];
        memcpy(name, _name, l);
    }
}

void Character::ActionCharacter::ChangeName(const char *_name)
{
    if (!_name)
        return;
    SetName(_name);
}

Character::ActionCharacter::~ActionCharacter()
{
    if (name)
    delete name;
    name = nullptr;
}

Character::ActionMove::ActionMove()
{
    speed = 0.0f;
    turnspd = 0.0f;
}

Character::ActionIdle::ActionIdle()
{
    p = 1.0f;
}

Character::ActionDead::ActionDead()
{
    ang = 0.0f;
}

Character::Blender::Blender()
{
    old = 0.0f;
    dlt = 0.0f;
    kTime = 0.0f;
    time = 1.0f;
}

void Character::Blender::Blend(float _old, float _new, float _time)
{
    old = _old;
    dlt = _new - _old;
    if (_time > 0.0f)
    {
        kTime = 1.0f / _time;
        time = 0.0f;
    }
    else
    {
        kTime = 1.0f;
        time = 1.0f;
    }
}

bool Character::Blender::Update(float dltTime)
{
    if (kTime <= 0.0f)
        return false;
    time += kTime * dltTime;
    if (time > 1.0f)
    {
        time = 1.0f;
        kTime = 0.0f;
    }
    return true;
}

float Character::Blender::Get() const
{
    return old + powf(time, 0.1f) * dlt;
}

Character::Detector::Detector(LocatorArray *_la)
{
    la = _la;
    lastLocator = -1;
    timeInLocator = 0.0f;
    lastEventTime = 0.0f;
}

void Character::Detector::Check(float dltTime, Character *ch)
{
    auto *const location = ch->GetLocation();
    auto dist = 0.0f;
    const auto lIndex = la->FindNearesLocatorCl(ch->curPos.x, ch->curPos.y, ch->curPos.z, 1.0f, dist);
    if (lIndex >= 0)
    {
        if (lastLocator >= 0)
        {
            timeInLocator += dltTime;
            if (lastLocator == lIndex)
            {
                if (lastEventTime > 1.0f)
                {
                    lastEventTime = 0.0f;
                    core.Event("Location_CharacterInLocator", "iissff", location->GetId(), ch->GetId(),
                               la->GetGroupName(), la->Name(lastLocator), dist, timeInLocator);
                }
                else
                    lastEventTime += dltTime;
                return;
            }
            core.Event("Location_CharacterExitFromLocator", "iissff", location->GetId(), ch->GetId(),
                       la->GetGroupName(), la->Name(lastLocator), dist, timeInLocator);
        }
        timeInLocator = 0.0f;
        lastEventTime = 0.0f;
        lastLocator = lIndex;
        core.Event("Location_CharacterEntryToLocator", "iissf", location->GetId(), ch->GetId(), la->GetGroupName(),
                   la->Name(lastLocator), dist);
    }
    else
    {
        if (lastLocator >= 0)
        {
            core.Event("Location_CharacterExitFromLocator", "iissf", location->GetId(), ch->GetId(), la->GetGroupName(),
                       la->Name(lastLocator), timeInLocator + dltTime);
            lastLocator = -1;
        }
    }
}

void Character::Detector::Exit(Character *ch)
{
    auto *const location = ch->GetLocation();

    if (lastLocator >= 0)
    {
        core.Event("Location_CharacterExitFromLocator", "iissf", location->GetId(), ch->GetId(), la->GetGroupName(),
                   la->Name(lastLocator), timeInLocator);
    }
    timeInLocator = 0.0f;
    lastEventTime = 0.0f;
    lastLocator = -1;
}

Character::RTuner::RTuner()
{
    alpha = 1.0f;
    chrAlpha = 1.0f;
    camAlpha = 1.0f;
    selected = 0.0f;
    character = nullptr;
    isVisible = true;
}

void Character::RTuner::Set(MODEL *model, VDX9RENDER *rs)
{
    auto *n = model->GetNode(0);
    if (!n)
        return;
    if (n->GetTechnique()[0])
        return;
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    if (chrAlpha < 0.0f)
        chrAlpha = 0.0f;
    if (chrAlpha > 1.0f)
        chrAlpha = 1.0f;
    if (camAlpha < 0.0f)
        camAlpha = 0.0f;
    if (camAlpha > 1.0f)
        camAlpha = 1.0f;
    auto a = camAlpha * chrAlpha * alpha;
    if (!isVisible)
        a = 0.0f;
    if (a < 0.5f)
        rs->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    if (a > 1.0f)
        a = 1.0f;
    if (selected < 0.0f)
        selected = 0.0f;
    if (selected > 1.0f)
        selected = 1.0f;
    const auto r = static_cast<long>(0x40 * selected);
    const auto g = static_cast<long>(0x10 * selected);
    const auto b = static_cast<long>(0x10 * selected);
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, (static_cast<uint32_t>(a * 255.0f) << 24) | (r << 16) | (g << 8) | b);
    if (selected > 0.0f)
    {
        if (a >= 1.0f)
        {
            n->SetTechnique("AnimationSelected");
        }
        else
        {
            n->SetTechnique("AnimationSelectedBlend");
        }
    }
    else
    {
        if (a >= 1.0f)
        {
            n->SetTechnique("Animation");
        }
        else
        {
            n->SetTechnique("AnimationBlend");
        }
    }
    if (auto *const location = character->GetLocation())
    {
        auto *ls = location->GetLights();
        if (ls)
            ls->SetCharacterLights(&character->curPos);
    }
}

void Character::RTuner::Restore(MODEL *model, VDX9RENDER *rs)
{
    if (auto *const location = character->GetLocation())
    {
        auto *ls = location->GetLights();
        if (ls)
            ls->DelCharacterLights();
    }
    auto *n = model->GetNode(0);
    if (!n)
        return;
    const auto *const chr = n->GetTechnique();
    if (*((uint32_t *)chr) != 'minA' || *((uint32_t *)(chr + 4)) != 'oita')
        return;
    n->SetTechnique("");
    rs->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

float Character::RTuner::GetAlpha() const
{
    auto a = camAlpha * chrAlpha * alpha;
    if (a > 1.0f)
        a = 1.0f;
    if (a < 0.0f)
        a = 0.0f;
    return a;
}

//Принять событие
void Character::EventListener::Event(Animation *animation, long index, long eventID, AnimationEvent event)
{
    if (!animation || index != 0)
        return;
    const auto *const c = animation->Player(0).GetAction();
    if (!c)
        return;
    character->ActionEvent(c, animation, index, eventID, event);
}

//Принять событие
void Character::EventListener::Event(Animation *animation, long playerIndex, const char *eventName)
{
    if (!animation || !eventName || playerIndex != 0)
        return;
    character->ActionEvent(animation, playerIndex, eventName);
}

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

Character::Character()
{
    isPlayerEnemy = false;
    enemyBarsAlpha = 0.0f;
    isLookFromEyes = false;
    soundService = nullptr;
    currentNode = -1;
    tuner.character = this;
    loc_id = {};
    //Размеры персонажа
    radiusNrm = 0.7f;
    radiusFgt = 1.5f;
    radius = radiusNrm;
    isDeleted = false;
    curPos = oldPos = grsPos = 0.0f;
    vy = 0.0f;
    ay = 0.0f;
    nay = 0.0f;
    turnDir = 0.0f;
    isTurnLock = false;
    height = 1.8f;
    //Действия
    turnSpeed = 1.0f;
    //Ходьба вперёд
    walk.SetName("walk");
    //Ходьба назад
    backwalk.SetName("back walk");
    //Бег вперёд
    run.SetName("run");
    //Бег назад
    backrun.SetName("back run");
    //Идти вверх по лестнице
    stsUp.SetName("stairs up");
    //Идти вниз по лестнице
    stsDown.SetName("stairs down");
    //Идти вверх по лестнице назад
    stsUpBack.SetName("back stairs up");
    //Идти вниз по лестнице назад
    stsDownBack.SetName("back stairs down");
    //Бежать вверх по лестнице
    stsUpRun.SetName("run stairs up");
    //Бежать вниз по лестнице
    stsDownRun.SetName("run stairs down");
    //Бежать вверх по лестнице назад
    stsUpRunBack.SetName("back run stairs up");
    //Бежать вниз по лестнице назад
    stsDownRunBack.SetName("back run stairs down");
    //Плыть
    swim.SetName("swim");
    //Спрыгивать
    jump.SetName("jump");
    fall.SetName("fall");
    fall_land.SetName("fall_land");
    fall_water.SetName("fall_water");
    //Реакция вне режима боя
    nfhit.SetName("HitNoFight");
    //Действие поворота влево
    actionTurnL.SetName("turn left");
    //Действие поворота вправо
    actionTurnR.SetName("turn right");
    //Действие стояния
    actionIdle[0].SetName("idle_1");
    actionIdle[1].SetName("idle_2");
    actionIdle[2].SetName("idle_3");
    actionIdle[3].SetName("idle_4");
    actionIdle[4].SetName("idle_5");
    actionIdle[5].SetName("idle_6");
    actionIdle[6].SetName("idle_7");
    actionIdle[7].SetName("idle_8");
    actionIdle[8].SetName("idle_9");
    actionIdle[9].SetName("idle_10");
    actionIdle[10].SetName("idle_11");
    numActionIdles = 11;
    //Режим боя
    //Действие стояния в режиме боя
    actionFightIdle[0].SetName("fight stand_1");
    actionFightIdle[1].SetName("fight stand_2");
    actionFightIdle[2].SetName("fight stand_3");
    actionFightIdle[3].SetName("fight stand_4");
    actionFightIdle[4].SetName("fight stand_5");
    actionFightIdle[5].SetName("fight stand_6");
    actionFightIdle[6].SetName("fight stand_7");
    // actionFightIdle[7].SetName("fight stand_8");
    numFightActionIdles = 7;
    //Ходьба вперёд в режиме боя
    fightwalk.SetName("fight walk");
    //Ходьба назад в режиме боя
    fightbackwalk.SetName("fight back walk");
    //Бег вперёд в режиме боя
    fightrun.SetName("fight run");
    //Бег назад в режиме боя
    fightbackrun.SetName("fight back run");
    //Смерть
    actionDead[0].SetName("death_citizen_1");
    actionDead[1].SetName("death_citizen_2");
    numActionDead = 2;
    actionFightDead[0].SetName("death_2");
    actionFightDead[1].SetName("death_2");
    actionFightDead[2].SetName("death_2");
    actionFightDead[3].SetName("death_2");
    numActionFightDead = 4;
    //Атаки блоки финты парирование
    char buf[64];
    for (long i = 0; i < 4; i++)
    {
        sprintf_s(buf, sizeof(buf) - 1, "attack_fast_%i", i + 1);
        attackFast[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "attack_force_%i", i + 1);
        attackForce[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "attack_round_%i", i + 1);
        attackRound[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "attack_break_%i", i + 1);
        attackBreak[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "attack_feint_%i", i + 1);
        attackFeint[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "attack_feintc_%i", i + 1);
        attackFeintC[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "hit_attack_%i", i + 1);
        hit[i].SetName(buf);
        sprintf_s(buf, sizeof(buf) - 1, "parry_%i", i + 1);
        parry[i].SetName(buf);
    }
    numAttackFast = 3;
    numAttackForce = 4;
    numAttackRound = 2;
    numAttackBreak = 4;
    numAttackFeint = 4;
    numParry = 4;
    shot.SetName("Shot");
    numHits = 3;
    hitFeint.SetName("hit_feint");
    hitParry.SetName("hit_parry");
    hitRound.SetName("hit_round");
    hitFire.SetName("hit_fire");
    block.SetName("block");
    blockaxe.SetName("block_axe");
    blockhit.SetName("block_hit");
    blockaxehit.SetName("block_axe_hit");
    blockbreak.SetName("block_break");
    recoil.SetName("recoil");
    strafe_l.SetName("strafeleft");
    strafe_r.SetName("straferight");
    fgtCurType = fgtSetType = fgt_none;
    fgtCurIndex = fgtSetIndex = -1;
    isParryState = false;
    isFeintState = false;
    isStunEnable = true;
    //
    isMove = false;
    isBack = false;
    isRun = false;
    isRunDisable = false;
    isFight = false;
    isDialog = false;
    isTurn = 0.0f;
    isFightWOWps = false;
    movecs = 1.0f;
    curMove = nullptr;
    speed = 0.0f;
    turnspd = turnSpeed;
    chclass = 0.5f;
    fatigue = 0.0f;
    kSpd = 1.0f;
    kSpdCur = 1.0f;
    kSpdDlt = rand() * (0.1f / RAND_MAX) - 0.05f;
    numDetectors = 0;
    startColCharacter = 0;
    numColCharacter = 0;
    isSlide = false;
    liveValue = 0.0f;
    eventListener.character = this;
    ayStackPointer = 0;
    isResetAutoAction = true;
    isSetPriorityAction = false;
    deadName = nullptr;
    curIdleIndex = -1;
    isFired = true;
    isLockIdleForCamera = false;
    isSwim = false;
    swimChange = 0.0f;
    priorityActionMoveSpd = 0.0f;
    priorityActionRotSpd = 0.0f;
    isBladeSet = false;
    isGunSet = false;
    isJump = false;
    isJumpSnd = false;
    isEnableJump = false;
    jumpSound = SOUND_INVALID_ID;
    isNFHit = false;
    characterID = nullptr;
    lockMove = false;
    lockRotate = false;
    impulse = 0.0f;
    strafeMove = 0.0f;
    recoilWait = 0.0f;
    strafeWait = 0.0f;
    camRotWait = 0.0f;
    isMale = true;
    recoilSound = SOUND_INVALID_ID;
    soundStep = false;
    soundGrass = false;
    // hScale = 1.0f;//0.9f + rand()*0.2f/RAND_MAX;
    // wScale = 1.2f;//0.95f + rand()*0.1f/RAND_MAX;
    group[0] = 0;
    numTargets = 0;
    groupID = -1;
    lockFightMode = false;
    stepsRate = 0.0f;
    noBlendTime = 1.0f;
    isActiveState = false;
    strafeAngle = 0.0f;
    strafeVel = 0.0f;

    m_pcHandLightLocator = "Saber_hand";
    m_nHandLightID = -1;
}

Character::~Character()
{
    auto *m = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdl));
    if (m)
    {
        auto *a = m->GetAnimation();
        if (a)
        {
            a->SetEventListener(nullptr);
            if (eventId)
            {
                a->DelEvent(eventId);
            }
        }
    }

    auto *const location = GetLocation();
    // удаляем источник в руке
    if (m_nHandLightID >= 0 && location && !isDeleted)
        location->GetLights()->DelMovingLight(m_nHandLightID);
    m_nHandLightID = -1;

    //Удаляемся из групп
    core.Send_Message(EntityManager::GetEntityId("CharactersGroups"), "si", "UnloadCharacter", GetId());

    //Анализируем детекторы
    //	for(long i = 0; i < numDetectors; i++) detector[i]->Exit(this);
    //Удаляем детекторы
    for (long i = 0; i < numDetectors; i++)
        delete detector[i];
    //
    if (location && !isDeleted)
        location->supervisor.DelCharacter(this);
    EntityManager::EraseEntity(shadow);
    EntityManager::EraseEntity(waterrings);
    EntityManager::EraseEntity(mdl);
    EntityManager::EraseEntity(blade);
    EntityManager::EraseEntity(sign);
    delete characterID;
}

//Инициализация
bool Character::Init()
{
    //Указатель на локацию
    auto *const location = GetLocation();
    if (!location)
        return false;
    effects = EntityManager::GetEntityId("LocationEffects");
    soundService = static_cast<VSoundService *>(core.CreateService("SoundService"));
    //Регестрируем своё попадание в локацию
    if (location->supervisor.numCharacters >= MAX_CHARACTERS)
    {
        core.Trace("Many characters in location");
        return false;
    }
    location->supervisor.AddCharacter(this);
    //Море
    sea = EntityManager::GetEntityId("sea");
    //Сохраним идентификатор
    const char *id = nullptr;
    if (AttributesPointer)
        id = AttributesPointer->GetAttribute("id");
    if (!id)
        id = "<none>";
    const long len = strlen(id) + 1;
    characterID = new char[len];
    strcpy_s(characterID, len, id);
    //Добавим в группу
    core.Send_Message(EntityManager::GetEntityId("CharactersGroups"), "sis", "MoveCharacter", GetId(), group);
    SetSignModel();
    SetSignTechnique();
    return PostInit();
}

//Сообщения
uint64_t Character::ProcessMessage(MESSAGE &message)
{
    if (message.GetCurrentFormatType() == 's')
    {
        return zPlaySound(message);
    }
    const auto messageID = message.Long();
    if (messageID == MSG_CHARACTER_EX_MSG)
    {
        return zExMessage(message);
    }
    if (deadName)
    {
        if (liveValue >= 0)
        {
            if (messageID == MSG_CHARACTER_EXIT_FROM_LOCATION)
            {
                ExitFromLocation();
                DelSavePosition(false);
                return 1;
            }
        }
        return 0;
    }
    switch (messageID)
    {
    case MSG_CHARACTER_SETMODEL:
        return zLoadModel(message);
    case MSG_CHARACTER_GETMODEL:
        message.ScriptVariablePointer()->Set(mdl);
        return 1;
    case MSG_CHARACTER_GETPOS:
        message.ScriptVariablePointer()->Set(curPos.x);
        message.ScriptVariablePointer()->Set(curPos.y);
        message.ScriptVariablePointer()->Set(curPos.z);
        return 1;
    case MSG_CHARACTER_GETAY:
        message.ScriptVariablePointer()->Set(ay);
        return 1;
    case MSG_CHARACTER_DIST2D:
        return zDistByCharacter(message, true);
    case MSG_CHARACTER_DIST3D:
        return zDistByCharacter(message, false);
    case MSG_CHARACTER_TELEPORT:
        return zTeleport(message, false);
    case MSG_CHARACTER_TELEPORT_AY:
        return zTeleport(message, true);
    case MSG_CHARACTER_TELEPORT_TO_LOCATOR:
        return zTeleportL(message);
    case MSG_CHARACTER_ADD_DETECTOR:
        return zAddDetector(message);
    case MSG_CHARACTER_DEL_DETECTOR:
        return zDelDetector(message);
    case MSG_CHARACTER_ENTRY_TO_LOCATION:
        return zEntry(message);
    case MSG_CHARACTER_EXIT_FROM_LOCATION:
        ExitFromLocation();
        DelSavePosition(false);
        return 1;
    case MSG_CHARACTER_TURNBYLOC:
        return zTurnByLoc(message);
    case MSG_CHARACTER_TURNBYCHR:
        return zTurnByChr(message);
    case MSG_CHARACTER_TURNBYPOINT:
        return zTurnByPoint(message);
    case MSG_CHARACTER_TURNAY:
        Turn(message.Float());
        return 1;
    case MSG_CHARACTER_TURNPUSH:
        if (ayStackPointer >= sizeof(ayStack) / sizeof(float))
            return 0;
        ayStack[ayStackPointer++] = nay;
        return 1;
    case MSG_CHARACTER_TURNPOP:
        if (ayStackPointer <= 0)
            return 0;
        Turn(ayStack[--ayStackPointer]);
        return 1;
    case MSG_CHARACTER_SETBLADE:
        return zSetBlade(message);
    case MSG_CHARACTER_SETGUN:
        return zSetGun(message);
    case MSG_CHARACTER_ACTIONPLAY:
        return zActionPlay(message);
    case MSG_CHARACTER_FINDNEAR:
        FindNearCharacters(message);
        return 1;
    case MSG_CHARACTER_VISIBLE:
        return CharactersVisibleTest(message);
    case MSG_CHARACTER_BLADEHAND:
        if (message.Long() == 0)
        {
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 1);
        }
        else
        {
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
        }
        return 1;
    case MSG_CHARACTER_VIEWDAMAGE: {
        auto *const location = GetLocation();
        float hit = message.Float();
        float chp = message.Float();
        float mhp = message.Float();
        if (hit < 0.0f)
            hit = 0.0f;
        if (chp < 0.0f)
            chp = 0.0f;
        if (mhp < 0.0f)
            mhp = 0.0f;
        location->AddDamageMessage(curPos + CVECTOR(0.0f, height, 0.0f), hit, chp, mhp);
    }
        return 1;
    default:
        return ChlProcessMessage(messageID, message);
    }
    return 0;
}

//Изменение атрибута
uint32_t Character::AttributeChanged(ATTRIBUTES *apnt)
{
    if (deadName || liveValue < 0)
        return 0;
    if (!apnt || !apnt->GetThisName())
        return 0;
    if (_stricmp(apnt->GetThisName(), "model") == 0)
    {
        SetSignModel();
    }
    else if (_stricmp(apnt->GetThisName(), "technique") == 0)
    {
        SetSignTechnique();
    }
    else if (_stricmp(apnt->GetThisName(), "id") == 0 && apnt->GetParent() && !apnt->GetParent()->GetParent())
    {
        const char *id = apnt->GetThisAttr();
        if (!id)
            id = "<none>";
        delete characterID;
        const long len = strlen(id) + 1;
        characterID = new char[len];
        strcpy_s(characterID, len, id);
    }
    else if (_stricmp(apnt->GetThisName(), "actions") == 0 && apnt->GetParent() && !apnt->GetParent()->GetParent())
    {
        //Читаем действия перемещения
        //Простые
        walk.ChangeName(apnt->GetAttribute("walk"));
        backwalk.ChangeName(apnt->GetAttribute("backwalk"));
        run.ChangeName(apnt->GetAttribute("run"));
        backrun.ChangeName(apnt->GetAttribute("backrun"));
        stsUp.ChangeName(apnt->GetAttribute("stsUp"));
        stsDown.ChangeName(apnt->GetAttribute("stsDown"));
        stsUpBack.ChangeName(apnt->GetAttribute("stsUpBack"));
        stsDownBack.ChangeName(apnt->GetAttribute("stsDownBack"));
        stsUpRun.ChangeName(apnt->GetAttribute("stsUpRun"));
        stsDownRun.ChangeName(apnt->GetAttribute("stsDownRun"));
        stsUpRunBack.ChangeName(apnt->GetAttribute("stsUpRunBack"));
        stsDownRunBack.ChangeName(apnt->GetAttribute("stsDownRunBack"));
        swim.ChangeName(apnt->GetAttribute("swim"));
        nfhit.ChangeName(apnt->GetAttribute("HitNoFight"));
        //Читаем действия поворота
        actionTurnL.ChangeName(apnt->GetAttribute("turnLeft"));
        actionTurnR.ChangeName(apnt->GetAttribute("turnRight"));
        //Читаем действия смерти
        ATTRIBUTES *at = apnt->FindAClass(apnt, "dead");
        if (at)
        {
            const long num = at->GetAttributesNum();
            long j = 0;
            for (long i = 0; i < num && j < sizeof(actionDead) / sizeof(ActionDead); i++)
            {
                const char *iname = at->GetAttribute(i);
                if (!iname || !iname[0])
                    continue;
                if (j == 0)
                    actionDead[j++].ChangeName(iname);
                else
                    actionDead[j++].SetName(iname);
            }
            if (j < 1)
                j = 1;
            numActionDead = j;
        }
        //Читаем действия простого стояния
        at = apnt->FindAClass(apnt, "idle");
        if (at)
        {
            curIdleIndex = -1;
            const long num = at->GetAttributesNum();
            long j = 0;
            for (long i = 0; i < num && j < sizeof(actionIdle) / sizeof(ActionIdle); i++)
            {
                const char *iname = at->GetAttribute(i);
                if (!iname || !iname[0])
                    continue;
                if (j == 0)
                    actionIdle[j++].ChangeName(iname);
                else
                    actionIdle[j++].SetName(iname);
            }
            if (j < 1)
                j = 1;
            numActionIdles = j;
        }
        //Читаем действия в режиме боя
        //Перемещение
        fightwalk.ChangeName(apnt->GetAttribute("fightwalk"));
        fightbackwalk.ChangeName(apnt->GetAttribute("fightbackwalk"));
        fightrun.ChangeName(apnt->GetAttribute("fightrun"));
        fightbackrun.ChangeName(apnt->GetAttribute("fightbackrun"));
        //Прямые атаки
        ReadFightActions(apnt->FindAClass(apnt, "attack_fast"), attackFast, numAttackFast);
        ReadFightActions(apnt->FindAClass(apnt, "attack_force"), attackForce, numAttackForce);
        ReadFightActions(apnt->FindAClass(apnt, "attack_round"), attackRound, numAttackRound);
        ReadFightActions(apnt->FindAClass(apnt, "attack_break"), attackBreak, numAttackBreak);
        long fnt1, fnt2;
        ReadFightActions(apnt->FindAClass(apnt, "attack_feint"), attackFeint, fnt1);
        ReadFightActions(apnt->FindAClass(apnt, "attack_feintc"), attackFeintC, fnt2);
        numAttackFeint = std::min(fnt1, fnt2);
        ReadFightActions(apnt->FindAClass(apnt, "hit_attack"), hit, numHits);
        ReadFightActions(apnt->FindAClass(apnt, "parry"), parry, numParry);
        //Выстрел
        shot.ChangeName(apnt->GetAttribute("shot"));
        //Попадания
        hitFeint.ChangeName(apnt->GetAttribute("hit_feint"));
        hitParry.ChangeName(apnt->GetAttribute("hit_parry"));
        hitRound.ChangeName(apnt->GetAttribute("hit_round"));
        hitFire.ChangeName(apnt->GetAttribute("hit_shot"));
        block.ChangeName(apnt->GetAttribute("block"));
        blockaxe.ChangeName(apnt->GetAttribute("block_axe"));
        blockhit.ChangeName(apnt->GetAttribute("blockhit"));
        blockaxehit.ChangeName(apnt->GetAttribute("blockaxehit"));
        blockbreak.ChangeName(apnt->GetAttribute("blockbreak"));
        recoil.ChangeName(apnt->GetAttribute("recoil"));
        strafe_l.ChangeName(apnt->GetAttribute("strafeleft"));
        strafe_r.ChangeName(apnt->GetAttribute("straferight"));
        //Читаем действия смерти
        at = apnt->FindAClass(apnt, "fightdead");
        if (at)
        {
            const long num = at->GetAttributesNum();
            long j = 0;
            for (long i = 0; i < num && j < sizeof(actionFightDead) / sizeof(ActionDead); i++)
            {
                const char *iname = at->GetAttribute(i);
                if (!iname || !iname[0])
                    continue;
                if (j == 0)
                    actionFightDead[j++].ChangeName(iname);
                else
                    actionFightDead[j++].SetName(iname);
            }
            if (j < 1)
                j = 1;
            numActionFightDead = j;
        }
        //Стояния
        at = apnt->FindAClass(apnt, "fightidle");
        if (at)
        {
            const long num = at->GetAttributesNum();
            long j = 0;
            for (long i = 0; i < num && j < sizeof(actionFightIdle) / sizeof(ActionIdle); i++)
            {
                const char *iname = at->GetAttribute(i);
                if (!iname || !iname[0])
                    continue;
                if (j == 0)
                    actionFightIdle[j++].ChangeName(iname);
                else
                    actionFightIdle[j++].SetName(iname);
            }
            if (j < 1)
                j = 1;
            numFightActionIdles = j;
        }
        //Обновляем параметры
        UpdateActionsData();
        //Обновляем проигрываемую анимацию
        UpdateAnimation();
    }
    return 0;
}

void Character::SetSignModel()
{
    ATTRIBUTES *apnt = AttributesPointer->FindAClass(AttributesPointer, "quest.questflag.model");
    const char *signModelName = "";
    if (apnt)
    {
        signModelName = apnt->GetThisAttr();
    }
    if (!signModelName)
        signModelName = "";
    if (signName == signModelName)
    {
        return;
    }
    signName = signModelName;
    EntityManager::EraseEntity(sign);
    if (!signModelName[0])
    {
        return;
    }
    //Путь до текстур
    VGEOMETRY *gs = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    if (gs)
        gs->SetTexturePath("quest_signs\\");
    //Путь до модельки
    std::string path = "quest_signs\\";
    path += signModelName;
    //Создаём и загружаем модельку
    if (!(sign = EntityManager::CreateEntity("modelr")))
    {
        if (gs)
            gs->SetTexturePath("");
        return;
    }
    if (!core.Send_Message(sign, "ls", MSG_MODEL_LOAD_GEO, path.c_str()))
    {
        if (gs)
            gs->SetTexturePath("");
        core.Trace("Quest sign model '%s' not loaded", path.c_str());
        return;
    }

    if (!signTechniqueName.empty())
    {
        core.Send_Message(sign, "ls", MSG_MODEL_SET_TECHNIQUE, signTechniqueName.c_str());
    }

    if (gs)
        gs->SetTexturePath("");
    EntityManager::AddToLayer(REALIZE, sign, 20000);
    EntityManager::AddToLayer(SUN_TRACE, sign, 10);
}

void Character::SetSignTechnique()
{
    // set technique
    ATTRIBUTES *pATechnique = AttributesPointer->FindAClass(AttributesPointer, "quest.questflag.technique");
    if (!pATechnique)
        return;

    char *pcTechniqueName = pATechnique->GetThisAttr();
    if (!pcTechniqueName)
        return;

    if (signTechniqueName == pcTechniqueName)
        return;
    signTechniqueName = pcTechniqueName;

    core.Send_Message(sign, "ls", MSG_MODEL_SET_TECHNIQUE, pcTechniqueName);
}

void Character::ReadFightActions(ATTRIBUTES *at, ActionCharacter actions[4], long &counter)
{
    if (at)
    {
        const long num = at->GetAttributesNum();
        long j = 0;
        for (long i = 0; i < num && j < 4; i++)
        {
            const char *iname = at->GetAttribute(i);
            if (!iname || !iname[0])
                continue;
            if (j == 0)
            {
                actions[j++].ChangeName(iname);
            }
            else
            {
                actions[j++].SetName(iname);
            }
        }
        if (j < 1)
            j = 1;
        counter = j;
    }
    else
    {
        counter = 1;
    }
}

//============================================================================================
// Character model
//============================================================================================

MODEL *Character::Model() const
{
    return static_cast<MODEL *>(EntityManager::GetEntityPointer(mdl));
}

//Переместить модельку в точку x, y, z
bool Character::Teleport(float x, float y, float z)
{
    auto *const location = GetLocation();
    //Модель
    isJump = false;
    MODEL *man = Model();
    if (!man)
        return false;
    //Новые координаты
    man->mtx.SetPosition(x, y, z);
    curPos = oldPos = grsPos = CVECTOR(x, y, z);
    vy = 0.0f;
    float bearingY;
    currentNode = location->GetPtcData().FindNode(curPos, bearingY);
    if (curPos.y < bearingY)
        curPos.y = bearingY;
    CharacterTeleport();
    return true;
}

//Переместить модельку в точку x, y, z и направить по ay
bool Character::Teleport(float x, float y, float z, float ay)
{
    auto *const location = GetLocation();
    //Модель
    isJump = false;
    MODEL *man = Model();
    if (!man)
        return false;
    //Новые координаты
    man->mtx.BuildMatrix(0.0f, ay, 0.0f, x, y, z);
    curPos = oldPos = grsPos = CVECTOR(x, y, z);
    vy = 0.0f;
    Turn(ay);
    this->ay = nay;
    float bearingY;
    currentNode = location->GetPtcData().FindNode(curPos, bearingY);
    if (curPos.y < bearingY)
        curPos.y = bearingY;
    CharacterTeleport();
    return true;
}

//Переместить модельку в локатор
bool Character::Teleport(const char *group, const char *locator)
{
    auto *const location = GetLocation();
    if (!group || !group[0] || !locator || !locator[0])
        return false;
    LocatorArray *la = location->FindLocatorsGroup(group);
    if (!la)
        return false;
    const long li = la->FindByName(locator);
    if (li < 0)
        return false;
    CMatrix mtx;
    if (!la->GetLocatorPos(li, mtx))
        return false;
    //Ищем угл поворота по Y
    double vz = mtx.Vz().z;
    const double vx = mtx.Vz().x;
    const double l = vx * vx + vz * vz;
    if (l > 0.0000001)
    {
        //Ищем углы
        vz = acos(vz / sqrt(l));
    }
    else
        vz = l;
    if (vx < 0)
        vz = -vz;

    // смотрим можем ли мы поставить в этот локатор чела
    const CVECTOR pos = mtx.Pos();
    if (location->supervisor.CheckPosition(pos.x, pos.y, pos.z, this))
        return Teleport(pos.x, pos.y, pos.z, static_cast<float>(vz));

    // кто-то стоит рядом, пытаемся найти точку по небольшому радиусу
    const float radius = 1.75f;
    for (long i = 0; i < 10; i++)
    {
        const float ang = static_cast<float>(i) / 9.0f * PIm2;
        const float x = radius * sinf(ang);
        const float z = radius * cosf(ang);

        // проверяем есть ли тут патч
        CVECTOR src = CVECTOR(pos.x + x, pos.y + 2.0f, pos.z + z);
        CVECTOR dst = CVECTOR(pos.x + x, pos.y - 2.0f, pos.z + z);
        const float k = location->GetPtcData().Trace(src, dst);
        // если нету патча - то следующая итерация
        if (k > 1.0f)
            continue;
        // если точка свободна - телепортимся туда
        const CVECTOR pnt = src + (dst - src) * k + CVECTOR(0.0f, 0.01f, 0.0f);
        if (location->supervisor.CheckPosition(pnt.x, pnt.y, pnt.z, this))
            return Teleport(pnt.x, pnt.y, pnt.z, static_cast<float>(vz));
    }

    core.Trace("Character Teleport Error: Can't find free place near locator: %s, %s", group, locator);
    return Teleport(pos.x, pos.y, pos.z, static_cast<float>(vz));
}

//Установить позиции для загрузки
void Character::SetSavePosition()
{
    if (!AttributesPointer)
        return;
    ATTRIBUTES *at = AttributesPointer->CreateSubAClass(AttributesPointer, "saveposition");
    at->SetAttributeUseFloat("x", curPos.x);
    at->SetAttributeUseFloat("y", curPos.y);
    at->SetAttributeUseFloat("z", curPos.z);
    at->SetAttributeUseFloat("ay", ay);
    at = at->CreateSubAClass(at, "savedata");
    SetSaveData(at);
}

//Удалить позиции для загрузки
void Character::DelSavePosition(bool isTeleport)
{
    if (!AttributesPointer)
        return;

    ATTRIBUTES *aPosLocator = AttributesPointer->FindAClass(AttributesPointer, "location");
    if (aPosLocator)
    {
        const char *pcLocGroupName = aPosLocator->GetAttribute("group");
        if (pcLocGroupName && _stricmp(pcLocGroupName, "sit") == 0)
            isTeleport = false;
    }

    ATTRIBUTES *at = AttributesPointer->FindAClass(AttributesPointer, "saveposition");
    if (at)
    {
        if (isTeleport)
        {
            curPos.x = at->GetAttributeAsFloat("x", curPos.x);
            curPos.y = at->GetAttributeAsFloat("y", curPos.y);
            curPos.z = at->GetAttributeAsFloat("z", curPos.z);
            ay = at->GetAttributeAsFloat("ay", ay);
            Teleport(curPos.x, curPos.y, curPos.z, ay);
            GetSaveData(at->FindAClass(at, "savedata"));
        }
        AttributesPointer->DeleteAttributeClassX(at);
    }
}

//============================================================================================
// Character commands
//============================================================================================

//Пойти
void Character::StartMove(bool isBack)
{
    if (priorityAction.name)
        return;
    isMove = true;
    this->isBack = isBack;
}

//Остановиться
void Character::StopMove()
{
    if (priorityAction.name)
        return;
    isMove = false;
}

//Направить персонажа по вектору
void Character::Turn(float dx, float dz)
{
    // if(priorityAction.name) return;
    const double vx = dx;
    double vz = dz;
    const double l = vx * vx + vz * vz;
    if (l <= 0.0)
        return;
    vz = acos(vz / sqrt(l));
    if (vx < 0)
        vz = -vz;
    Turn(static_cast<float>(vz));
}

//Направить персонажа по углу
void Character::Turn(float _ay)
{
    if (isTurnLock)
        return;
    const float pi = 3.14159265359f;
    nay = _ay;
    nay -= static_cast<long>(nay / pi) * 2.0f * pi;
    if (nay - ay > pi)
        ay += 2 * pi;
    if (nay - ay < -pi)
        ay -= 2 * pi;
    if (fabs(nay - ay) > 0.00001f)
    {
        if (ay < nay)
            turnDir = 1.0f;
        else
            turnDir = -1.0f;
    }
}

//Установить режим бега
void Character::SetRunMode(bool _isRun)
{
    if (priorityAction.name)
        return;
    isRun = _isRun;
}

//Устанорвить режим боя
bool Character::SetFightMode(bool _isFight, bool isPlayAni)
{
    UpdateWeapons();
    if (lockFightMode)
        return isFight;
    if (!isFightWOWps)
    {
        if (!isBladeSet)
            return isFight;
    }
    if (isFight == _isFight)
        return isFight;
    if (priorityAction.name)
        return isFight;
    if (isSwim)
        return isFight;
    if (!IsFightEnable())
    {
        if (!isFight)
            return false;
        _isFight = false;
    }
    isNFHit = false;
    const bool old = isFight;
    isFight = _isFight;
    if (isFight)
    {
        radius = radiusFgt;
        if (isPlayAni)
        {
            if (!SetPriorityAction(CHARACTER_NORM_TO_FIGHT))
            {
                core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
                core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
            }
        }
        else
        {
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
        }
    }
    else
    {
        radius = radiusNrm;
        if (isPlayAni)
        {
            if (!SetPriorityAction(CHARACTER_FIGHT_TO_NORM))
            {
                core.Send_Message(blade, "ll", MSG_BLADE_BELT, 0);
                core.Send_Message(blade, "ll", MSG_BLADE_BELT, 1);
            }
        }
        else
        {
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
        }
        fgtCurType = fgtSetType = fgt_none;
        fgtCurIndex = fgtSetIndex = -1;
    }
    core.Event("Character_ChangeFightMode", "ll", isFight, old);
    return old;
}

//Проверить, можно ли переходить в режим боя
bool Character::IsFightEnable() const
{
    //Спросим у скрипта о возможности стрельбы
    VDATA *vd = core.Event("Location_CharacterIsFight", "i", GetId());
    long res = 0;
    if (vd && vd->Get(res))
    {
        if (!res)
            return false;
    }
    else
    {
        //!!!		core.Trace("Event \"Location_CharacterIsFight\" -> return type is not int");
        return true;
    }
    return true;
}

//В данный момент идёт выбор цели для выстрела (игроком)
bool Character::IsFireFindTarget() const
{
    if (!priorityAction.name || !shot.name)
        return false;
    if (_stricmp(priorityAction.name, shot.name) == 0)
        return !isFired;
    return false;
}

//Атаковать
void Character::Attack(Character *enemy, FightAction type)
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (!isFight)
        return;
    if (type < fgt_attack_fast || type > fgt_attack_feintc)
        return;
    // if(!fightTbl[fgtCurType][type]) return;
    if (enemy)
    {
        enemyAttack = enemy->GetId();
        const float dx = enemy->curPos.x - curPos.x;
        const float dz = enemy->curPos.z - curPos.z;
        float cs = dx * dx + dz * dz;
        Turn(dx, dz);
        isTurnLock = true;
        /*
        if(cs > 0.0f)
        {
          float cdx = sinf(ay);
          float cdz = cosf(ay);
          cs = (cdx*dx + cdz*dz)/sqrtf(cs);
          static float csang = cosf(60.0f*3.141592654f/180.0f);
          if(cs >= csang)
          {
            Turn(dx, dz);
            isTurnLock = true;
          }
        }*/
    }
    else
        memset(&enemyAttack, 0, sizeof(enemyAttack));
    VDATA *res = nullptr;
    const char *aname = nullptr;
    switch (type)
    {
    case fgt_attack_fast:
        aname = FGT_ATTACK_FAST;
        fgtSetIndex = rand() % numAttackFast;
        break;
    case fgt_attack_force:
        aname = FGT_ATTACK_FORCE;
        ObstacleZone zone[4];
        // выпад 1
        zone[0].use = true;
        zone[0].x = 0.f;
        zone[0].z = 1.f;
        zone[0].dw = 0.001f;
        // выпад 2
        zone[1].use = true;
        zone[1].x = 0.f;
        zone[1].z = 1.f;
        zone[1].dw = 0.001f;
        // выпад 3
        zone[2].use = true;
        zone[2].x = 0.f;
        zone[2].z = 1.f;
        zone[2].dw = 0.001f;
        // выпад 4
        zone[3].use = true;
        zone[3].x = 0.f;
        zone[3].z = 1.f;
        zone[3].dw = 0.001f;
        fgtSetIndex = GetRandomIndexByObstacle(zone, 4);
        //если выпад на заступ за патч, то производим быстрый удар, а не выпад
        if (fgtSetIndex < 0)
        {
            type = fgt_attack_fast;
            aname = FGT_ATTACK_FAST;
            fgtSetIndex = rand() % numAttackFast;
        }
        break;
    case fgt_attack_round:
        aname = FGT_ATTACK_ROUND;
        fgtSetIndex = rand() % numAttackRound;
        break;
    case fgt_attack_break:
        aname = FGT_ATTACK_BREAK;
        fgtSetIndex = rand() % numAttackBreak;
        break;
    case fgt_attack_feint:
        aname = FGT_ATTACK_FEINT;
        fgtSetIndex = rand() % numAttackFeint;
        break;
    case fgt_attack_feintc:
        fgtSetType = fgt_attack_feintc; // fix boal
        // fgtCurType =
        if (fgtCurIndex >= 0 && fgtCurIndex < numAttackFeint)
        {
            fgtSetIndex = fgtCurIndex;
        }
        else
        {
            fgtSetIndex = rand() % numAttackFeint;
        }
        return; // fix boal
        break;
    default:
        fgtSetType = fgt_none;
        fgtSetIndex = -1;
        return;
    }
    if (aname) //~!~
    {
        res = core.Event("ChrAttackAction", "is", GetId(), aname);
        if (res)
        {
            long isEnable = 1;
            if (res->Get(isEnable))
            {
                if (!isEnable)
                {
                    res = nullptr;
                }
            }
            else
            {
                res = nullptr;
            }
        }
    }
    if (res)
    {
        fgtSetType = type;
    }
    else
    {
        fgtSetType = fgt_none;
        fgtSetIndex = -1;
    }
}

//Блок
void Character::Block()
{
    // boal прервывние всего блоком if(priorityAction.name) return;
    if (isSwim)
        return;
    if (!isFight)
        return;
    if (fgtCurType != fgt_blockhit && fgtSetType != fgt_blockhit && fgtCurType != fgt_blockbreak &&
        fgtSetType != fgt_blockbreak)
    {
        fgtSetType = fgt_block;
        fgtSetIndex = 0;
        isTurnLock = false;
    }
}

//Парирование
void Character::Parry()
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (!isFight)
        return;
    fgtSetType = fgt_parry;

    // fgtSetIndex = rand() % numParry;
    ObstacleZone zone[4];
    // уход влево
    zone[0].use = true;
    zone[0].x = -1.f;
    zone[0].z = 0.f;
    zone[0].dw = 0.5f;
    // уход вправо блок сверху
    zone[1].use = true;
    zone[1].x = 1.f;
    zone[1].z = 0.f;
    zone[1].dw = 0.5f;
    // уход вправо блок снизу
    zone[2].use = true;
    zone[2].x = 1.f;
    zone[2].z = 0.f;
    zone[2].dw = 0.5f;
    // никаких уходов - на месте
    zone[3].use = true;
    zone[3].x = 0.f;
    zone[3].z = 0.f;
    zone[3].dw = 0.5f;
    fgtSetIndex = GetRandomIndexByObstacle(zone, 4);
    // core.Trace("Результат выбора: \"%i\"", fgtSetIndex);
    if (fgtSetIndex < 0)
        fgtSetIndex = 3;

    isTurnLock = false;
}

//Отскок
void Character::Recoil()
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (!isFight)
        return;
    if (recoilWait > 0.0f)
        return;
    if (!GetLocation()->IsSwimming())
        return;
    fgtSetType = fgt_recoil;
    fgtSetIndex = 0;
    recoilWait = 0.8f;
    isTurnLock = false;
}

//Пыжёк влево
void Character::StrafeLeft()
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (strafeWait > 0.0f)
        return;
    if (!GetLocation()->IsSwimming())
        return;
    strafeWait = 0.8f;
    fgtSetType = fgt_strafe_l;
    fgtSetIndex = 0;
    isTurnLock = false;
    // impulse += 15.0f*CVECTOR(-cosf(ay), 0.0f, sinf(ay));
}

//Прыжок вправо
void Character::StrafeRight()
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (strafeWait > 0.0f)
        return;
    if (!GetLocation()->IsSwimming())
        return;
    strafeWait = 0.8f;
    fgtSetType = fgt_strafe_r;
    fgtSetIndex = 0;
    isTurnLock = false;
    // impulse -= 15.0f*CVECTOR(-cosf(ay), 0.0f, sinf(ay));
}

//Финт
void Character::Hit(FightAction type)
{
    /*
    //!!!
    entid_t eid;
    if(_stricmp(characterID, "Blaze") == 0)
    {
      core.FindClass(&eid, "ILogAndActions", 0);
    }
    char sbuf[256];
    sprintf_s(sbuf, "Hit! cur act: %s", fightNamesTbl[fgtCurType]);
    core.Send_Message(eid, "lls", 45020, false, sbuf);
    //!!!
    //*/
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    isNFHit = false;
    // if(!isFight)
    {
        if (!IsDead())
        {
            isNFHit = true;
        }
    }
    impulse = 0.0f;
    const bool restBlockBreak = (fgtSetType == fgt_blockbreak);
    switch (type)
    {
    case fgt_hit_attack:
        if (fgtCurType != fgt_block && fgtCurType != fgt_blockhit)
        {
            HitChild(false);
            if (fgtSetType < fgt_hit_attack)
            {
                fgtSetType = fgt_hit_attack;
                fgtSetIndex = rand() % numHits;
            }
            // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_attack");
        }
        else
        {
            HitChild(true);
            fgtSetType = fgt_blockhit;
            fgtSetIndex = 0;
            // core.Send_Message(eid, "lls", 45020, false, "fgt_blockhit");
        }
        break;
    case fgt_hit_round:
        if (fgtCurType != fgt_block && fgtCurType != fgt_blockhit)
        {
            HitChild(false);
            fgtSetType = fgt_hit_round;
            fgtSetIndex = 0;
            // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_round");
        }
        else
        {
            HitChild(true);
            fgtSetType = fgt_blockhit;
            fgtSetIndex = 0;
            // core.Send_Message(eid, "lls", 45020, false, "fgt_blockhit");
        }
        break;
    case fgt_blockbreak:
        if (fgtCurType != fgt_block && fgtCurType != fgt_blockhit)
        {
            HitChild(false);
            fgtSetType = fgt_hit_attack;
            fgtSetIndex = rand() % numHits;
            // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_attack");
        }
        else
        {
            HitChild(true);
            fgtSetType = fgt_blockbreak;
            fgtSetIndex = 0;
            // core.Send_Message(eid, "lls", 45020, false, "fgt_blockbreak");
        }
        break;
    case fgt_hit_feint:
        HitChild(false);
        fgtSetType = fgt_hit_feint;
        fgtSetIndex = 0;
        // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_feint");
        break;
    case fgt_hit_parry:
        HitChild(true);
        fgtSetType = fgt_hit_parry;
        fgtSetIndex = 0;
        // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_parry");
        break;
    case fgt_hit_fire:
        HitChild(false);
        fgtSetType = fgt_hit_fire;
        fgtSetIndex = 0;
        // core.Send_Message(eid, "lls", 45020, false, "fgt_hit_fire");
        break;
    default:
        HitChild(false);
        fgtSetType = fgt_hit_attack;
        fgtSetIndex = rand() % numHits;
        // core.Send_Message(eid, "lls", 45020, false, "default hit!...");
    }
    if (restBlockBreak)
        fgtSetType = fgt_blockbreak;
}

//Выстрел
void Character::Fire()
{
    if (priorityAction.name)
        return;
    if (isSwim)
        return;
    if (!IsGunLoad())
        return;
    if (!GetLocation()->IsSwimming())
        return;
    //Ставим действие стрельбы
    fgtSetType = fgt_fire;
    fgtSetIndex = 0;
}

//Проверить, заряжен ли пистолет
bool Character::IsGunLoad() const
{
    if (!isGunSet)
        return false;
    if (!isFight || liveValue < 0 || deadName)
        return false;
    //Спросим у скрипта о возможности стрельбы
    VDATA *vd = core.Event("Location_CharacterIsFire", "i", GetId());
    long res = 0;
    if (vd && vd->Get(res))
    {
        if (!res)
            return false;
    }
    else
    {
        core.Trace("Event \"Location_CharacterIsFire\" -> return type is not int");
        // return false;
        //!!!
        return true;
    }
    return true;
}

//Смерть
void Character::Dead()
{
    if (deadName)
        return;
    DelSavePosition(false);
    long num = numActionDead;
    ActionDead *dead = actionDead;
    if (isFight)
    {
        num = numActionFightDead;
        dead = actionFightDead;
    }
    Assert(num);
    Assert(dead);
    //Разбрасываем веса в зависимости от направления
    const float _ay = ay;
    static Supervisor::FindCharacter fnd[MAX_CHARACTERS];
    static long numChr = 0;
    auto *const location = GetLocation();
    for (long i = 0; i < num; i++)
    {
        ay = _ay + dead[i].ang;
        if (location->supervisor.FindCharacters(fnd, numChr, this, 2.0f, 0.0f, 0.0f))
            dead[i].p *= 0.1f;
        const float cs = cosf(dead[i].ang);
        const float sn = sinf(dead[i].ang);
        CVECTOR p = curPos + CVECTOR(0.0f, 0.5f, 0.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p += CVECTOR(sn * 0.5f, 0.0f, -cs * 0.5f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p -= CVECTOR(sn * 1.0f, 0.0f, -cs * 1.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p = curPos + CVECTOR(0.0f, 1.0f, 0.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p += CVECTOR(sn * 0.5f, 0.0f, -cs * 0.5f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p -= CVECTOR(sn * 1.0f, 0.0f, -cs * 1.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.5f;
        p = curPos + CVECTOR(0.0f, 1.5f, 0.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.9f;
        p += CVECTOR(sn * 0.5f, 0.0f, -cs * 0.5f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.9f;
        p -= CVECTOR(sn * 1.0f, 0.0f, -cs * 1.0f);
        if (!location->VisibleTest(p, p + CVECTOR(cs, 0.0f, sn)))
            dead[i].p *= 0.9f;
    }
    ay = _ay;
    //Сумарный вероятностный отрезок
    float sum = 0.0f;
    long i;
    for (i = 0; i < num; i++)
        sum += dead[i].p;
    //Выбираем действия
    if (sum > 0.0f)
    {
        const float p = rand() * sum / RAND_MAX;
        sum = 0.0f;
        for (i = 0; i < num; i++)
        {
            sum += dead[i].p;
            if (sum >= p)
                break;
        }
    }
    else
    {
        i = rand() % num;
    }
    Assert(i < num);
    //Ставим действие
    deadName = dead[i].name;
    core.Event("Event_ChrSnd_Dead", "i", GetId());
}

//Заход в локацию
void Character::EntryToLocation()
{
    tuner.alpha = 0.0f;
    liveValue = 1.2f;
}

//Выход из локации
void Character::ExitFromLocation()
{
    liveValue = -1.2f;
}

void Character::StartJump()
{
    if (!isJump && BuildJump(curPos, ay))
    {
        isJump = true;
        isJumpSnd = true;
        jumpSound = SOUND_INVALID_ID;
        SetPriorityAction(jump.name);
    }
}

//============================================================================================
// Character
//============================================================================================

//Сбросить состояние
void Character::Reset()
{
    if (fgtCurType == fgt_block && !IsPlayer())
        fgtCurType = fgt_none;
}

//Перемещаем персонажа в желаемую позицию
void Character::Move(float dltTime)
{
    auto *const location = GetLocation();

    if (lockMove)
        dltTime = 0.0f;
    if (recoilWait > 0.0f)
        recoilWait -= dltTime;
    if (strafeWait > 0.0f)
        strafeWait -= dltTime;
    if (camRotWait > 0.0f)
        camRotWait -= dltTime;
    if (isTurnLock)
    {
        if (fgtCurType >= fgt_attack_fast && fgtCurType <= fgt_attack_feintc)
        {
            auto *eAttack = static_cast<Character *>(EntityManager::GetEntityPointer(enemyAttack));
            if (eAttack)
            {
                isTurnLock = false;
                Turn(eAttack->curPos.x - curPos.x, eAttack->curPos.z - curPos.z);
                isTurnLock = true;
            }
        }
    }

    //Снимем выделение
    if (tuner.selected > 0.0f)
        tuner.selected -= dltTime * 3.0f;
    //Коэфициент скорости
    float k = dltTime * 0.5f;
    if (k > 1.0f)
        k = 1.0f;
    if (kSpd < 0.0f)
        kSpd = 0.0f;
    if (kSpd > 1.0f)
        kSpd = 1.0f;
    kSpdCur += (kSpd - kSpdCur) * k;
    k = kSpdCur * 0.3f + 0.7f + kSpdDlt;
    MODEL *m = Model();
    if (m)
    {
        Animation *a = m->GetAnimation();
        if (a)
        {
            a->Player(0).SetSpeed(a->Player(0).GetDefSpeed() * k);
            a->Player(1).SetSpeed(a->Player(1).GetDefSpeed() * k);
        }
    }
    //Текущая позиция
    oldPos = curPos;
    if (!isJump)
    {
        //Скорость перемещения
        if (bspeed.Update(dltTime))
            speed = bspeed.Get();
        //Скорость вращения
        if (bturn.Update(dltTime))
            turnspd = bturn.Get();
        //Перемещение
        const float spd = dltTime * speed * k;
        CVECTOR moveVec(sinf(ay) * spd, 0.0f, cosf(ay) * spd);
        curPos += moveVec;
        if (spd < 0.0f)
            moveVec = -moveVec;
        curPos += CVECTOR(moveVec.z, 0.0f, -moveVec.x) * (strafeMove * 0.8f);
        curPos += impulse * dltTime;
        const float kStrafe = dltTime * 8.0f;
        bool noStrafe = true;
        bool noStrafeMove = true;
        if (fabsf(speed) > 0.1f)
        {
            float sm = strafeMove;
            if (speed < 0.0f)
            {
                sm = -sm;
            }
            if (sm > 0.1f)
            {
                noStrafe = false;
                strafeAngle += kStrafe;
                if (strafeAngle > 1.0f)
                    strafeAngle = 1.0f;
            }
            else if (sm < -0.1f)
            {
                noStrafe = false;
                strafeAngle -= kStrafe;
                if (strafeAngle < -1.0f)
                    strafeAngle = -1.0f;
            }
        }
        else
        {
            if (!IsFight())
            {
                if (strafeMove > 0.1f)
                {
                    noStrafeMove = false;
                    strafeVel += kStrafe;
                    if (strafeVel > 1.0f)
                        strafeVel = 1.0f;
                }
                else if (strafeMove < -0.1f)
                {
                    noStrafeMove = false;
                    strafeVel -= kStrafe;
                    if (strafeVel < -1.0f)
                        strafeVel = -1.0f;
                }
            }
        }
        if (noStrafe)
        {
            if (strafeAngle >= 0.0f)
            {
                strafeAngle -= kStrafe;
                if (strafeAngle < 0.0f)
                    strafeAngle = 0.0f;
            }
            else
            {
                strafeAngle += kStrafe;
                if (strafeAngle > 0.0f)
                    strafeAngle = 0.0f;
            }
        }
        if (noStrafeMove)
        {
            if (strafeVel >= 0.0f)
            {
                strafeVel -= kStrafe;
                if (strafeVel < 0.0f)
                    strafeVel = 0.0f;
            }
            else
            {
                strafeVel += kStrafe;
                if (strafeVel > 0.0f)
                    strafeVel = 0.0f;
            }
        }
        curPos += CVECTOR(cosf(ay), 0.0f, -sinf(ay)) * (strafeVel * dltTime * 0.9f);
        //Вращение
        if (turnDir > 0.0f)
        {
            ay += turnspd * dltTime;
            if (ay > nay)
            {
                ay = nay;
                turnDir = 0.0f;
            }
            isTurn += 0.8f * turnspd * dltTime;
            if (isTurn > 1.0f)
                isTurn = 1.0f;
        }
        else if (turnDir < 0.0f)
        {
            ay -= turnspd * dltTime;
            if (ay < nay)
            {
                ay = nay;
                turnDir = 0.0f;
            }
            isTurn -= 0.8f * turnspd * dltTime;
            if (isTurn < -1.0f)
                isTurn = -1.0f;
        }
        else
        {
            if (isTurn > 0.0f)
            {
                isTurn -= dltTime * 1.7f;
                if (isTurn < 0.0f)
                    isTurn = 0.0f;
            }
            else if (isTurn < 0.0f)
            {
                isTurn += dltTime * 1.7f;
                if (isTurn > 0.0f)
                    isTurn = 0.0f;
            }
        }
        // location->Print(curPos, 1000.0f, 4, 1.0f, 0xffffffff, 1.0f, "ay = %f, nay = %f, turnDir = %f, turnspd = %f",
        // ay, nay, turnDir, turnspd);
        kSpd = 1.0f;
    }
    else
    {
        //Движемся по траектории падения
        curJumpFallTime += dltTime;
        if (isJumpSnd && priorityAction.name)
        {
            auto *sb = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea));
            if (sb && location->IsSwimming())
            {
                seaY = sb->WaveXZ(curPos.x, curPos.z, nullptr);
                if (seaY > curPos.y + 0.01f && PriorityActionIsJump())
                {
                    PlaySound("jump_water");
                    isJumpSnd = false;
                    core.Send_Message(effects, "sffff", "Splashes", curPos.x, seaY - 0.01f, curPos.z, 1.0f);
                    isSwim = true;
                }
            }
        }
        if (curJumpFallTime < jumpFallTime)
        {
            //Ищем ключевые позиции
            long s = static_cast<long>(curJumpFallTime / CHARACTER_JUMP_TIMESTEP);
            if (s >= jumpPoints)
                s = jumpPoints - 1;
            long e = s + 1;
            if (e >= jumpPoints)
                e = jumpPoints - 1;
            float kBlend = (curJumpFallTime / CHARACTER_JUMP_TIMESTEP) - s;
            if (s + kBlend > e)
                kBlend = static_cast<float>(e - s);
            if (kBlend < 0.0f)
                kBlend = 0.0f;
            if (kBlend > 1.0f)
                kBlend = 1.0f;
            curPos = jumpTrack[s] + (jumpTrack[e] - jumpTrack[s]) * kBlend;
        }
        else
        {
            //Закончили прыжок
            if (isJumpSnd)
            {
                PlaySound("jump");
                isJumpSnd = false;
            }
            Teleport(osculationPoint.x, osculationPoint.y, osculationPoint.z, ay);

            if (priorityAction.name)
            {
                MODEL *m = Model();
                if (m)
                {
                    Animation *a = m->GetAnimation();
                    if (a)
                    {
                        //Продолжим проигрывание
                        a->Player(0).Play();
                    }
                    else
                        priorityAction.SetName(nullptr);
                }
                else
                    priorityAction.SetName(nullptr);
            }
        }
        if (isJumpSnd)
        {
            // SetSoundPosition(jumpSound);
        }
        else
        {
            // ReleaseSound(jumpSound);
            jumpSound = SOUND_INVALID_ID;
        }
    }
    //Ослабим импульс
    k = dltTime * 5.0f;
    if (k > 1.0f)
        k = 1.0f;
    impulse -= impulse * k;

    //!!!
    // if(core.Controls->GetDebugAsyncKeyState('7') < 0) core.Send_Message(effects, "sffff", "Splashes", curPos.x,
    // curPos.y, curPos.z, 1.0f);

    //Высота волны в данной точке
    if (swimChange <= 0.0f && location->IsSwimming())
    {
        const bool old = isSwim;
        auto *sb = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea));
        isSwim = false;
        isRunDisable = false;
        if (sb)
        {
            CVECTOR n;
            seaY = sb->WaveXZ(curPos.x, curPos.z, &n);
            isRunDisable = seaY - curPos.y > CHARACTER_SEA_NOTRUN;
            if (seaY > curPos.y)
            {
                stepsRate += dltTime;
                if (stepsRate > 0.15f)
                {
                    stepsRate = 0.0f;
                    core.Send_Message(waterrings, "ffflll", curPos.x, curPos.z, ay, isMove, IsRun(), isSwim);
                }
            }
            if (seaY > curPos.y + CHARACTER_SEA_MOVE * height)
            {
                //Игрока сносит или он плывёт
                const float dlt = old ? -0.03f : 0.03f;
                if (seaY > curPos.y + CHARACTER_SEA_SWIM * height + dlt)
                {
                    //Игрок плывёт
                    seaY -= CHARACTER_SEA_SWIM * height;
                    isSwim = true;
                }
                float k = 5.0f * (seaY - curPos.y - CHARACTER_SEA_MOVE * height) * dltTime;
                if (k > 2.0f)
                    k = 2.0f;
                n.y = 0.0f;
                curPos += n * k;
            }
        }
        if (isSwim != old)
            swimChange = 0.8f;
    }
    else
        swimChange -= dltTime;
}

//Обновить позицию персонажа
void Character::Update(float dltTime)
{
    auto *const location = GetLocation();

    //Обновление полосок
    if (isPlayerEnemy && isFight && !IsDead())
    {
        enemyBarsAlpha += dltTime * 2.0f;
        if (enemyBarsAlpha > 1.0f)
            enemyBarsAlpha = 1.0f;
    }
    else
    {
        enemyBarsAlpha -= dltTime * 2.0f;
        if (enemyBarsAlpha < 0.0f)
            enemyBarsAlpha = 0.0f;
    }
    if (enemyBarsAlpha > 1.0f / 255.0f)
    {
        float hp = 0.5f;
        float energy = 0.5f;
        //Получим параметры жизни и энергии
        VDATA *retVal = core.Event("NpcEvtHP", "i", GetId());
        if (retVal)
        {
            if (!retVal->Get(hp))
            {
                hp = 0.5f;
            }
        }
        retVal = core.Event("NpcEvtEny", "i", GetId());
        if (retVal)
        {
            if (!retVal->Get(energy))
            {
                energy = 0.5f;
            }
        }
        //Добавимся в список рисуемых
        const CVECTOR pos = curPos + CVECTOR(0.0f, 2.0f, 0.0f);
        location->DrawEnemyBars(pos, hp, energy, tuner.GetAlpha() * enemyBarsAlpha);
    }
    //
    if (lockMove)
        dltTime = 0.0f;
    //Посчитаем время для работы групп
    {
        for (long i = 0; i < numTargets; i++)
        {
            grpTargets[i].time += dltTime;
        }
    }
    //Время блендинга анимации
    noBlendTime -= dltTime;
    if (noBlendTime <= 0.0f)
        noBlendTime = 0.0f;
    //Блендинг оружия и теней
    {
        const float alpha = tuner.GetAlpha() * 255.0f;
        const uint32_t blendColor = (static_cast<uint32_t>(alpha) << 24) | 0x00ffffff;
        core.Send_Message(blade, "ll", MSG_BLADE_ALPHA, blendColor);
        core.Send_Message(shadow, "ll", MSG_BLADE_ALPHA, blendColor);
    }
    //
    PtcData &ptc = location->GetPtcData();
    if (deadName)
    {
        if (!priorityAction.name || _stricmp(priorityAction.name, deadName) != 0)
        {
            priorityAction.SetName(deadName);
            isSetPriorityAction = false;
        }
    }
    MODEL *m = Model();
    if (!m)
        return;
    if (!isJump)
    {
        //Скорость падения
        vy -= 9.8f * dltTime;
        curPos.y += vy * dltTime;
        //Сверяемся с патчем
        currentNode = ptc.Move(currentNode, curPos, oldPos);
        curPos = oldPos;
        if (ptc.isBearing)
            vy = 0.0f;
        isSlide = ptc.isSlide;
        if (ptc.isSlide && isEnableJump)
        {
            if (fall.name && !isFight && !priorityAction.name)
            {
                if (TestJump(curPos + CVECTOR(0.001f * sinf(ay), -0.001f, 0.001f * cosf(ay))))
                {
                    PlaySound("recoil");
                    isJump = true;
                    isJumpSnd = true;
                    jumpSound = SOUND_INVALID_ID;
                    PlaySound("clothes");
                    if (fabsf(curPos.y - osculationPoint.y) > 1.5f)
                        SetPriorityAction(fall.name);
                    return;
                }
            }
        }
    }
    else
    {
        vy = 0.0f;
    }
    // SetSoundPosition(recoilSound);
    //Высота волны в данной точке
    CVECTOR chrPos = curPos;
    if (isSwim && location->IsSwimming())
    {
        vy = 0.0f;
        chrPos.y = seaY;
    }
    //Ставим матрицу для модельки
    m->mtx.BuildMatrix(CVECTOR(0.0f, ay + strafeAngle * 3.1415f * 0.25f * 0.8f, 0.0f), chrPos);
    // - CVECTOR(0.0f, 0.14f, 0.0f));
    //Хождение по ступенькам
    float sn = 0.0f;
    CVECTOR nodeNorm;
    ptc.GetNodeNormal(currentNode, nodeNorm);
    float k = 10.0f * dltTime;
    if (k > 1.0f)
        k = 1.0f;
    movecs += (nodeNorm.y - movecs) * k;
    isUp = nodeNorm.x * m->mtx.Vz().x + nodeNorm.z * m->mtx.Vz().z <= 0;
    //Анимация
    /*
    if(_stricmp(characterID, "Blaze") == 0)
    {
      location->GetRS()->Print(10, 150, "curFgt = %s", fightNamesTbl[fgtCurType]);
      location->GetRS()->Print(10, 170, "setFgt = %s", fightNamesTbl[fgtSetType]);
    }
    */
    UpdateAnimation();
    //Анализируем детекторы
    for (long i = 0; i < numDetectors; i++)
        detector[i]->Check(dltTime, this);
    //Если упали - пишем дебажную строчку
    if (curPos.y < -1000.0f)
    {
        // Assert(false);
        core.Trace("Character [%s] fall to underworld!!!", characterID ? characterID : "Unknow id");
        curPos.y = -500.0f;
        vy = 0.0f;
    }
    //Появление, удаление
    if (liveValue != 0.0f)
    {
        tuner.alpha += liveValue * dltTime;
        if (liveValue > 0.0f)
        {
            if (tuner.alpha >= 1.0f)
            {
                tuner.alpha = 1.0f;
                liveValue = 0.0f;
                core.Event("Location_CharacterEntryToLocation", "e", GetId());
            }
        }
        else
        {
            if (tuner.alpha <= 0.0f)
            {
                tuner.alpha = 0.0f;
                if (deadName)
                {
                    core.Event("Location_CharacterDead", "e", GetId());
                }
                else
                {
                    core.Event("Location_CharacterExitFromLocation", "e", GetId());
                }
                EntityManager::EraseEntity(GetId());
            }
        }
    }
    CVECTOR camPos, camAng;
    float perspective;
    location->GetRS()->GetCamera(camPos, camAng, perspective);
    const float dxz = (curPos.x - camPos.x) * (curPos.x - camPos.x) + (curPos.z - camPos.z) * (curPos.z - camPos.z);
    if (dxz < CHARACTER_HIDE_DIST * CHARACTER_HIDE_DIST)
    {
        tuner.chrAlpha -= dltTime * 3.0f;
        if (tuner.chrAlpha < 0.1f)
            tuner.chrAlpha = 0.1f;
    }
    else
    {
        tuner.chrAlpha += dltTime * 3.0f;
        if (tuner.chrAlpha > 1.0f)
            tuner.chrAlpha = 1.0f;
    }
    //
    k = dltTime * 4.0f;
    if (k > 1.0f)
        k = 1.0f;
    grsPos += (curPos - grsPos) * k;
    //Звук шагов
    if (soundStep)
    {
        PlayStep();
        soundStep = false;
    }
    soundGrass = false;
    //
    MODEL *signMdl = static_cast<MODEL *>(EntityManager::GetEntityPointer(sign));
    if (signMdl)
    {
        CVECTOR dir = camPos - curPos;
        dir.y = 0.0f;
        const float len = sqrtf(~dir);
        if (len > 1e-10f)
        {
            signMdl->mtx.Vz() = dir / len;
            signMdl->mtx.Vy() = CVECTOR(0.0f, 1.0f, 0.0f);
            signMdl->mtx.Vx() = signMdl->mtx.Vz() ^ signMdl->mtx.Vy();
        }
        signMdl->mtx.Pos() = curPos;
    }

    // источник света у персонажа
    if (m_nHandLightID >= 0)
    {
        location->GetLights()->UpdateMovingLight(m_nHandLightID, GetHandLightPos());
    }
}

//Событие окончания проигрывания анимации действия
void Character::ActionEvent(const char *actionName, Animation *animation, long index, long eventID,
                            AnimationEvent event)
{
    if (index != 0)
        return;
    if (event == ae_end)
        curIdleIndex = -1;

    if (fgtCurType != fgt_block)
        fgtCurType = fgt_none;
    isTurnLock = false;
    if (priorityAction.name && _stricmp(actionName, priorityAction.name) == 0)
    {
        if (_stricmp(priorityAction.name, CHARACTER_NORM_TO_FIGHT) == 0)
        {
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
            if (event == ae_end)
            {
                priorityAction.SetName(nullptr);
                animation->Player(0).Pause();
                animation->Player(0).SetPosition(1.0f);
            }
        }
        else if (_stricmp(priorityAction.name, CHARACTER_FIGHT_TO_NORM) == 0)
        {
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 1);
            if (event == ae_end)
            {
                priorityAction.SetName(nullptr);
                animation->Player(0).Pause();
                animation->Player(0).SetPosition(1.0f);
            }
        }
        else if (shot.name && _stricmp(priorityAction.name, shot.name) == 0)
        {
            core.Send_Message(blade, "l", MSG_BLADE_GUNBELT);
            if (event == ae_end)
            {
                //Закончился выстрел
                priorityAction.SetName(nullptr);
                animation->Player(0).Pause();
                animation->Player(0).SetPosition(1.0f);
                if (fgtCurType == fgt_fire)
                {
                    fgtSetType = fgtCurType = fgt_none;
                }
            }
        }
        else if (recoil.name && _stricmp(priorityAction.name, recoil.name) == 0)
        {
            if (rand() % 10 > 7)
                recoilLook = true; //в стоянии проиграть дразнилку
            priorityAction.SetName(nullptr);
        }
        else if (deadName && _stricmp(priorityAction.name, deadName) == 0)
        {
            animation->Player(0).Pause();
            animation->Player(0).SetPosition(1.0f);
            core.Event("Location_Character_Dead", "i", GetId());
        }
        else if (PriorityActionIsJump())
        {
            priorityAction.SetName(nullptr);
            animation->Player(0).Pause();
            animation->Player(0).SetPosition(1.0f);

            // проверим куда упали и проиграем анимацию после падения на землю и в воду.
            SEA_BASE *sb = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea));
            if (sb)
            {
                if (sb->WaveXZ(curPos.x, curPos.z, nullptr) - curPos.y > CHARACTER_SEA_SWIM)
                    SetPriorityAction(fall_water.name);
                else
                    SetPriorityAction(fall_land.name);
            }
            else
                SetPriorityAction(fall_land.name);
        }
        else if (_stricmp(priorityAction.name, fall_land.name) == 0 ||
                 _stricmp(priorityAction.name, fall_water.name) == 0)
        {
            priorityAction.SetName(nullptr);
            animation->Player(0).Pause();
            animation->Player(0).SetPosition(1.0f);
        }
    }
    else if (userIdle.name)
    {
        if (_stricmp(actionName, userIdle.name) != 0)
            return;
        core.Event("Location_Character_EndAction", "i", GetId());
    }
}

void Character::ActionEvent(Animation *animation, long playerIndex, const char *eventName)
{
    if (playerIndex)
        return;
    const char *actionName = animation->Player(playerIndex).GetAction();
    const char *alliace = nullptr;
    if (!actionName)
        return;
    if (_stricmp(eventName, "LStep") == 0)
    {
        soundStep = true;
    }
    else if (_stricmp(eventName, "RStep") == 0)
    {
        soundStep = true;
    }
    else if (_stricmp(eventName, "swim") == 0)
    {
        PlaySound("swiming");
    }
    else if ((alliace = GetValueByPrefix(eventName, "SndAlliace_")) != nullptr)
    {
        PlaySound(alliace);
    }
    else
        /*
        if(_stricmp(eventName, "Sound_wind_fast") == 0)
        {
          PlaySound("sword_wind_fast");
        }else
        if(_stricmp(eventName, "Sound_wind_force") == 0)
        {
          PlaySound("sword_wind_force");
        }else
        if(_stricmp(eventName, "Sound_wind_round") == 0)
        {
          PlaySound("sword_wind_round");
        }else
        if(_stricmp(eventName, "Sound_wind_break") == 0)
        {
          PlaySound("sword_wind_break");
        }else
        if(_stricmp(eventName, "Sound_wind_feint") == 0)
        {
          PlaySound("sword_wind_feint");
        }else
        if(_stricmp(eventName, "Sound_wind_feintc") == 0)
        {
          PlaySound("sword_wind_feintc");
        }else
        if(_stricmp(eventName, "Sound_wind_feintend") == 0)
        {
          PlaySound("sword_wind_feintend");
        }else*/
        if (_stricmp(eventName, "Resact") == 0)
    {
        fgtSetType = fgt_none;
        fgtSetIndex = -1;
    }
    else if (_stricmp(eventName, "Attack") == 0)
    {
        CheckAttackHit();
    }
    else if (_stricmp(eventName, "Parry start") == 0)
    {
        isParryState = true;
        isFeintState = false;
    }
    else if (_stricmp(eventName, "Parry end") == 0)
    {
        isParryState = false;
        isFeintState = false;
    }
    else if (_stricmp(eventName, "Feint start") == 0)
    {
        isParryState = false;
        isFeintState = true;
    }
    else if (_stricmp(eventName, "Feint end") == 0)
    {
        isParryState = false;
        isFeintState = false;
    }
    else /*
    if(_stricmp(eventName, "sound_pistol") == 0)
    {
        PlaySound("pistol_out");
    }else
    if(_stricmp(eventName, "Blade to hand") == 0)
    {
        if(!isFightWOWps)
        {
            float pos = animation->Player(0).GetPosition();
            if(pos < 0.99f) PlaySound("sword_sh");
        }
    }else
    if(_stricmp(eventName, "Blade to belt") == 0)
    {
        if(!isFightWOWps)
        {
            float pos = animation->Player(0).GetPosition();
            if(pos < 0.99f) PlaySound("sword_out");
        }
    }else */
        if (_stricmp(eventName, "Death sound") == 0)
    {
        core.Event("Event_ChrSnd_Body", "i", GetId());
    }
    else if ((alliace = GetValueByPrefix(eventName, "itemaction_")) != nullptr)
    {
        const char *pcActionName = nullptr;
        long nIdx = -1;
        if (_strnicmp(alliace, "set", 3) == 0)
        {
            pcActionName = "set";
            if (alliace[3] >= '0' && alliace[3] <= '9')
                nIdx = atoi(&alliace[3]);
        }
        else if (_strnicmp(alliace, "reset", 5) == 0)
        {
            pcActionName = "reset";
            if (alliace[5] >= '0' && alliace[5] <= '9')
                nIdx = atoi(&alliace[5]);
        }
        if (pcActionName)
            core.Event("Location_CharacterItemAction", "isl", GetId(), pcActionName, nIdx);
    }
    else if (priorityAction.name && _stricmp(actionName, priorityAction.name) == 0)
    {
        if (_stricmp(priorityAction.name, CHARACTER_NORM_TO_FIGHT) == 0)
        {
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_HAND, 1);
        }
        else if (_stricmp(priorityAction.name, CHARACTER_FIGHT_TO_NORM) == 0)
        {
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 0);
            core.Send_Message(blade, "ll", MSG_BLADE_BELT, 1);
        }
        else if (shot.name && _stricmp(priorityAction.name, shot.name) == 0)
        {
            if (eventName)
            {
                if (_stricmp(eventName, CHARACTER_FIGHT_GUNBELT) == 0)
                {
                    core.Send_Message(blade, "l", MSG_BLADE_GUNBELT);
                }
                else if (_stricmp(eventName, CHARACTER_FIGHT_GUNHAND) == 0)
                {
                    core.Send_Message(blade, "l", MSG_BLADE_GUNHAND);
                }
                else if (_stricmp(eventName, CHARACTER_FIGHT_GUNFIRE) == 0)
                {
                    core.Send_Message(blade, "l", MSG_BLADE_GUNFIRE);
                    // PlaySound("pistol_shot");
                    isFired = true;
                    float kDist;
                    Character *chr = FindGunTarget(kDist);
                    entid_t enemy;
                    if (chr)
                    {
                        enemy = chr->GetId();
                        chr->Hit(fgt_hit_fire);
                    }
                    core.Event("Location_CharacterFire", "iifl", GetId(), enemy, kDist, chr != nullptr);
                }
            }
        }
        else if (isJump && PriorityActionIsJump())
        {
            if (eventName && _stricmp("Jump pause", eventName) == 0)
            {
                animation->Player(playerIndex).Pause();
            }
        }
    }
    else if (userIdle.name)
    {
        if (_stricmp(actionName, userIdle.name) == 0)
        {
            if (shot.name && _stricmp(actionName, shot.name) == 0)
            {
                if (eventName)
                {
                    if (_stricmp(eventName, CHARACTER_FIGHT_GUNBELT) == 0)
                    {
                        core.Send_Message(blade, "l", MSG_BLADE_GUNBELT);
                    }
                    else if (_stricmp(eventName, CHARACTER_FIGHT_GUNHAND) == 0)
                    {
                        core.Send_Message(blade, "l", MSG_BLADE_GUNHAND);
                    }
                    else if (_stricmp(eventName, CHARACTER_FIGHT_GUNFIRE) == 0)
                    {
                        core.Send_Message(blade, "l", MSG_BLADE_GUNFIRE);
                        core.Event("ActorMakeShot", "i", GetId());
                    }
                }
            }
        }
    }
}

long Character::PlaySound(const char *soundName, bool isLoop, bool isCached)
{
    if (!soundService)
        return SOUND_INVALID_ID;
    CVECTOR pos = curPos + CVECTOR(0.0f, 1.0f, 0.0f);
    const long sID = soundService->SoundPlay(soundName, PCM_3D, VOLUME_FX, false, false, isCached, 0, &pos);
    return sID;
}

void Character::PlayStep()
{
    if (!soundService)
        return;
    if (isSwim)
        return;
    auto *sb = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea));
    auto *const location = GetLocation();
    if (sb && location->IsSwimming())
    {
        //Проверим высоту моря
        seaY = sb->WaveXZ(curPos.x, curPos.z, nullptr);
        if (seaY > curPos.y)
        {
            PlaySound("step_water", false, true);
            return;
        }
    }
    //Идём по земле

    if (IsRun() && !isBack)
    {
        if (!soundGrass)
        {
            const char *defSnd = "run_grass";
            const char *mtl = location->GetPtcData().GetMaterial(currentNode);
            if (mtl)
            {
                if (_stricmp(mtl, "run_grass") == 0)
                {
                    defSnd = "run_grass";
                }
                else if (_stricmp(mtl, "snd_wood") == 0)
                {
                    defSnd = "run_wood";
                }
                else if (_stricmp(mtl, "snd_ground") == 0)
                {
                    defSnd = "run_ground";
                }
                else if (_stricmp(mtl, "snd_sand") == 0)
                {
                    defSnd = "run_sand";
                }
                else if (_stricmp(mtl, "snd_stone") == 0)
                {
                    defSnd = "run_stone";
                }
                else if (_stricmp(mtl, "snd_stairway") == 0)
                {
                    defSnd = "run_stairway";
                }
                else if (_stricmp(mtl, "snd_carpet") == 0)
                {
                    defSnd = "run_carpet";
                }
                else if (_stricmp(mtl, "snd_church") == 0)
                {
                    defSnd = "run_church";
                }
                else if (_stricmp(mtl, "snd_echo") == 0)
                {
                    defSnd = "run_echo";
                }
                /*else
                if(_stricmp(mtl, "snd_iron") == 0)
                {
                  defSnd = "run_iron";
                }*/
            }
            PlaySound(defSnd, false, true);
        }
        else
        {
            PlaySound("run_grass_noise", false, true);
        }
    }
    else
    {
        if (!soundGrass)
        {
            const char *defSnd = "step_grass";
            const char *mtl = location->GetPtcData().GetMaterial(currentNode);
            if (mtl)
            {
                if (_stricmp(mtl, "snd_grass") == 0)
                {
                    defSnd = "step_grass";
                }
                else if (_stricmp(mtl, "snd_wood") == 0)
                {
                    defSnd = "step_wood";
                }
                else if (_stricmp(mtl, "snd_ground") == 0)
                {
                    defSnd = "step_ground";
                }
                else if (_stricmp(mtl, "snd_sand") == 0)
                {
                    defSnd = "step_sand";
                }
                else if (_stricmp(mtl, "snd_stone") == 0)
                {
                    defSnd = "step_stone";
                }
                else if (_stricmp(mtl, "snd_stairway") == 0)
                {
                    defSnd = "step_stairway";
                }
                else if (_stricmp(mtl, "snd_carpet") == 0)
                {
                    defSnd = "step_carpet";
                }
                else if (_stricmp(mtl, "snd_church") == 0)
                {
                    defSnd = "step_church";
                }
                else if (_stricmp(mtl, "snd_echo") == 0)
                {
                    defSnd = "step_echo";
                }
                else if (_stricmp(mtl, "snd_iron") == 0)
                {
                    defSnd = "step_iron";
                }
            }
            PlaySound(defSnd, false, true);
        }
        else
        {
            PlaySound("grass_noise", false, true);
        }
    }
}

void Character::SetSoundPosition(long id)
{
    if (!soundService || id == SOUND_INVALID_ID)
        return;
    CVECTOR pos = curPos + CVECTOR(0.0f, 1.0f, 0.0f);
    auto *const location = GetLocation();
    if (location->supervisor.player)
    {
        VDX9RENDER *rs = location->GetRS();
        if (rs)
        {
            static CMatrix view, cur;
            rs->GetTransform(D3DTS_VIEW, view);
            cur.BuildMatrix(CVECTOR(0.0f, ay, 0.0f), location->supervisor.player->curPos);
            cur.MulToInv(CVECTOR(pos), pos);
            view.MulToInv(CVECTOR(pos), pos);
        }
    }
    soundService->SoundSet3DParam(id, SM_POSITION, &pos);
}

void Character::ReleaseSound(long id)
{
    if (!soundService)
        return;
    if (id != SOUND_INVALID_ID)
        soundService->SoundRelease(id);
}

//============================================================================================
//Инкапсуляция
//============================================================================================

bool Character::zLoadModel(MESSAGE &message)
{
    char name[256];
    char ani[256];
    char mpath[300];
    EntityManager::EraseEntity(shadow);
    EntityManager::EraseEntity(waterrings);
    EntityManager::EraseEntity(mdl);
    message.String(256, name);
    name[255] = 0;
    message.String(256, ani);
    ani[255] = 0;
    //Путь до текстур
    auto *gs = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    if (gs)
        gs->SetTexturePath("characters\\");
    //Путь до модельки
    strcpy_s(mpath, "characters\\");
    strcat_s(mpath, name);
    //Создаём и загружаем модельку
    if (!(mdl = EntityManager::CreateEntity("modelr")))
    {
        if (gs)
            gs->SetTexturePath("");
        return false;
    }
    if (!core.Send_Message(mdl, "ls", MSG_MODEL_LOAD_GEO, mpath))
    {
        if (gs)
            gs->SetTexturePath("");
        core.Trace("Character model '%s' not loaded", mpath);
        return false;
    }
    if (gs)
        gs->SetTexturePath("");
    if (!core.Send_Message(mdl, "ls", MSG_MODEL_LOAD_ANI, ani) != 0)
    {
        core.Trace("Character animation '%s' not loaded", ani);
        EntityManager::EraseEntity(mdl);
        return false;
    }
    auto *m = static_cast<MODEL *>(EntityManager::GetEntityPointer(mdl));
    if (!m)
    {
        EntityManager::EraseEntity(mdl);
        return false;
    }
    Animation *a = m->GetAnimation();
    if (a)
    {
        a->SetEventListener(&eventListener);
        eventId = a->SetEvent(ae_end, 0, &eventListener);
    }
    m->SetRenderTuner(&tuner);
    EntityManager::AddToLayer(REALIZE, mdl, 20);
    EntityManager::AddToLayer(SUN_TRACE, mdl, 10);
    if (shadow = EntityManager::CreateEntity("shadow"))
    {
        core.Send_Message(shadow, "li", 0, mdl);
    }
    else
    {
        core.Trace("Shadow not created!");
    }
    if (!EntityManager::GetEntityId("waterrings"))
    {
        waterrings = EntityManager::CreateEntity("waterrings");
    }
    UpdateActionsData();
    return true;
}

bool Character::zTeleport(MESSAGE &message, bool isAy)
{
    const float x = message.Float();
    const float y = message.Float();
    const float z = message.Float();
    if (isAy)
        return Teleport(x, y, z, message.Float());
    return Teleport(x, y, z);
}

bool Character::zTeleportL(MESSAGE &message)
{
    char group[256];
    char name[256];
    message.String(256, group);
    group[255] = 0;
    message.String(256, name);
    name[255] = 0;
    return Teleport(group, name);
}

bool Character::zAddDetector(MESSAGE &message)
{
    if (numDetectors >= sizeof(detector) / sizeof(Detector *))
        return false;
    char group[256];
    message.String(256, group);
    group[255] = 0;
    if (!group[0])
        return false;
    //Проверяем на созданность
    for (long i = 0; i < numDetectors; i++)
    {
        if (_stricmp(detector[i]->la->GetGroupName(), group) == 0)
            return false;
    }
    //Ищем группу
    auto *const location = GetLocation();
    LocatorArray *la = location->FindLocatorsGroup(group);
    if (!la)
        return false;
    detector[numDetectors++] = new Detector(la);
    return true;
}

bool Character::zDelDetector(MESSAGE &message)
{
    char group[256];
    message.String(256, group);
    group[255] = 0;
    for (long i = 0; i < numDetectors; i++)
    {
        if (_stricmp(detector[i]->la->GetGroupName(), group) == 0)
        {
            detector[i]->Exit(this);
            delete detector[i];
            detector[i] = detector[--numDetectors];
            return true;
        }
    }
    return false;
}

bool Character::zActionPlay(MESSAGE &message)
{
    char buf[256];
    message.String(sizeof(buf) - 1, buf);
    buf[sizeof(buf) - 1] = 0;
    if (buf[0])
    {
        userIdle.SetName(buf);
        MODEL *m = Model();
        if (!m)
            return false;
        Animation *a = m->GetAnimation();
        if (!a)
            return false;
        UpdateActionCharacterData(userIdle, a);
    }
    else
        userIdle.SetName(nullptr);
    return true;
}

bool Character::zEntry(MESSAGE &message)
{
    EntryToLocation();
    char grp[256];
    char name[256];
    message.String(sizeof(grp) - 1, grp);
    grp[sizeof(grp) - 1] = 0;
    message.String(sizeof(name) - 1, name);
    name[sizeof(name) - 1] = 0;
    return Teleport(grp, name);
}

bool Character::zSetBlade(MESSAGE &message)
{
    const long nBladeIdx = message.Long();

    isBladeSet = false;
    char name[256];
    message.String(sizeof(name) - 1, name);
    name[sizeof(name) - 1] = 0;
    isBladeSet = true;
    if (!name[0])
    {
        isBladeSet = false;
        SetFightMode(false);
    }
    const float t = message.Float();
    const long s = message.Long();
    const long e = message.Long();
    if (!EntityManager::GetEntityPointer(blade))
    {
        if (!(blade = EntityManager::CreateEntity("blade")))
            return false;
    }
    core.Send_Message(blade, "llisfll", MSG_BLADE_SET, nBladeIdx, mdl, name, t, s, e);
    UpdateWeapons();
    return true;
}

bool Character::zSetGun(MESSAGE &message)
{
    isGunSet = false;
    // if(!isBladeSet) return false;  //eddy. пистолет пусть виснет, он не мешает
    char name[256];
    message.String(sizeof(name) - 1, name);
    name[sizeof(name) - 1] = 0;
    isGunSet = true;
    if (!name[0])
        isGunSet = false;
    if (!EntityManager::GetEntityPointer(blade))
    {
        if (!(blade = EntityManager::CreateEntity("blade")))
            return false;
    }
    core.Send_Message(blade, "lis", MSG_BLADE_GUNSET, mdl, name);
    UpdateWeapons();
    return true;
}

bool Character::zTurnByLoc(MESSAGE &message)
{
    char group[256];
    char name[256];
    message.String(sizeof(group) - 1, group);
    group[sizeof(group) - 1] = 0;
    message.String(sizeof(name) - 1, name);
    name[sizeof(name) - 1] = 0;
    auto *const location = GetLocation();
    LocatorArray *la = location->FindLocatorsGroup(group);
    if (!la)
        return false;
    const long li = la->FindByName(name);
    if (li < 0)
        return false;
    CMatrix mtx;
    if (!la->GetLocatorPos(li, mtx))
        return false;
    const bool isTo = message.Long() != 0;
    if (isTo)
    {
        Turn(mtx.Pos().x - curPos.x, mtx.Pos().z - curPos.z);
    }
    else
    {
        Turn(mtx.Vz().x, mtx.Vz().z);
    }
    return true;
}

bool Character::zTurnByChr(MESSAGE &message)
{
    const entid_t chr = message.EntityID();
    auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(chr));
    if (!c)
        return false;
    Turn(c->curPos.x - curPos.x, c->curPos.z - curPos.z);
    return true;
}

bool Character::zTurnByPoint(MESSAGE &message)
{
    const float dx = message.Float() - curPos.x;
    message.Float();
    const float dz = message.Float() - curPos.z;
    Turn(dx, dz);
    return true;
}

bool Character::zDistByCharacter(MESSAGE &message, bool is2D)
{
    const entid_t chr = message.EntityID();
    auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(chr));
    if (!c)
        return false;
    const float dx = curPos.x - c->curPos.x;
    const float dz = curPos.z - c->curPos.z;
    if (is2D)
    {
        message.ScriptVariablePointer()->Set(sqrtf(dx * dx + dz * dz));
    }
    else
    {
        const float dy = curPos.y - c->curPos.y;
        message.ScriptVariablePointer()->Set(sqrtf(dx * dx + dy * dy + dz * dz));
    }
    return true;
}

uint32_t Character::zExMessage(MESSAGE &message)
{
    char msg[64];
    char grp[32];
    message.String(sizeof(msg), msg);
    msg[sizeof(msg) - 1] = 0;
    LocatorArray *la;
    long i;
    VDATA *v;
    CVECTOR pos;
    if (_stricmp(msg, "TieItem") == 0)
    {
        i = message.Long();
        char modelName[MAX_PATH];
        message.String(sizeof(modelName), modelName);
        char locatorName[128];
        message.String(sizeof(locatorName), locatorName);
        if (!EntityManager::GetEntityPointer(blade))
        {
            if (!(blade = EntityManager::CreateEntity("blade")))
                return 0;
            UpdateWeapons();
        }
        core.Send_Message(blade, "lilss", 1001, mdl, i, modelName, locatorName);
        return 1;
    }
    if (_stricmp(msg, "UntieItem") == 0)
    {
        i = message.Long();
        core.Send_Message(blade, "ll", 1002, i);
        return 1;
    }
    auto *const location = GetLocation();
    if (_stricmp(msg, "HandLightOn") == 0)
    {
        // удалим старый источник
        if (m_nHandLightID >= 0)
            location->GetLights()->DelMovingLight(m_nHandLightID);
        // создадим новый
        message.String(sizeof(msg), msg);
        m_nHandLightID = location->GetLights()->AddMovingLight(msg, GetHandLightPos());
        return 1;
    }
    if (_stricmp(msg, "HandLightOff") == 0)
    {
        if (m_nHandLightID >= 0)
            location->GetLights()->DelMovingLight(m_nHandLightID);
        m_nHandLightID = -1;
        return 1;
    }
    if (_stricmp(msg, "PlaySound") == 0)
    {
        message.String(32, msg);
        msg[31] = 0;
        return PlaySound(msg) != SOUND_INVALID_ID;
    }
    if (_stricmp(msg, "IsFightMode") == 0)
    {
        return IsFight();
    }
    if (_stricmp(msg, "IsSetBalde") == 0)
    {
        return IsSetBlade();
    }
    if (_stricmp(msg, "IsDead") == 0)
    {
        return deadName != nullptr;
    }
    if (!deadName)
    {
        if (_stricmp(msg, "FindDialogCharacter") == 0)
        {
            Character *chr = FindDialogCharacter();
            if (chr && chr->AttributesPointer)
            {
                return chr->AttributesPointer->GetAttributeAsDword("index", -1);
            }
            return -1;
        }
        if (_stricmp(msg, "SetFightMode") == 0)
        {
            return SetFightMode(message.Long() != 0, false);
        }
        if (_stricmp(msg, "ChangeFightMode") == 0)
        {
            return SetFightMode(message.Long() != 0, true);
        }
        if (_stricmp(msg, "FindForvardLocator") == 0)
        {
            //Имя групы
            message.String(sizeof(grp), grp);
            grp[sizeof(grp) - 1] = 0;
            //Переменная для имени локатора
            v = message.ScriptVariablePointer();
            //Ищем локатор
            la = location->FindLocatorsGroup(grp);
            i = location->supervisor.FindForvardLocator(la, curPos, CVECTOR(sinf(ay), 0.0f, cosf(ay)));
            if (i < 0)
            {
                v->Set("");
                return 0;
            }
            v->Set((char *)la->LocatorName(i));
            return 1;
        }
        if (_stricmp(msg, "DistToLocator") == 0)
        {
            //Имя групы
            message.String(sizeof(grp), grp);
            grp[sizeof(grp) - 1] = 0;
            //Имя локатора
            message.String(sizeof(msg), msg);
            msg[sizeof(msg) - 1] = 0;
            v = message.ScriptVariablePointer();
            la = location->FindLocatorsGroup(grp);
            i = -1;
            if (la)
                i = la->FindByName(msg);
            if (i < 0)
            {
                v->Set(-1.0f);
                return 0;
            }
            la->GetLocatorPos(i, pos.x, pos.y, pos.z);
            v->Set(sqrtf(~(pos - curPos)));
            return 1;
        }
        if (_stricmp(msg, "InDialog") == 0)
        {
            isDialog = message.Long() != 0;
            return 1;
        }
        if (_stricmp(msg, "SetSex") == 0)
        {
            isMale = message.Long() != 0;
            return 1;
        }
        if (_stricmp(msg, "SetFightWOWeapon") == 0)
        {
            isFightWOWps = message.Long() != 0;
            UpdateWeapons();
            return 1;
        }
        if (_stricmp(msg, "LockFightMode") == 0)
        {
            lockFightMode = message.Long() != 0;
            return 1;
        }
        if (_stricmp(msg, "CheckFightMode") == 0)
        {
            return isFight;
        }
        if (_stricmp(msg, "IsActive") == 0)
        {
            return isActiveState;
        }
        if (_stricmp(msg, "CheckID") == 0)
        {
#ifdef _DEBUG
            message.String(sizeof(msg), msg);
            msg[sizeof(msg) - 1] = 0;
            if (AttributesPointer)
            {
                const char *id = AttributesPointer->GetAttribute("id");
                if (id)
                {
                    if (strcmp(id, characterID) != 0)
                        __debugbreak();
                    if (strcmp(id, msg) != 0)
                        __debugbreak();
                }
            }
            if (strcmp(msg, characterID) != 0)
                __debugbreak();
#endif
            return 1;
        }
        if (_stricmp(msg, "GunBelt") == 0)
        {
            if (message.Long() != 0)
                core.Send_Message(blade, "l", MSG_BLADE_GUNBELT);
            else
                core.Send_Message(blade, "l", MSG_BLADE_GUNHAND);
        }
    }
    return 0;
}

bool Character::zPlaySound(MESSAGE &message)
{
    char name[64];
    message.String(sizeof(name), name);
    name[sizeof(name) - 1] = 0;
    return PlaySound(name) != SOUND_INVALID_ID;
}

//Проверка возможности слететь
bool Character::TestJump(CVECTOR pos)
{
    auto *const location = GetLocation();
    MODEL *jpm = location->JmpPatch();
    if (!jpm)
        return false;
    const CVECTOR src(pos.x, pos.y + 1.0f, pos.z);
    const CVECTOR dst(pos.x, pos.y - 1.0f, pos.z);
    if (jpm->Trace(src, dst) >= 1.0f)
        return false;
    //Проверяем траекторию падения
    const float speed = 3.3f;
    CVECTOR v(speed * sinf(ay), vy, speed * cosf(ay));
    for (long i = 0; i < CHARACTER_MAXJUMPPOINTS; i++)
    {
        //Сохраняем позицию в треке
        jumpTrack[i] = pos;
        //Проверяем следующий отрезок
        CVECTOR dlt = v * CHARACTER_JUMP_TIMESTEP;
        v.y -= 9.8f * CHARACTER_JUMP_TIMESTEP;
        CVECTOR src = pos - dlt * 0.001f;
        CVECTOR dst = pos + dlt * 1.001f;
        // location->DrawLine(src, 0xff00ff00, dst, 0xff00ff00, false);
        const float k = location->GetPtcData().Trace(src, dst);
        if (k < 1.0f)
        {
            //Нашли точку приземления
            osculationPoint = src + (dst - src) * k + CVECTOR(0.0f, 0.01f, 0.0f);
            jumpFallTime = (i + k) * CHARACTER_JUMP_TIMESTEP;
            curJumpFallTime = 0.0f;
            jumpPoints = i + 1;
            return true;
        }
        //Сохраним новую позицию
        pos += dlt;
    }
    return false;
}

bool Character::BuildJump(CVECTOR pos, float fAng)
{
    auto *const location = GetLocation();
    // нет патча для прыгания - нет и прыжков
    MODEL *jpm = location->JmpPatch();
    if (!jpm)
        return false;

    const CVECTOR src(pos.x, pos.y + 1.0f, pos.z);
    const CVECTOR dst(pos.x, pos.y - 1.0f, pos.z);
    const float fTrace = location->GetPtcData().Trace(src, dst);
    if (fTrace > 1.0f)
        return false;
    pos.y = src.y + (dst.y - src.y) * (fTrace - 0.1f);

    //Проверяем траекторию прыжка
    const float speed = 3.3f;
    CVECTOR v(speed * sinf(fAng), speed, speed * cosf(fAng)); // скорость прыжка
    //Строим трек для прыжка
    for (long i = 0; i < CHARACTER_MAXJUMPPOINTS; i++)
    {
        //Сохраняем позицию в треке
        jumpTrack[i] = pos;
        //Проверяем следующий отрезок
        CVECTOR dlt = v * CHARACTER_JUMP_TIMESTEP;
        v.y -= 9.8f * CHARACTER_JUMP_TIMESTEP;
        CVECTOR src = pos - dlt * 0.001f;
        CVECTOR dst = pos + dlt * 1.001f;
        // location->DrawLine(src, 0xff00ff00, dst, 0xff00ff00, false);
        //если врезаемся в какую нить бню, то не можем прыгать
        const float k = location->GetPtcData().Trace(src, dst);
        if (k < 1.0f)
        {
            //Нашли точку приземления
            osculationPoint = src + (dst - src) * k + CVECTOR(0.0f, 0.01f, 0.0f);
            jumpFallTime = (i + k) * CHARACTER_JUMP_TIMESTEP;
            curJumpFallTime = 0.0f;
            jumpPoints = i + 1;

            //Проверим приземление на джамп патч
            if (jpm)
            {
                //~!~
                dst = src = osculationPoint;
                src.y += 10.f;
                dst.y -= 20.f;
                const float fT = jpm->Trace(src, dst);
                //да мы приземляемся на прыжковый патч
                if (fT < 1.f)
                {
                    CVECTOR vdlt = src + (dst - src) * fT - osculationPoint;
                    //учитываем поправку если она достаточно велика
                    if (vdlt.GetLength() > 0.1f)
                    {
                        vdlt *= 1.f / jumpPoints;
                        for (long i = 1; i < jumpPoints; i++)
                        {
                            jumpTrack[i] += vdlt;
                        }
                    }
                }
            }

            return true;
        }
        if (TraceWithObstacle(src, dst))
            return false;
        //Сохраним новую позицию
        pos += dlt;
    }
    return false;
}

bool Character::TraceWithObstacle(const CVECTOR &src, const CVECTOR &dst)
{
    auto *const location = GetLocation();
    if (location->Trace(src, dst) > 1.f)
        return false;
    return true;
}

void Character::UpdateActionsData()
{
    MODEL *m = Model();
    if (!m)
        return;
    Animation *a = m->GetAnimation();
    if (!a)
        return;
    //Действия перемещения
    turnSpeed = 2.0f;
    const char *s = a->GetData("Turn speed");
    if (s && s[0])
        turnSpeed = static_cast<float>(atof(s));
    UpdateActionMoveData(walk, a);
    UpdateActionMoveData(backwalk, a);
    UpdateActionMoveData(run, a);
    UpdateActionMoveData(backrun, a);
    UpdateActionMoveData(stsUp, a);
    UpdateActionMoveData(stsDown, a);
    UpdateActionMoveData(stsUpBack, a);
    UpdateActionMoveData(stsDownBack, a);
    UpdateActionMoveData(stsUpRun, a);
    UpdateActionMoveData(stsDownRun, a);
    UpdateActionMoveData(stsUpRunBack, a);
    UpdateActionMoveData(stsDownRunBack, a);
    UpdateActionMoveData(swim, a);
    UpdateActionMoveData(fall, a);
    UpdateActionMoveData(fall_land, a);
    UpdateActionMoveData(fall_water, a);
    //Смерти
    long i;
    for (i = 0; i < numActionDead; i++)
        UpdateActionDeadData(actionDead[i], a);
    for (i = 0; i < numActionFightDead; i++)
        UpdateActionDeadData(actionFightDead[i], a);
    //Бой
    UpdateActionMoveData(fightwalk, a);
    UpdateActionMoveData(fightbackwalk, a);
    UpdateActionMoveData(fightrun, a);
    UpdateActionMoveData(fightbackrun, a);
    for (i = 0; i < numAttackFast; i++)
        UpdateActionCharacterData(attackFast[i], a);
    for (i = 0; i < numAttackForce; i++)
        UpdateActionCharacterData(attackForce[i], a);
    for (i = 0; i < numAttackRound; i++)
        UpdateActionCharacterData(attackRound[i], a);
    for (i = 0; i < numAttackBreak; i++)
        UpdateActionCharacterData(attackBreak[i], a);
    for (i = 0; i < numAttackFeint; i++)
        UpdateActionCharacterData(attackFeint[i], a);
    for (i = 0; i < numAttackFeint; i++)
        UpdateActionCharacterData(attackFeintC[i], a);
    UpdateActionCharacterData(shot, a);
    for (i = 0; i < numHits; i++)
        UpdateActionCharacterData(hit[i], a);
    for (i = 0; i < numParry; i++)
        UpdateActionCharacterData(parry[i], a); // eddy
    UpdateActionCharacterData(hitFeint, a);
    UpdateActionCharacterData(hitParry, a);
    UpdateActionCharacterData(hitRound, a);
    UpdateActionCharacterData(hitFire, a);
    UpdateActionCharacterData(block, a);
    UpdateActionCharacterData(blockaxe, a);
    UpdateActionCharacterData(blockhit, a);
    UpdateActionCharacterData(blockaxehit, a);
    UpdateActionCharacterData(blockbreak, a);
    UpdateActionCharacterData(recoil, a);
    //Действия поворота на месте
    UpdateActionCharacterData(actionTurnL, a);
    UpdateActionCharacterData(actionTurnR, a);
    //Действия стояния при стоянии
    for (i = 0; i < numActionIdles; i++)
        UpdateActionIdleData(actionIdle[i], a);
    //Действия стояния в режиме боя
    for (i = 0; i < numFightActionIdles; i++)
        UpdateActionIdleData(actionFightIdle[i], a);
    noBlendTime = 1.0f;
    UpdateAnimation();
}

void Character::UpdateActionMoveData(ActionMove &am, Animation *a)
{
    const char *s;
    if (!a)
        return;
    // set player action
    if (!a->Player(1).SetAction(am.name))
        return;
    // speed
    am.speed = 0.0f;
    s = a->Player(1).GetData("Walk speed");
    if (s && s[0])
        am.speed = static_cast<float>(atof(s));
    // Blend time
    am.tblend = 0.2f;
    s = a->Player(1).GetData("Blend time");
    if (s && s[0])
        am.tblend = static_cast<float>(atof(s));
    if (am.tblend < 0.0f)
        am.tblend = 0.0f;
    // Turn speed
    am.turnspd = turnSpeed;
    s = a->Player(1).GetData("Turn speed");
    if (s && s[0])
        am.turnspd = static_cast<float>(atof(s));
    // reset player
    a->Player(1).SetAction(nullptr);
}

void Character::UpdateActionCharacterData(ActionCharacter &ac, Animation *a)
{
    const char *s;
    if (!a)
        return;
    // set player action
    if (!a->Player(2).SetAction(ac.name))
        return;
    // Blend time
    ac.tblend = 0.0f;
    s = a->Player(2).GetData("Blend time");
    if (s && s[0])
        ac.tblend = static_cast<float>(atof(s));
    if (ac.tblend < 0.0f)
        ac.tblend = 0.0f;
    // reset player
    a->Player(2).SetAction(nullptr);
}

void Character::UpdateActionIdleData(ActionIdle &ai, Animation *a)
{
    const char *s;
    if (!a)
        return;
    // set player action
    if (!a->Player(2).SetAction(ai.name))
        return;
    // Blend time
    ai.tblend = 0.0f;
    s = a->Player(2).GetData("Blend time");
    if (s && s[0])
        ai.tblend = static_cast<float>(atof(s));
    if (ai.tblend < 0.0f)
        ai.tblend = 0.0f;
    //Вероятность
    ai.p = 1.0f;
    s = a->Player(2).GetData("Probability");
    if (s && s[0])
        ai.p = static_cast<float>(atof(s));
    if (ai.p > 1.0f)
        ai.p = 1.0f;
    if (ai.p < 0.0f)
        ai.p = 0.0f;
    // reset player
    a->Player(2).SetAction(nullptr);
}

void Character::UpdateActionDeadData(ActionDead &ai, Animation *a)
{
    const char *s;
    if (!a)
        return;
    // set player action
    if (!a->Player(2).SetAction(ai.name))
        return;
    // Blend time
    ai.tblend = 0.0f;
    s = a->Player(2).GetData("Blend time");
    if (s && s[0])
        ai.tblend = static_cast<float>(atof(s));
    if (ai.tblend < 0.0f)
        ai.tblend = 0.0f;
    //Вероятность
    ai.p = 1.0f;
    s = a->Player(2).GetData("Probability");
    if (s && s[0])
        ai.p = static_cast<float>(atof(s));
    if (ai.p > 1.0f)
        ai.p = 1.0f;
    if (ai.p < 0.0f)
        ai.p = 0.0f;
    // s
    ai.ang = 0.0f;
    s = a->Player(2).GetData("Angle");
    if (s && s[0])
        ai.ang = static_cast<float>(atof(s));
    if (ai.ang > 360.0f)
        ai.ang = 360.0f;
    if (ai.ang < 0.0f)
        ai.ang = 0.0f;
    ai.ang *= 3.141592654f / 180.0f;
    // reset player
    a->Player(2).SetAction(nullptr);
}

bool Character::SetAction(const char *actionName, float tblend, float movespeed, float trnspeed, bool forceStart)
{
    MODEL *m = Model();
    if (!m)
        return false;
    Animation *a = m->GetAnimation();
    if (!a)
        return false;
    const char *curAction = "";
    if (a->Player(0).IsPlaying() && !a->Player(0).IsPause())
    {
        curAction = a->Player(0).GetAction();
        if (!forceStart && curAction && actionName && _stricmp(curAction, actionName) == 0)
            return true;
    }
    if (noBlendTime > 0.0f)
        tblend = 0.0f;
    bspeed.Blend(speed, movespeed, tblend);
    bturn.Blend(turnspd, trnspeed, tblend * 0.1f);
    if (!actionName || !actionName[0])
    {
        if (tblend <= 0.0f)
        {
            a->Player(0).Stop();
            a->Player(0).SetAction(nullptr);
            a->Player(1).Stop();
            a->Player(1).SetAction(nullptr);
            return true;
        }
        if (a->Player(1).IsPlaying())
        {
            a->Player(1).Stop();
            a->Player(1).SetAction(nullptr);
        }
        if (!a->Player(0).IsPlaying())
            return true;
        float start = 0.0f;
        if (a->Timer(0).IsWork())
        {
            start = (1.0f - a->Timer(0).GetCurrent()) * tblend;
            Assert(0.0f <= start && start <= tblend);
        }
        if (start < tblend)
        {
            a->Timer(0).ResetTimer();
            a->Timer(0).Start(tblend, start);
            a->Timer(0).SetPlayer(0, true);
            a->Player(0).SetAutoStop(true);
        }
        else
        {
            a->Player(0).Stop();
            a->Player(0).SetAction(nullptr);
        }
        return true;
    }
    //Сбросим привязанные вещи
    core.Send_Message(blade, "l", 1003);
    //Индекс текущего плеера
    if (a->Player(0).IsPlaying())
        a->CopyPlayerState(0, 1);
    a->Player(0).SetAction(nullptr);
    const bool res = a->Player(0).SetAction(actionName) && a->Player(0).Play();
    a->Timer(0).ResetTimer();
    a->Timer(1).ResetTimer();
    if (tblend)
    {
        a->Timer(0).Start(tblend);
        a->Player(0).SetAutoStop(false);
        a->Player(1).SetAutoStop(true);
        a->Timer(0).SetPlayer(0, false);
        a->Timer(0).SetPlayer(1, true);
    }
    else
    {
        a->Player(1).Stop();
        a->Player(1).SetAction(nullptr);
    }
    return res;
}

void Character::UpdateAnimation()
{
    isActiveState = true;
    if (priorityAction.name)
    {
        curIdleIndex = -1;
        //Специяльные действия
        if (!isSetPriorityAction)
        {
            isSetPriorityAction = true;
            if (!SetAction(priorityAction.name, priorityAction.tblend, priorityActionMoveSpd, priorityActionRotSpd))
            {
                core.Trace("Character animation: not set priority action: \"%s\"", priorityAction.name);
            }
            curMove = nullptr;
            fgtCurType = fgtSetType = fgt_none;
            fgtSetIndex = -1;
        }
    }
    else if (isSwim)
    {
        if (!SetAction(swim.name, swim.tblend, swim.speed, swim.turnspd))
        {
            core.Trace("Character animation: not set priority action: \"%s\"", swim.name);
        }
        curMove = nullptr;
        fgtCurType = fgtSetType = fgt_none;
        fgtSetIndex = -1;
    }
    else
    {
        isSetPriorityAction = false;
        if (!isFight)
        {
            if (isNFHit)
            {
                isNFHit = false;
                curMove = nullptr;
                if (userIdle.name &&
                    (_stricmp(userIdle.name, "Ground_SitDown") == 0 || _stricmp(userIdle.name, "Ground_StandUp") == 0))
                {
                    core.Trace("Not int: \"%s\"", userIdle.name);
                }
                else
                {
                    if (!SetAction(nfhit.name, nfhit.tblend, 0.0f, 0.0f))
                    {
                        curIdleIndex = -1;
                        if (noBlendTime <= 0.0f)
                            core.Trace("Character animation: not set non fight hit action: \"%s\"", nfhit.name);
                    }
                }
            }
            else if (isJump)
            {
                if (!SetAction(fall.name, fall.tblend, 0.0f, 0.0f))
                {
                    core.Trace("Character animation: not fall action: \"%s\"", fall.name);
                }
            }
            else if (isMove)
            {
                curIdleIndex = -1;
                // core.Trace("movesn = %f", movesn);
                if (movecs > CHARACTER_COS_STAIR || IsRun())
                {
                    if (!IsRun())
                    {
                        if (!isBack)
                        {
                            //Простая ходьба
                            if (curMove != &walk)
                            {
                                curMove = &walk;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set walk action: \"%s\"", curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Хотьба назад
                            if (curMove != &backwalk)
                            {
                                curMove = &backwalk;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set back walk action: \"%s\"", curMove->name);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!isBack)
                        {
                            //Бег
                            if (curMove != &run)
                            {
                                curMove = &run;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set run action: \"%s\"", curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Бег назад
                            if (curMove != &backrun)
                            {
                                curMove = &backrun;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set buck run action: \"%s\"", curMove->name);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (!IsRun() || true)
                    {
                        if (!isBack)
                        {
                            //Ходьба по лестнице
                            if (isUp)
                            {
                                curMove = &stsUp;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set stair up action: \"%s\"", curMove->name);
                                }
                            }
                            else
                            {
                                curMove = &stsDown;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set stair down action: \"%s\"", curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Хотьба назад по лестнице
                            if (isUp)
                            {
                                curMove = &stsUpBack;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set back stair up action: \"%s\"",
                                               curMove->name);
                                }
                            }
                            else
                            {
                                curMove = &stsDownBack;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set back stair down action: \"%s\"",
                                               curMove->name);
                                }
                            }
                        }
                    }
                    else
                    {
                        Assert(false);
                        if (!isBack)
                        {
                            //Бег по лестнице вперёд
                            if (isUp)
                            {
                                curMove = &stsUpRun;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set run stair up action: \"%s\"",
                                               curMove->name);
                                }
                            }
                            else
                            {
                                curMove = &stsDownRun;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set run stair down action: \"%s\"",
                                               curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Бег по лестнице назад
                            if (isUp)
                            {
                                curMove = &stsUpRunBack;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set back run stair up action: \"%s\"",
                                               curMove->name);
                                }
                            }
                            else
                            {
                                curMove = &stsDownRunBack;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set back run stair down action: \"%s\"",
                                               curMove->name);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                float ts;
                const char *an;
                //Анимация стояния по умолчанию
                if (!curMove)
                {
                    if (fabsf(strafeVel) < 0.1f)
                    {
                        //Стояние
                        an = FindIdleAnimation(ts);
                        if (an)
                        {
                            if (!SetAction(an, ts, 0.0f, turnSpeed))
                            {
                                curIdleIndex = -1;
                                if (noBlendTime <= 0.0f)
                                    core.Trace("Character animation: not set idle action: \"%s\"", an);
                            }
                        }
                        else
                        {
                            core.Trace("Character: No set idle animation!!!");
                            if (noBlendTime <= 0.0f)
                                SetAction(nullptr, 0.3f, 0.0f, turnSpeed);
                        }
                    }
                    else
                    {
                        //Шагаем в сторну
                        if (strafeVel < 0.0f)
                        {
                            curIdleIndex = -1;
                            if (!SetAction("strafe_left", 0.2f, 0.0f, turnSpeed))
                            {
                                if (noBlendTime <= 0.0f)
                                    core.Trace("Character animation: not set \"strafe_left\" action");
                            }
                        }
                        else
                        {
                            curIdleIndex = -1;
                            if (!SetAction("strafe_right", 0.2f, 0.0f, turnSpeed))
                            {
                                if (noBlendTime <= 0.0f)
                                    core.Trace("Character animation: not set \"strafe_right\" action");
                            }
                        }
                    }
                }
                else
                {
                    //Остановиться
                    an = FindIdleAnimation(ts);
                    if (an)
                    {
                        if (!SetAction(an, ts, 0.0f, turnSpeed))
                        {
                            curIdleIndex = -1;
                            if (noBlendTime <= 0.0f)
                                core.Trace("Character animation: not set idle action: \"%s\"", an);
                        }
                    }
                    else
                    {
                        SetAction(nullptr, curMove->tblend, 0.0f, turnSpeed);
                        curIdleIndex = -1;
                        if (noBlendTime <= 0.0f)
                            core.Trace("Character: No set idle animation!!!");
                    }
                    curMove = nullptr;
                }
            }
        }
        else
        {
            //Режим боя

            if (fgtSetType == fgt_blockbreak)
            {
                int i = 0;
            }

            if (!fightTbl[fgtCurType][fgtSetType])
                return;
            bool isSet = false;
            if (fgtSetType != fgt_none)
            {
                isParryState = false;
                isFeintState = false;
                /*
                        if(_stricmp(characterID, "Blaze") == 0)
                        {
                          entid_t eid;
                          core.FindClass(&eid, "ILogAndActions", 0);
                          char sbuf[256];
                          sprintf_s(sbuf, "Set act: %s", fightNamesTbl[fgtSetType]);
                          core.Send_Message(eid, "lls", 45020, false, sbuf);
                          if(fgtCurType == fgt_blockhit && fgtSetType == fgt_blockhit)
                          {
                            int i = 0;
                          }
                        }
                 */
                char *pWeaponID;
                VDATA *pdat = core.Event("eGetWeaponID", "s", characterID);
                if (pdat)
                {
                    pWeaponID = pdat->GetString();
                }

                switch (fgtSetType)
                {
                case fgt_attack_fast: //Быстрый удар
                    if (!(isSet = SetAction(attackFast[fgtSetIndex].name, attackFast[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set fast attack action: \"%s\"",
                                   attackFast[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "fast");
                        // boal перенос в момент удара core.Event("ChrFgtActApply", "is", GetId(), FGT_ATTACK_FAST);
                        camRotWait = camRotMax = 0.3f;
                        impulse += 5.0f * GetEnemyDirForImpulse();
                    }
                    break;
                case fgt_attack_force: //Выпад и выпад с рубящим ударом
                    if (_stricmp(pWeaponID, "topor") == 0 && fgtSetIndex == 3)
                    {
                        //если с топором, то выпад меняем на рубящий удар
                        fgtSetIndex = rand() % 2;
                    }
                    if (!(isSet =
                              SetAction(attackForce[fgtSetIndex].name, attackForce[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set force attack action: \"%s\"",
                                   attackForce[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "force");
                        // boal перенос в момент удара core.Event("ChrFgtActApply", "is", GetId(), FGT_ATTACK_FORCE);
                        camRotWait = camRotMax = 0.3f;
                        impulse += 2.5f * GetEnemyDirForImpulse();
                    }
                    break;
                case fgt_attack_round: //Круговой удар
                    if (!(isSet =
                              SetAction(attackRound[fgtSetIndex].name, attackRound[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set round attack action: \"%s\"",
                                   attackRound[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "round");
                        // boal перенос в момент удара core.Event("ChrFgtActApply", "is", GetId(), FGT_ATTACK_ROUND);
                        camRotWait = camRotMax = 0.8f;
                    }
                    break;
                case fgt_attack_break: //Пробивающий удар
                    if (!(isSet =
                              SetAction(attackBreak[fgtSetIndex].name, attackBreak[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set break attack action: \"%s\"",
                                   attackBreak[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "break");
                        // boal перенос в момент удара core.Event("ChrFgtActApply", "is", GetId(), FGT_ATTACK_BREAK);
                        camRotWait = camRotMax = 0.3f;
                        impulse += 1.0f * GetEnemyDirForImpulse();
                    }
                    break;
                case fgt_attack_feint: //Финт - спрециальный обманный удар
                    if (!(isSet =
                              SetAction(attackFeint[fgtSetIndex].name, attackFeint[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set feint action: \"%s\"", attackFeint[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "feint");
                        core.Event("ChrFgtActApply", "is", GetId(), FGT_ATTACK_FEINT);
                        camRotWait = camRotMax = 0.3f;
                    }
                    break;
                case fgt_attack_feintc: //Атакующие продолжение финта
                    if (!(isSet =
                              SetAction(attackFeintC[fgtSetIndex].name, attackFeintC[fgtSetIndex].tblend, 0.0f, 4.0f)))
                    {
                        core.Trace("Character animation: not set feint action: \"%s\"", attackFeintC[fgtSetIndex].name);
                    }
                    else
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_ON, 0);
                        core.Event("Event_ChrSnd_Attack", "is", GetId(), "feintc");
                        core.Event("ChrFgtActApply", "is", GetId(), "feintc"); // boal жрем энергию за успех
                        camRotWait = camRotMax = 0.3f;
                    }
                    break;
                case fgt_fire: //Выстрел из пистолета
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    priorityAction.SetName(shot.name);
                    priorityAction.tblend = shot.tblend;
                    priorityActionMoveSpd = 0.0f;
                    priorityActionRotSpd = 3.0f;
                    fgtCurType = fgtSetType = fgt_none;
                    fgtSetIndex = -1;
                    isFired = false;
                    break;
                case fgt_hit_attack: //Реакция попадания удара по персонажу вводящая его в stall
                    if (IsPlayer() && !isStunEnable)
                    // boal не нашел лучшего, но у нас ГГ всегда имеет это ИД, работать будет
                    {
                        if (rand() % 100 >= 50)
                            break; // boal не всегда пробиваться в анимацию
                    }
                    if (isStunEnable)
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                        if (!(isSet = SetAction(hit[fgtSetIndex].name, hit[fgtSetIndex].tblend, 0.0f, 1.0f, true)))
                        {
                            core.Trace("Character animation: not set fight attack hit action: \"%s\"",
                                       hit[fgtSetIndex].name);
                        }
                    }
                    break;
                case fgt_blockbreak: //Реакция попадания удара по персонажу вводящая его в stall
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (!(isSet = SetAction(blockbreak.name, blockbreak.tblend, 0.0f, 1.0f, true)))
                    {
                        core.Trace("Character animation: not set fight blockbreak action: \"%s\"", blockbreak.name);
                    }
                    break;
                case fgt_hit_feint: //Реакция от финта вводящая его в stall
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (!(isSet = SetAction(hitFeint.name, hitFeint.tblend, 0.0f, 0.0f, true)))
                    {
                        core.Trace("Character animation: not set fight feint hit action: \"%s\"", hitFeint.name);
                    }
                    break;
                case fgt_hit_parry: //Реакция от парирования вводящая его в stall
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (!(isSet = SetAction(hitParry.name, hitParry.tblend, 0.0f, 0.0f, true)))
                    {
                        core.Trace("Character animation: not set fight parry hit action: \"%s\"", hitParry.name);
                    }
                    core.Event("ChrFgtActApply", "is", GetId(), "hit_parry");
                    // boal баг фикс FGT_ATTACK_PARRY);
                    break;
                case fgt_hit_round: //Реакция отталкивание круговым ударом
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    impulse.x -= 3.0f * sinf(ay);
                    impulse.z -= 3.0f * cosf(ay);
                    if (!(isSet = SetAction(hitRound.name, hitRound.tblend, 0.0f, 0.0f, true)))
                    {
                        core.Trace("Character animation: not set fight round hit action: \"%s\"", hitRound.name);
                    }
                    break;
                case fgt_hit_fire: //Реакция от выстрела вводящая его в stall
                    if (IsPlayer() && !isStunEnable)
                    {
                        if (rand() % 100 >= 50)
                            break; // boal не всегда пробиваться в анимацию
                    }
                    if (isStunEnable)
                    {
                        core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                        if (!(isSet = SetAction(hitFire.name, hitFire.tblend, 0.0f, 0.0f, true)))
                        {
                            core.Trace("Character animation: not set fight fire hit action: \"%s\"", hitFire.name);
                        }
                    }
                    break;
                case fgt_block: //Защита саблей
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (_stricmp(pWeaponID, "topor") == 0)
                    {
                        if (!(isSet = SetAction(blockaxe.name, blockaxe.tblend, 0.0f, 5.0f, true)))
                        {
                            core.Trace("Character animation: not set block action: \"%s\"", blockaxe.name);
                        }
                    }
                    else
                    {
                        if (!(isSet = SetAction(block.name, block.tblend, 0.0f, 5.0f, true)))
                        {
                            core.Trace("Character animation: not set block action: \"%s\"", block.name);
                        }
                    }
                    break;
                case fgt_blockhit: //Защита саблей
                    if (_stricmp(characterID, "Blaze") == 0)
                    // boal не нашел лучшего, но у нас ГГ всегда имеет это ИД, работать будет
                    {
                        if (rand() % 100 >= 65)
                            break; // boal не всегда пробиваться в анимацию
                    }
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (_stricmp(pWeaponID, "topor") == 0)
                    {
                        if (!(isSet = SetAction(blockaxehit.name, blockaxehit.tblend, 0.0f, 2.0f, true)))
                        {
                            core.Trace("Character animation: not set block axe hit action: \"%s\"", blockaxehit.name);
                        }
                    }
                    else
                    {
                        if (!(isSet = SetAction(blockhit.name, blockhit.tblend, 0.0f, 2.0f, true)))
                        {
                            core.Trace("Character animation: not set block hit action: \"%s\"", blockhit.name);
                        }
                    }
                    break;
                case fgt_parry: //Парирование, защитное движение вводящее противника в stall
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    if (!(isSet = SetAction(parry[fgtSetIndex].name, parry[fgtSetIndex].tblend, 0.0f, 5.0f)))
                    {
                        core.Trace("Character animation: not set block(parry) action: \"%s\"", parry[fgtSetIndex].name);
                    }
                    break;
                case fgt_recoil: //Отскок назад
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    PlaySound("recoil", true);
                    impulse.x -= 2.0f * sinf(ay);
                    impulse.z -= 2.0f * cosf(ay);
                    priorityAction.SetName(recoil.name); //для проверки окончания анимации
                    if (!(isSet = SetAction(recoil.name, recoil.tblend, -3.0f, 0.0f)))
                    {
                        core.Trace("Character animation: not set recoil action: \"%s\"", recoil.name);
                    }
                    break;
                case fgt_strafe_l: //Отскок влево
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    recoilSound = SOUND_INVALID_ID; // PlaySound("recoil", true);
                    impulse += 15.0f * CVECTOR(-cosf(ay), 0.0f, sinf(ay));
                    if (!(isSet = SetAction(strafe_l.name, strafe_l.tblend, 0.0f, 0.0f)))
                    {
                        core.Trace("Character animation: not set recoil action: \"%s\"", strafe_l.name);
                    }
                    break;
                case fgt_strafe_r: //Отскок влево
                    core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                    recoilSound = SOUND_INVALID_ID; // PlaySound("recoil", true);
                    impulse -= 15.0f * CVECTOR(-cosf(ay), 0.0f, sinf(ay));
                    if (!(isSet = SetAction(strafe_r.name, strafe_r.tblend, 0.0f, 0.0f)))
                    {
                        core.Trace("Character animation: not set recoil action: \"%s\"", strafe_l.name);
                    }
                    break;
                }
                if (isSet)
                {
                    fgtCurType = fgtSetType;
                    fgtCurIndex = fgtSetIndex;
                }
                fgtSetType = fgt_none;
                fgtSetIndex = -1;
                curMove = nullptr;
                curIdleIndex = -1;
            }
            else
            {
                core.Send_Message(blade, "ll", MSG_BLADE_TRACE_OFF, 0);
                if (isMove)
                {
                    curIdleIndex = -1;
                    if (!IsRun())
                    {
                        if (!isBack)
                        {
                            //Хотьба вперёд
                            if (curMove != &fightwalk)
                            {
                                curMove = &fightwalk;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set fight walk action: \"%s\"", curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Хотьба назад
                            if (curMove != &fightbackwalk)
                            {
                                curMove = &fightbackwalk;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set fight back walk action: \"%s\"",
                                               curMove->name);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!isBack)
                        {
                            //Бег вперёд
                            if (curMove != &fightrun)
                            {
                                curMove = &fightrun;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set fight run action: \"%s\"", curMove->name);
                                }
                            }
                        }
                        else
                        {
                            //Бег назад
                            if (curMove != &fightbackrun)
                            {
                                curMove = &fightbackrun;
                                if (!SetAction(curMove->name, curMove->tblend, curMove->speed, curMove->turnspd))
                                {
                                    core.Trace("Character animation: not set fight back run action: \"%s\"",
                                               curMove->name);
                                }
                            }
                        }
                    }
                }
                else
                {
                    float ts;
                    const char *an;
                    //Анимация стояния по умолчанию
                    if (!curMove)
                    {
                        //Стояние
                        an = FindFightIdleAnimation(ts);
                        if (an)
                        {
                            if (!SetAction(an, ts, 0.0f, turnSpeed))
                            {
                                curIdleIndex = -1;
                                if (noBlendTime <= 0.0f)
                                    core.Trace("Character animation: not set fight idle \"%s\" action", an);
                            }
                        }
                        else
                        {
                            if (noBlendTime <= 0.0f)
                                core.Trace("Character: No set idle animation!!!");
                            SetAction(nullptr, 0.3f, 0.0f, turnSpeed);
                            curIdleIndex = -1;
                        }
                    }
                    else
                    {
                        //Остановиться
                        an = FindFightIdleAnimation(ts);
                        if (an)
                        {
                            if (!SetAction(an, ts, 0.0f, turnSpeed))
                            {
                                curIdleIndex = -1;
                                if (noBlendTime <= 0.0f)
                                    core.Trace("Character animation: not set fight idle \"%s\" action", an);
                            }
                        }
                        else
                        {
                            SetAction(nullptr, curMove->tblend, 0.0f, turnSpeed);
                            if (noBlendTime <= 0.0f)
                                core.Trace("Character: No set idle animation!!!");
                            curIdleIndex = -1;
                        }
                        curMove = nullptr;
                    }
                }
            }
        }
    }
}

const char *Character::FindIdleAnimation(float &tblend)
{
    //Пользовательская анимация
    if (userIdle.name)
    {
        curIdleIndex = -1;
        tblend = userIdle.tblend;
        return userIdle.name;
    }
    //Стандартная анимация
    if (fabsf(isTurn) <= (curIdleIndex >= 0 ? 0.8f : 0.1f))
    {
        if (numActionIdles <= 0)
        {
            tblend = 0.0f;
            return nullptr;
        }
        if (!isLockIdleForCamera)
        {
            if (curIdleIndex < 0)
            {
                //Расчитываем текущии вероятности и сумарную вероятность
                float allp = 0.0f;
                long i;
                for (i = 0; i < numActionIdles; i++)
                    allp += actionIdle[i].p;
                //Текущее действие
                const float rnd = rand() * allp / RAND_MAX;
                //Выбираем из списка то которое будем проигрывать
                allp = 0.0f;
                for (i = 0; i < numActionIdles; i++)
                {
                    allp += actionIdle[i].p;
                    if (rnd < allp)
                        break;
                }
                if (i >= numActionIdles)
                    i = numActionIdles - 1;
                curIdleIndex = i;
            }
        }
        else
            curIdleIndex = 0;
        tblend = actionIdle[curIdleIndex].tblend;
        isActiveState = false;
        return actionIdle[curIdleIndex].name;
    }
    curIdleIndex = -1;
    //Поворот
    if (isTurn < 0)
    {
        tblend = actionTurnL.tblend;
        return actionTurnL.name;
    }
    tblend = actionTurnR.tblend;
    return actionTurnR.name;
    return nullptr;
}

const char *Character::FindFightIdleAnimation(float &tblend)
{
    //Пользовательская анимация
    if (userIdle.name)
    {
        curIdleIndex = -1;
        tblend = userIdle.tblend;
        return userIdle.name;
    }
    if (curIdleIndex < 0 || curIdleIndex >= numFightActionIdles)
    {
        //Расчитываем текущии вероятности и сумарную вероятность
        float allp = 0.0f;
        long i;
        for (i = 0; i < numFightActionIdles; i++)
            allp += actionFightIdle[i].p;
        //Текущее действие
        const float rnd = rand() * allp / RAND_MAX;
        //Выбираем из списка то которое будем проигрывать
        allp = 0.0f;
        for (i = 0; i < numFightActionIdles; i++)
        {
            allp += actionFightIdle[i].p;
            if (rnd < allp)
                break;
        }
        if (i >= numFightActionIdles)
            i = numFightActionIdles - 1;
        if (recoilLook)
        {
            curIdleIndex = 7; //проиграть дразнилку после отскока
            recoilLook = false;
        }
        else
            curIdleIndex = i;
    }
    if (fabsf(isTurn) <= (curIdleIndex >= 0 ? 0.8f : 0.1f))
        isActiveState = false;
    tblend = actionFightIdle[curIdleIndex].tblend;
    return actionFightIdle[curIdleIndex].name;
    /*
    //Стандартная анимация
    if(fabsf(isTurn) <= 0.0001f)
    {
      tblend = actionFightIdle[curIdleIndex].tblend;
      return actionFightIdle[curIdleIndex].name;
    }else{
      //Поворот
      if(isTurn < 0)
      {
        tblend = actionFightIdle[curIdleIndex].tblend;
        return actionFightIdle[curIdleIndex].name;
      }else{
        tblend = actionFightIdle[curIdleIndex].tblend;
        return actionFightIdle[curIdleIndex].name;
      }
    }
    return null;
    */
}

float Character::GetCurrentPlayerPos()
{
    MODEL *m = Model();
    if (!m)
        return 1.0f;
    Animation *a = m->GetAnimation();
    if (!a)
        return 1.0f;
    float p = a->Player(0).GetPosition();
    if (!a->Player(0).IsPlaying())
        p = 1.0f;
    return p;
}

float Character::GetAniPlayTime()
{
    MODEL *m = Model();
    if (!m)
        return 1.0f;
    Animation *a = m->GetAnimation();
    if (!a)
        return 1.0f;
    float p = a->Player(0).GetPosition();
    if (!a->Player(0).IsPlaying())
        p = 1.0f;
    p = 1.0f - p;
    p *= a->Player(0).GetFrames() * (1.0f / 15.0f);
    return p;
}

Character *Character::FindDialogCharacter()
{
    auto *const location = GetLocation();
    if (IsFight() || liveValue < 0 || deadName)
        return nullptr;
    //Найдём окружающих персонажей
    static Supervisor::FindCharacter fndCharacter[MAX_CHARACTERS];
    static long num = 0;
    if (!location->supervisor.FindCharacters(fndCharacter, num, this, 3.0f))
        return nullptr;
    //Выбираем лутшего
    float minDst;
    long j = -1;
    for (long i = 0; i < num; i++)
    {
        //Персонаж
        Supervisor::FindCharacter &fc = fndCharacter[i];
        //Если воюем, то не говорим с ним
        if (fc.c->IsFight() || fc.c->liveValue < 0 || fc.c->deadName)
            continue;
        //Дистанция
        if (fabsf(fc.dy) > 2.0f)
            continue;
        float d = sqrtf(fc.d2);
        //Углы
        static const float testAng = cosf(0.5f * CHARACTER_DIALOG_ANG * (3.1415926535f / 180.0f));
        float cs = -1.0f;
        if (fc.d2 > 0.0f)
            cs = (fc.dx * sinf(ay) + fc.dz * cosf(ay)) / d;
        if (cs < testAng)
            continue;
        //Дистанция с учётом угла
        d /= cs;
        if (j >= 0)
        {
            if (minDst > d)
            {
                minDst = d;
                j = i;
            }
        }
        else
        {
            minDst = d;
            j = i;
        }
    }
    if (j >= 0)
        return fndCharacter[j].c;
    return nullptr;
}

bool Character::SetPriorityAction(const char *action)
{
    priorityAction.SetName(nullptr);
    if (!action)
        return false;
    MODEL *m = Model();
    if (!m)
        return false;
    Animation *a = m->GetAnimation();
    if (!a)
        return false;
    isResetAutoAction = true;
    priorityAction.SetName(action);
    priorityAction.tblend = 0.0f;
    priorityActionMoveSpd = 0.0f;
    priorityActionRotSpd = 0.0f;
    if (!a->Player(1).SetAction(action))
    {
        a->Player(1).SetAction(nullptr);
        priorityAction.SetName(nullptr);
        return false;
    }
    UpdateActionCharacterData(priorityAction, a);
    isSetPriorityAction = false;
    return true;
}

inline void Character::CheckAttackHit()
{
    //Получим название атаки и реакцию
    bool isBlockBreak = false;
    const char *aname;
    FightAction hitReaction;
    switch (fgtCurType)
    {
    case fgt_attack_fast:
        aname = FGT_ATTACK_FAST;
        hitReaction = fgt_hit_attack;
        break;
    case fgt_attack_force:
        aname = FGT_ATTACK_FORCE;
        hitReaction = fgt_hit_attack;
        break;
    case fgt_attack_round:
        aname = FGT_ATTACK_ROUND;
        hitReaction = fgt_hit_round;
        break;
    case fgt_attack_break:
        aname = FGT_ATTACK_BREAK;
        hitReaction = fgt_blockbreak;
        isBlockBreak = true;
        break;
    case fgt_attack_feintc:
        aname = FGT_ATTACK_FEINT;
        hitReaction = fgt_hit_feint;
        break;
        /*case fgt_attack_feint: //boal для теста добавил и убрал
          aname = FGT_ATTACK_FEINT;
          hitReaction = fgt_hit_feint;
          break;*/
    default:
        return;
    }
    float attackDist = 2.4f, attackAng = (fgtCurType != fgt_attack_round ? 60.0f : 0.0f);
    MODEL *m = Model();
    if (m)
    {
        Animation *a = m->GetAnimation();
        if (a)
        {
            const char *s = a->Player(0).GetData("Dist");
            if (s && s[0])
                attackDist = static_cast<float>(atof(s));
            s = a->Player(0).GetData("Ang");
            if (s && s[0])
                attackAng = static_cast<float>(atof(s));
        }
    }
    //Найдём окружающих персонажей
    static Supervisor::FindCharacter fndCharacter[MAX_CHARACTERS];
    long num = 0;
    auto *const location = GetLocation();
    if (!location->supervisor.FindCharacters(fndCharacter, num, this, attackDist, attackAng, 0.1f, 0.0f, false, true))
        return;
    //Перебираем всех врагов
    bool isParry = false;
    bool isHrrrSound = true;
    bool isUseEnergy = true; // снять энергию один раз boal
    for (long i = 0; i < num; i++)
    {
        //Персонаж
        Supervisor::FindCharacter &fc = fndCharacter[i];
        if (fc.c->liveValue < 0 || fc.c->deadName || fc.d2 <= 0.0f)
            continue;

        if (fc.c->fgtCurType == fgt_parry && fc.c->isParryState)
        {
            isParry = true;
            // fc.c->PlaySound("fgt_parry");
        }
        else if (fc.c->fgtCurType == fgt_attack_feint && fc.c->isFeintState)
        {
            // boal был баг, нет сброса анимации, соотв нельзя проиграть ролик атаки  -->
            // fc.c->StopFightAnimation();
            // StopFightAnimation(); //стопим самого НПС
            // boal <--
            fc.c->Attack(this, fgt_attack_feintc);
            fc.c->UpdateAnimation(); // boal
                                     // fc.c->PlaySound("fgt_feint");
        }
        else
        {
            bool isBlocked = (fc.c->fgtCurType == fgt_block || fc.c->fgtCurType == fgt_blockhit);
            if (isBlockBreak)
                isBlocked = false;
            if (isBlocked)
            {
                // fc.c->PlaySound("fgt_block");
            }
            else
            {
                if (fc.c->fgtCurType >= fgt_attack_fast && fc.c->fgtCurType <= fgt_attack_feintc)
                {
                    fc.c->PlaySound("fgt_collision");
                }
                else
                {
                    // fc.c->PlaySound("fgt_blockbreak");
                }
                // fc.c->PlaySound("fgt_inbody");
                if (isHrrrSound && (rand() & 3))
                {
                    isHrrrSound = false;
                    core.Event("Event_ChrSnd_Hit", "i", fc.c->GetId());
                }
            }
            fc.c->Hit(hitReaction);
            core.Event("Location_CharacterAttack", "iisl", GetId(), fc.c->GetId(), aname, static_cast<long>(isBlocked));
            // boal 12.09.06 отжор энергии по факту удара -->
            if (isUseEnergy && fgtCurType != fgt_attack_feintc)
            {
                // для fgt_attack_feintc идет отжор в анимации, а тут будет "финт", а он стоит 0
                core.Event("ChrFgtActApply", "is", GetId(), aname);
                isUseEnergy = false;
            }
            // boal <--
        }
    }
    if (isParry)
    {
        Hit(fgt_hit_parry);
    }
}

//Найти персонажа в которого попали из пистолета
Character *Character::FindGunTarget(float &kDist, bool bOnlyEnemyTest, bool bAbortIfFriend)
{
    CharactersGroups *chrGroup;
    long grp;
    if (bOnlyEnemyTest || bAbortIfFriend)
    {
        chrGroup = static_cast<CharactersGroups *>(
            EntityManager::GetEntityPointer(EntityManager::GetEntityId("CharactersGroups")));
        grp = chrGroup->FindGroupIndex(group);
       if (grp < 0)
            return nullptr;
    }

    //Найдём окружающих персонажей
    static Supervisor::FindCharacter fndCharacter[MAX_CHARACTERS];
    static long num = 0;
    auto *const location = GetLocation();
    if (!location->supervisor.FindCharacters(fndCharacter, num, this, CHARACTER_FIGHT_FIREDIST, CHARACTER_FIGHT_FIREANG,
                                             0.4f, 30.0f, false))
        return nullptr;
    float minDst;
    long j = -1;
    for (long i = 0; i < num; i++)
    {
        Supervisor::FindCharacter &fc = fndCharacter[i];
        if (fc.d2 <= 0.0f || fc.c->radius <= 0.0f)
            continue;
        //Определяем видимость
        if (!VisibleTest(fc.c))
            continue;
        if (bOnlyEnemyTest)
        {
            const long enemygrpIndex = chrGroup->FindGroupIndex(fc.c->group);
            if (enemygrpIndex < 0)
                continue;
            if (chrGroup->FindRelation(enemygrpIndex, grp).curState != CharactersGroups::rs_enemy)
                continue;
        }
        //Ищем дистанцию с поправками
        fc.d2 = sqrtf(fc.d2);
        float cs = (fc.dx * sinf(ay) + fc.dz * cosf(ay)) / fc.d2;
        cs *= cs * cs;
        if (cs < 0.1f)
            cs = 0.1f;
        const float dist = fc.d2 / cs;
        if (j >= 0)
        {
            if (minDst > dist)
            {
                minDst = dist;
                j = i;
            }
        }
        else
        {
            minDst = dist;
            j = i;
        }
    }
    
    if (j >= 0)
    {
        if (bAbortIfFriend)
        {
            long enemygrpIndex = chrGroup->FindGroupIndex(fndCharacter[j].c->group);
            if (enemygrpIndex < 0)
                return nullptr;
            if (chrGroup->FindRelation(enemygrpIndex, grp).curState != CharactersGroups::rs_enemy)
                return nullptr;
        }
        kDist = 1.0f - fndCharacter[j].d2 / CHARACTER_FIGHT_FIREDIST;
        if (kDist < 0.0f)
            kDist = 0.0f;
        if (kDist > 1.0f)
            kDist = 1.0f;
        return fndCharacter[j].c;
    }
    return nullptr;
}

void Character::FindNearCharacters(MESSAGE &message)
{
    VDATA *array = message.ScriptVariablePointer();
    VDATA *num = message.ScriptVariablePointer();
    const float rad = message.Float();
    const float ax = message.Float();
    const float viewAng = message.Float();
    const float planeDist = message.Float();
    const bool visTest = message.Long() != 0;
    const bool isSort = message.Long() != 0;
    //Ищем персонажей
    //Найдём окружающих персонажей
    static Supervisor::FindCharacter fndCharacter[MAX_CHARACTERS];
    static long n = 0;
    auto *const location = GetLocation();
    if (!location->supervisor.FindCharacters(fndCharacter, n, this, rad, viewAng, planeDist, ax, isSort))
    {
        num->Set(0L);
        return;
    }
    num->Set(n);
    if (!n)
        return;
    if (n > static_cast<long>(array->GetElementsNum()))
        array->SetElementsNum(n);
    char buf[64];
    long nn = 0;
    for (long i = 0; i < n; i++)
    {
        //Информация
        Supervisor::FindCharacter &fc = fndCharacter[i];
        Assert(fc.c);
        if (!fc.c->AttributesPointer)
            continue;
        //Определяем видимость
        if (visTest && !VisibleTest(fc.c))
            continue;
        //Обновим колшичество
        nn++;
        //Элемент массива
        auto *e = (VDATA *)array->GetArrayElement(i);
        //Устанавливаем поля
        e->Set("index", fc.c->AttributesPointer->GetAttribute("index"));
        sprintf_s(buf, "%f", sqrtf(fc.d2));
        e->Set("dist", buf);
        sprintf_s(buf, "%f", fc.dx);
        e->Set("dx", buf);
        sprintf_s(buf, "%f", fc.dy);
        e->Set("dy", buf);
        sprintf_s(buf, "%f", fc.dz);
        e->Set("dz", buf);
    }
    num->Set(nn);
}

//Проверить видимость
bool Character::CharactersVisibleTest(MESSAGE &message)
{
    auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(message.EntityID()));
    if (!chr)
        return false;
    return VisibleTest(chr);
}

//Проверка видимости двух персонажей
bool Character::VisibleTest(Character *chr)
{
    if (!chr)
        return false;
    auto *const location = GetLocation();
    //Тест по оси персонажа
    if (location->VisibleTest(curPos + CVECTOR(0.0f, height * 0.9f, 0.0f),
                              chr->curPos + CVECTOR(0.0f, chr->height * 0.97f, 0.0f)))
        return true;
    if (location->VisibleTest(curPos + CVECTOR(0.0f, height * 0.9f, 0.0f),
                              chr->curPos + CVECTOR(0.0f, chr->height * 0.67f, 0.0f)))
        return true;
    if (location->VisibleTest(curPos + CVECTOR(0.0f, height * 0.9f, 0.0f),
                              chr->curPos + CVECTOR(0.0f, chr->height * 0.37f, 0.0f)))
        return true;
    //Трейсим лучи смещённые от центра
    if (location->VisibleTest(curPos + CVECTOR(0.0f, height * 0.9f, 0.0f),
                              chr->curPos + CVECTOR(0.3f * cosf(chr->ay), chr->height * 0.8f, -0.3f * sinf(chr->ay))))
        return true;
    if (location->VisibleTest(curPos + CVECTOR(0.0f, height * 0.9f, 0.0f),
                              chr->curPos + CVECTOR(-0.3f * cosf(chr->ay), chr->height * 0.8f, 0.3f * sinf(chr->ay))))
        return true;
    //Спрятался...
    return false;
}

//Обновить состояние оружия
void Character::UpdateWeapons()
{
    if (isFightWOWps)
    {
        EntityManager::RemoveFromLayer(REALIZE, blade);
    }
    else
    {
        EntityManager::AddToLayer(REALIZE, blade, 65550);
    }
}

//Получить направление на противника для подскока при ударе
CVECTOR Character::GetEnemyDirForImpulse()
{
    auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(enemyAttack));
    if (!chr)
        return CVECTOR(0.0f);
    CVECTOR dir = chr->curPos - curPos;
    dir.y = 0.0f;
    const float l = ~dir;
    if (l > 0.0f)
    {
        dir *= 1.0f / sqrtf(l);
        static const float jAng = cosf(20.0f * 3.141592653f / 180.0f);
        if ((dir.x * sinf(ay) + dir.z * cosf(ay)) >= jAng)
        {
            return dir;
        }
    }
    return CVECTOR(0.0f);
}

//Получить постфикс
const char *Character::GetValueByPrefix(const char *str, const char *pref)
{
    if (!str || !str[0])
        return nullptr;
    if (!pref || !pref[0])
        return nullptr;
    while (true)
    {
        if (*pref == 0)
            return str;
        if (*str++ != *pref++)
            break;
    }
    return nullptr;
}

CVECTOR Character::GetHandLightPos()
{
    MODEL *mdlChr = Model();
    NODE *mdlNode = mdlChr ? mdlChr->GetNode(0) : nullptr;
    if (mdlNode)
    {
        long sti = -1;
        auto idLocator = mdlNode->geo->FindName(m_pcHandLightLocator);

        if ((sti = mdlNode->geo->FindLabelN(sti + 1, idLocator)) > -1)
        {
            Animation *ani = mdlChr->GetAnimation();
            CMatrix *bones = &ani->GetAnimationMatrix(0);

            GEOS::LABEL lb;
            mdlNode->geo->GetLabel(sti, lb);
            CMatrix mt;
            mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
            mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
            mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
            mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

            CMatrix mbn = mt * bones[lb.bones[0]];
            mbn.Pos().x *= -1.0f;
            mbn.Vx().x *= -1.0f;
            mbn.Vy().x *= -1.0f;
            mbn.Vz().x *= -1.0f;

            CMatrix scl;
            scl.Vx().x = -1.0f;
            scl.Vy().y = 1.0f;
            scl.Vz().z = 1.0f;
            mbn.EqMultiply(scl, CMatrix(mbn));

            return (mbn * mdlChr->mtx).Pos();
        }
    }

    return CVECTOR(0.f, 0.f, 0.f);
}

// boal -->
void Character::StopFightAnimation()
{
    MODEL *m = Model();
    Animation *a = nullptr;
    if (m)
        a = m->GetAnimation();
    if (a)
    {
        priorityAction.SetName(nullptr);
        a->Player(0).Pause();
        a->Player(0).SetPosition(1.0f);
        fgtSetType = fgtCurType = fgt_none;
    }
}

// boal <--

bool Character::CheckObstacle(float fx, float fz, float fzlen)
{
    const float ca = cosf(ay);
    const float sa = sinf(ay);
    const float fdx = fx * ca + fz * sa;
    const float fdz = fz * ca - fx * sa;
    const float fk = fzlen / sqrtf(fdx * fdx + fdz * fdz);
    const float xmed = curPos.x + fdx;
    const float zmed = curPos.z + fdz;
    const float xbeg = xmed - fk * fdz;
    const float xend = xmed + fk * fdz;
    const float zbeg = zmed + fk * fdx;
    const float zend = zmed - fk * fdx;
    /*float xbeg = xmed - fzlen*0.5f*sa;
    float xend = xmed + fzlen*0.5f*sa;
    float zbeg = zmed + fzlen*0.5f*ca;
    float zend = zmed - fzlen*0.5f*ca;*/

    float fTrace;
    float fHDist;
    CVECTOR vsrc, vdst;

    auto *const location = GetLocation();

    // проверим среднюю точку
    vsrc = CVECTOR(xmed, 50.f, zmed);
    vdst = CVECTOR(xmed, -50.f, zmed);
    fTrace = location->GetPtcData().Trace(vsrc, vdst);
    if (fTrace < 0.f || fTrace > 1.f)
        return true;
    fHDist = vsrc.y + fTrace * (vdst.y - vsrc.y) - curPos.y;
    if (fHDist * fHDist > 0.25f)
        return true;

    // проверим начальную точку
    vsrc = CVECTOR(xbeg, 50.f, zbeg);
    vdst = CVECTOR(xbeg, -50.f, zbeg);
    fTrace = location->GetPtcData().Trace(vsrc, vdst);
    if (fTrace < 0.f || fTrace > 1.f)
        return true;
    fHDist = vsrc.y + fTrace * (vdst.y - vsrc.y) - curPos.y;
    if (fHDist * fHDist > 0.25f)
        return true;

    // проверим конечную точку
    vsrc = CVECTOR(xend, 50.f, zend);
    vdst = CVECTOR(xend, -50.f, zend);
    fTrace = location->GetPtcData().Trace(vsrc, vdst);
    if (fTrace < 0.f || fTrace > 1.f)
        return true;
    fHDist = vsrc.y + fTrace * (vdst.y - vsrc.y) - curPos.y;
    if (fHDist * fHDist > 0.25f)
        return true;

    return false;
}

long Character::GetRandomIndexByObstacle(ObstacleZone *pZone, long num)
{
    long n, m, i, q;

    // найдем количество используемых зон
    q = 0;
    for (i = 0; i < num; i++)
        if (pZone[i].use)
            q++;

    while (q > 0)
    {
        // найдем рандомный номер из еще используемых зон
        n = rand() % q;
        m = 0;
        for (i = 0; i < num; i++)
        {
            if (!pZone[i].use)
                continue;
            if (m == n)
                break;
            m++;
        }

        // ничего тут нет
        if (i == num)
            break;

        // проверим зону на наличие препятсвий в ней
        if (pZone[i].x == 0 && pZone[i].z == 0)
            return i;
        if (!CheckObstacle(pZone[i].x, pZone[i].z, pZone[i].dw))
            return i;

        // отметим зону как неиспользуемую
        pZone[i].use = false;
        q--;
    }
    return -1;
}

Location *Character::GetLocation()
{
    auto *const location = static_cast<Location *>(EntityManager::GetEntityPointer(loc_id));

    if (location)
        return location;

    loc_id = EntityManager::GetEntityId("location");
    return static_cast<Location *>(EntityManager::GetEntityPointer(loc_id));
}
