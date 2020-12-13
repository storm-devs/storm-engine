#include "TSink.h"
#include "SinkSplashDefines.h"

//--------------------------------------------------------------------
TSink::TSink() : enabled(false), texture(0), ivManager(nullptr), time(0)
{
}

//--------------------------------------------------------------------
TSink::~TSink()
{
    Release();
}

//--------------------------------------------------------------------
void TSink::Initialize(INIFILE *_ini, IDirect3DDevice9 *_device, SEA_BASE *_sea, VDX9RENDER *_renderer)
{
    sea = _sea;
    renderer = _renderer;

    ivManager = new TIVBufferManager(renderer, SINK_FVF, sizeof(SINK_VERTEX), 3 * TRIANGLES_COUNT,
                                     GRID_STEPS * GRID_STEPS, MAX_SPLASHES);
    texture = renderer->TextureCreate("explos.tga");
    for (auto i = 0; i < MAX_SPLASHES; ++i)
    {
        splashes[i].Initialize(_ini, sea);
        ivIndexes[i] = -1;
    }

    for (auto i = 0; i < MAX_FLOTSAMS; ++i)
    {
        flotsams[i].Initialize(sea);
    }
}

//--------------------------------------------------------------------
void TSink::Release()
{
    if (ivManager)
    {
        delete ivManager;
        ivManager = nullptr;
    }
    if (texture)
    {
        renderer->TextureRelease(texture);
        texture = 0;
    }
}

//--------------------------------------------------------------------
void TSink::Start(const CVECTOR &_pos, float _radius)
{
    if (enabled)
        return;

    time = 0;
    center = _pos;
    radius = _radius;

    enabled = true;
    for (auto i = 0; i < MAX_SPLASHES; i++)
    {
        times[i] = static_cast<long>(rand(SINK_TIME));
        // ivIndexes[i] = -1;
    }

    for (auto i = 0; i < MAX_FLOTSAMS; i++)
    {
        flotsamTimes[i] = static_cast<long>(rand(SINK_TIME));
    }
}

//--------------------------------------------------------------------
void TSink::Process(uint32_t _dTime)
{
    if (!enabled)
        return;

    time += _dTime;
    uint16_t *indexes;
    SINK_VERTEX *vertices;

    if (time > (SINK_TIME + MAX_SPLASH_TIME))
    {
        enabled = false;
        ivManager->LockBuffers();
        for (auto i = 0; i < MAX_SPLASHES; i++)
        {
            if (splashes[i].Enabled())
            {
                ivManager->GetPointers(ivIndexes[i], &indexes, (void **)&vertices);
                splashes[i].Reset(indexes, vertices);
            }
            ivIndexes[i] = -1;
        }
        ivManager->UnlockBuffers();
        ivManager->FreeAll();
        return;
    }

    ivManager->LockBuffers();
    for (auto i = 0; i < MAX_SPLASHES; i++)
    {
        if (times[i] > 0)
            times[i] -= _dTime;
        else
        {
            if (!splashes[i].Enabled())
            {
                if (ivIndexes[i] != -1)
                {
                    ivManager->FreeElement(ivIndexes[i]);
                    times[i] = static_cast<long>(rand(SINK_TIME));
                    ivIndexes[i] = -1;
                }
                else
                {
                    auto splashCenter = center;
                    splashCenter.x += randCentered(2.0f * radius);
                    splashCenter.z += randCentered(2.0f * radius);
                    ivIndexes[i] = ivManager->ReserveElement();
                    if (ivIndexes[i] != -1)
                    {
                        long vOffset;
                        ivManager->GetPointers(ivIndexes[i], &indexes, (void **)&vertices, &vOffset);
                        splashes[i].Start(splashCenter, indexes, vertices, vOffset);
                    }
                }
            }
            else
            {
                ivManager->GetPointers(ivIndexes[i], &indexes, (void **)&vertices);
                splashes[i].Process(_dTime, indexes, vertices);
            }
        }
    }
    ivManager->UnlockBuffers();

    for (auto i = 0; i < MAX_FLOTSAMS; i++)
    {
        if (flotsamTimes[i] > 0)
            flotsamTimes[i] -= _dTime;
        else
        {
            if (!flotsams[i].Enabled())
                flotsams[i].Start(center.x, center.z, radius);
            else
                flotsams[i].Process(_dTime);
        }
    }
}

//--------------------------------------------------------------------
void TSink::Realize(uint32_t _dTime)
{
    if (!enabled)
        return;

    const CMatrix m;
    uint32_t ambient;

    renderer->SetTransform(D3DTS_WORLD, static_cast<D3DMATRIX *>(m));
    renderer->TextureSet(0, texture);

    renderer->GetRenderState(D3DRS_AMBIENT, &ambient);
    ambient |= 0x55000000;
    renderer->SetRenderState(D3DRS_TEXTUREFACTOR, ambient);
    ivManager->DrawBuffers("sink");
    renderer->SetRenderState(D3DRS_AMBIENT, ambient);

    for (auto i = 0; i < MAX_SPLASHES; i++)
    {
        if (splashes[i].Enabled())
            splashes[i].AdditionalRealize(_dTime);
    }

    for (auto i = 0; i < MAX_FLOTSAMS; i++)
    {
        if (flotsams[i].Enabled())
            flotsams[i].Realize(_dTime);
    }
}

//--------------------------------------------------------------------
bool TSink::Enabled()
{
    return enabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
