//============================================================================================
//    Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocationCamera
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LocationCamera.h"

#include "core.h"

#include "Character.h"
#include "Location.h"
#include "entity.h"
#include "sea_base.h"
#include "shared/messages.h"

//#define CAMERA_VIEW_TEST_ENABLE
#define CAMERA_VIEW_TEST_R 3
#define CAMERA_VIEW_TEST_ANG 30

//============================================================================================

//#define LOCATIONCAMERA_DEBUG

#define LOCATIONCAMERA_RADIUS 0.5f      // Camera size
#define LOCATIONCAMERA_DISTQUANTUM 0.1f // Step with which you can move the camera

#define LOCATIONCAMERA_DISTALPHA_MAX 1.6f // Distance from the character at which it is necessary to fade out alpha
#define LOCATIONCAMERA_DISTALPHA_MIN 0.8f // Distance from the character at which it is necessary to fade out alpha

#define LOCATIONCAMERA_PERSPECTIVE (1.57f + 1.0f) * 0.5f

// ============================================================================================
// Construction, destruction
// ============================================================================================

LocationCamera::LocationCamera()
{
    // Camera parameters
    ax = 0.4f;
    vAx = 0.0f;
    axmin = -1.0f;
    axmax = 0.75f;
    teleport_dist = 12.0f;
    lookHeight = 0.8f;
    radius = 3.0f;
    rotInertia = 5.0f;
    radInertia = 7.0f;
    // Camera values
    isSleep = false;
    kMorph = 1.0f;
    wmode = cwm_follow;
    camPos = 0.0f;
    lookTo = 0.0f;
    fromLook = 0.0f;
    freeAx = freeAy = 0.0f;
    cf.SetLocationCamera(this);
    isTeleport = true;
    character = nullptr;
    lockAx = false;
    isViewExecute = false;
    location = nullptr;
    isLockFPMode = false;
    isLookMode = false;
    isSpecialMode = false;
    dAx = dAy = 0.0f;
    dynamic_fog.isOn = false;

    m_bTrackMode = false;
}

LocationCamera::~LocationCamera()
{
    StoreRestoreDynamicFov(true);
}

// Initialization
bool LocationCamera::Init()
{
    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    // core.LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 0);

    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), 100000);

    // The sea
    sea = EntityManager::GetEntityId("sea");

    // try to get the location
    loc = EntityManager::GetEntityId("location");

    rs->SetPerspective(LOCATIONCAMERA_PERSPECTIVE);
    // rs->SetPerspective(1.0f);

    StoreRestoreDynamicFov(false);

    return true;
}

