//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    AICharacter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "character.h"

class AICharacter : public Character
{
  protected:
    enum AICommand
    {
        aicmd_unknow = 0,
        aicmd_none,
        // No command, the character is controlled from the outside
        aicmd_stay,
        // Stand still
        aicmd_gotopoint,
        // Go to the point
        aicmd_escape,
        // Move away from the point
        aicmd_max
    };

    struct PathNode
    {
        CVECTOR pos; // Point of arrival
        CVECTOR nrm; // Path direction
        float dst;   // Distance to the plane
        int32_t node;   // Node to be reached
    };

    struct Command
    {
        AICommand cmd;     // Command
        CVECTOR pnt;       // Point for the command
        int32_t node;         // Node to go to
        CVECTOR tpnt;      // The point we are going at the moment
        int32_t tnode;        // The node we're on
        float radius;      // Radius to complete the task
        float waitTime;    // Waiting time (stand)
        AICharacter *exch; // don't collide with this character
        // Flags
        struct
        {
            uint32_t cnt : 8;    // Repetition counter
            uint32_t isWait : 1; // waiting
            uint32_t isBusy : 1; // Check if a point is busy
        };
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AICharacter();
    virtual ~AICharacter();

    //--------------------------------------------------------------------------------------------
    // Character
    //--------------------------------------------------------------------------------------------
  public:
    // Move the character to the desired position
    void Move(float dltTime) override;
    // Carry out additional calculations
    void Calculate(float dltTime) override;
    // Update character position
    void Update(float dltTime) override;
    // Mark the movement of the character
    void CharacterTeleport() override;

    //--------------------------------------------------------------------------------------------
    // AICharacter
    //--------------------------------------------------------------------------------------------
  public:
    // Nothing to do
    bool CmdNone();
    // To stand
    bool CmdStay();
    // Go to the point
    bool CmdGotoPoint(float x, float y, float z, float rad, int32_t node = -1, bool isCheckBusyPos = true);
    // Move away from the point
    bool CmdEscape(float x, float y, float z, float rad);
    // Set the character we don't collide with
    void SetExCharacter(AICharacter *chr);

    // Events

    // Cannot further execute the command
    virtual void FailureCommand(){};
    // The character arrived at the point
    virtual void EndGotoCommand(){};
    // The character moved away from the point to the required radius
    virtual void EndEscapeCommand(){};
    // if colliding with a character too often
    virtual void CollisionThreshold(){};

    static const char *GetCommandName(AICommand cmd);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
    // private:
  protected:
    // Go to the point
    void CmdProcessGotoPoint(float dltTime);
    void CmdUpdateGotoPoint(float dltTime);
    // Move away from the point
    void CmdProcessEscape(float dltTime);
    void CmdUpdateEscape(float dltTime);

    // Find the node index for a given coordinate
    int32_t FindNodeIndex(const CVECTOR &pos, float *hy = nullptr);
    // Find direction where to go (orientation on the terrain)
    bool FindDirectional();
    // Find the pushing forces
    void CalcRepulsionForces();
    // Calculate the point defined by the intersection and lying on the edge
    static bool FindIntersection(const CVECTOR &s, const CVECTOR &e, const CVECTOR &cur, const CVECTOR &to,
                                 CVECTOR &res);
    // Calculate angle from direction vector
    static float Angle(double vx, double vz, float defAy);

  protected:
    // Current command
    Command command;
    PathNode path[2];
    // Preferred speed factor
    float likeKSpd;
    float collisionValue;

    // Control forces
    CVECTOR force;      // The power of pushing
    CVECTOR goForce;    // Force determining the direction of movement
    CVECTOR separation; // Pushback force
    CVECTOR alignment;  // Force of directions alignment
    CVECTOR around;     // Character Traversal Strength

    bool bMusketer;
    bool bMusketerNoMove;
};
