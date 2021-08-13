//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Fader
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Fader.h"
#include "core.h"
#include "entity.h"
#include "shared/messages.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

long Fader::numberOfTips = 0;
long Fader::currentTips = -1;

Fader::Fader()
    : fadeIn(false), isStart(false), isAutodelete(false), fadeSpeed(0), w(0), h(0)
{
    rs = nullptr;
    isWork = false;
    haveFrame = false;
    endFade = false;
    alpha = 0.0f;
    surface = nullptr;
    renderTarget = nullptr;
    textureID = -1;
    textureBackID = -1;
    tipsID = -1;
    eventStart = false;
    eventEnd = false;
    deleteMe = 0;
}

Fader::~Fader()
{
    if (surface)
        rs->Release(surface);
    if (renderTarget)
        renderTarget->Release();
    surface = nullptr;
    renderTarget = nullptr;
    if (textureID >= 0)
        rs->TextureRelease(textureID);
    if (textureBackID >= 0)
        rs->TextureRelease(textureBackID);
    if (tipsID >= 0)
        rs->TextureRelease(tipsID);
}

// Initialization
bool Fader::Init()
{
    // check that it's the only one

    const auto &entities = EntityManager::GetEntityIdVector("Fader");
    for (auto eid : entities)
    {
        if (eid == GetId())
            continue;

        if (fadeIn == static_cast<Fader *>(EntityManager::GetEntityPointer(eid))->fadeIn)
        {
            core.Trace("Fader::Init() -> Fader already created, %s", fadeIn ? "fade in phase" : "fade out phase");
        }
        //!!!
        // return false;
    }
    // Layers
    EntityManager::SetLayerType(FADER_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(FADER_REALIZE, GetId(), -256);
    EntityManager::SetLayerType(FADER_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(FADER_EXECUTE, GetId(), -256);

    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);
    w = static_cast<float>(vp.Width);
    h = static_cast<float>(vp.Height);
    if (w <= 0 || h <= 0)
        return false;
    if (rs->GetRenderTarget(&renderTarget) != D3D_OK)
        return false;
    // read the number of tips, if necessary
    if (!numberOfTips)
    {
        auto ini = fio->OpenIniFile(core.EngineIniFileName());
        if (ini)
        {
            numberOfTips = ini->GetLong(nullptr, "ProgressFrame", 1);
        }
        else
            numberOfTips = -1;
        if (numberOfTips > 1)
            numberOfTips = 1;
        if (numberOfTips < 0)
            numberOfTips = 0;
    }
    return true;
}

// Messages
uint64_t Fader::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case FADER_OUT: // Start screen dimming
        alpha = 0.0f;
        fadeSpeed = message.Float();
        if (fadeSpeed > 0.0f)
            fadeSpeed = 1.0f / fadeSpeed;
        else
            fadeSpeed = 0.0f;
        isWork = true;
        haveFrame = false;
        fadeIn = false;
        isStart = true;
        endFade = false;
        isAutodelete = message.Long() != 0;
        eventStart = false;
        eventEnd = false;
        break;
    case FADER_IN: // Start screen appearance
        alpha = 0.0f;
        fadeSpeed = message.Float();
        if (fadeSpeed < 0.00001f)
            fadeSpeed = 0.00001f;
        fadeSpeed = 1.0f / fadeSpeed;
        haveFrame = false;
        isWork = true;
        fadeIn = true;
        isStart = true;
        endFade = false;
        isAutodelete = message.Long() != 0;
        haveFrame = false;
        if (surface)
            surface->Release();
        surface = nullptr;
        eventStart = false;
        eventEnd = false;
        break;
    case FADER_STARTFRAME:
        haveFrame = true;
        break;
    case FADER_PICTURE: {
        if (textureID >= 0 && rs)
            rs->TextureRelease(textureID);
        const std::string &_name = message.String();
        textureID = rs->TextureCreate(_name.c_str());
        rs->SetProgressImage(_name.c_str());
        // Hint texture
        if (numberOfTips > 0)
        {
            const std::string texturePath = "interfaces\\int_border.tga";
            tipsID = rs->TextureCreate(texturePath.c_str());
            rs->SetTipsImage(texturePath.c_str());
        }
        break;
    }
    case FADER_PICTURE0: {
        if (textureBackID >= 0 && rs)
            rs->TextureRelease(textureBackID);
        const std::string &_name = message.String();
        textureBackID = rs->TextureCreate(_name.c_str());
        rs->SetProgressBackImage(_name.c_str());
        // Hint texture
        if (numberOfTips > 0)
        {
            // sprintf_s(_name, "tips\\tips_%.4u.tga", rand() % numberOfTips);
            auto *const pTipsName = rs->GetTipsImage();
            if (pTipsName)
            {
                tipsID = rs->TextureCreate(pTipsName);
                // rs->SetTipsImage(_name);
            }
        }
        break;
    }
    }
    return 0;
}

// Work
void Fader::Execute(uint32_t delta_time)
{
    // core.Trace("fader frame");
    if (deleteMe)
    {
        deleteMe++;
        if (deleteMe >= 3)
            EntityManager::EraseEntity(GetId());
    }
    if (eventStart)
    {
        eventStart = false;
        if (!fadeIn)
        {
            core.PostEvent("FaderEvent_StartFade", 0, "li", fadeIn, GetId());
            // core.Trace("FaderEvent_StartFade");
        }
        else
        {
            core.PostEvent("FaderEvent_StartFadeIn", 0, "li", fadeIn, GetId());
            //    core.Trace("FaderEvent_StartFadeIn");
        }
    }
    if (eventEnd)
    {
        eventEnd = false;
        deleteMe = isAutodelete;
        if (!fadeIn)
        {
            core.PostEvent("FaderEvent_EndFade", 0, "li", fadeIn, GetId());
            // core.Trace("FaderEvent_EndFade");
        }
        else
        {
            core.PostEvent("FaderEvent_EndFadeIn", 0, "li", fadeIn, GetId());
            //    core.Trace("FaderEvent_EndFadeIn");
        }
    }
}

