#include "TButterflies.h"

#include "core.h"
#include "shared/messages.h"
#include "vfile_service.h"

#pragma warning(disable : 4244)

//--------------------------------------------------------------------
TButterflies::TButterflies() : enabled(false), yDefineTime(0)
{
    butterflies.resize(BUTTERFLY_COUNT);
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
    {
        return;
    }        

    butterfliesCount = ini->GetLong(ANIMALS_BUTTERFLIES_SECTION, "count", BUTTERFLY_COUNT);
    maxDistance = ini->GetFloat(ANIMALS_BUTTERFLIES_SECTION, "distance", BUTTERFLY_DISTANCE);
}

//--------------------------------------------------------------------
void TButterflies::Init()
{
    LoadSettings();

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!renderService)
    {
        throw std::runtime_error("!Butterflies: No service 'dx9render'");
    }        

    collide = static_cast<COLLIDE *>(core.CreateService("coll"));
    if (!collide)
    {
        throw std::runtime_error("!Butterflies: No service COLLIDE");
    }
        
    ivManager =
        new IVBufferManager(renderService, BUTTERFLY_VERTEX_TYPE, sizeof(tButterflyVertex), 3 * 4, 6, butterfliesCount);

    for (auto& butterfly : butterflies)
    {
        butterfly = std::make_unique<TButterfly>();
        butterfly->Initialize(CVECTOR(0.0f, 0.0f, 0.0f), maxDistance, ivManager->ReserveElement(), rand() % 4,
                              rand() % 4);
    }
    texture = renderService->TextureCreate("butter.tga");
}

//--------------------------------------------------------------------
uint64_t TButterflies::ProcessMessage(const long _code, MESSAGE &message)
{
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
        for (auto& butterfly : butterflies)
        {
            butterfly->Effect(affectVector);
        }
    }
    break;
    }
    return 0;
}

//--------------------------------------------------------------------
void TButterflies::Execute(const uint32_t _dTime)
{
    if (!enabled)
    {
        return;
    }        

    // re-set center
    CVECTOR pos, ang;
    float persp;
    renderService->GetCamera(pos, ang, persp);
    butterflies[0]->SetCenter(pos);

    const auto its = EntityManager::GetEntityIdIterators(SHADOW);

    // redefine minY
    yDefineTime += _dTime;
    if (yDefineTime > Y_REDEFINE_TIME)
    {
        static const auto ALL_Y = 1000.0f;
        for (auto& butterfly : butterflies)
        {            
            auto topVector = butterfly->GetPosition();
            auto bottomVector = butterfly->GetPosition();
            topVector.y = ALL_Y;
            bottomVector.y = -ALL_Y;

            const auto ray = collide->Trace(its, topVector, bottomVector, nullptr, 0);
            if (ray <= 1.0f)
            {
                butterfly->SetMinY(-ALL_Y + (1.0f - ray) * 2.0f * ALL_Y);
            }               
            else
            {
                butterfly->SetMinY(-ALL_Y);
            }                
        }
    }

    // recalculate & redraw
    ivManager->LockBuffers();

    for (auto &butterfly : butterflies)
    {
        butterfly->Calculate(_dTime, collide, its);
        butterfly->Draw(ivManager);
        // butterflies[i].Draw(renderService);
    }

    ivManager->UnlockBuffers();
}

//--------------------------------------------------------------------
void TButterflies::Realize(const uint32_t dTime)
{
    if (!enabled)
    {
        return;
    }        

    const CMatrix wMatrix;

    renderService->SetTransform(D3DTS_WORLD, static_cast<D3DMATRIX *>(wMatrix));
    renderService->TextureSet(0, texture);
    // for (int i = 0; i<butterfliesCount; i++)
    //    butterflies[i].Draw(renderService, butterfly);
    ivManager->DrawBuffers("Butterfly");
}

//--------------------------------------------------------------------
