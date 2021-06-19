//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Character
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "model.h"
#include "vmodule_api.h"
#include <string>

class Location;
class MODEL;
class Animation;
class LocatorArray;
class Supervisor;
class VSoundService;

#define CHARACTER_ATTACK_DIST 2.4f // Maximum attack distance (m)
#define CHARACTER_ATTACK_ANG 60.0f // Maximum angle of attack (degrees)
#define CHARACTER_BLOCK_ANG 50.0f // Maximum angle of block (degrees)
#define CHARACTER_MAX_JMP_STEPS 50

class Character : public Entity
{
    float hScale;
    float wScale;

    friend Supervisor;

    struct ActionCharacter
    {
        ActionCharacter();
        ~ActionCharacter();
        void SetName(const char *_name);
        void ChangeName(const char *_name);

        char *name;   // Action name
        float tblend; // Blending start time
    };

    // Move action
    struct ActionMove : public ActionCharacter
    {
        ActionMove();
        float speed;   // Travel speed for a given action m / s
        float turnspd; // Turn speed rad / sec
    };

    // Inaction action
    struct ActionIdle : public ActionCharacter
    {
        ActionIdle();
        float p; // The overall probability of choosing this action
    };

    // Combat actions

    // Attack
    struct ActionFightAttack : ActionCharacter
    {
        float hitTime; // Hit response time
    };

    // Death action
    struct ActionDead : public ActionIdle
    {
        ActionDead();
        float ang; // Fall direction
    };

    struct Blender
    {
        Blender();
        void Blend(float _old, float _new, float _time);
        bool Update(float dltTime);
        float Get() const;

        float old;
        float dlt;
        float kTime;
        float time;
    };

    struct Detector
    {
        Detector(LocatorArray *_la);
        void Check(float dltTime, Character *ch);
        void Exit(Character *ch);

        LocatorArray *la;
        long lastLocator;
        float timeInLocator;
        float lastEventTime;
    };

    friend Detector;

    class RTuner : public MODEL::RenderTuner
    {
        void Set(MODEL *model, VDX9RENDER *rs) override;
        void Restore(MODEL *model, VDX9RENDER *rs) override;

      public:
        RTuner();
        float alpha;
        float chrAlpha;
        float camAlpha;
        float selected;
        Character *character;
        bool isVisible;

        float GetAlpha() const;
    };

    class EventListener : public AnimationEventListener
    {
      public:
        Character *character;
        // Accept event
        void Event(Animation *animation, long index, long eventID, AnimationEvent event) override;
        // Accept event
        void Event(Animation *animation, long playerIndex, const char *eventName) override;
    };

    friend RTuner;
    friend EventListener;

    struct ObstacleZone
    {
        float x, z;
        float dw;
        bool use;
    };

  protected:
    enum FightAction
    {
        fgt_none = 0,
        // There is no fighting
        fgt_attack_fast,
        // Quick hit
        fgt_attack_force,
        // Swipe
        fgt_attack_round,
        // Circular blow
        fgt_attack_break,
        // Piercing blow
        fgt_attack_feint,
        // Feint - special trick
        fgt_attack_feintc,
        // Continuation of the feint in case of a counterattack
        fgt_fire,
        // Pistol shot
        fgt_hit_attack,
        // The reaction of hitting a character putting him into the stall
        fgt_hit_feint,
        // The reaction from the feint putting him into the stall
        fgt_hit_parry,
        // The parry reaction putting him into the stall
        fgt_hit_round,
        // Knockback reaction with a circular hit
        fgt_hit_fire,
        // The reaction from the shot, putting him into stall
        fgt_block,
        // Saber protection
        fgt_blockhit,
        // Saber protection
        fgt_blockbreak,
        // Punching the block
        fgt_parry,
        // Parry, a defensive move putting the opponent into a stall
        fgt_recoil,
        // Bounce back
        fgt_strafe_l,
        // Bounce to the left
        fgt_strafe_r,
        // Bounce right
        fgt_max,
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Character();
    virtual ~Character();

    // Initialization
    bool Init() override;
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;
    // Changing an attribute
    uint32_t AttributeChanged(ATTRIBUTES *apnt) override;

    void ProcessStage(Stage, uint32_t) override
    {
    }

    void SetSignModel();
    void SetSignTechnique();
    void ReadFightActions(ATTRIBUTES *at, ActionCharacter actions[4], long &counter);

    virtual bool PostInit()
    {
        return true;
    };

    virtual uint32_t ChlProcessMessage(long messageID, MESSAGE &message)
    {
        return 0;
    };

    void AlreadySTORM_DELETE();

