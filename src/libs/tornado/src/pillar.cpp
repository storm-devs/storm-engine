//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Pillar
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "pillar.h"

#include <chrono>

#include "c_vector.h"
#include "storm_assert.h"

//============================================================================================

#define TRND_NUMSEC (sizeof(section) / sizeof(Section))
#define TRND_SEGMENTS (sizeof(segment) / sizeof(Segment))

#define TRND_HEIGHT 500.0f
#define TRND_MINRAD 6.0f
#define TRND_MAXRAD 150.0f
#define TRND_KSHAPE 5.0f

// ============================================================================================
// Construction, destruction
// ============================================================================================

Pillar::Pillar()
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    // Sections
    int32_t i;
    for (i = 0; i < TRND_NUMSEC; i++)
    {
        auto kHeight = i / static_cast<float>(TRND_NUMSEC - 1);
        section[i].radius = GetRaduis(kHeight * TRND_HEIGHT);
        section[i].x = 0.0f;
        section[i].y = i * (TRND_HEIGHT / (TRND_NUMSEC - 1));
        section[i].z = 0.0f;
        section[i].alpha = (1.0f - powf(kHeight, 2.0f));
        kHeight = 1.0f - (kHeight - 0.5f) * (kHeight - 0.5f) * 4.0f;
        section[i].alpha *= kHeight * kHeight;
    }
    // Segment vectors
    for (i = 0; i < TRND_SEGMENTS; i++)
    {
        const auto ang = i * 2.0f * TRND_PI / TRND_SEGMENTS;
        segment[i].x = sinf(ang);
        segment[i].z = cosf(ang);
    }
    x = z = 0.0f;
    const auto dir = rand() * 2.0f * TRND_PI / RAND_MAX;
    const auto spd = 3.0f + rand() * 6.0f / RAND_MAX;
    dx = spd * sinf(dir);
    dz = spd * cosf(dir);
    phaseX = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseZ = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseX1 = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseZ1 = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseX2 = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseZ2 = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseX3 = rand() * 2.0f * TRND_PI / RAND_MAX;
    phaseZ3 = rand() * 2.0f * TRND_PI / RAND_MAX;
    SetGlobalAlpha(1.0f);
}

Pillar::~Pillar()
{
}

void Pillar::Update(float dltTime)
{
    x += dx * dltTime;
    z += dz * dltTime;
    phaseX += dltTime * 0.1f;
    if (phaseX > 2.0f * TRND_PI)
        phaseX -= 2.0f * TRND_PI;
    phaseZ += dltTime * 0.14f;
    if (phaseZ > 2.0f * TRND_PI)
        phaseZ -= 2.0f * TRND_PI;
    phaseX1 += dltTime * 0.014f;
    if (phaseX1 > 2.0f * TRND_PI)
        phaseX1 -= 2.0f * TRND_PI;
    phaseZ1 += dltTime * 0.01f;
    if (phaseZ1 > 2.0f * TRND_PI)
        phaseZ1 -= 2.0f * TRND_PI;
    phaseX2 += dltTime * (3.3f + 2.0f * sinf(phaseX1));
    if (phaseX2 > 2.0f * TRND_PI)
        phaseX2 -= 2.0f * TRND_PI;
    phaseZ2 += dltTime * (2.28f + 3.0f * sinf(phaseZ1));
    if (phaseZ2 > 2.0f * TRND_PI)
        phaseZ2 -= 2.0f * TRND_PI;
    phaseX3 += dltTime * (0.3f + 0.3f * sinf(phaseX1));
    if (phaseX3 > 2.0f * TRND_PI)
        phaseX3 -= 2.0f * TRND_PI;
    phaseZ3 += dltTime * (0.28f + 0.2f * sinf(phaseZ1));
    if (phaseZ3 > 2.0f * TRND_PI)
        phaseZ3 -= 2.0f * TRND_PI;
    for (int32_t i = 0; i < TRND_NUMSEC; i++)
    {
        const auto kHeight = i / static_cast<float>(TRND_NUMSEC - 1);
        section[i].x = GetX(kHeight * TRND_HEIGHT);
        section[i].z = GetZ(kHeight * TRND_HEIGHT);
    }
}