// Execution
#ifndef LOCATIONCAMERA_DEBUG
void LocationCamera::Execute(uint32_t delta_time)
#else
void LocationCamera::Realize(uint32_t delta_time)
#endif
{
    if (m_bTrackMode)
    {
        ProcessTrackCamera();
        if (m_bTrackMode)
            return;
    }

    // Camera mode
    CONTROL_STATE cs;
    core.Controls->GetControlState("ChrCamSpecMode", cs);
    isSpecialMode = cs.state == CST_ACTIVE;
    // Time period
    const auto dltTime = delta_time * 0.001f;
    if (isSleep)
        return;
    if (!Set())
        return;
    // Controls
    const auto oldAx = ax;
    core.Controls->GetControlState("ChrCamTurnV", cs);
    dAx = -cs.lValue * 0.05f; //*0.005f;
    if (character->IsDead())
        dAx = 0.0f;
    auto kvax = 8.0f * dltTime;
    if (isLookMode && !character->IsFight())
    {
        vAx += 3.0f * dAx;
        ax += vAx * dltTime;
        kvax *= 2.6f;
        if (kvax > 1.0f)
            kvax = 1.0f;
        vAx -= kvax * vAx;
    }
    else
    {
        vAx += 10.0f * dAx * dltTime;
        ax += vAx * dltTime;
        if (kvax > 1.0f)
            kvax = 1.0f;
        vAx -= kvax * vAx;
    }
    if (vAx > 2.0f)
        vAx = 2.0f;
    if (vAx < -2.0f)
        vAx = -2.0f;
    if (ax < axmin)
        ax = axmin;
    if (ax > axmax)
        ax = axmax;
    character->LockRotate(false);
    core.Controls->GetControlState("ChrTurnH", cs);
    dAy = cs.lValue * 0.005f;
    if (dAy > 1.0f)
        dAy = 1.0f;
    if (dAy < -1.0f)
        dAy = -1.0f;
    if (character->IsDead())
        dAy = 0.0f;

    if (location->IsPaused())
        return;

    character->SetCameraAlpha(1.0f);
    character->LockIdle(false);
    character->LockMove(false);
    character->LookFromEyes(false);
    // Execute the current mode
    auto isELook = false;

    if (!isLockFPMode)
    {
        switch (wmode)
        {
        case cwm_follow: // Follow The Character

            //*
            if (!character->IsFight() && !character->IsDialog() && !character->IsDead())
            {
                core.Controls->GetControlState("ChrCamCameraSwitch", cs);
                if (cs.state == CST_ACTIVATED)
                    isLookMode = !isLookMode;
                isELook = isLookMode;
            } // else isLookMode = false;

            //*/
            /*
            if(cs.state == CST_ACTIVE && !character->IsFight() && !character->IsSwim())
            {
              isELook = true;
            }//*/

            if (lockAx)
            {
                isELook = isLookMode = false;
                ax = lAx;
            }
            else
            {
                if (core.Controls->GetControlState("ChrCamNormalize", cs))
                {
                    if (cs.state == CST_ACTIVATED)
                    {
                        if (!isELook)
                            ax = 0.2f;
                        else
                            ax = 0.0f;
                    }
                }
            }

            if (isELook)
            {
                if (!isViewExecute)
                {
                    // kMorph = 0.0f;
                    oldPos = camPos;
                    oldLookTo = lookTo;
                }
                if (kMorph >= 1.0f)
                    character->SetCameraAlpha(0.0f);
                character->LockIdle(true);
                ExecuteLook(dltTime);
                isViewExecute = true;
                character->LookFromEyes(true);
            }
            else
            {
                if (isSpecialMode)
                {
                    // recalculate the angle change for the special mode
                    core.Controls->GetControlState("ChrCamTurnH", cs);
                    dAy = cs.lValue * 0.05f;
                    if (dAy > 1.0f)
                        dAy = 1.0f;
                    if (dAy < -1.0f)
                        dAy = -1.0f;
                    if (character->IsDead())
                        dAy = 0.0f;
                }
                if (isViewExecute)
                {
                    // kMorph = 0.0f;
                    oldPos = camPos;
                    oldLookTo = lookTo;
                    for (long i = 0; i < 10; i++)
                        cf.Update(0.1f);
                }
                cf.Update(dltTime * character->CameraTurnSpeed());
                isViewExecute = false;
            }

#ifdef CAMERA_VIEW_TEST_ENABLE
            {
                float r = (float)(CAMERA_VIEW_TEST_R);
                float ang = (float)(CAMERA_VIEW_TEST_ANG);

                float h = r * sinf(ang * PI / 180.0f);
                r = r * cosf(ang * PI / 180.0f);
                character->GetPosition(lookTo);
                CVECTOR v = CVECTOR(-r * sinf(character->GetAY()), h, -r * cosf(character->GetAY()));
                camPos = lookTo + v;
            }
#endif
            break;
        case cwm_topos: // Locator view
            ExecuteTopos(dltTime);
            ax = oldAx;
            vAx = 0.0f;
            break;
        case cwm_free: // Free flying camera
            character->LockMove(true);
            ExecuteFree(dltTime);
            break;
        }
    }
    else
    {
        ExecuteLook(dltTime);
        isViewExecute = true;
    }

    cf.DrawDebug();

    // Position morphing
    if (kMorph < 1.0f)
    {
        if (!isTeleport)
        {
            camPos = oldPos + (camPos - oldPos) * kMorph;
            lookTo = oldLookTo + (lookTo - oldLookTo) * kMorph;
            kMorph += dltTime * 1.0f;
        }
        else
        {
            oldPos = camPos;
            oldLookTo = lookTo;
        }
        isTeleport = false;
    }
    auto realPos = camPos;
    auto *sb = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea));
    if (sb && wmode != cwm_free && location->IsSwimming())
    {
        const auto seaY = sb->WaveXZ(camPos.x, camPos.z) + 1.0f;
        if (realPos.y < seaY)
            realPos.y = seaY;
    }

    auto vUp = CVECTOR(0.0f, 1.0f, 0.0f);
    if (dynamic_fog.isOn)
        ProcessDynamicFov(core.GetDeltaTime() * .001f, realPos, lookTo, vUp);

    rs->SetCamera(realPos, lookTo, vUp);

#ifdef CAMERA_VIEW_TEST_ENABLE
    // Set the new camera position
    rs->SetCamera(camPos, lookTo, vUp);