    //--------------------------------------------------------------------------------------------
    // Character model
    //--------------------------------------------------------------------------------------------
  public:
    // Get a character model
    MODEL *Model() const;

    // Move model to point x, y, z
    bool Teleport(float x, float y, float z);
    // Move the model to point x, y, z and direct along ay
    bool Teleport(float x, float y, float z, float ay);
    // Move model to locator
    bool Teleport(const char *group, const char *locator);

    // Set loading positions
    void SetSavePosition();
    // Delete loading positions
    void DelSavePosition(bool isTeleport);
    // Save parameters
    virtual void SetSaveData(ATTRIBUTES *sdata){};
    // Restore parameters
    virtual void GetSaveData(ATTRIBUTES *sdata){};
    void StopFightAnimation(); // boal

  protected:
    virtual void CharacterTeleport(){};

    virtual void HitChild(bool isInBlock){};

    //--------------------------------------------------------------------------------------------
    // Character animation
    //--------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------
    // Character commands
    //--------------------------------------------------------------------------------------------
  public:
    // To go
    void StartMove(bool isBack = false);
    // Stay
    void StopMove();
    // Guide the character using the vector
    void Turn(float dx, float dz);
    // Guide the character using the angle
    void Turn(float _ay);
    // Find out direction
    float GetAY() const;
    // Set running mode
    void SetRunMode(bool _isRun = true);
    // Are we moving
    bool IsMove() const;
    // Check running mode
    bool IsRun() const;
    // Are we sailing
    bool IsSwim() const;
    // Set battle mode
    bool SetFightMode(bool _isFight, bool isPlayAni = true);
    // Check if can switch to combat mode
    bool IsFightEnable() const;
    // Check the battle mode
    bool IsFight() const;
    // At the moment, there is a choice of a target for a shot (by the player)
    bool IsFireFindTarget() const;
    // In dialogue
    bool IsDialog() const;
    // Get character height
    float GetHeight() const;
    // Get character position
    void GetPosition(CVECTOR &pos) const;
    // Get character position
    void GetGrassPosition(CVECTOR &pos, CVECTOR &gpos) const;
    // Set the flag of walking on the grass
    void SetGrassSound();
    // Get maximum character radius
    float GetRadius() const;
    // to attack
    void Attack(Character *enemy, FightAction type);
    // Block
    void Block();
    // Parry
    void Parry();
    // Rebound
    void Recoil();
    // Jump left
    void StrafeLeft();
    // Jump right
    void StrafeRight();
    // Hit
    void Hit(FightAction type);
    // Shot
    void Fire();
    // Check if the gun is loaded
    bool IsGunLoad() const;
    // Death
    void Dead();
    // Entering the location
    void EntryToLocation();
    // Exit the location
    void ExitFromLocation();
    // May have already died
    bool IsDead() const;
    // bounce
    void StartJump();

    // Is there a weapon
    bool IsSetBlade() const;

    // Set transparency of character
    void SetCameraAlpha(float alpha);
    // Block selection of idle animation
    void LockIdle(bool isLock);
    // Highlight character
    void Select();

    // Block character movement
    void LockMove(bool isLock);
    // Lock character rotation behind the camera
    void LockRotate(bool isLock);
    // Relative speed of camera rotation
    float CameraTurnSpeed() const;
    // Viewing from the eyes
    void LookFromEyes(bool isLook);

    //--------------------------------------------------------------------------------------------
    // Character
    //--------------------------------------------------------------------------------------------
  public:
    // Reset state
    virtual void Reset();
    // Move the character to the desired position
    virtual void Move(float dltTime);
    // Carry out additional calculations
    virtual void Calculate(float dltTime){};
    // Update character position
    virtual void Update(float dltTime);

    // Action animation end event
    virtual void ActionEvent(const char *actionName, Animation *animation, long index, long eventID,
                             AnimationEvent event);
    virtual void ActionEvent(Animation *animation, long playerIndex, const char *eventName);

    // The character attacks, analyze to whom to apply damage
    // virtual void CharacterNowIsAttacked(){};

    long PlaySound(const char *soundName, bool isLoop = false, bool isCached = false);
    void PlayStep();
    void SetSoundPosition(long id);
    void ReleaseSound(long id);

    virtual bool IsPlayer()
    {
        return false;
    }

  protected:
    // Find a character for dialogue
    Character *FindDialogCharacter();

