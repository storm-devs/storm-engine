//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmClouds
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_clouds.h"
#include "wdm_objects.h"
#include "defines.h"

//============================================================================================

#define WdmCloudsCloudRad 80.0f
#define WdmCloudsCloudHeight 80.0f
#define WdmCloudsSizeMin 30.0f
#define WdmCloudsSizeMax 50.0f

IDirect3DVertexDeclaration9 *WdmClouds::vertexDecl_ = nullptr;

WdmClouds::Cloud::Cloud()
{
    count = 0;
}

bool WdmClouds::Cloud::Reset(bool isFirstTime)
{
    if (count > 0)
        return false;
    // Determine the quantity
    count = static_cast<int32_t>((rand() * (0.5f / RAND_MAX) + 0.5f) * WDMCLOUDSMAX);
    if (count < 1)
        count = 1;
    // Base position
    const auto baseX = (rand() * (1.0f / RAND_MAX) - 0.5f) * wdmObjects->worldSizeX;
    const auto baseZ = (rand() * (1.0f / RAND_MAX) - 0.5f) * wdmObjects->worldSizeZ;
    // Scaling factor
    auto scaleX = (rand() * (0.8f / RAND_MAX) + 0.2f);
    auto scaleZ = (rand() * (0.8f / RAND_MAX) + 0.2f);
    const auto nrm = 1.0f / sqrtf(scaleX * scaleX + scaleZ * scaleZ);
    scaleX *= nrm;
    scaleZ *= nrm;
    // Determining starting positions
    for (int32_t i = 0; i < count; i++)
    {
        auto ang = rand() * (2.0f * PI / RAND_MAX);
        auto rad = rand() * (WdmCloudsCloudRad / RAND_MAX);
        auto &cld = cloud[i];
        cld.pos.x = baseX + rad * scaleX * sinf(ang);
        cld.pos.y = WdmCloudsCloudHeight;
        cld.pos.z = baseZ + rad * scaleZ * cosf(ang);
        cld.angle = rand() * (2.0f * PI / RAND_MAX);
        const uint8_t size = rand() & 0xff;
        cld.size = WdmCloudsSizeMin + size * ((WdmCloudsSizeMax - WdmCloudsSizeMin) / 255.0f);
        cld.alpha = 1.0f;
        const uint32_t dclr = rand() & 0xf;
        cld.color = 0x80f00000 | (dclr << 16) | size | ((rand() & 0x7f) << 24);
        cld.pict = rand() & 3;
        cld.index = 0;
        cld.aspd = (rand() * (1.0f / RAND_MAX) - 0.5f) * 0.3f;
        cld.dir = 0.0f;
        ang = rand() * (2.0f * PI / RAND_MAX);
        rad = rand() * (0.3f / RAND_MAX) + 0.01f;
        cld.rdr.x = rad * sinf(ang);
        cld.rdr.y = 0.0f;
        cld.rdr.z = rad * cosf(ang);
    }
    if (!isFirstTime)
    {
        alpha = 0.0f;
    }
    else
    {
        alpha = 1.0f;
    }
    lifeTime = 80.0f + rand() * (80.0f / RAND_MAX);
    return true;
}