#endif
}

#ifndef LOCATIONCAMERA_DEBUG
void LocationCamera::Realize(uint32_t delta_time)
#else
void LocationCamera::Execute(uint32_t delta_time)
#endif
{
    // rs->Print(10, 10, "%f", cf.fndRadius);
    /*
    CMatrix mtx;
    rs->GetTransform(D3DTS_VIEW, mtx);
    mtx.Transposition();
    float dst = location->Trace(CVECTOR(mtx.Pos()), CVECTOR(mtx.Pos() + (mtx.Vz()*100.0f)));
    rs->Print(10, 10, "%f", dst);
    */
}

// Messages
uint64_t LocationCamera::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_CAMERA_SETTARGET:
        chr = message.EntityID();
        if (EntityManager::GetEntityPointer(chr) == nullptr)
        {
            core.Trace("LocationCamera -> MSG_CAMERA_SETTARGET -> invalidate character id");
            return 0;
        }
        return 1;
    case MSG_CAMERA_FOLLOW:
        forcedPos = false;
        if (wmode != cwm_follow)
        {
            oldPos = camPos;
            oldLookTo = lookTo;
            kMorph = 0.0f;
        }
        wmode = cwm_follow;
        return 1;
    case MSG_CAMERA_TOPOS:
        forcedPos = false;
        fromLook.x = message.Float();
        fromLook.y = message.Float();
        fromLook.z = message.Float();
        if (!message.Long())
        {
            oldPos = camPos;
            oldLookTo = lookTo;
            kMorph = 0.0f;
        }
        else
            kMorph = 1.0f;
        wmode = cwm_topos;
        return 1;
    case MSG_CAMERA_MOVE:
        lockAx = true;
        lAx = message.Float();
        return 1;
    case MSG_CAMERA_FREE:
        forcedPos = false;
        wmode = cwm_free;
        kMorph = 1.0f;
        return 1;
    case MSG_CAMERA_SLEEP:
        isSleep = message.Long() != 0;
        return 1;
    case MSG_CAMERA_SET_RADIUS:
        radius = message.Float();
        return 1;

    // internal
    case -1: {
        const auto fSpeed = message.Float();
        const auto fTime = message.Float();
        const auto fMin = message.Float();
        const auto fMax = message.Float();
        const auto fAngSpeed = message.Float();
        const auto fAngMax = message.Float();
        TurnOnDynamicFov(fSpeed, fTime, fMin, fMax, fAngSpeed, fAngMax);
        return 1;
    }    

    case -2: {
        const std::string &trackname = message.String();
        const auto fTrackTime = message.Float();
        LoadCameraTrack(trackname.c_str(), fTrackTime);
        auto *pA = message.AttributePointer();
        // SetTrackCameraPauses(pA);
        return 1;
    }

    case -3:
        TurnOffTrackCamera();
        return 1;

    case -4:
        // from point to point
        fromLook.x = message.Float();
        fromLook.y = message.Float();
        fromLook.z = message.Float();
        if (!message.Long())
        {
            oldPos = camPos;
            oldLookTo = lookTo;
            kMorph = 0.0f;
        }
        else
            kMorph = 1.0f;
        wmode = cwm_topos;

        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();
        forcedPos = true;

        return 1;

    default:
        ;
    }

    return 0;
}

// Changing an attribute
uint32_t LocationCamera::AttributeChanged(ATTRIBUTES *apnt)
{
    // Reading out attributes
    if (!AttributesPointer)
        return 0;

    return 0;
}

// Prepare data
bool LocationCamera::Set()
{
    // Character pointer
    auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(chr));
    if (!c)
        return false;
    // Character characteristics
    if (!forcedPos)
    {
        c->GetPosition(pos);
    }
    chay = c->GetAY();
    height = c->GetHeight();
    lheight = height * lookHeight;
    chradius = c->GetRadius();
    character = c;
    location = static_cast<Location *>(EntityManager::GetEntityPointer(loc));
    return location != nullptr;
}

// Change camera mode
void LocationCamera::SetMode(CameraWorkMode wm)
{
    if (wmode == wm)
        return;
    oldPos = camPos;
    oldLookTo = lookTo;
    wmode = wm;
}

//============================================================================================

//============================================================================================

