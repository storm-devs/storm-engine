//============================================================================================
//	Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	LocationCamera
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LocationCamera.h"
#include "Character.h"
#include "Location.h"
#include "sea_base.h"

//#define CAMERA_VIEW_TEST_ENABLE
#define CAMERA_VIEW_TEST_R 3
#define CAMERA_VIEW_TEST_ANG 30

//============================================================================================

//#define LOCATIONCAMERA_DEBUG

#define LOCATIONCAMERA_RADIUS 0.5f      //Размер камеры
#define LOCATIONCAMERA_DISTQUANTUM 0.1f //Шаг с которым можно перемещать камеру

#define LOCATIONCAMERA_DISTALPHA_MAX 1.6f //Дистанция от персонажа при которой надо гасить альфу
#define LOCATIONCAMERA_DISTALPHA_MIN 0.8f //Дистанция от персонажа при которой надо гасить альфу

#define LOCATIONCAMERA_PERSPECTIVE (1.57f + 1.0f) * 0.5f

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

LocationCamera::LocationCamera() : m_aTrackPauses(_FL_)
{
    //Параметры камеры
    ax = 0.4f;
    vAx = 0.0f;
    axmin = -1.0f;
    axmax = 0.75f;
    teleport_dist = 12.0f;
    lookHeight = 0.8f;
    radius = 3.0f;
    rotInertia = 5.0f;
    radInertia = 7.0f;
    //Значения камеры
    isSleep = false;
    kMorph = 1.0f;
    wmode = cwm_follow;
    camPos = 0.0f;
    lookTo = 0.0f;
    fromLook = 0.0f;
    freeAx = freeAy = 0.0f;
    cf.SetLocationCamera(this);
    isTeleport = true;
    character = null;
    lockAx = false;
    isViewExecute = false;
    location = null;
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

//Инициализация
bool LocationCamera::Init()
{
    // DX8 render
    rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
    if (!rs)
        SE_THROW_MSG("No service: dx8render");

    _CORE_API->LayerCreate("execute", true, false);
    _CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
    _CORE_API->LayerAdd("execute", GetID(), 0);

    _CORE_API->LayerCreate("realize", true, false);
    _CORE_API->LayerSetFlags("realize", LRFLAG_REALIZE);
    _CORE_API->LayerAdd("realize", GetID(), 100000);

    //Море
    _CORE_API->FindClass(&sea, "sea", 0);

    //Попробуем получить локацию
    _CORE_API->FindClass(&loc, "location", 0);

    rs->SetPerspective(LOCATIONCAMERA_PERSPECTIVE);
    // rs->SetPerspective(1.0f);

    StoreRestoreDynamicFov(false);

    return true;
}

//Исполнение
#ifndef LOCATIONCAMERA_DEBUG
void LocationCamera::Execute(dword delta_time)
#else
void LocationCamera::Realize(dword delta_time)
#endif
{
    if (m_bTrackMode)
    {
        ProcessTrackCamera();
        if (m_bTrackMode)
            return;
    }

    //Режим камеры
#ifdef _XBOX
    VDATA *vd = api->Event("EventGetSpecialMode", 0);
    if (vd)
    {
        long isSpec = 0;
        if (vd->Get(isSpec))
        {
            isSpecialMode = (isSpec != 0);
        }
    }
#endif
    CONTROL_STATE cs;
    api->Controls->GetControlState("ChrCamSpecMode", cs);
    isSpecialMode = cs.state == CST_ACTIVE;
    //Временной отрезок
    float dltTime = delta_time * 0.001f;
    if (isSleep)
        return;
    if (!Set())
        return;
    //Управление
    float oldAx = ax;
    _CORE_API->Controls->GetControlState("ChrCamTurnV", cs);
    dAx = -cs.lValue * 0.05f; //*0.005f;
    if (character->IsDead())
        dAx = 0.0f;
    float kvax = 8.0f * dltTime;
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
    _CORE_API->Controls->GetControlState("ChrTurnH", cs);
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
    //Исполняем текущий режим
    bool isELook = false;

    if (!isLockFPMode)
    {
        switch (wmode)
        {
        case cwm_follow: //Следуем за персонажем

            //*
            if (!character->IsFight() && !character->IsDialog() && !character->IsDead())
            {
                _CORE_API->Controls->GetControlState("ChrCamCameraSwitch", cs);
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
                if (_CORE_API->Controls->GetControlState("ChrCamNormalize", cs))
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
                    //Пересчитаем для специального режима велечину изменения угла
                    _CORE_API->Controls->GetControlState("ChrCamTurnH", cs);
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
        case cwm_topos: //Вид из локатора
            ExecuteTopos(dltTime);
            ax = oldAx;
            vAx = 0.0f;
            break;
        case cwm_free: //Свободно летающая камера
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

    //Морфинг позиций
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
    CVECTOR realPos = camPos;
    SEA_BASE *sb = (SEA_BASE *)_CORE_API->GetEntityPointer(&sea);
    if (sb && wmode != cwm_free && location->IsSwimming())
    {
        float seaY = sb->WaveXZ(camPos.x, camPos.z) + 1.0f;
        if (realPos.y < seaY)
            realPos.y = seaY;
    }

    CVECTOR vUp = CVECTOR(0.0f, 1.0f, 0.0f);
    if (dynamic_fog.isOn)
        ProcessDynamicFov(api->GetDeltaTime() * .001f, realPos, lookTo, vUp);

    rs->SetCamera(realPos, lookTo, vUp);

#ifdef CAMERA_VIEW_TEST_ENABLE
    //Устанавливаем новою позицию камеры
    rs->SetCamera(camPos, lookTo, vUp);
#endif
}

#ifndef LOCATIONCAMERA_DEBUG
void LocationCamera::Realize(dword delta_time)
#else
void LocationCamera::Execute(dword delta_time)
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

//Сообщения
dword _cdecl LocationCamera::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_CAMERA_SETTARGET:
        chr = message.EntityID();
        if (_CORE_API->GetEntityPointer(&chr) == 0)
        {
            _CORE_API->Trace("LocationCamera -> MSG_CAMERA_SETTARGET -> invalidate character id");
            return 0;
        }
        return 1;
    case MSG_CAMERA_FOLLOW:
        if (wmode != cwm_follow)
        {
            oldPos = camPos;
            oldLookTo = lookTo;
            kMorph = 0.0f;
        }
        wmode = cwm_follow;
        return 1;
    case MSG_CAMERA_TOPOS:
        /*if(lockAx)
        {
            _CORE_API->Send_Message(GetID(), "l", MSG_CAMERA_FOLLOW);
            return 1;
        }*/
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
        break;
    case MSG_CAMERA_FREE:
        wmode = cwm_free;
        kMorph = 1.0f;
        return 1;
    case MSG_CAMERA_SLEEP:
        isSleep = message.Long() != 0;
        return 1;

    case -1: {
        float fSpeed = message.Float();
        float fTime = message.Float();
        float fMin = message.Float();
        float fMax = message.Float();
        float fAngSpeed = message.Float();
        float fAngMax = message.Float();
        TurnOnDynamicFov(fSpeed, fTime, fMin, fMax, fAngSpeed, fAngMax);
    }
    break;

    case -2: {
        char trackname[MAX_PATH];
        message.String(sizeof(trackname), trackname);
        float fTrackTime = message.Float();
        LoadCameraTrack(trackname, fTrackTime);
        ATTRIBUTES *pA = message.AttributePointer();
        // SetTrackCameraPauses(pA);
    }
    break;
    case -3:
        TurnOffTrackCamera();
        break;
    }
    return 0;
}

//Изменение атрибута
dword LocationCamera::AttributeChanged(ATTRIBUTES *apnt)
{
    //Зачитывание атрибутов
    if (!AttributesPointer)
        return 0;

    return 0;
}

//Подготовить данные
bool LocationCamera::Set()
{
    //Указатель на персонажа
    Character *c = (Character *)_CORE_API->GetEntityPointer(&chr);
    if (!c)
        return false;
    //Характеристики персонажа
    c->GetPosition(pos);
    chay = c->GetAY();
    height = c->GetHeight();
    lheight = height * lookHeight;
    chradius = c->GetRadius();
    character = c;
    location = (Location *)_CORE_API->GetEntityPointer(&loc);
    return location != null;
}

//Изменить режим работы камеры
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

//Исполнение камеры вида из глаз
void LocationCamera::ExecuteLook(float dltTime)
{
    const float blendSpd = 1.0f;
    //Определим точку размещения камеры
    MODEL *mdl = character->Model();
    if (!mdl)
        return;
    NODE *node = mdl->GetNode(0);
    if (!node || !node->geo)
        return;
    Animation *ani = mdl->GetAnimation();
    long strIndex = -1;
    long idCamera = node->geo->FindName("camera");
    if (ani && (strIndex = node->geo->FindLabelN(strIndex + 1, idCamera)) > -1)
    {
        GEOS::LABEL lb;
        //Матрица локатора
        node->geo->GetLabel(strIndex, lb);
        CMatrix mtl;
        mtl.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
        mtl.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
        mtl.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
        mtl.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);
        //Матрица кости
        CMatrix ml = mtl * ani->GetAnimationMatrix(lb.bones[0]);
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
    //Определим куда смотрим
    lookTo.x = camPos.x + cosf(ax) * sinf(chay);
    lookTo.y = camPos.y - sinf(ax);
    lookTo.z = camPos.z + cosf(ax) * cosf(chay);
}

//Исполнение камеры наблюдающей из точки
void LocationCamera::ExecuteTopos(float dltTime)
{
    //Куда смотрим
    lookTo = pos + CVECTOR(0.0f, lheight, 0.0f);
    //Откуда смотрим
    camPos = fromLook;
}

//Исполнение свободно летающей камеры
void LocationCamera::ExecuteFree(float dltTime)
{
    if (LOWORD(GetKeyState(VK_NUMLOCK)) != 0)
        return;

    CVECTOR oldPos(camPos);

    const float pi = 3.14159265359f;
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
#ifndef _XBOX
    if (api->Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0)
        dltTime *= 10.0f;
    if (api->Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0)
        dltTime *= 4.0f;
    if (api->Controls->GetDebugAsyncKeyState(VK_LBUTTON) < 0)
        camPos += 5.0f * lookTo * dltTime;
    if (api->Controls->GetDebugAsyncKeyState(VK_RBUTTON) < 0)
        camPos -= 5.0f * lookTo * dltTime;
#endif
    lookTo += camPos;
}

//Протрейсит луч через локацию
float LocationCamera::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    if (!location)
        return 2.0f;
    return location->Trace(src, dst);
}

bool LocationCamera::GetCollideTriangle(Triangle &trg)
{
    if (!location)
        return false;
    return location->GetCollideTriangle(trg);
}

void LocationCamera::Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, long num))
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

        CVECTOR v = vTo - vFrom;
        float fAng = atan2f(v.x, v.z);
        CMatrix mrot(0.f, 0.f, dynamic_fog.fCurAngle);
        mrot.RotateY(fAng);
        vUp = mrot * vUp;
    }
}