void Pillar::FillVertexBuffer(Vertex *buffer)
{
    for (int32_t i = 0; i < TRND_NUMSEC; i++)
    {
        auto &s = section[i];
        for (uint16_t j = 0; j < TRND_SEGMENTS; j++, buffer++)
        {
            buffer->x = s.x + s.radius * segment[j].x;
            buffer->y = s.y;
            buffer->z = s.z + s.radius * segment[j].z;
            const auto alpha = static_cast<int32_t>(s.alpha * galpha * 0.2f);
            buffer->color = (alpha << 24) | 0x32323f;
        }
    }
}

void Pillar::FillIndexBuffer(uint16_t *buffer)
{
    // By sections
    for (int32_t i = 0; i < TRND_NUMSEC - 1; i++)
    {
        const auto base = static_cast<uint16_t>(i * TRND_SEGMENTS);
        auto *const buf = buffer + base * 2 * 3;
        // By segments
        for (uint16_t j = 0; j < TRND_SEGMENTS; j++)
        {
            const uint16_t j1 = j < (TRND_SEGMENTS - 1) ? j + 1 : 0;
            buf[j * 6 + 0] = base + j;
            buf[j * 6 + 1] = base + j1;
            buf[j * 6 + 2] = base + j + TRND_SEGMENTS;
            buf[j * 6 + 3] = base + j1;
            buf[j * 6 + 4] = base + j1 + TRND_SEGMENTS;
            buf[j * 6 + 5] = base + j + TRND_SEGMENTS;
            for (int32_t t = 0; t < 6; t++)
            {
                Assert(buf[j * 6 + t] < GetNumVerteces());
            }
        }
    }
}

int32_t Pillar::GetNumVerteces()
{
    const int32_t num = TRND_NUMSEC * TRND_SEGMENTS;
    Assert(num < 65535);
    return num;
}

int32_t Pillar::GetNumTriangles()
{
    return (TRND_NUMSEC - 1) * TRND_SEGMENTS * 2;
}

float Pillar::GetHeight()
{
    return TRND_HEIGHT;
}

float Pillar::GetKHeight(float y)
{
    return y / TRND_HEIGHT;
}

float Pillar::GetRaduis(float y)
{
    auto kH = y / TRND_HEIGHT;
    if (kH < 0.0f)
        kH = 0.0f;
    if (kH > 1.0f)
        kH = 1.0f;
    return TRND_MINRAD + (TRND_MAXRAD - TRND_MINRAD) * powf(kH, TRND_KSHAPE);
}

float Pillar::GetX(float y)
{
    const auto kH = y / TRND_HEIGHT;
    return x + (10.0f + (1.0f - kH) * 110.0f) * sinf(phaseX + kH * (2.0f + 1.3f * sinf(-phaseX3))) +
           1.0f * sinf(16.0f * kH - phaseX2);
}

float Pillar::GetZ(float y)
{
    const auto kH = y / TRND_HEIGHT;
    return z + (10.0f + (1.0f - kH) * 110.0f) * sinf(phaseZ + kH * (2.0f + 1.3f * sinf(-phaseZ3))) +
           1.2f * sinf(20.0f * kH - phaseZ2);
}

float Pillar::RandomPos(CVECTOR &pos)
{
    pos.y = rand() * TRND_HEIGHT / RAND_MAX;
    const auto ang = rand() * 2.0f * TRND_PI / (RAND_MAX + 1);
    const auto r = GetRaduis(pos.y);
    pos.x = r * sinf(ang) + GetX(pos.y);
    pos.z = r * cosf(ang) + GetZ(pos.y);
    return ang;
}

void Pillar::SetGlobalAlpha(float a)
{
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;
    galpha = a * 255.0f;
}