// First-person View Camera
void LocationCamera::ExecuteLook(float dltTime)
{
    // Determine the camera placement point
    auto *mdl = character->Model();
    if (!mdl)
        return;
    auto *node = mdl->GetNode(0);
    if (!node || !node->geo)
        return;
    auto *ani = mdl->GetAnimation();
    long strIndex = -1;
    auto idCamera = node->geo->FindName("camera");
    if (ani && (strIndex = node->geo->FindLabelN(strIndex + 1, idCamera)) > -1)
    {
        GEOS::LABEL lb;
        // Locator matrix
        node->geo->GetLabel(strIndex, lb);
        CMatrix mtl;
        mtl.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
        mtl.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
        mtl.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
        mtl.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);
        // Bone matrix
        auto ml = mtl * ani->GetAnimationMatrix(lb.bones[0]);
        ml.Pos().x *= -1.0f;
        ml.Vx().x *= -1.0f;
        ml.Vy().x *= -1.0f;
        ml.Vz().x *= -1.0f;
        CMatrix m(ml, mdl->mtx);
        camPos = m.Pos() - m.Vz() * 0.3f;
    }
    else
    {
        camPos = pos + CVECTOR(0.0f, height, 0.0f);
    }
    // Determine where we are looking at
    lookTo.x = camPos.x + cosf(ax) * sinf(chay);
    lookTo.y = camPos.y - sinf(ax);
    lookTo.z = camPos.z + cosf(ax) * cosf(chay);
}

// Execution of a camera observing from a point
void LocationCamera::ExecuteTopos(float dltTime)
{
    // Where are we looking at
    lookTo = pos + CVECTOR(0.0f, lheight, 0.0f);
    // Where are we looking from
    camPos = fromLook;
}

// Free flying camera execution
void LocationCamera::ExecuteFree(float dltTime)
{
    if (LOWORD(GetKeyState(VK_NUMLOCK)) != 0)
        return;

    const auto pi = 3.14159265359f;
    freeAx -= dAx * 0.1f;
    freeAy += dAy * 1.0f;
    if (freeAx < -pi * 0.4f)
        freeAx = -pi * 0.4f;
    if (freeAx > pi * 0.4f)
        freeAx = pi * 0.4f;
    if (freeAy > pi * 2.0f)
        freeAy -= pi * 2.0f;
    if (freeAy < -pi * 2.0f)
        freeAy += pi * 2.0f;
    lookTo.x = cosf(freeAx) * sinf(freeAy);
    lookTo.y = sinf(freeAx);
    lookTo.z = cosf(freeAx) * cosf(freeAy);
    if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0)
        dltTime *= 10.0f;
    if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
        dltTime *= 4.0f;
    if (core.Controls->GetDebugAsyncKeyState(VK_LBUTTON) < 0)
        camPos += 5.0f * lookTo * dltTime;
    if (core.Controls->GetDebugAsyncKeyState(VK_RBUTTON) < 0)
        camPos -= 5.0f * lookTo * dltTime;
    lookTo += camPos;
}

// Trace the ray through the location
float LocationCamera::Trace(const CVECTOR &src, const CVECTOR &dst) const
{
    if (!location)
        return 2.0f;
    return location->Trace(src, dst);
}

bool LocationCamera::GetCollideTriangle(TRIANGLE &trg) const
{
    if (!location)
        return false;
    return location->GetCollideTriangle(trg);
}

void LocationCamera::Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad,
                          bool (*fnc)(const CVECTOR *vtx, long num)) const
{
    if (location)
        location->Clip(p, numPlanes, cnt, rad, fnc);
}

void LocationCamera::TurnOnDynamicFov(float fSpeed, float fTime, float fRelationMin, float fRelationMax,
                                      float fAngSpeed, float fAngMax)
{
    dynamic_fog.fMinFov = LOCATIONCAMERA_PERSPECTIVE * fRelationMin;
    dynamic_fog.fMaxFov = LOCATIONCAMERA_PERSPECTIVE * fRelationMax;
    if (!dynamic_fog.isOn)
    {
        dynamic_fog.fCurFov = LOCATIONCAMERA_PERSPECTIVE;
        dynamic_fog.bFogUp = true;
        dynamic_fog.fCurAngle = 0.f;
        dynamic_fog.bAngleUp = true;
    }
    dynamic_fog.isOn = true;
    dynamic_fog.fFogChangeSpeed = fSpeed * LOCATIONCAMERA_PERSPECTIVE;
    dynamic_fog.fFogTimeCur = 0;
    dynamic_fog.fFogTimeMax = fTime;

    dynamic_fog.fAngleSpeed = fAngSpeed;
    dynamic_fog.fMaxAngle = fAngMax;
}

