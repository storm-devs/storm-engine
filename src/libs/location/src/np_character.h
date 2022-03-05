//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    NPCaracter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "ai_character.h"
#include "entity.h"
#include "core.h"

class NPCharacter : public AICharacter
{
  protected:
    enum NPCTask
    {
        npct_unknow = 0,
        npct_none,
        // No task, the character is controlled from the outside
        npct_stay,
        // Stand still
        npct_gotopoint,
        // Go to the point
        npct_runtopoint,
        // Run to the point
        npct_followcharacter,
        // Follow the character
        npct_fight,
        // Fight another character
        npct_escape,
        // Leave the character
        npct_dead,
        // Death of a character
        npct_max
    };

    struct Task
    {
        NPCTask task;
        CVECTOR to;
        entid_t target;

        union {
            uint32_t flags;

            struct
            {
                uint32_t isRun : 1;
                uint32_t isFight : 1;
                uint32_t isFollowInit : 1;
            };
        };
    };

    struct EnemyState
    {
        NPCharacter *chr; // Pointer to the enemy
        float look;       // Enemy direction towards us (cos)
        float dir;        // The location of the enemy relative to us (cos)
        float state;      // Enemy Condition Coefficient
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    NPCharacter();
    ~NPCharacter() override;

    bool PostInit() override;

    uint32_t ChlProcessMessage(int32_t messageID, MESSAGE &message) override;
    void Move(float dltTime) override;
    void Update(float dltTime) override;

    //--------------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------------
  public:
    // Get attacking character
    Character *GetAttackedCharacter() const;

    // --------------------------------------------------------------------------------------------
    // Tasks
    // --------------------------------------------------------------------------------------------

    // Set a new task
    bool SetNewTask(NPCTask tsk, MESSAGE &message);

    bool InitFollowChartacter(entid_t eid);
    bool InitFightChartacter(entid_t eid);

    // --------------------------------------------------------------------------------------------
    // Executing tasks
    // --------------------------------------------------------------------------------------------
  protected:
    // Completing the task of following a character
    void UpdateFollowCharacter(float dltTime);
    // Execution of the escape task
    void UpdateEscapeCharacter(float dltTime);
    // Completing the task of fighting a character
    void UpdateFightCharacter(float dltTime);

    // The fight
    // Combat behavior
    void DoFightAction(float dltTime, NPCharacter *enemy);
    // Combat behavior - idle
    void DoFightActionAnalysisNone(float dltTime, NPCharacter *enemy);
    // Combat behavior - attack
    void DoFightAttack(Character *enemy, int32_t enemyCounter, bool wishDefence);
    // Combat behavior - block, parry
    void DoFightBlock(bool needParry = false);

    // Get energy
    float GetEnergy() const;
    // Get energy for action
    float GetActEnergy(const char *act) const;

    // Events

    // Cannot further execute the command
    void FailureCommand() override;
    // The character arrived at the point
    void EndGotoCommand() override;
    // The character moved away from the point to the required radius
    void EndEscapeCommand() override;
    // colliding with a character too often
    void CollisionThreshold() override;

    void HitChild(bool isInBlock) override;

    // Save task on stack
    bool PushTask();
    // Retrieve a task from the stack
    bool PopTask();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Cannot further execute the command
    void FailureCommand(NPCTask task) const;
    // Making decisions
    void FightTick();
    // Get task type by name
    static NPCTask GetTaskID(const char *taskName);
    // Get task name by type
    static const char *GetTaskName(NPCTask t);
    // Check event
    static bool PrTest(float probability, float &testTime);
    static bool PrTest(float probability);

  protected:
    Task task;           // The task to be performed
    NPCTask lastSetTask; // Last task set

  private:
    Task taskstack[16]; // Task stack
    int32_t stackPointer;  // Stack pointer

    // Groups object
    entid_t charactersGroups;

    // Fight system
    float fightLevel; // Behavior level in combat 0..1

    // Attacks
    float attackCur;      // The rate of increase in the probability of attack, in parrots
    float attackPrbFast;  // fgt_attack_fast probability
    float attackPrbForce; // fgt_attack_force probability
    float attackPrbRound; // fgt_attack_round probability
    float attackPrbBreak; // fgt_attack_break probability
    float attackPrbFeint; // fgt_attack_feint probability

    // Defence
    float defenceCur;      // Rate of rise of block probability, in parrots
    float blockTime;       // Block time
    float defencePrbBlock; // fgt_block probability
    float defencePrbParry; // fgt_parry probability
    bool isRecoilEnable;   // Is a rebound allowed?

    // Shooting
    float fireCur;     // The rate of rise of the probability of a shot, in parrots
    bool isFireEnable; // Is the shot allowed

    float fightTick;    // Time until the next decision tick
    bool wantToAttack;  // Desire to attack
    bool wantToDefence; // Desire to defend
    bool wantToFire;    // Desire to shoot

    // The current state of the enemy
    bool isFgtChanged;
    FightAction enemyFgtType;

    bool bMusketer; //~!~
    float fMusketerDistance;
    float fMusketerTime, fMusketerFireTime, fMusketerCheckFireTime;
    bool bMusketerNoMove; //~!~
    bool bTryAnyTarget;

    void SetEscapeTask(Character *c);
};

// Get attacking character
inline Character *NPCharacter::GetAttackedCharacter() const
{
    if (task.task != npct_fight)
        return nullptr;
    return static_cast<Character *>(EntityManager::GetEntityPointer(task.target));
}

// Check event
inline bool NPCharacter::PrTest(float probability, float &testTime)
{
    if (testTime < 1.0f / 5.0f)
        return false;
    testTime = rand() * (0.02f / RAND_MAX);
    if (probability <= 0.0f)
        return false;
    const auto r = rand() * (1.0f / RAND_MAX);
    return r < probability;
}

// Check event
inline bool NPCharacter::PrTest(float probability)
{
    const auto r = rand() * (1.0f / RAND_MAX);
    return r < probability;
}