void Fader::Realize(uint32_t delta_time)
{
    if (!isWork)
        return;
    if (isStart)
        eventStart = true;
    // Capturing and drawing a start frame
    if (!endFade)
    {
        if (haveFrame)
        {
            if (isStart)
            {
                // need to take a shot
                auto isOk = false;
                D3DSURFACE_DESC desc;
                if (renderTarget->GetDesc(&desc) == D3D_OK)
                {
                    if (rs->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, &surface) == D3D_OK)
                    {
                        if (rs->GetRenderTargetData(renderTarget, surface) == D3D_OK)
                        {
                            isOk = true;
                        }
                    }
                }
                if (!isOk)
                    core.Trace("Screen shot for fader not created!");
            }
            else
            {
                // Copying the shot
                if (rs->UpdateSurface(surface, nullptr, 0, renderTarget, nullptr) != D3D_OK)
                {
                    core.Trace("Can't copy fader screen shot to render target!");
                }
            }
        }
    }
    // Draw a shading rectangle
    static struct
    {
        float x, y, z, rhw;
        uint32_t color;
        float u, v;
    } drawbuf[6];
    if (alpha >= 1.0f)
    {
        alpha = 1.0f;
        if (!endFade)
        {
            endFade = true;
            eventEnd = true;
        }
    }
    auto color = (static_cast<uint32_t>((fadeIn ? (1.0f - alpha) : alpha) * 255.0f) << 24);
    if (textureBackID >= 0)
        color |= 0x00ffffff;
    drawbuf[0].x = 0.0f;
    drawbuf[0].y = 0.0f;
    drawbuf[0].z = 0.5f;
    drawbuf[0].rhw = 1.0f;
    drawbuf[0].color = color;
    drawbuf[0].u = 0.0f;
    drawbuf[0].v = 0.0f;
    drawbuf[1].x = w;
    drawbuf[1].y = 0.0f;
    drawbuf[1].z = 0.5f;
    drawbuf[1].rhw = 1.0f;
    drawbuf[1].color = color;
    drawbuf[1].u = 1.0f;
    drawbuf[1].v = 0.0f;
    drawbuf[2].x = 0.0f;
    drawbuf[2].y = h;
    drawbuf[2].z = 0.5f;
    drawbuf[2].rhw = 1.0f;
    drawbuf[2].color = color;
    drawbuf[2].u = 0.0f;
    drawbuf[2].v = 1.0f;
    drawbuf[3].x = 0.0f;
    drawbuf[3].y = h;
    drawbuf[3].z = 0.5f;
    drawbuf[3].rhw = 1.0f;
    drawbuf[3].color = color;
    drawbuf[3].u = 0.0f;
    drawbuf[3].v = 1.0f;
    drawbuf[4].x = w;
    drawbuf[4].y = 0.0f;
    drawbuf[4].z = 0.5f;
    drawbuf[4].rhw = 1.0f;
    drawbuf[4].color = color;
    drawbuf[4].u = 1.0f;
    drawbuf[4].v = 0.0f;
    drawbuf[5].x = w;
    drawbuf[5].y = h;
    drawbuf[5].z = 0.5f;
    drawbuf[5].rhw = 1.0f;
    drawbuf[5].color = color;
    drawbuf[5].u = 1.0f;
    drawbuf[5].v = 1.0f;
    rs->TextureSet(0, textureBackID);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2,
                        drawbuf, sizeof(drawbuf[0]), "Fader");

    if (textureID >= 0)
        color |= 0x00ffffff;

    float dy = 0.0f;
    float dx = ((float(w) - (4.0f * float(h) / 3.0f)) / 2.0f);
    if (dx < 10.0f)
        dx = 0.0f;
    else
    {
        dy = 25.0f;
        dx = ((float(w) - (4.0f * (float(h) - 2.0f * dy) / 3.0f)) / 2.0f);
    }

    drawbuf[0].x = 0.0f + dx;
    drawbuf[0].y = 0.0f + dy;
    drawbuf[1].x = w - dx;
    drawbuf[1].y = 0.0f + dy;
    drawbuf[2].x = 0.0f + dx;
    drawbuf[2].y = h - dy;
    drawbuf[3].x = 0.0f + dx;
    drawbuf[3].y = h - dy;
    drawbuf[4].x = w - dx;
    drawbuf[4].y = 0.0f + dy;
    drawbuf[5].x = w - dx;
    drawbuf[5].y = h - dy;

    rs->TextureSet(0, textureID);
    if (tipsID >= 0)
    {
        rs->TextureSet(1, tipsID);
        rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2,
                            drawbuf, sizeof(drawbuf[0]), "FaderWithTips");
    }
    else
    {
        rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2, 2,
                            drawbuf, sizeof(drawbuf[0]), "Fader");
    }
    // Increase alpha
    if (!endFade)
    {
        if (fadeSpeed > 0.0f)
            alpha += delta_time * 0.001f * fadeSpeed;
        else
            alpha = 1.0f;
    }
    isStart = false;
}
