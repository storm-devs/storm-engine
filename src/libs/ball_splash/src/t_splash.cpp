#include "t_splash.h"
#include "ball_splash_defines.h"

VDX9RENDER *TSplash::renderer = nullptr;
bool TSplash::startRender = true;
float dU = 1.f / SPLASH_FRAMES_COUNT;

int TSplash::buffersUsage = 0;
/*
WORD        *TSplash::iBuffer       = 0;
GRID_VERTEX *TSplash::vBuffer       = 0;
WORD        *TSplash::iBuffer2      = 0;
GRID_VERTEX *TSplash::vBuffer2      = 0;
*/
int32_t TSplash::iBuffer = 0;
int32_t TSplash::vBuffer = 0;
int32_t TSplash::iBuffer2 = 0;
int32_t TSplash::vBuffer2 = 0;

int32_t TSplash::texture = 0;
int32_t TSplash::texture2 = 0;

uint64_t TSplash::lockTicks = 0;
uint64_t TSplash::fillTicks = 0;
uint64_t TSplash::unlockTicks = 0;
uint64_t TSplash::realizeTicks = 0;
uint32_t TSplash::processCount = 0;
uint32_t TSplash::topIndex = 0;
uint32_t TSplash::topIndex2 = 0;

uint32_t ambientColor = 0;
//--------------------------------------------------------------------
uint32_t Desaturate(uint32_t _color, float _k)
{
    auto r = (_color >> 16) & 0xFF;
    auto g = (_color >> 8) & 0xFF;
    auto b = (_color)&0xFF;
    const auto grayed = (r + g + b) / 3;

    r = static_cast<uint32_t>(grayed * _k + r * (1.f - _k));
    g = static_cast<uint32_t>(grayed * _k + g * (1.f - _k));
    b = static_cast<uint32_t>(grayed * _k + b * (1.f - _k));
    return (r << 16) | (g << 8) | b;
}

//--------------------------------------------------------------------
TSplash::TSplash() : enabled(false), time(0)
{
}

//--------------------------------------------------------------------
TSplash::~TSplash()
{
    Release();

    if (!--buffersUsage)
    {
        if (iBuffer)
            // delete iBuffer;
            renderer->ReleaseIndexBuffer(iBuffer);
        if (vBuffer)
            // delete vBuffer;
            renderer->ReleaseVertexBuffer(vBuffer);
        if (iBuffer2)
            // delete iBuffer2;
            renderer->ReleaseIndexBuffer(iBuffer2);
        if (vBuffer2)
            // delete vBuffer2;
            renderer->ReleaseVertexBuffer(vBuffer2);
        if (texture)
            renderer->TextureRelease(texture);
        if (texture2)
            renderer->TextureRelease(texture2);
    }
}