void LocationCamera::ProcessDynamicFov(float fDeltaTime, const CVECTOR &vFrom, const CVECTOR &vTo, CVECTOR &vUp)
{
    if (dynamic_fog.fFogTimeMax > 0.f)
        dynamic_fog.fFogTimeCur += fDeltaTime;
    if (dynamic_fog.fFogTimeMax > 0.f && dynamic_fog.fFogTimeCur >= dynamic_fog.fFogTimeMax)
    {
        rs->SetPerspective(LOCATIONCAMERA_PERSPECTIVE);
        dynamic_fog.isOn = false;
    }
    else
    {
        if (dynamic_fog.bFogUp)
        {
            dynamic_fog.fCurFov += dynamic_fog.fFogChangeSpeed * fDeltaTime;
            if (dynamic_fog.fCurFov >= dynamic_fog.fMaxFov)
            {
                dynamic_fog.fCurFov = dynamic_fog.fMaxFov;
                dynamic_fog.bFogUp = false;
            }
        }
        else
        {
            dynamic_fog.fCurFov -= dynamic_fog.fFogChangeSpeed * fDeltaTime;
            if (dynamic_fog.fCurFov <= dynamic_fog.fMinFov)
            {
                dynamic_fog.fCurFov = dynamic_fog.fMinFov;
                dynamic_fog.bFogUp = true;
            }
        }
        rs->SetPerspective(dynamic_fog.fCurFov);

        if (dynamic_fog.bAngleUp)
        {
            dynamic_fog.fCurAngle += dynamic_fog.fAngleSpeed * fDeltaTime;
            if (dynamic_fog.fCurAngle >= dynamic_fog.fMaxAngle)
            {
                dynamic_fog.fCurAngle = dynamic_fog.fMaxAngle;
                dynamic_fog.bAngleUp = false;
            }
        }
        else
        {
            dynamic_fog.fCurAngle -= dynamic_fog.fAngleSpeed * fDeltaTime;
            if (dynamic_fog.fCurAngle <= -dynamic_fog.fMaxAngle)
            {
                dynamic_fog.fCurAngle = -dynamic_fog.fMaxAngle;
                dynamic_fog.bAngleUp = true;
            }
        }

        const auto v = vTo - vFrom;
        const auto fAng = atan2f(v.x, v.z);
        CMatrix mrot(0.f, 0.f, dynamic_fog.fCurAngle);
        mrot.RotateY(fAng);
        vUp = mrot * vUp;
    }
}

void LocationCamera::StoreRestoreDynamicFov(bool bStore)
{
    if (!AttributesPointer)
        return;
    auto *pA = AttributesPointer->GetAttributeClass("DynamicFov");
    if (!pA)
        pA = AttributesPointer->CreateSubAClass(AttributesPointer, "DynamicFov");
    if (!pA)
        return;

    if (bStore)
    {
        // store
        pA->SetAttributeUseDword("ison", dynamic_fog.isOn);
        if (dynamic_fog.isOn)
        {
            pA->SetAttributeUseFloat("minfov", dynamic_fog.fMinFov);
            pA->SetAttributeUseFloat("maxfov", dynamic_fog.fMaxFov);
            pA->SetAttributeUseFloat("curfov", dynamic_fog.fCurFov);
            pA->SetAttributeUseDword("fogup", dynamic_fog.bFogUp);
            pA->SetAttributeUseFloat("speed", dynamic_fog.fFogChangeSpeed);
            pA->SetAttributeUseFloat("timecur", dynamic_fog.fFogTimeCur);
            pA->SetAttributeUseFloat("timemax", dynamic_fog.fFogTimeMax);

            pA->SetAttributeUseFloat("curangle", dynamic_fog.fCurAngle);
            pA->SetAttributeUseFloat("maxangle", dynamic_fog.fMaxAngle);
            pA->SetAttributeUseFloat("speedangle", dynamic_fog.fAngleSpeed);
            pA->SetAttributeUseDword("angleup", dynamic_fog.bAngleUp);
        }
    }
    else
    {
        // restore
        dynamic_fog.isOn = pA->GetAttributeAsDword("ison", false) != 0;
        if (dynamic_fog.isOn)
        {
            dynamic_fog.fMinFov = pA->GetAttributeAsFloat("minfov", LOCATIONCAMERA_PERSPECTIVE);
            dynamic_fog.fMaxFov = pA->GetAttributeAsFloat("maxfov", LOCATIONCAMERA_PERSPECTIVE);
            dynamic_fog.fCurFov = pA->GetAttributeAsFloat("curfov", LOCATIONCAMERA_PERSPECTIVE);
            dynamic_fog.bFogUp = pA->GetAttributeAsDword("fogup", true) != 0;
            dynamic_fog.fFogChangeSpeed = pA->GetAttributeAsFloat("speed", 0.1f);
            dynamic_fog.fFogTimeCur = pA->GetAttributeAsFloat("timecur", 0.f);
            dynamic_fog.fFogTimeMax = pA->GetAttributeAsFloat("timemax", 0.f);

            dynamic_fog.fCurAngle = pA->GetAttributeAsFloat("curangle", 0.f);
            dynamic_fog.fMaxAngle = pA->GetAttributeAsFloat("maxangle", 0.f);
            dynamic_fog.fAngleSpeed = pA->GetAttributeAsFloat("speedangle", 0.1f);
            dynamic_fog.bAngleUp = pA->GetAttributeAsDword("angleup", true) != 0;
        }
    }
}