    long startColCharacter;
    long numColCharacter;
    bool isSlide;
    CVECTOR slideDir;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    bool zLoadModel(MESSAGE &message);
    bool zTeleport(MESSAGE &message, bool isAy);
    bool zTeleportL(MESSAGE &message);
    bool zAddDetector(MESSAGE &message);
    bool zDelDetector(MESSAGE &message);
    bool zActionPlay(MESSAGE &message);
    bool zEntry(MESSAGE &message);
    bool zSetBlade(MESSAGE &message);
    bool zSetGun(MESSAGE &message);
    bool zTurnByLoc(MESSAGE &message);
    bool zTurnByChr(MESSAGE &message);
    bool zTurnByPoint(MESSAGE &message);
    bool zDistByCharacter(MESSAGE &message, bool is2D);
    uint32_t zExMessage(MESSAGE &message);
    bool zPlaySound(MESSAGE &message);
    bool TestJump(CVECTOR pos);
    bool BuildJump(CVECTOR pos, float fAng);
    bool TraceWithObstacle(const CVECTOR &src, const CVECTOR &dst);
    void UpdateActionsData();
    void UpdateActionMoveData(ActionMove &am, Animation *a);
    void UpdateActionCharacterData(ActionCharacter &ac, Animation *a);
    void UpdateActionIdleData(ActionIdle &ai, Animation *a);
    void UpdateActionDeadData(ActionDead &ai, Animation *a);
    bool SetAction(const char *actionName, float tstart = 0.0f, float movespeed = 0.0f, float trnspeed = 0.0f,
                   bool forceStart = false);
    void UpdateAnimation();
    const char *FindIdleAnimation(float &tstart);
    const char *FindFightIdleAnimation(float &tstart);
    float GetCurrentPlayerPos();
    float GetAniPlayTime();
    bool SetPriorityAction(const char *action);
    // Check another character for hit by us
    void CheckAttackHit();
    // Refresh weapon state
    void UpdateWeapons();
    // Get direction towards the enemy to bounce on hit
    CVECTOR GetEnemyDirForImpulse();

    bool PriorityActionIsJump() const;

  protected:
    // Find a character who was hit by a pistol kDist = 1..0
    Character *FindGunTarget(float &kDist, bool bOnlyEnemyTest = false, bool bAbortIfFriend = false);
    // Find characters "eeffffll"
    void FindNearCharacters(MESSAGE &message);
    // Check visibility
    bool CharactersVisibleTest(MESSAGE &message);

  public:
    // Checking the visibility of two characters
    bool VisibleTest(Character *chr);
    // Get postfix
    static const char *GetValueByPrefix(const char *str, const char *pref);

  protected:
    //
    VSoundService *soundService;
    // Character sizes
    float radius, radiusNrm, radiusFgt;
    // Current and previous positions
    CVECTOR curPos, oldPos, colMove, grsPos, impulse;
    float strafeMove;
    float seaY;

    long currentNode; // Current node

    float speed;    // Current travel speed
    float turnspd;  // Turn speed
    Blender bspeed; // Linear Speed Blender
    Blender bturn;  // Turn Speed Blender
    float ay;       // Character direction
    float nay;      // Desired direction
    float turnDir;  // Direction of rotation + 1..0 ..- 1
    float vy;       // Falling speed
    float height;   // Character height

    float turnSpeed; // Turn speed in radians

    // Character parameters
    float chclass;        // Character class 0..1, defining behavior
    float fatigue;        // Fatigue 0..1
    float kSpd;           // Desired speed factor
    float kSpdCur;        // Current speed factor
    float kSpdDlt;        // Variation in speed for different characters
    bool isMove;          // Are we standing or moving
    bool isBack;          // Go back
    bool isRun;           // Is running mode set
    bool isRunDisable;    // Is running mode prohibited
    bool isFight;         // Combat mode
    bool lockFightMode;   // Block combat mode
    bool isSwim;          // are swimming
    bool isJump;          // jump
    bool isJumpSnd;       // Gotta play the jump sound
    bool isEnableJump;    // Is jumping off allowed
    bool isUp;            // Displacement gradient direction
    bool isCollision;     // Collided with another character
    bool isTurnLock;      // Tutn lock
    bool lockMove;        // Forbid Move
    bool lockRotate;      // Prevent rotation
    bool isDialog;        // Character in dialogue
    bool isFightWOWps;    // Allow to switch to unarmed combat mode
    bool isActiveState;   // If true then character is active
    bool recoilLook;      // To trigger the teaser after rebound
    float swimChange;     // Mode change response time
    float isTurn;         // Turning around
    bool isNFHit;         // Out of combat mode
    float movecs;         // Cosine of the angle of inclination of the plane
    const char *deadName; // Death action name
    long jumpSound;       // The sound of clothing when jumping
    long recoilSound;     // The sound of recoil