//--------------------------------------------------------------------
void TSplash::Initialize(INIFILE *_ini, IDirect3DDevice9 *_device, SEA_BASE *_sea, VDX9RENDER *_renderer)
{
    ambientColor = 0;
    sea = _sea;
    if (!renderer)
        renderer = _renderer;

    if (!buffersUsage++)
    {
        /*
        iBuffer  = new WORD[TRIANGLES_COUNT*3];
        vBuffer  = new GRID_VERTEX[GRID_STEPS*GRID_STEPS];
        iBuffer2 = new WORD[VPLANES_COUNT*6];
        vBuffer2 = new GRID_VERTEX[VPLANES_COUNT*4];
        */
        iBuffer =
            renderer->CreateIndexBuffer(MAX_SPLASHES * TRIANGLES_COUNT * 3 * sizeof(uint16_t), D3DUSAGE_WRITEONLY);
        vBuffer = renderer->CreateVertexBuffer(GRID_FVF, MAX_SPLASHES * GRID_STEPS * GRID_STEPS * sizeof(GRID_VERTEX),
                                               D3DUSAGE_WRITEONLY);
        iBuffer2 = renderer->CreateIndexBuffer(MAX_SPLASHES * VPLANES_COUNT * 6 * sizeof(uint16_t), D3DUSAGE_WRITEONLY);
        vBuffer2 = renderer->CreateVertexBuffer(GRID_FVF2, MAX_SPLASHES * VPLANES_COUNT * 4 * sizeof(GRID_VERTEX2),
                                                D3DUSAGE_WRITEONLY);

        texture = renderer->TextureCreate("explos.tga");
        texture2 = renderer->TextureCreate("splash.tga");

        // INDEXES 1
        auto *indexes = /*iBuffer*/ static_cast<uint16_t *>(renderer->LockIndexBuffer(iBuffer));

        auto startIndex = 0;
        for (auto i = 0; i < MAX_SPLASHES; i++, startIndex += GRID_STEPS * GRID_STEPS)
            for (auto z = 0; z < GRID_STEPS - 1; ++z)
                for (auto x = 0; x < GRID_STEPS - 1; ++x)
                {
                    *(indexes++) = startIndex + GRID_STEPS * z + x;
                    *(indexes++) = startIndex + GRID_STEPS * (z + 1) + x;
                    *(indexes++) = startIndex + GRID_STEPS * (z + 1) + x + 1;

                    *(indexes++) = startIndex + GRID_STEPS * z + x;
                    *(indexes++) = startIndex + GRID_STEPS * (z + 1) + x + 1;
                    *(indexes++) = startIndex + GRID_STEPS * z + x + 1;
                }
        renderer->UnLockIndexBuffer(iBuffer);

        // INDEXES 2
        startIndex = 0;
        indexes = /*iBuffer2*/ static_cast<uint16_t *>(renderer->LockIndexBuffer(iBuffer2));
        for (auto i = 0; i < MAX_SPLASHES; i++, startIndex += VPLANES_COUNT * 4)
            for (auto j = 0; j < VPLANES_COUNT; j++)
            {
                *(indexes++) = startIndex + j * 4 + 0;
                *(indexes++) = startIndex + j * 4 + 1;
                *(indexes++) = startIndex + j * 4 + 2;

                *(indexes++) = startIndex + j * 4 + 0;
                *(indexes++) = startIndex + j * 4 + 2;
                *(indexes++) = startIndex + j * 4 + 3;
            }
        renderer->UnLockIndexBuffer(iBuffer2);
    }
}

//--------------------------------------------------------------------
void TSplash::Release()
{
}

//--------------------------------------------------------------------
void TSplash::Start(const CVECTOR &_pos, const CVECTOR &_dir)
{
    time = 0;
    center = _pos;
    dir = _dir;
    //_VSYSTEM_core.Trace("start dir.x=%f, dir.z=%f", this->dir.x, this->dir.z);
    enabled = true;
    growK = randUpper(2.0f);

    midY = sea->WaveXZ(center.x, center.z) + SPLASH_MOVE_Y;
    distortDivider = rand(SPLASH_DISTORT_DIVIDER) + 10.0f;

    height = randUpper(VPLANES_HEIGHT);
    auto a = 0.f;
    width2 = randUpper(VPLANES_WIDTH / 2.f);

    if (!ambientColor)
    {
        renderer->GetRenderState(D3DRS_AMBIENT, &ambientColor);
        ambientColor = Desaturate(ambientColor, 1.0f);
    }
}

//--------------------------------------------------------------------
float TSplash::HeightF(uint32_t _time, float _r, float _k)
{
    auto tPhase = static_cast<float>(_time) * (2.f * PId2 / (SPLASH_FRAME_DELAY * SPLASH_FRAMES_COUNT));
    const auto rK = (GRID_LENGTH - _r) / GRID_LENGTH;
    auto k = static_cast<float>(_time) / SPLASH_FADE_TIME;
    if (k > 1.0f)
        k = 1.0f;
    k = 1.f - k;
    return (.55f + k * fabsf(cosf(rK * 10.f + _time / 7e2f))) * rK * rK;
}

