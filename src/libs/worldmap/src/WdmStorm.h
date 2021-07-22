//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmStorm
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmRenderObject.h"

class ATTRIBUTES;
class WdmCloud;

class WdmStorm : public WdmRenderObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmStorm();
    ~WdmStorm() override;

    void SetLiveTime(float t);
    float GetLiveTime() const;
    void GetPosition(float &x, float &z) const;
    bool IsActive() const;

    bool CheckIntersection(float x, float z, float r);

    // Calculations
    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    const char *GetId() const;

  public:
    // Setting parameters
    void SetSaveAttribute(ATTRIBUTES *save);
    void DeleteUpdate();

    bool isTornado;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Updating stored data
    void UpdateSaveData();

  private:
    CVECTOR pos, dir;

    float isActiveTime;
    float liveTime;
    float liveAlpha;
    float speed;
    bool isBrn, isKl;

    long num;
    WdmCloud *cloud[8];  // Pointers to the clouds
    CVECTOR cloudPos[8]; // Positions
    float rotSpd[8];     // Rotational speeds around the center

    ATTRIBUTES *saveAttribute;

    // Rain
    long rainTexture;
    RS_RECT rainRect[48 * 8];

    static char cloudPosName[16];
    static char rotSpdName[16];
};

inline void WdmStorm::SetLiveTime(float t)
{
    if (t < 1.0f)
        t = 1.0f;
    liveTime = t;
}

inline float WdmStorm::GetLiveTime() const
{
    return liveTime >= 0.0f ? liveTime : 0.0f;
}

inline void WdmStorm::GetPosition(float &x, float &z) const
{
    x = pos.x;
    z = pos.z;
}

inline bool WdmStorm::IsActive() const
{
    return isActiveTime <= 0.0f;
}
