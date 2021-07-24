//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WideScreen
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WideScreen.h"
#include "Entity.h"
#include "core.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

WideScreen::WideScreen()
{
    state = 0.0f;
    dlt = 1.0f;
}

WideScreen::~WideScreen()
{
}

// Initialization
bool WideScreen::Init()
{
    // Layers
    // core.LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(REALIZE, GetId(), -257);
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);
    w = static_cast<float>(vp.Width);
    h = static_cast<float>(vp.Height);
    if (w <= 0 || h <= 0)
        return false;
    return true;
}

// Messages
uint64_t WideScreen::ProcessMessage(MESSAGE &message)
{
    dlt = -1.0f;
    return 0;
}

// Work
void WideScreen::Realize(uint32_t delta_time)
{
    // Current state
    state += dlt * delta_time * 0.001f;
    if (state < 0.0f)
    {
        EntityManager::EraseEntity(GetId());
        return;
    }
    if (state > 1.0f)
    {
        state = 1.0f;
        dlt = 0.0f;
    }
    // Draw
    static struct
    {
        float x, y, z, rhw;
    } buf[12];
    const auto hg = state * h * 0.1f;
    buf[0].x = 0.0f;
    buf[0].y = 0.0f;
    buf[0].z = 0.5f;
    buf[0].rhw = 1.0f;
    buf[1].x = 0.0f;
    buf[1].y = hg;
    buf[1].z = 0.5f;
    buf[1].rhw = 1.0f;
    buf[2].x = w;
    buf[2].y = 0.0f;
    buf[2].z = 0.5f;
    buf[2].rhw = 1.0f;
    buf[3].x = w;
    buf[3].y = 0.0f;
    buf[3].z = 0.5f;
    buf[3].rhw = 2.0f;
    buf[4].x = 0.0f;
    buf[4].y = hg;
    buf[4].z = 0.5f;
    buf[4].rhw = 2.0f;
    buf[5].x = w;
    buf[5].y = hg;
    buf[5].z = 0.5f;
    buf[5].rhw = 2.0f;
    buf[6].x = 0.0f;
    buf[6].y = h - hg;
    buf[6].z = 0.5f;
    buf[6].rhw = 2.0f;
    buf[7].x = 0.0f;
    buf[7].y = h;
    buf[7].z = 0.5f;
    buf[7].rhw = 2.0f;
    buf[8].x = w;
    buf[8].y = h - hg;
    buf[8].z = 0.5f;
    buf[8].rhw = 2.0f;
    buf[9].x = w;
    buf[9].y = h - hg;
    buf[9].z = 0.5f;
    buf[9].rhw = 2.0f;
    buf[10].x = 0.0f;
    buf[10].y = h;
    buf[10].z = 0.5f;
    buf[10].rhw = 2.0f;
    buf[11].x = w;
    buf[11].y = h;
    buf[11].z = 0.5f;
    buf[11].rhw = 2.0f;
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW, 4, buf, sizeof(buf[0]), "WideScreen");
}