//--------------------------------------------------------------------
bool TSplash::Process(uint32_t _dTime)
{
    if (!enabled)
        return false;

    ++processCount;
    time += _dTime;

    if (time > SPLASH_FADE_TIME)
    {
        enabled = false;
        return false;
    }

    auto rho = rand(MAX_RHO);
    auto r = SPLASH_RADIUS;
    int x, z;

    static GRID_VERTEX *startVertices;
    midY = sea->WaveXZ(center.x, center.z) + SPLASH_MOVE_Y;

    uint64_t ticksLock;
    RDTSC_B(ticksLock);
    if (startRender)
    {
        startVertices = static_cast<GRID_VERTEX *>(renderer->LockVertexBuffer(vBuffer));
        startRender = false;
    }
    RDTSC_E(ticksLock);
    lockTicks += ticksLock;
    uint64_t ticksFill;
    RDTSC_B(ticksFill);

    auto *vertices = startVertices + topIndex * GRID_STEPS * GRID_STEPS;
    const auto stepSize = static_cast<float>(GRID_LENGTH) / static_cast<float>(GRID_STEPS);
    const auto halfSize = GRID_LENGTH / 2.0f;
    CVECTOR curPos;
    float localHeight;

    // calculate alpha
    const int dt = time - SPLASH_FRAMES_COUNT * SPLASH_FRAME_DELAY / 2;
    uint32_t alpha;
    if (time < SPLASH_FADE_IN_TIME)
        alpha = static_cast<uint32_t>(255.f * time / SPLASH_FADE_IN_TIME);
    else if (dt < 0)
        alpha = 0xFF;
    else
        alpha = static_cast<uint32_t>(255.f -
                                      dt * 255.f / (SPLASH_FADE_TIME - SPLASH_FRAMES_COUNT * SPLASH_FRAME_DELAY / 2));
    alpha = (alpha << 24) | (0x00FFFFFF & ambientColor);

    // fill vertices
    for (z = 0; z < GRID_STEPS; ++z)
        for (x = 0; x < GRID_STEPS; ++x)
        {
            vertices->pos.x = center.x + growK * (stepSize * x - halfSize);
            vertices->pos.z = center.z + growK * (stepSize * z - halfSize);

            if ((x > 0) && (z > 0) && (x < GRID_STEPS - 1) && (z < GRID_STEPS - 1))
            {
                curPos.x = (center.x + stepSize * x) - halfSize;
                curPos.z = (center.z + stepSize * z) - halfSize;
                rho = sqrtf((curPos.x - center.x) * (curPos.x - center.x) +
                            (curPos.z - center.z) * (curPos.z - center.z));
                localHeight = HeightF(time, rho, (GRID_LENGTH - rho) / GRID_LENGTH);

                vertices->pos.y = midY + localHeight;
            }
            else
                vertices->pos.y = midY + 2 * SPLASH_MOVE_Y;

            vertices->color = alpha;
            vertices->tu = static_cast<float>(x) / (GRID_STEPS - 1);
            vertices->tv = static_cast<float>(z) / (GRID_STEPS - 1);
            ++vertices;
        }

    RDTSC_E(ticksFill);
    fillTicks += ticksFill;
    ++topIndex;
    return true;
}

//--------------------------------------------------------------------
void TSplash::PostProcess()
{
    if (startRender)
        return;
    uint64_t ticksUnlock;
    RDTSC_B(ticksUnlock);
    renderer->UnLockVertexBuffer(vBuffer);
    RDTSC_E(ticksUnlock);
    unlockTicks += ticksUnlock;
}

