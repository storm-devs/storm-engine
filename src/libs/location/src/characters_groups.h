//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    CharactersGroups
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "matrix.h"
#include "vma.hpp"

#include "character.h"
#include "location.h"

class Location;
class Character;

class CharactersGroups : public Entity
{
  public:
    struct String
    {
        String();
        String(const char *str);
        ~String();
        char *name;
        int32_t len;
        int32_t max;
        int32_t hash;

        void operator=(const char *str);

        operator const char *() const
        {
            return name;
        };
        bool Cmp(const char *str, int32_t l, int32_t h) const;
        static int32_t GetHash(const char *str);
        static int32_t GetLen(const char *str);
    };

    enum RelState
    {
        rs_beginvalue,
        rs_friend,
        rs_neitral,
        rs_enemy,
        rs_endvalue,
    };

    struct Relation
    {
        float alarm;       // Current alarm state
        float alarmdown;   // Alarm fading speed
        float alarmmin;    // Deactivation threshold
        float alarmmax;    // Activation threshold
        bool isActive;     // Is the alarm active
        RelState curState; // Current relationship between groups
        RelState actState; // Relationship between groups to be established in case of alarm activation
        RelState relState; // Relationship between groups that will be established in case of alarm deactivation
    };

    struct Group
    {
        int32_t index;                // Group index
        String name;               // Group name
        float look;                // Enemy sight radius
        float hear;                // The radius at which the character always determines the enemy
        float say;                 // The radius at which the character can inform neighbors about the danger
        int32_t priority;             // Protection priority
        Relation *relations;       // Relationship list - the size corresponds to the group index in the list
        std::vector<entid_t> c;    // List of characters in the group
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    CharactersGroups();
    ~CharactersGroups() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;
    // Changing an attribute
    uint32_t AttributeChanged(ATTRIBUTES *apnt) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
            // case Stage::realize:
            //    Realize(delta); break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Checking the character detects others
    void CharacterVisibleCheck(Character *chr);
    // Check found characters for enemies
    void FindEnemyFromFindList(Character *chr, Group *grp, bool visCheck);
    // Add or update an enemy
    bool AddEnemyTarget(Character *chr, Character *enemy, float maxtime = -1.0);
    // Remove all inactive or invalid targets
    void RemoveAllInvalidTargets();
    // Remove inactive or invalid targets
    bool RemoveInvalidTargets(Character *chr, Character *check = nullptr);

  private:
    // Check target for validity
    bool MsgIsValidateTarget(MESSAGE &message);
    // Find the optimal goal
    bool MsgGetOptimalTarget(MESSAGE &message) const;
    // Is this character an enemy
    bool MsgIsEnemy(MESSAGE &message);
    // Group reaction to attack
    void MsgAttack(MESSAGE &message);
    // Add target
    void MsgAddTarget(MESSAGE &message);
    // Refresh goals for this character
    void MsgUpdChrTrg(MESSAGE &message);

    // Register a group
    void MsgRegistryGroup(MESSAGE &message);
    // Delete group
    void MsgReleaseGroup(MESSAGE &message);
    // Register a group
    int32_t RegistryGroup(const char *groupName);
    // Delete group
    void ReleaseGroup(const char *groupName);

    // Set the visibility radius for the group
    bool MsgSetGroupLook(MESSAGE &message);
    // Set the radius of hearing for the group
    bool MsgSetGroupHear(MESSAGE &message);
    // Set message radius for the group
    bool MsgSetGroupSay(MESSAGE &message);
    // Set group priority
    bool MsgSetGroupPriority(MESSAGE &message);
    // Set speed alarm level
    bool MsgSetAlarm(MESSAGE &message);
    // Set alarm fading speed
    bool MsgSetAlarmDown(MESSAGE &message);
    // Add character to group
    bool MoveCharacterToGroup(MESSAGE &message);
    // Establish relationships between groups
    void MsgSetRelation(MESSAGE &message);
    // Set alarm response for a pair of groups
    void MsgSetAlarmReaction(MESSAGE &message);

    // Unloading a character
    void UnloadCharacter(MESSAGE &message);

    // Remove character from all groups
    void RemoveCharacterFromAllGroups(entid_t chr);

    // Remove all empty groups
    void DeleteEmptyGroups();

  public:
    // Get group from message
    Group *GetGroup(MESSAGE &message, bool isRegistry = true);
    // Find a group by name
    Group *FindGroup(const char *name);
    // Find a group by name
    int32_t FindGroupIndex(const char *name);
    // Find group relationship
    Relation &FindRelation(MESSAGE &message, bool *selfgroup = nullptr);
    // Find group relationship
    Relation &FindRelation(const char *name1, const char *name2, bool *selfgroup = nullptr);
    // Find group relationship
    Relation &FindRelation(int32_t g1, int32_t g2, bool *selfgroup = nullptr);
    // Get character group index
    int32_t GetCharacterGroup(Character *c);

    // Delete all targets
    void ClearAllTargets() const;
    // Save data to object
    void SaveData();
    // Read relationship data from object
    void LoadDataRelations();
    // Establish relationships for active groups
    void RestoreStates();

    // Display information about relationships
    void DumpRelations();
    // Get the state as a string
    const char *GetTextState(RelState state);

  private:
    std::vector<Group *> groups; // Groups
    int32_t numGroups;              // Number of groups
    int32_t maxGroups;              // Number of groups
    Location *location;          // Current location
    int32_t curExecuteChr;          // The index of the currently executing character
    float waveTime;              // Time since last wave launch

    // Character search array
    std::vector<Supervisor::FindCharacter> fnd;
};
