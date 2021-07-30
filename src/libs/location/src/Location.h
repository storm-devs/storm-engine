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

#include <cstdint>

#include "LocatorArray.h"
#include "ModelArray.h"
#include "PtcData.h"
#include "Supervisor.h"

class MODEL;
class Lights;

class Location : public Entity
{
    struct SphVertex
    {
        Vector v;
        uint32_t c;
    };

    struct BarVertex
    {
        Vector p;
        float rhw;
        uint32_t c;
        float u, v;
    };

    struct DmgMessage
    {
        Vector p;
        float alpha;
        float hit, hp;
        uint32_t c;
    };

    struct EnemyBar
    {
        Vector p;
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

    // Get a character patch
    PtcData &GetPtcData();
    // Get the model of the jump patch
    MODEL *JmpPatch();

    // Check the visibility of 2 points
    bool VisibleTest(const Vector &p1, const Vector &p2);

    // Trace the ray through the location
    float Trace(const Vector &src, const Vector &dst);
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(Plane *p, long numPlanes, Vector &cnt, float rad, bool (*fnc)(const Vector *vtx, long num));

    Lights *GetLights() const;

    VDX9RENDER *GetRS() const;
    void DrawLine(const Vector &s, uint32_t cs, const Vector &d, uint32_t cd, bool useZ = true) const;
    // Write text
    void Print(const Vector &pos3D, float rad, long line, float alpha, uint32_t color, float scale, const char *format,
               ...) const;

    bool IsDebugView();
    bool IsExDebugView();

    bool IsPaused() const;

    bool IsSwimming() const;

    // Add a damage message
    void AddDamageMessage(const Vector &pos3D, float hit, float curhp, float maxhp);
    // Draw bars above the enemy in this frame
    void DrawEnemyBars(const Vector &pos, float hp, float energy, float alpha);

  public:
    // The object that controls the repulsion of characters
    Supervisor supervisor;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void Update(uint32_t delta_time);
    long LoadStaticModel(const char *modelName, const char *tech, long level, bool useDynamicLights);
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
    void DrawBar(const Vector4 &vrt, uint32_t color, float hp, float energy);
    void CorrectBar(float v, float start, float end, BarVertex *vrt);

  private:
    PtcData ptc;
    long patchJump;

    long lastLoadStaticModel;

    // All locators
    std::vector<LocatorArray *> locators;
    long numLocators;
    long maxLocators;

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
    long sphereNumTrgs;

    float locationTimeUpdate;

    entid_t lighter;
    // entid_t cubeShotMaker;

    DmgMessage message[32];
    long curMessage;

    EnemyBar enemyBar[32];
    long enemyBarsCount;
    long enemyBarsTexture;
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
inline bool Location::VisibleTest(const Vector &p1, const Vector &p2)
{
    return model.VisibleTest(p1, p2);
}

inline Lights *Location::GetLights() const
{
    return lights;
}

// Trace the ray through the location
inline float Location::Trace(const Vector &src, const Vector &dst)
{
    return model.Trace(src, dst);
}

inline bool Location::GetCollideTriangle(TRIANGLE &trg) const
{
    return model.GetCollideTriangle(trg);
}

inline void Location::Clip(Plane *p, long numPlanes, Vector &cnt, float rad, bool (*fnc)(const Vector *vtx, long num))
{
    model.Clip(p, numPlanes, cnt, rad, fnc);
}

inline bool Location::IsPaused() const
{
    return isPause;
}