//--------------------------------------------------------------------
bool TSplash::Process2(uint32_t _dTime)
{
    if (!enabled)
        return false;

    if (time > (SPLASH_FRAMES_COUNT - 1) * SPLASH_FRAME_DELAY)
        return false;

    // VERTICES 2
    static GRID_VERTEX2 *startVertices2;
    uint64_t ticksLock;
    RDTSC_B(ticksLock);
    if (startRender)
    {
        startVertices2 = static_cast<GRID_VERTEX2 *>(renderer->LockVertexBuffer(vBuffer2));
        startRender = false;
    }
    RDTSC_E(ticksLock);
    lockTicks += ticksLock;
    uint64_t ticksFill;
    RDTSC_B(ticksFill);

    auto *vertices = startVertices2 + topIndex * 4 * VPLANES_COUNT;
    auto u = dU * static_cast<int>(time / SPLASH_FRAME_DELAY);
    if (u > (1.f - dU))
        u = 1.f - dU;

    auto alpha = static_cast<uint32_t>((static_cast<float>(time % SPLASH_FRAME_DELAY) / SPLASH_FRAME_DELAY) * 0xFF);
    alpha = (alpha << 24) | (0x00FFFFFF & ambientColor);

    auto a = 0.f;
    auto dx = VPLANES_WIDTH / 2.f;
    auto dy = 0.f;
    for (auto i = 0; i < VPLANES_COUNT; i++, a += PI / VPLANES_COUNT)
    {
        vertices->pos.x = center.x - width2 * cosf(a);
        vertices->pos.z = center.z - width2 * sinf(a);
        vertices->pos.y = midY + height + SPLASH_MOVE_Y2;
        vertices->tu = u;
        vertices->tu2 = vertices->tu + dU;
        vertices->tv = 0.f;
        vertices->tv2 = vertices->tv;
        vertices->color = alpha;
        ++vertices;

        vertices->pos.x = center.x - width2 * cosf(a);
        vertices->pos.z = center.z - width2 * sinf(a);
        vertices->pos.y = midY + SPLASH_MOVE_Y2;
        vertices->tu = u;
        vertices->tu2 = vertices->tu + dU;
        vertices->tv = 1.f;
        vertices->tv2 = vertices->tv;
        vertices->color = alpha;
        ++vertices;

        vertices->pos.x = center.x + width2 * cosf(a);
        vertices->pos.z = center.z + width2 * sinf(a);
        vertices->pos.y = midY + SPLASH_MOVE_Y2;
        vertices->tu = u + dU;
        vertices->tu2 = vertices->tu + dU;
        vertices->tv = 1.f;
        vertices->tv2 = vertices->tv;
        vertices->color = alpha;
        ++vertices;

        vertices->pos.x = center.x + width2 * cosf(a);
        vertices->pos.z = center.z + width2 * sinf(a);
        vertices->pos.y = midY + height + SPLASH_MOVE_Y2;
        vertices->tu = u + dU;
        vertices->tu2 = vertices->tu + dU;
        vertices->tv = 0.f;
        vertices->tv2 = vertices->tv;
        vertices->color = alpha;
        ++vertices;
    }

    RDTSC_E(ticksFill);
    fillTicks += ticksFill;
    ++topIndex;
    return true;
}

//--------------------------------------------------------------------
void TSplash::PostProcess2()
{
    if (startRender)
        return;
    uint64_t ticksUnlock;
    RDTSC_B(ticksUnlock);
    renderer->UnLockVertexBuffer(vBuffer2);
    RDTSC_E(ticksUnlock);
    unlockTicks += ticksUnlock;
}

//--------------------------------------------------------------------
void TSplash::Realize(uint32_t _dTime)
{
    if (startRender)
        return;

    uint64_t ticksRealize;
    RDTSC_B(ticksRealize);

    static uint32_t ambient, tfactor, oldAmbient, alpha;
    const CMatrix m;
    renderer->SetTransform(D3DTS_WORLD, static_cast<D3DMATRIX *>(m));
    renderer->TextureSet(0, texture);

    renderer->DrawBuffer(vBuffer, sizeof(GRID_VERTEX), iBuffer, 0, GRID_STEPS * GRID_STEPS * topIndex, 0,
                         TRIANGLES_COUNT * topIndex, "splash");

    RDTSC_E(ticksRealize);
    realizeTicks += ticksRealize;
}

//--------------------------------------------------------------------
void TSplash::Realize2(uint32_t _dTime)
{
    if (!topIndex)
        return;

    uint64_t ticksRealize;
    RDTSC_B(ticksRealize);

    static uint32_t ambient, fogColor, oldAmbient;
    const CMatrix m;
    renderer->SetTransform(D3DTS_WORLD, static_cast<D3DMATRIX *>(m));
    renderer->TextureSet(0, texture2);
    renderer->TextureSet(1, texture2);

    renderer->DrawBuffer(vBuffer2, sizeof(GRID_VERTEX2), iBuffer2, 0, VPLANES_COUNT * 4 * topIndex, 0,
                         VPLANES_COUNT * 2 * topIndex, nullptr);

    RDTSC_E(ticksRealize);
    realizeTicks += ticksRealize;
}

//--------------------------------------------------------------------
bool TSplash::Enabled()
{
    return enabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
