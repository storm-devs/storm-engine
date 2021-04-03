#include "TButterflies.h"

#include "../../Shared/messages.h"
#include "core.h"
#include "vfile_service.h"

#pragma warning(disable : 4244)

//--------------------------------------------------------------------
TButterflies::TButterflies() : enabled(false), yDefineTime(0)
{
}

//--------------------------------------------------------------------
TButterflies::~TButterflies()
{
    renderService->TextureRelease(texture);

    delete ivManager;
}

//--------------------------------------------------------------------
void TButterflies::LoadSettings()
{
    auto ini = fio->OpenIniFile(ANIMALS_INI_FILENAME);
    if (!ini)
        return;

    butterfliesCount = ini->GetLong(ANIMALS_BUTTERFLIES_SECTION, "count", BUTTERFLY_COUNT);
    maxDistance = ini->GetFloat(ANIMALS_BUTTERFLIES_SECTION, "distance", BUTTERFLY_DISTANCE);

    // DEBUG!
    // butterfliesCount = 1;
}

//--------------------------------------------------------------------
void TButterflies::Init()
{
    LoadSettings();

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!renderService)
        throw std::exception("!Butterflies: No service 'dx9render'");

    collide = static_cast<COLLIDE *>(core.CreateService("coll"));
    if (!collide)
        throw std::exception("!Butterflies: No service COLLIDE");

    ivManager =
        new IVBufferManager(renderService, BUTTERFLY_VERTEX_TYPE, sizeof(tButterflyVertex), 3 * 4, 6, butterfliesCount);

    for (auto i = 0; i < butterfliesCount; i++)
        butterflies[i].Initialize(CVECTOR(0.0f, 0.0f, 0.0f), maxDistance, ivManager->ReserveElement(), rand() % 4,
                                  rand() % 4);

    texture = renderService->TextureCreate("butter.tga");
}

//--------------------------------------------------------------------
uint64_t TButterflies::ProcessMessage(long _code, MESSAGE &message)
{
    const uint32_t outValue = 0;
    switch (_code)
    {
    case MSG_ANIMALS_BUTTERFLIES_SHOW:
        enabled = true;
        break;

    case MSG_ANIMALS_BUTTERFLIES_HIDE:
        enabled = false;
        break;

    case MSG_ANIMALS_BUTTERFLIES_XYZ: {
        static CVECTOR affectVector(0.f, 0.f, 0.f);
        affectVector.x = message.Float();
        affectVector.z = message.Float();
        for (auto i = 0; i < butterfliesCount; i++)
        {
            butterflies[i].Effect(affectVector);
        }
    }
    break;
    }
    return outValue;
}

//--------------------------------------------------------------------
void TButterflies::Execute(uint32_t _dTime)
{
    if (!enabled)
        return;

    // re-set center
    CVECTOR pos, ang;
    float persp;
    renderService->GetCamera(pos, ang, persp);
    butterflies[0].SetCenter(pos);
    int i;

    const auto its = EntityManager::GetEntityIdIterators(SHADOW);

    // redefine minY
    yDefineTime += _dTime;
    if (yDefineTime > Y_REDEFINE_TIME)
    {
        for (i = 0; i < butterfliesCount; i++)
        {
            static const auto ALL_Y = 1000.0f;
            auto topVector = butterflies[i].GetPosition();
            auto bottomVector = butterflies[i].GetPosition();
            topVector.y = ALL_Y;
            bottomVector.y = -ALL_Y;

            const auto ray = collide->Trace(its, topVector, bottomVector, nullptr, 0);
            if (ray <= 1.0f)
                butterflies[i].SetMinY(-ALL_Y + (1.f - ray) * 2.f * ALL_Y);
            else
                butterflies[i].SetMinY(-ALL_Y);
        }
    }

    // recalculate & redraw
    ivManager->LockBuffers();

    for (i = 0; i < butterfliesCount; i++)
    {
        butterflies[i].Calculate(_dTime, collide, its);
        butterflies[i].Draw(ivManager);
        // butterflies[i].Draw(renderService);
    }

    ivManager->UnlockBuffers();
}

//--------------------------------------------------------------------
void TButterflies::Realize(uint32_t dTime)
{
    if (!enabled)
        return;

    const CMatrix wMatrix;

    renderService->SetTransform(D3DTS_WORLD, static_cast<D3DMATRIX *>(wMatrix));
    renderService->TextureSet(0, texture);
    // for (int i = 0; i<butterfliesCount; i++)
    //    butterflies[i].Draw(renderService, butterfly);
    ivManager->DrawBuffers("Butterfly");
}

//--------------------------------------------------------------------