// Calculations
void WdmClouds::Cloud::Update(float dltTime)
{
    if (count <= 0)
        return;
    const auto minDist = WdmCloudsCloudRad * 0.5f;
    const auto midDist = WdmCloudsCloudRad * 1.0f;
    const auto maxDist = WdmCloudsCloudRad * 1.5f;
    const auto minWorldX = -0.5f * wdmObjects->worldSizeX;
    const auto maxWorldX = 0.5f * wdmObjects->worldSizeX;
    const auto minWorldZ = -0.5f * wdmObjects->worldSizeZ;
    const auto maxWorldZ = 0.5f * wdmObjects->worldSizeZ;
    // Moving the clouds
    int32_t outsideCount = 0;
    for (int32_t i = 0; i < count; i++)
    {
        // Particle
        auto &cld = cloud[i];
        // Modifying the direction
        if (cld.index >= count)
            cld.index = 0;
        auto dir = cloud[cld.index].pos - cld.pos;        // Direction
        auto dist = sqrtf(dir.x * dir.x + dir.z * dir.z); // get the distance
        if (dist > minDist)
        {
            // Modifying the alpha channel
            if (dist < midDist)
            {
                cld.alpha += dltTime * 0.06f;
                if (cld.alpha < 0.0f)
                    cld.alpha = 0.0f;
            }
            else if (dist > maxDist)
            {
                cld.alpha -= dltTime * 0.03f;
                if (cld.alpha < 0.0f)
                    cld.alpha = 0.0f;
            }
            // getting closer
            if (dist > midDist)
            {
                const auto nrm = 1.0f / dist;
                dist = (dist - midDist) / (maxDist - midDist);
                if (dist > 1.0f)
                    dist = 1.0f;
                dir *= nrm * dist;
            }
            else
            {
                dir = 0.0f;
            }
        }
        else
        {
            // getting further
            dist = (dist - minDist) / minDist;
            if (dist < -1.0f)
                dist = -1.0f;
            dir *= dist;
            cld.alpha -= dltTime * 0.05f;
            if (cld.alpha < 0.0f)
                cld.alpha = 0.0f;
        }
        cld.dir += (dir - cld.dir + cld.rdr) * (0.5f / count); // blend regardless of the time
        cld.index++;                                           // On the next frame, the next one will act
        // Direction of the wind
        wdmObjects->GetWind(cld.pos.x, cld.pos.z, dir);
        // Rotation
        auto arot = cld.dir.z * dir.x - cld.dir.x * dir.z;
        if (arot < -1.0f)
            arot = -1.0f;
        if (arot > 1.0f)
            arot = 1.0f;
        cld.aspd += arot * dltTime * 0.3f;
        if (cld.aspd < -1.0f)
            cld.aspd = -1.0f;
        if (cld.aspd > 1.0f)
            cld.aspd = 1.0f;
        cld.angle += cld.aspd * dltTime * 0.2f;
        // Position
        dir += cld.dir * 0.5f;
        cld.pos += dir * (dltTime * 10.0f);
        if (cld.pos.x < minWorldX || cld.pos.x > maxWorldX || cld.pos.z < minWorldZ || cld.pos.z > maxWorldZ)
        {
            outsideCount++;
        }
        // Taking into account the particle in the bounding sphere
        const auto cldr = cld.size * 1.5f;
        if (i)
        {
            dir = center - cld.pos;
            const auto r = sqrtf(dir.x * dir.x + dir.z * dir.z);
            if (r + cldr < radius)
                continue;
            if (r > 1e-8f)
            {
                center = (center + cld.pos + dir * ((radius - cldr) / r)) * 0.5f;
                radius = (r + radius + cldr) * 0.5f;
            }
            else
            {
                if (radius < cldr)
                    radius = cldr;
            }
        }
        else
        {
            center = cld.pos;
            radius = cldr;
        }
    }
    // Lifetime
    if (outsideCount >= count)
        lifeTime = 0.0f;
    lifeTime -= dltTime;
    if (lifeTime > 0.0f)
    {
        alpha += dltTime * 0.1f;
        if (alpha > 1.0f)
            alpha = 1.0f;
    }
    else
    {
        alpha -= dltTime * 0.1f;
        if (alpha <= 0.0f)
        {
            alpha = 0.0f;
            count = 0;
        }
    }
}

int32_t WdmClouds::Cloud::FillRects(RS_RECT *rects, int32_t cnt, float galpha)
{
    for (int32_t i = 0; i < count; i++)
    {
        auto &c = cloud[i];
        // Calculating the alpha
        auto a = static_cast<float>(static_cast<uint8_t>(c.color >> 24));
        a *= alpha;
        a *= 0.1f + c.alpha * 0.9f;
        auto la = static_cast<int32_t>(a * galpha);
        if (la > 0xff)
            la = 0xff;
        if (la <= 0)
            continue;
        // fill in
        auto &r = rects[cnt++];
        r.vPos = c.pos;
        r.fAngle = c.angle;
        r.fSize = c.size;
        r.dwColor = (c.color & 0x00ffffff) | (la << 24);
        r.dwSubTexture = c.pict;
    }
    return cnt;
}

// Get sphere center and radius
float WdmClouds::Cloud::GetBound(CVECTOR &_center) const
{
    if (!count)
    {
        _center = 0.0f;
        return 0.0f;
    }
    _center = center;
    return radius;
}

