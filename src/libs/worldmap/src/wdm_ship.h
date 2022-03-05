//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_render_model.h"

// #define WDM_SHIP_MODELL 6.0f // Half the length of the ship
// #define WDM_SHIP_MODELW 1.9f // Half Ship Width
// #define WDM_SHIP_RADIUS2        (WDM_SHIP_MODELL*WDM_SHIP_MODELL + WDM_SHIP_MODELW*WDM_SHIP_MODELW)

#define WDM_SHIP_SPEED 7.5f    // Ship speed
#define WDM_SHIP_TSPEED 1.0f   // Swing speed
#define WDM_SHIP_INER_ST 2.5f  // Acceleration inertia
#define WDM_SHIP_INER_BR 1.25f // Inertia of braking

#define WDM_SHIP_MAX_SPEED 20.0f   // Maximum ship speed
#define WDM_SHIP_MAX_TURNSPD 0.65f // Maximum swing speed

class WdmShip : public WdmRenderModel
{
    struct Line
    {
        float x, z;
        float ay, size;
    };

    struct Vertex
    {
        float x, y, z;
        uint32_t color;
        float tu, tv;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmShip();
    ~WdmShip() override;

    void Teleport(float x, float z, float ay);
    void GetPosition(float &x, float &z, float &ay) const;
    void SetMaxSpeed(float k);

    bool Load(const char *modelName) override;
    // Calculations
    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    // true if free
    static bool CheckPosition(float x, float z, float objRadius);

    bool isLive;
    bool isSelect;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  protected:
    void ShipUpdate(float dltTime);
    void UpdateWaterMark(float dltTime);

    virtual void Collide(){};

  public:
    // Model sizes relative to 0
    float modelL05;     // Half the length of the model relative to 0
    float modelW05;     // Half the width of the model relative to 0
    float modelRadius;  // sqrtf(shipModelL*shipModelL + shipModelW*shipModelW)
    float modelRadius2; // shipModelL*shipModelL + shipModelW*shipModelW

  protected:
    float ax, ay, az;
    float dltAx, dltAz;
    float speed, turnspd;
    float baseV;
    float kMaxSpeed;
    CVECTOR rspeed;
    int32_t collisionCounter;
    float slope;
    int32_t wmtexture;
    bool isWMRender;

    Line lines[8];
};
