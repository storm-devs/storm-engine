#include "free_camera.h"
#include "sd2_h/save_load.h"
#include "collide.h"

#define SENSITIVITY 0.0015f
#define FOV 1.285f

entid_t sphere;
COLLIDE *pCollide;

FREE_CAMERA::FREE_CAMERA()
{
    SetOn(false);
    SetActive(false);

    pIslandBase = nullptr;
    pRS = nullptr;
    ZERO2(vPos, vAng);
    vPos.z = 250.0f;
    vPos.y = 3.0f;
    fFov = FOV;

    iLockX = 0;
    iLockY = 0;

    fCameraOnEarthHeight = 3.0f;
    bCameraOnEarth = false;
}

FREE_CAMERA::~FREE_CAMERA()
{
}

bool FREE_CAMERA::Init()
{
    // GUARD(FREE_CAMERA::FREE_CAMERA())
    // core.LayerCreate("realize",true,false);
    // EntityManager::AddToLayer("system_messages",GetId(),1);
    SetDevice();
    // UNGUARD
    return true;
}

void FREE_CAMERA::SetDevice()
{
    pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(pRS);
    pCollide = static_cast<COLLIDE *>(core.GetService("COLL"));
    Assert(pCollide);

    /*EntityManager::CreateEntity(&sphere,"modelr");
    core.Send_Message(sphere,"ls",MSG_MODEL_LOAD_GEO,"mirror");
    EntityManager::AddToLayer(realize,sphere,10000);*/
}

bool FREE_CAMERA::CreateState(ENTITY_STATE_GEN *state_gen) const
{
    state_gen->SetState("vv", sizeof(vPos), vPos, sizeof(vAng), vAng);
    return true;
}

bool FREE_CAMERA::LoadState(ENTITY_STATE *state)
{
    SetDevice();
    state->Struct(sizeof(vPos), (char *)&vPos);
    state->Struct(sizeof(vAng), (char *)&vAng);
    return true;
}

void FREE_CAMERA::Execute(uint32_t Delta_Time)
{
    if (!isOn())
        return;

    SetPerspective(AttributesPointer->GetAttributeAsFloat("Perspective"));
    
    float persp;
    pRS->GetCamera(vPos, vAng, persp);

    if (!pIslandBase)
        pIslandBase = static_cast<ISLAND_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("island")));

    if (!pIslandBase)
        return;

    Move(core.GetDeltaTime());
}

void FREE_CAMERA::Move(uint32_t DeltaTime)
{
    if (!isActive())
        return;
    if (LOWORD(GetKeyState(VK_NUMLOCK)) != 0)
        return;

    // POINT pnt;
    // GetCursorPos(&pnt);
    // if(pnt.x != iLockX || pnt.y != iLockY)
    CONTROL_STATE cs;

    {
        core.Controls->GetControlState("FreeCamera_Turn_H", cs);
        vAng.y += SENSITIVITY * static_cast<float>(cs.fValue);
        core.Controls->GetControlState("FreeCamera_Turn_V", cs);
        vAng.x += SENSITIVITY * static_cast<float>(cs.fValue);
        // SetCursorPos(iLockX,iLockY);
    }
    if (bCameraOnEarth && pIslandBase)
    {
        pIslandBase->GetDepth(vPos.x, vPos.z, &vPos.y);
        vPos.y += fCameraOnEarthHeight;
    }
    const auto c0 = cosf(vAng.y);
    const auto s0 = sinf(vAng.y);
    const auto c1 = cosf(vAng.x);
    const auto s1 = sinf(vAng.x);
    auto c2 = cosf(vAng.z);
    float s2 = sinf(vAng.z);
    float speed = 5.0f * 0.001f * static_cast<float>(DeltaTime);

    if (GetAsyncKeyState(VK_SHIFT))
        speed *= 4.0f;
    if (GetAsyncKeyState(VK_CONTROL))
        speed *= 8.0f;

    core.Controls->GetControlState("FreeCamera_Forward", cs);
    if (cs.state == CST_ACTIVE)
        vPos += speed * CVECTOR(s0 * c1, -s1, c0 * c1);
    core.Controls->GetControlState("FreeCamera_Backward", cs);
    if (cs.state == CST_ACTIVE)
        vPos -= speed * CVECTOR(s0 * c1, -s1, c0 * c1);

    /*if (GetAsyncKeyState(VK_LBUTTON))    vPos += speed*CVECTOR(s0*c1, -s1, c0*c1);
    if (GetAsyncKeyState(VK_RBUTTON))    vPos -= speed*CVECTOR(s0*c1, -s1, c0*c1);
    if(GetAsyncKeyState('I'))    vPos += speed*CVECTOR(0.0f, 0.1f , 0.0f);
    if(GetAsyncKeyState('K'))    vPos += speed*CVECTOR(0.0f, -0.1f, 0.0f);*/

    // vPos = CVECTOR(0.0f, 20.0f, 0.0f);

    pRS->SetCamera(vPos, vAng, GetPerspective());

    /*CVECTOR vRes;
    CVECTOR vDst = vPos + 2000.0f*CVECTOR(s0*c1, -s1, c0*c1);

    walker_tpVW = core.LayerGetWalker("sun_trace");
    float fRes = pCollide->Trace(*pVW,vPos,vDst,nullptr,0);
    if (fRes > 1.0f) vRes = vDst;
    else
    {
      vRes = vPos + fRes * (vDst - vPos);
      entid_t ent = pCollide->GetObjectID();
      MODELR *pEntity = (MODELR*)EntityManager::GetEntityPointer(ent);
    }


    MODEL* pModel = (MODEL*)EntityManager::GetEntityPointer(sphere);
    pModel->mtx.BuildPosition(vRes.x,vRes.y,vRes.z);
    delete pVW;*/
}

void FREE_CAMERA::Save(CSaveLoad *pSL)
{
    pSL->SaveVector(vPos);
    pSL->SaveVector(vAng);
    pSL->SaveFloat(fFov);
    pSL->SaveLong(iLockX);
    pSL->SaveLong(iLockY);

    pSL->SaveDword(bCameraOnEarth);
    pSL->SaveFloat(fCameraOnEarthHeight);
}

void FREE_CAMERA::Load(CSaveLoad *pSL)
{
    vPos = pSL->LoadVector();
    vAng = pSL->LoadVector();
    fFov = pSL->LoadFloat();
    iLockX = pSL->LoadLong();
    iLockY = pSL->LoadLong();

    bCameraOnEarth = pSL->LoadDword() != 0;
    fCameraOnEarthHeight = pSL->LoadFloat();
}
