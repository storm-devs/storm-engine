#include "WaterRings.h"
#include "rands.h"

#include "defines.h"

#include "IVBufferManager.h"

CREATE_CLASS(WaterRings)

//------------------------------------------------------------------------------------
WaterRings::WaterRings() : ivManager(nullptr)
{
}

//------------------------------------------------------------------------------------
WaterRings::~WaterRings()
{
    delete ivManager;
    renderService->TextureRelease(ringTexture);
}

//------------------------------------------------------------------------------------
bool WaterRings::Init()
{
    // GUARD(WaterRings::Init())

    EntityManager::AddToLayer(REALIZE, GetId(), 65551);

    const auto seaID = EntityManager::GetEntityId("sea");
    sea = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(seaID));

    renderService = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!renderService)
        throw std::runtime_error("No service: dx9render");

    ivManager =
        new IVBufferManager(renderService, waterrings::RING_FVF, sizeof(RING_VERTEX), waterrings::TRIANGLES_COUNT * 3,
                            waterrings::GRID_STEPS_COUNT * waterrings::GRID_STEPS_COUNT, waterrings::MAX_RINGS);

    ringTexture = renderService->TextureCreate("ring.tga");

    for (auto i = 0; i < waterrings::MAX_RINGS; i++)
    {
        rings[i].ivIndex = ivManager->ReserveElement();
        rings[i].activeTime = 0;
        rings[i].active = false;
        rings[i].firstUpdate = true;
        rings[i].x = 0.f;
        rings[i].z = 0.f;
    }

    return true;
    // UNGUARD
}

//------------------------------------------------------------------------------------
void WaterRings::Realize(uint32_t _dTime)
{
    if (!sea)
        return;

    // update buffers for rings
    ivManager->LockBuffers();
    uint16_t *iPointer;
    RING_VERTEX *vPointer;
    long vOffset;
    for (auto i = 0; i < waterrings::MAX_RINGS; i++)
    {
        // check if ring needs to be removed
        if (rings[i].activeTime > (waterrings::FADE_IN_TIME + waterrings::FADE_OUT_TIME))
            rings[i].active = false;
        ivManager->GetPointers(rings[i].ivIndex, static_cast<uint16_t **>(&iPointer), (void **)&vPointer, &vOffset);
        UpdateGrid(i, iPointer, vPointer, vOffset);

        if (rings[i].active)
            rings[i].activeTime += _dTime;
    }
    ivManager->UnlockBuffers();

    renderService->TextureSet(0, ringTexture);
    ivManager->DrawBuffers("waterring");
}

//------------------------------------------------------------------------------------
uint64_t WaterRings::ProcessMessage(MESSAGE &message)
{
    // add new ring
    for (auto i = 0; i < waterrings::MAX_RINGS; i++)
    {
        if (!rings[i].active)
        {
            // found free element, add ring here
            rings[i].activeTime = 0;
            rings[i].active = true;
            rings[i].x = message.Float() /*+randCentered(0.15f)*/;
            rings[i].z = message.Float() /*+randCentered(0.15f)*/;

            const auto a = message.Float() + PI + randCentered(PId2 / 1.5f);
            rings[i].cosA = cosf(a);
            rings[i].sinA = sinf(a);

            const auto walk = message.Long() != 0;
            const auto run = message.Long() != 0;
            const auto swim = message.Long() != 0;
            if (!(walk || run || swim))
            {
                rings[i].activeTime = 0;
                rings[i].active = false;
                return 0;
            }

            if (swim)
                rings[i].state = RING_SWIM;
            else if (run)
                rings[i].state = RING_RUN;
            else
                rings[i].state = RING_WALK;

            return 0;
        }
    }
    /*
    switch(message.Long())
    {
      case MSG_BLADE_SET:
        return 0;
      break;
    }
    */

    return 0;
}

//------------------------------------------------------------------------------------
void WaterRings::UpdateGrid(int _ringI, uint16_t *_iPointer, RING_VERTEX *_vPointer, long _vOffset)
{
    Assert(_iPointer);
    Assert(_vPointer);

    float a;
    tRing *ring = &rings[_ringI];
    int x, z;

    if (ring->firstUpdate)
    {
        uint16_t *indexes = _iPointer;
        for (z = 0; z < waterrings::GRID_STEPS_COUNT - 1; ++z)
            for (x = 0; x < waterrings::GRID_STEPS_COUNT - 1; ++x)
            {
                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * z + x);
                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * (z + 1) + x);
                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * (z + 1) + x + 1);

                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * z + x);
                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * (z + 1) + x + 1);
                *(indexes++) = static_cast<uint16_t>(_vOffset + waterrings::GRID_STEPS_COUNT * z + x + 1);
            }
        ring->firstUpdate = false;
    }

    if (ring->activeTime < waterrings::FADE_IN_TIME)
        a = static_cast<float>(ring->activeTime) / waterrings::FADE_IN_TIME;
    else
        a = 1.f - (static_cast<float>(ring->activeTime - waterrings::FADE_IN_TIME) / waterrings::FADE_OUT_TIME);

    const float midX = (waterrings::GRID_STEPS_COUNT - 1) / 2.f;
    const float midZ = (waterrings::GRID_STEPS_COUNT - 1) / 2.f;
    RING_VERTEX *ringVertex = _vPointer;
    float gX, gZ;
    if (ring->active)
    {
        const uint32_t texA = static_cast<uint32_t>(a * 50) << 24;
        const float r = .4f + 1.5f * ring->activeTime / (waterrings::FADE_IN_TIME + waterrings::FADE_OUT_TIME);

        for (z = 0; z < waterrings::GRID_STEPS_COUNT; ++z)
            for (x = 0; x < waterrings::GRID_STEPS_COUNT; ++x)
            {
                ringVertex->color = texA;
                gX = (x - midX) / midX;
                gZ = (z - midZ) / midZ;
                ringVertex->pos.x = ring->x + r * (gX * ring->cosA + gZ * ring->sinA);
                ringVertex->pos.z = ring->z + r * (gZ * ring->cosA - gX * ring->sinA);
                ringVertex->pos.y = waterrings::Y_DELTA + sea->WaveXZ(ringVertex->pos.x, ringVertex->pos.z);
                // if (ring->state == RING_WALK)
                {
                    ringVertex->tu = (static_cast<float>(x) / (waterrings::GRID_STEPS_COUNT - 1)) * .5f;
                    ringVertex->tv = static_cast<float>(z) / (waterrings::GRID_STEPS_COUNT - 1);
                }
                /*
                else if (ring->state == RING_RUN)
                {
                  ringVertex->tu = ((float) x / (GRID_STEPS_COUNT - 1))*.5f+.5f;
                  ringVertex->tv = (float) z / (GRID_STEPS_COUNT - 1);
                }
                */
                ++ringVertex;
            }
    }
    else
    {
        for (z = 0; z < waterrings::GRID_STEPS_COUNT; ++z)
            for (x = 0; x < waterrings::GRID_STEPS_COUNT; ++x)
            {
                ringVertex->color = 0x00000000;
                ringVertex->pos.x = 0.f;
                ringVertex->pos.z = 0.f;
                ringVertex->pos.y = 0.f;
                ringVertex->tu = 0.f;
                ringVertex->tv = 0.f;
                ++ringVertex;
            }
    }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
