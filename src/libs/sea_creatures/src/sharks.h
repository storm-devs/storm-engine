//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Sharks
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "animation.h"
#include "island_base.h"
#include "matrix.h"
#include "dx9render.h"
#include "sea_base.h"
#include "ship_base.h"

class Sharks : public Entity
{
    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    class Shark : public AnimationEventListener
    {
        static uint16_t indeces[];

      public:
        Shark();
        ~Shark() override;

        bool Init(float vp_x, float vp_z, bool isLoadModel = true);

        void Reset(float cam_x, float cam_z);
        void Repulsion(Shark &shr);
        void ShipApply(float x, float z, float r2);
        void Coordination(float cam_x, float cam_z, float dltTime, SEA_BASE *sb, ISLAND_BASE *ib);
        void IslandCollision(ISLAND_BASE *ib, int32_t numPnt, float rad, float frc);
        void Event(Animation *animation, int32_t index, int32_t eventID, AnimationEvent event) override;
        int32_t GenerateTrack(uint16_t *inds, Vertex *vrt, uint16_t base, SEA_BASE *sb);

        // Waypoint
        CVECTOR pos;    // Waypoint position
        CVECTOR vel;    // Move direction
        CVECTOR force;  // Directional force
        float yDir;     // The tendency to ascend or sink
        float dirTime;  // Time until next change
        float accs;     // Shark acceleration
        CVECTOR rForce; // Accidental external force
        float rTime;
        // Shark
        CVECTOR fforce; // Directional force
        CVECTOR spos;   // Model position
        CVECTOR angs;   // Model angles
        float turn;     // Angle change rate
        float imspd;    // Dive speed
        float speed;    // Move speed
        float shipY;    // Limit by ship
        float vBase;    // Shark track offset
        entid_t model;  // Shark model
        // Animation
        float aniTime;  // Playing time of the current animation
        float jumpTime; // Time until the next jump
        bool speedUp;
    };

    friend Shark;

    struct Periscope
    {
        CVECTOR pos;
        float ay;
        float time;
        entid_t model;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Sharks();
    ~Sharks() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
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
    bool LoadPeriscopeModel();

  private:
    VDX9RENDER *rs;
    Shark shark[6];
    int32_t numShakes;
    Periscope periscope;
    float waitPTime;
    CVECTOR camPos;
    entid_t sea;
    entid_t island;
    int32_t trackTx;
    uint16_t indeces[7 * 10 * 3];
    Vertex vrt[7 * 10];
};
