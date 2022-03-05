//============================================================================================
//    Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocationCamera
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "matrix.h"
#include "path_tracks.h"
#include "dx9render.h"
#include "v_module_api.h"
#include <string>
#include <vector>

#include "camera_follow.h"

class MODEL;
class Character;
class Location;

class LocationCamera : public Entity
{
    friend CameraFollow;

    enum CameraWorkMode
    {
        cwm_none = 0,
        cwm_follow,
        // Follow Camera
        cwm_topos,
        // Move camera to preset position
        cwm_free,
        // Free flying camera (debug)
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LocationCamera();
    ~LocationCamera() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
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
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void LockFPMode(bool isLock);
    float GetAx() const;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Prepare data
    bool Set();
    // Change camera mode
    void SetMode(CameraWorkMode wm);
    // Execute the camera following the character
    void ExecuteFollow(float dltTime);
    // First-person View Camera
    void ExecuteLook(float dltTime);
    // Execution of a camera observing from a point
    void ExecuteTopos(float dltTime);
    // Free flying camera execution
    void ExecuteFree(float dltTime);

    // Moving a camera from a given position to a new one
    bool MoveFollow(CVECTOR &pos, const CVECTOR &cpos, const CVECTOR &to);

    // Trace the ray through the location
    float Trace(const CVECTOR &src, const CVECTOR &dst) const;
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(PLANE *p, int32_t numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, int32_t num)) const;

  private:
    VDX9RENDER *rs;
    // The sea
    entid_t sea;
    // Camera parameters
    float ax; // Camera tilt angle
    float lAx;
    float vAx;           // Current speed of change
    float axmin, axmax;  // Camera tilt angle
    float teleport_dist; // Maximum distance at which the camera teleports
    float lookHeight;    // The value of where to look in height (relative)
    float radius;        // Radius of distance from the character
    float rotInertia;    // Rotational inertia
    float radInertia;    // Inertia of approaching

    bool isSpecialMode;
    bool isLockFPMode;
    bool isViewExecute;

    entid_t loc;
    Location *location;

    // Work mode
    CameraWorkMode wmode;
    bool isSleep;
    bool lockAx;

    float dAx, dAy;

    CVECTOR camPos, lookTo;    // Current camera parameters
    CVECTOR oldPos, oldLookTo; // Last camera position in the previous mode
    float kMorph;              // Current morph ratio

    // Models
    entid_t chr;      // Character
    entid_t patchMdl; // Camera patch

    CameraFollow cf; // Camera following the character

    float obstructTime; // Character obstruction time
    CVECTOR obstruct;   // After

    float freeAx, freeAy; // Free camera angles

    bool isLookMode;
    bool isTeleport;

    // Camera observing the character from a point
    CVECTOR fromLook;

    // Temporary values at the time of execution
    Character *character; // Character pointer
    CVECTOR pos;          // Character position
    float chay;           // Character direction
    float lheight;        // Height where to look
    float height;         // Character height
    float chradius;       // Character radius

    // dynamic change of perspective (drunkenness)
    // ~!~ rearrange this!
    struct
    {
        bool isOn;
        float fMinFov;
        float fMaxFov;
        float fCurFov;
        bool bFogUp;
        float fFogChangeSpeed;
        float fFogTimeCur;
        float fFogTimeMax;
        float fCurAngle;
        float fMaxAngle;
        float fAngleSpeed;
        bool bAngleUp;
    } dynamic_fog;

    void TurnOnDynamicFov(float fSpeed, float fTime, float fRelationMin, float fRelationMax, float fAngSpeed,
                          float fAngMax);
    void ProcessDynamicFov(float fDeltaTime, const CVECTOR &vFrom, const CVECTOR &vTo, CVECTOR &vUp);
    void StoreRestoreDynamicFov(bool bStore);

    // track mode section
    bool m_bTrackMode;
    float m_fTrackCurTime;
    float m_fTrackMaxTime;
    std::string m_sCurTrackName;
    float m_fPauseTime;

    struct CameraTrackPause
    {
        float trackTime;
        float pauseTime;
    };

    std::vector<CameraTrackPause> m_aTrackPauses;
    int32_t m_nCurPauseIndex;

    bool LoadCameraTrack(const char *pcTrackFile, float fTrackTime);
    void TurnOffTrackCamera();
    void ProcessTrackCamera();
    float TrackPauseProcess();

    PathTracks m_track;

    bool forcedPos = false;

    float cameraPerspective;
};

inline void LocationCamera::LockFPMode(bool isLock)
{
    isLockFPMode = isLock;
}

inline float LocationCamera::GetAx() const
{
    return ax;
}