    float noBlendTime;

    bool isLookFromEyes;

    ActionMove *curMove; // Current move action

    ActionMove walk;           // Walking forward
    ActionMove backwalk;       // Walking back
    ActionMove run;            // Running forward
    ActionMove backrun;        // Running back
    ActionMove stsUp;          // Go up the stairs
    ActionMove stsDown;        // Go down the stairs
    ActionMove stsUpBack;      // Go up the stairs back
    ActionMove stsDownBack;    // Go down the stairs back
    ActionMove stsUpRun;       // Run up the stairs
    ActionMove stsDownRun;     // Run down the stairs
    ActionMove stsUpRunBack;   // Run up the stairs back
    ActionMove stsDownRunBack; // Run down the stairs back
    ActionMove swim;           // Swim
    ActionMove fall;           // Jumping off
    ActionMove fall_land;      // Jumping off, getting up on the ground
    ActionMove fall_water;     // Jumping, surfacing in water
    ActionMove jump;           // Jumping

    ActionIdle nfhit; // Reaction outside combat mode

    // Idle actions
    long curIdleIndex;
    ActionIdle actionIdle[32];
    long numActionIdles;
    ActionIdle actionFightIdle[32];
    long numFightActionIdles;

    // Death
    ActionDead actionDead[8];
    long numActionDead;
    ActionDead actionFightDead[8];
    long numActionFightDead;

    ActionCharacter actionTurnL; // Left turn action
    ActionCharacter actionTurnR; // Right turn action
    ActionCharacter userIdle;    // Action set from outside

    // The fight
    // Normal movements
    ActionMove fightwalk;     // Walking forward in combat mode
    ActionMove fightbackwalk; // Walking back in combat mode
    ActionMove fightrun;      // Running forward in combat mode
    ActionMove fightbackrun;  // Running back in combat mode
    // Attacks
    ActionCharacter attackFast[4];   // Rapid attacks
    long numAttackFast;              // Number of quick attacks
    ActionCharacter attackForce[4];  // Power attacks
    long numAttackForce;             // Number of power attacks
    ActionCharacter attackRound[4];  // Circular attack
    long numAttackRound;             // Number of circular attacks
    ActionCharacter attackBreak[4];  // Providing attacks
    long numAttackBreak;             // Number of piercing attacks
    ActionCharacter attackFeint[4];  // Feint
    ActionCharacter attackFeintC[4]; // Attacking continuation of the feint
    long numAttackFeint;             // Number of feints
    ActionCharacter parry[4];        // Parry
    long numParry;                   // Number of parries
    // Remote attacks
    ActionCharacter shot; // Pistol shot
    // Reactions
    ActionCharacter hit[4];   // Character hit
    long numHits;             // Number of hit options
    ActionCharacter hitFeint; // Reaction to feint
    ActionCharacter hitParry; // Parry reaction
    ActionCharacter hitRound; // Knockback reaction with a circular hit
    ActionCharacter hitFire;  // Shot reaction
    // Protection
    ActionCharacter block;       // Block
    ActionCharacter blockaxe;    // Block with an ax
    ActionCharacter blockhit;    // Hitting a character in a block
    ActionCharacter blockaxehit; // Hitting a character in a block with an ax
    ActionCharacter blockbreak;  // Punching the block
    ActionCharacter recoil;      // Bounce back
    ActionCharacter strafe_l;    // Bounce to the left
    ActionCharacter strafe_r;    // Bounce to the right
    long stunChance;             // Is the stun allowed after the enemy's blow
    // Logical state
    FightAction fgtCurType;   // Current action type
    long fgtCurIndex;         // Current Activity Index
    FightAction fgtSetType;   // Set action type
    long fgtSetIndex;         // Set action index
    bool isParryState;        // Parry condition
    bool isFeintState;        // Parry condition
    bool isFired;             // Is a shot fired
    bool isLockIdleForCamera; //
    float recoilWait;         // Delay after jump
    float strafeWait;         // Delay after strafe
    float camRotWait;         // Current turn delay ratio
    float camRotMax;          // Maximum turning time
    float strafeAngle;        // Strafe angle
    float strafeVel;          // Strafe in place
    entid_t enemyAttack;      // Who do we focus on during the attack

    // Table of the possibility of changing one action by another
    static uint8_t fightTbl[fgt_max][fgt_max];
    static const char *fightNamesTbl[fgt_max];

    // Action to be played
    ActionCharacter priorityAction;
    float priorityActionMoveSpd;
    float priorityActionRotSpd;
    bool isResetAutoAction;
    bool isSetPriorityAction;

