//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	Tornado
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "Tornado.h"
#include "EntityManager.h"

//============================================================================================

INTERFACE_FUNCTION
CREATE_CLASS(Tornado)

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

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

//Инициализация
bool Tornado::Init()
{
    // api->LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    // api->LayerCreate("realize", true, false);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::AddToLayer(EXECUTE, GetId(), 70000);
    EntityManager::AddToLayer(REALIZE, GetId(), 70000);

    // DX9 render
    rs = static_cast<VDX9RENDER *>(api->CreateService("dx9render"));
    if (!rs)
        throw std::exception("No service: dx9render");

    //Создаём буфера для столба
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
    //Создаём звук
    soundService = static_cast<VSoundService *>(api->CreateService("SoundService"));
    if (soundService)
    {
        const auto pos = CVECTOR(pillar.GetX(0.0f), 0.0f, pillar.GetZ(0.0f));
        sID = soundService->SoundPlay("tornado", PCM_3D, VOLUME_FX, false, true, false, 0, &pos);
    }
    return true;
}

//Исполнение
void Tornado::Execute(uint32_t delta_time)
{
    const auto dltTime = delta_time * 0.001f * 1.0f;
    pillar.Update(dltTime);
    particles.Update(dltTime);
    noiseCloud.Update(dltTime);
    debris.Update(dltTime);
    eventCounter += dltTime;
    if (eventCounter > 1.0f)
        api->Event("TornadoDamage", "fff", eventCounter, pillar.GetX(0.0f), pillar.GetZ(0.0f));
    if (liveTime < 0.0f)
    {
        SetAlpha(galhpa);
        galhpa -= dltTime * 0.2f;
        if (galhpa < 0.0f)
        {
            galhpa = 0.0f;
            api->Event("TornadoDelete", nullptr);
            EntityManager::EraseEntity(GetId());
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
    //Обломки
    debris.Draw(rs);
    //Единичная мировая матрица
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    //Облака
    noiseCloud.Draw(rs);
    //Столб
    auto *vrt = static_cast<Pillar::Vertex *>(rs->LockVertexBuffer(vb));
    if (!vrt)
        return;
    rs->TextureSet(0, -1);
    pillar.FillVertexBuffer(vrt);
    rs->UnLockVertexBuffer(vb);
    rs->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, vb, sizeof(Pillar::Vertex), ib, 0, pillar.GetNumVerteces(), 0,
                                      pillar.GetNumTriangles(), "TornadoPillar");
    //Системы частиц
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
