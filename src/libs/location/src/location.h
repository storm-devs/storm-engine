//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Location
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "matrix.h"
#include "vma.hpp"
#include <cstdint>

#include "locator_array.h"
#include "model_array.h"
#include "ptc_data.h"
#include "supervisor.h"

class MODEL;
class Lights;

class Location : public Entity
{
    struct SphVertex
    {
        CVECTOR v;
        uint32_t c;
    };

    struct BarVertex
    {
        CVECTOR p;
        float rhw;
        uint32_t c;
        float u, v;
    };

    struct DmgMessage
    {
        CVECTOR p;
        float alpha;
        float hit, hp;
        uint32_t c;
    };

    struct EnemyBar
    {
        CVECTOR p;
        float hp;
        float energy;
        float alpha;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Location();
    ~Location() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;

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

    //--------------------------------------------------------------------------------------------
    // Location
    //--------------------------------------------------------------------------------------------
  public:
    // Find locator group
    LocatorArray *FindLocatorsGroup(const char *gName);
    // Find locator
    bool CheckIfLocatorExists(const char *lName);

    // Get a character patch
    PtcData &GetPtcData();
    // Get the model of the jump patch
    MODEL *JmpPatch();

    // Check the visibility of 2 points
    bool VisibleTest(const CVECTOR &p1, const CVECTOR &p2);

    // Trace the ray through the location
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(PLANE *p, int32_t numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, int32_t num));

    Lights *GetLights() const;

    VDX9RENDER *GetRS() const;
    void DrawLine(const CVECTOR &s, uint32_t cs, const CVECTOR &d, uint32_t cd, bool useZ = true) const;
    // Write text
    void Print(const CVECTOR &pos3D, float rad, int32_t line, float alpha, uint32_t color, float scale, const char *format,
               ...) const;

    bool IsDebugView();
    bool IsExDebugView();

    bool IsPaused() const;

    bool IsSwimming() const;

    // Add a damage message
    void AddDamageMessage(const CVECTOR &pos3D, float hit, float curhp, float maxhp);
    // Draw bars above the enemy in this frame
    void DrawEnemyBars(const CVECTOR &pos, float hp, float energy, float alpha);

  public:
    // The object that controls the repulsion of characters
    Supervisor supervisor;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void Update(uint32_t delta_time);
    int32_t LoadStaticModel(const char *modelName, const char *tech, int32_t level, bool useDynamicLights);
    bool LoadCharacterPatch(const char *ptcName);
    void LoadCaustic() const;
    bool LoadJumpPatch(const char *modelName);
    bool LoadGrass(const char *modelName, const char *texture);
    bool MessageEx(const char *name, MESSAGE &message);
    void UpdateLocators();
    void DrawLocators(LocatorArray *la);
    void CreateSphere();
    void TestLocatorsInPatch(MESSAGE &message);
    // Drawing bars over characters
    void DrawEnemyBars();
    void DrawBar(const MTX_PRJ_VECTOR &vrt, uint32_t color, float hp, float energy);
    void CorrectBar(float v, float start, float end, BarVertex *vrt);

  private:
    PtcData ptc;
    int32_t patchJump;

    int32_t lastLoadStaticModel;

    // All locators
    std::vector<LocatorArray *> locators;
    int32_t numLocators;
    int32_t maxLocators;

    bool isPause;
    bool isDebugView;

    VDX9RENDER *rs;

    // All models
    ModelArray model;

    // Grass
    entid_t grass;
    // Eagle
    entid_t eagle;
    // Lizards
    entid_t lizards;
    // Rats
    entid_t rats;
    // Crabs
    entid_t crabs;
    // Blood
    entid_t blood;

    entid_t lightsid;
    Lights *lights; // Pointer for the current frame

    entid_t loceffectsid;

    SphVertex *sphereVertex;
    int32_t sphereNumTrgs;

    float locationTimeUpdate;

    entid_t lighter;
    // entid_t cubeShotMaker;

    DmgMessage message[32];
    int32_t curMessage;

    EnemyBar enemyBar[32];
    int32_t enemyBarsCount;
    int32_t enemyBarsTexture;
    bool bDrawBars;

    bool bSwimming;
};

// Get a character patch
inline PtcData &Location::GetPtcData()
{
    return ptc;
}

// Get the model of the jump patch
inline MODEL *Location::JmpPatch()
{
    if (patchJump < 0)
        return nullptr;
    return model[patchJump];
}

inline VDX9RENDER *Location::GetRS() const
{
    return rs;
}

// Check the visibility of 2 points
inline bool Location::VisibleTest(const CVECTOR &p1, const CVECTOR &p2)
{
    return model.VisibleTest(p1, p2);
}

inline Lights *Location::GetLights() const
{
    return lights;
}

// Trace the ray through the location
inline float Location::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    return model.Trace(src, dst);
}

inline bool Location::GetCollideTriangle(TRIANGLE &trg) const
{
    return model.GetCollideTriangle(trg);
}

inline void Location::Clip(PLANE *p, int32_t numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, int32_t num))
{
    model.Clip(p, numPlanes, cnt, rad, fnc);
}

inline bool Location::IsPaused() const
{
    return isPause;
}