// Run the cloud removal mechanism if there is an intersection
void WdmClouds::Cloud::Kill(const Cloud &cld)
{
    if (count && cld.count)
    {
        const auto dx = center.x - cld.center.x;
        const auto dz = center.z - cld.center.z;
        const auto d = dx * dx + dz * dz;
        const auto minDist = (radius + cld.radius) * 0.2f;
        if (d < minDist * minDist)
        {
            lifeTime = 0.0f;
        }
    }
}

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmClouds::WdmClouds()
{
    // set positions and directions to groups
    for (int32_t i = 0; i < sizeof(clouds) / sizeof(Cloud); i++)
    {
        clouds[i].Reset(true);
    }
    texture = wdmObjects->rs->TextureCreate("\\WorldMap\\clouds.tga");
    light = wdmObjects->rs->TextureCreate("\\WorldMap\\cloudslight.tga");
}

WdmClouds::~WdmClouds()
{
    if (texture >= 0)
        wdmObjects->rs->TextureRelease(texture);
    if (light >= 0)
        wdmObjects->rs->TextureRelease(light);
}

// Calculations
void WdmClouds::Update(float dltTime)
{
    // Restart killed ones
    for (int32_t i = 0; i < sizeof(clouds) / sizeof(Cloud); i++)
    {
        if (clouds[i].Reset())
            break;
    }
    // Updating positions
    for (int32_t i = 0; i < sizeof(clouds) / sizeof(Cloud); i++)
    {
        clouds[i].Update(dltTime);
    }
    // Remove strongly intersecting
    for (int32_t i = 0; i < sizeof(clouds) / sizeof(Cloud) - 1; i++)
    {
        for (auto j = i + 1; j < sizeof(clouds) / sizeof(Cloud); j++)
        {
            clouds[j].Kill(clouds[i]);
        }
    }
}

// Drawing
void WdmClouds::LRender(VDX9RENDER *rs)
{
    int32_t cnt = 0;
    // Getting camera frustum
    auto *plane = rs->GetPlanes();
    // Determine the global alpha depending on the distance to the camera
    CMatrix view;
    rs->GetTransform(D3DTS_VIEW, view);
    CVECTOR camPos;
    view.MulToInv(CVECTOR(0.0f), camPos);
    auto alpha = (camPos.y - WdmCloudsCloudHeight - 10.0f) * 1.0f / 80.0f;
    if (alpha <= 0.0f)
        return;
    if (alpha > 1.0f)
        alpha = 1.0f;
    alpha *= alpha;
    // Draw visible
    int32_t count = 0;
    for (int32_t i = 0; i < sizeof(clouds) / sizeof(Cloud); i++)
    {
        // get the sphere
        CVECTOR c;
        auto r = clouds[i].GetBound(c);
        // test for visibility
        int32_t j;
        for (j = 0; j < 4; j++)
        {
            auto &p = plane[j];
            auto dist = c.x * p.Nx + c.y * p.Ny + c.z * p.Nz - p.D;
            if (dist < -r)
                break;
        }
        if (j < 4)
            continue;
        // Add to buffer
        count = clouds[i].FillRects(rects, count, alpha);

        cnt++;
    }
    if (count > 0)
    {
        CreateVertexDeclaration(rs);
        rs->SetVertexDeclaration(vertexDecl_);

        rs->TextureSet(0, texture);
        rs->TextureSet(1, light);

        CMatrix prj;
        rs->GetTransform(D3DTS_PROJECTION, prj);
        view.Transposition();
        rs->SetVertexShaderConstantF(0, prj, 4);
        prj.matrix[0] = view.matrix[1];
        prj.matrix[1] = view.matrix[5];
        prj.matrix[2] = view.matrix[9];
        prj.matrix[3] = view.matrix[13] - WdmCloudsCloudHeight;
        prj.matrix[4] = WdmCloudsSizeMin * 1.4f;
        prj.matrix[5] = 0.0f;
        prj.matrix[6] = (WdmCloudsSizeMax - WdmCloudsSizeMin) * 1.4f;
        prj.matrix[7] = 0.5f;
        rs->SetVertexShaderConstantF(4, prj, 2);
        rs->DrawRects(rects, count, "WdmClouds", 2, 2);
    }
    // rs->Print(20, 200, "Visible clouds = %i, Visible particles = %i", cnt, count);
}

void WdmClouds::CreateVertexDeclaration(VDX9RENDER *rs)
{
    if (vertexDecl_ != nullptr)
        return;

    const D3DVERTEXELEMENT9 VertexElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    rs->CreateVertexDeclaration(VertexElements, &vertexDecl_);
}