void LocationCamera::StoreRestoreDynamicFov(bool bStore)
{
    if (!AttributesPointer)
        return;
    ATTRIBUTES *pA = AttributesPointer->GetAttributeClass("DynamicFov");
    if (!pA)
        pA = AttributesPointer->CreateSubAClass(AttributesPointer, "DynamicFov");
    if (!pA)
        return;

    if (bStore)
    { // store
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
    { // restore
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

    // установка камеры на начало трека
    CVECTOR pos;
    Quaternion ang;
    CMatrix view;
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

        CVECTOR pos;
        Quaternion ang;
        CMatrix view;
        m_track.GetPoint(0.99999f, pos, ang);
        ang.GetMatrix(view);
        view.vx = -view.vx;
        view.vz = -view.vz;
        view.Inverse();

        oldPos = *(CVECTOR *)&pos;
        (*(CMatrix *)&view).MulToInvNorm(CVECTOR(0.f, 0.f, 1.f), oldLookTo);
    }
    api->Event("TrackCameraOff", "s", m_sCurTrackName.GetBuffer());
}

void LocationCamera::ProcessTrackCamera()
{
    float fTrackTime = TrackPauseProcess();

    if (fTrackTime >= m_fTrackMaxTime)
    {
        TurnOffTrackCamera();
        return;
    }

    CVECTOR pos;
    Quaternion ang;
    CMatrix view;
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
    float fOldTime = m_fTrackCurTime;
    m_fTrackCurTime += api->GetDeltaTime() * 0.001f;

    //	for( long nPause=m_nCurPauseIndex+1; nPause<m_aTrackPauses; nPause++ )
    //		if( m_aTrackPauses[nPause].trackTime <= m_fTrackCurTime ) {
    //		}

    /*	if( m_nCurPauseIndex >= 0 ) // счас в паузе
        {
            if( m_fTrackCurTime >= m_aTrackPauses[m_nCurPauseIndex].pauseTime )
        }*/

    return m_fTrackCurTime;

    /*	float fCurTime = m_fTrackCurTime + api->GetDeltaTime() * 0.001f;

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