    // Detectors
    Detector *detector[64];
    long numDetectors;

    // Character model
    entid_t mdl;
    // Shadow
    entid_t shadow;
    // Saber
    entid_t blade;
    bool isBladeSet;
    bool isGunSet;
    // The sea
    entid_t sea;

    entid_t effects;
    // Anchored sign model
    entid_t sign;
    std::string signName;
    std::string signTechniqueName;

    // Circles on the water
    entid_t waterrings;
    float stepsRate;

    // If set, do not remove from supervisor
    bool isDeleted;

    // Character gender
    bool isMale;
    // Footstep sound
    bool soundStep;
    // The sound of rustling in the grass
    bool soundGrass;

    // Dynamic appearance and removal
    RTuner tuner;
    float liveValue; // >0 appears, <0 disappears, 0 does not change

    // Animation events
    EventListener eventListener;

    float ayStack[16];
    long ayStackPointer;

    CVECTOR jumpTrack[50];
    CVECTOR osculationPoint;
    float jumpFallTime;
    float curJumpFallTime;
    long jumpPoints;

    char *characterID; // Character ID

    //
    long m_nHandLightID;
    const char *m_pcHandLightLocator;
    CVECTOR GetHandLightPos();

    bool CheckObstacle(float fx, float fz, float fzlen);
    long GetRandomIndexByObstacle(ObstacleZone *pZone, long num);

    Location *GetLocation();

    long eventId = {};

  public:
    bool isPlayerEnemy;
    float enemyBarsAlpha;

    // Information for groups
    struct GrpTarget
    {
        entid_t chr;   // Goal ID
        float time;    // Time to lose sight of the target
        float timemax; // Maximum target holding time
    };

    GrpTarget grpTargets[32]; // Goal list
    long numTargets;          // Number of targets
    long groupID;             // Group index for faster search
    char group[128];          // Current group name

  private:
    // The location the character is in
    entid_t loc_id;
};

inline void Character::AlreadySTORM_DELETE()
{
    isDeleted = true;
}

// Find out direction
inline float Character::GetAY() const
{
    return ay;
}

// Get running mode
inline bool Character::IsRun() const
{
    return isRun && !isRunDisable;
}

// Are we sailing
inline bool Character::IsSwim() const
{
    return isSwim;
}

// Get the battle mode
inline bool Character::IsFight() const
{
    return isFight;
}

// In dialogue
inline bool Character::IsDialog() const
{
    return isDialog;
}

// Get character height
inline float Character::GetHeight() const
{
    return height;
}

// Get character position
inline void Character::GetPosition(CVECTOR &pos) const
{
    pos = curPos;
    if (isSwim)
        pos.y = seaY;
}

// Get character position
inline void Character::GetGrassPosition(CVECTOR &pos, CVECTOR &gpos) const
{
    pos = curPos;
    gpos = grsPos;
}

// Set the flag of walking on the grass
inline void Character::SetGrassSound()
{
    soundGrass = true;
}

// Get maximum character radius
inline float Character::GetRadius() const
{
    return radius;
}

// Set transparency of character
inline void Character::SetCameraAlpha(float alpha)
{
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    tuner.camAlpha = alpha;
}

// Are we moving
inline bool Character::IsMove() const
{
    return isMove;
}

// Block selection of idle animation
inline void Character::LockIdle(bool isLock)
{
    isLockIdleForCamera = isLock;
}

// Highlight character
inline void Character::Select()
{
    tuner.selected = 1.0f;
}

// Block character movement
inline void Character::LockMove(bool isLock)
{
    lockMove = isLock;
}

// Lock character rotation behind the camera
inline void Character::LockRotate(bool isLock)
{
    lockRotate = isLock;
}

// Relative speed of camera rotation
inline float Character::CameraTurnSpeed() const
{
    if (camRotWait <= 0.0f || camRotMax <= 0.0f)
        return 1.0f;
    auto k = camRotWait / camRotMax;
    if (k > 1.0f)
        k = 1.0f;
    return powf(1.0f - k, 1.8f);
}

// Viewing from the eyes
inline void Character::LookFromEyes(bool isLook)
{
    isLookFromEyes = isLook;
}

// May have already died
inline bool Character::IsDead() const
{
    return (liveValue < 0 || deadName);
}

// Is there a weapon
inline bool Character::IsSetBlade() const
{
    return isBladeSet | isFightWOWps;
}

inline bool Character::PriorityActionIsJump() const
{
    return (priorityAction.name &&
            (_stricmp(priorityAction.name, jump.name) == 0 || _stricmp(priorityAction.name, fall.name) == 0));
}
