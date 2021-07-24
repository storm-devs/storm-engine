//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lights
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "collide.h"
#include "dx9render.h"
#include "vmodule_api.h"
#include <vector>

class Lights : public Entity
{
    // Light source description
    struct LightType
    {
        char *name;
        D3DLIGHT9 dxLight;
        D3DCOLORVALUE color;
        float flicker;
        float flickerSlow;
        float freq;
        float freqSlow;
        float p;
        float pSlow;
        float coronaRange;
        float coronaRange2;
        float invCoronaRange;
        float coronaSize;
        long corona;
    };

    // Source
    struct Light
    {
        D3DCOLORVALUE color; // Current source color
        D3DVECTOR pos;       // Source position
        float time;          // Time since last change of flickering intensity
        float timeSlow;      // Time since the last change in the interpolated intensity
        float itens;         // Shimmering intensity
        float itensSlow;     // Necessary interpolated intensity
        float itensDlt;      // Interpolated intensity difference
        float i;             // Resulting intensity
        long type;           // Source type index
        float corona;        // Crown transparency
    };

    // Controllable (moving) source
    struct MovingLight
    {
        long id;
        long light;
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    // To sort by distance
    struct lt_elem
    {
        long idx;
        float dst;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Lights();
    ~Lights() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    // Drawing crowns
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

    // Find source index
    long FindLight(const char *name);
    // Add source to location
    void AddLight(long index, const CVECTOR &pos);
    // Add lantern model
    bool AddLampModel(entid_t lampModel);
    //
    void DelAllLights();

    // Add portable source
    long AddMovingLight(const char *type, const CVECTOR &pos);
    // Put portable source in new position
    void UpdateMovingLight(long id, const CVECTOR &pos);
    // Remove portable source
    void DelMovingLight(long id);

    // Set light sources for the character
    void SetCharacterLights(const CVECTOR *pos = nullptr);

    // Disable light sources set for the character
    void DelCharacterLights();

    // Update source types
    void UpdateLightTypes(long i);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;
    COLLIDE *collide;

    // Installed light sources
    struct
    {
        bool set;
        long light;
    } lt[8];

    // Types of light sources
    std::vector<LightType> types;
    long numTypes;
    long maxTypes;
    // Existing lighting sources
    std::vector<Light> lights;
    long numLights;
    long maxLights;
    long lighter_code;

    // portable light sources
    std::vector<MovingLight> aMovingLight;

    // Lantern models
    entid_t lampModels[16];
    long numLampModels;

    Vertex buf[6 * 1];

    // Sorted array of sources for the last calculation
    std::vector<lt_elem> aLightsDstSort;
};
