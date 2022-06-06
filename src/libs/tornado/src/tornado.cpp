//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Tornado
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "tornado.h"
#include "entity.h"
#include "core.h"

CREATE_CLASS(Tornado)

// ============================================================================================
// Construction, destruction
// ============================================================================================

Tornado::Tornado() : particles(pillar), noiseCloud(pillar), debris(pillar)
{
    ib = -1;
    vb = -1;
    rs = nullptr;
    eventCounter = 0.0f;
    liveTime = 60.0f;
    galhpa = 1.0f;
    soundService = nullptr;
    sID = SOUND_INVALID_ID;
}

Tornado::~Tornado()
{
    if (rs)
    {
        if (ib >= 0)
            rs->ReleaseIndexBuffer(ib);
        if (vb >= 0)
            rs->ReleaseVertexBuffer(vb);
        if (noiseCloud.texture >= 0)
            rs->TextureRelease(noiseCloud.texture);
        if (particles.txtPillarPrts >= 0)
            rs->TextureRelease(particles.txtPillarPrts);
        if (particles.txtGroundPrts >= 0)
            rs->TextureRelease(particles.txtGroundPrts);
    }
    if (soundService && sID != SOUND_INVALID_ID)
        soundService->SoundRelease(sID);
}

//============================================================================================

// Initialization
bool Tornado::Init()
{
    // core.LayerCreate("execute", true, false);
    core.SetLayerType(EXECUTE, layer_type_t::execute);
    // core.LayerCreate("realize", true, false);
    core.SetLayerType(REALIZE, layer_type_t::realize);
    core.AddToLayer(EXECUTE, GetId(), 70000);
    core.AddToLayer(REALIZE, GetId(), 70000);

    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    // Create buffers for the pillar
    ib = rs->CreateIndexBuffer(pillar.GetNumTriangles() * 3 * sizeof(uint16_t));
    if (ib < 0)
        return false;
    vb = rs->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE, pillar.GetNumVerteces() * sizeof(Pillar::Vertex),
                                D3DUSAGE_WRITEONLY);
    if (vb < 0)
        return false;
    auto *ibpnt = static_cast<uint16_t *>(rs->LockIndexBuffer(ib));
    if (!ibpnt)
        return false;
    pillar.FillIndexBuffer(ibpnt);
    rs->UnLockIndexBuffer(ib);
    noiseCloud.texture = rs->TextureCreate("Tornado\\trncloud.tga");
    particles.txtPillarPrts = rs->TextureCreate("Tornado\\pillarprts.tga");
    particles.txtGroundPrts = rs->TextureCreate("Tornado\\groundprts.tga");
    particles.SetSea();
    particles.Update(0.0f);
    debris.Init();
    // Create sound
    soundService = static_cast<VSoundService *>(core.GetService("SoundService"));
    if (soundService)
    {
        const auto pos = CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
        sID = soundService->SoundPlay("tornado", PCM_3D, VOLUME_FX, false, true, false, 0, &pos);
    }
    return true;
}

// Execution
void Tornado::Execute(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f * 1.0f;
    pillar.Update(dltTime);
    particles.Update(dltTime);
    noiseCloud.Update(dltTime);
    debris.Update(dltTime);
    eventCounter += dltTime;
    if (eventCounter > 1.0f)
        core.Event("TornadoDamage", "fff", eventCounter, pillar.GetX(0.0f), pillar.GetZ(0.0f));
    if (liveTime < 0.0f)
    {
        SetAlpha(galhpa);
        galhpa -= dltTime * 0.2f;
        if (galhpa < 0.0f)
        {
            galhpa = 0.0f;
            core.Event("TornadoDelete");
            core.EraseEntity(GetId());
        }
    }
    else
        liveTime -= dltTime;
    if (soundService && sID != SOUND_INVALID_ID)
    {
        const auto pos = CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
        soundService->SoundSet3DParam(sID, SM_POSITION, &pos);
    }
}

void Tornado::Realize(uint32_t delta_time)
{
    liveTime = 1000.0f;
    // Wreckage
    debris.Draw(rs);
    // World identity matrix
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    // The clouds
    noiseCloud.Draw(rs);
    // Pillar
    auto *vrt = static_cast<Pillar::Vertex *>(rs->LockVertexBuffer(vb));
    if (!vrt)
        return;
    rs->TextureSet(0, -1);
    pillar.FillVertexBuffer(vrt);
    rs->UnLockVertexBuffer(vb);
    rs->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, vb, sizeof(Pillar::Vertex), ib, 0, pillar.GetNumVerteces(), 0,
                                      pillar.GetNumTriangles(), "TornadoPillar");
    // Particle systems
    particles.Draw(rs);
}

uint64_t Tornado::ProcessMessage(MESSAGE &message)
{
    liveTime = message.Float();
    if (liveTime < 5.0f)
        liveTime = 5.0f;
    if (liveTime > 3600.0f)
        liveTime = 3600.0f;
    return 0;
}

void Tornado::SetAlpha(float a)
{
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;
    galhpa = a;
    pillar.SetGlobalAlpha(a);
    particles.SetGlobalAlpha(a);
    noiseCloud.SetGlobalAlpha(a);
    debris.SetGlobalAlpha(a);
}