bool LocationCamera::LoadCameraTrack(const char *pcTrackFile, float fTrackTime)
{
    m_fTrackCurTime = 0.f;
    m_fTrackMaxTime = fTrackTime;
    m_fPauseTime = 0.f;
    m_nCurPauseIndex = -1;
    m_bTrackMode = false;

    if (!m_track.Load(pcTrackFile))
        return false;

    m_bTrackMode = true;
    m_sCurTrackName = pcTrackFile;

    // setting the camera to the beginning of the track
    Vector pos;
    Quaternion ang;
    Matrix view;
    m_track.GetPoint(0.f, pos, ang);
    ang.GetMatrix(view);
    view.vx = -view.vx;
    view.vz = -view.vz;
    view.Inverse();
    view.pos = view * -pos;
    rs->SetView(*(CMatrix *)&view);
    rs->SetPerspective(LOCATIONCAMERA_PERSPECTIVE);

    return true;
}

void LocationCamera::TurnOffTrackCamera()
{
    m_bTrackMode = false;
    if (m_fTrackCurTime >= m_fTrackMaxTime)
    {
        kMorph = 0.f;

        Vector pos;
        Quaternion ang;
        Matrix view;
        m_track.GetPoint(0.99999f, pos, ang);
        ang.GetMatrix(view);
        view.vx = -view.vx;
        view.vz = -view.vz;
        view.Inverse();

        oldPos = *(CVECTOR *)&pos;
        (*(CMatrix *)&view).MulToInvNorm(CVECTOR(0.f, 0.f, 1.f), oldLookTo);
    }
    core.Event("TrackCameraOff", "s", m_sCurTrackName.c_str());
}

void LocationCamera::ProcessTrackCamera()
{
    const auto fTrackTime = TrackPauseProcess();

    if (fTrackTime >= m_fTrackMaxTime)
    {
        TurnOffTrackCamera();
        return;
    }

    Vector pos;
    Quaternion ang;
    Matrix view;
    m_track.GetPoint(fTrackTime / m_fTrackMaxTime, pos, ang);
    ang.GetMatrix(view);
    view.vx = -view.vx;
    view.vz = -view.vz;
    view.Inverse();
    view.pos = view * -pos;
    rs->SetView(*(CMatrix *)&view);
    rs->SetPerspective(LOCATIONCAMERA_PERSPECTIVE);
}

float LocationCamera::TrackPauseProcess()
{
    auto fOldTime = m_fTrackCurTime;
    m_fTrackCurTime += core.GetDeltaTime() * 0.001f;

    //    for( long nPause=m_nCurPauseIndex+1; nPause<m_aTrackPauses; nPause++ )
    //        if( m_aTrackPauses[nPause].trackTime <= m_fTrackCurTime ) {
    //        }

    /*if(m_nCurPauseIndex >= 0) // in pause now
    {
      if( m_fTrackCurTime >= m_aTrackPauses[m_nCurPauseIndex].pauseTime )
    }*/

    return m_fTrackCurTime;

    /*    float fCurTime = m_fTrackCurTime + core.GetDeltaTime() * 0.001f;

      if( m_nCurPauseIndex < 0 ) {
        m_nCurPauseIndex = FindPauseIndex(m_fTrackCurTime,fCurTime);
      } else {
        fCurTime -= m_aTrackPauses[m_nCurPauseIndex].trackTime;
        if( fCurTime >= m_aTrackPauses[m_nCurPauseIndex].pauseTime )
        {
          m_nCurPauseIndex =
        }
      }*/
}
