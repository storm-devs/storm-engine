//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Blots
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "blots.h"

#include "core.h"

#include "entity.h"
#include "shared/messages.h"

CREATE_CLASS(Blots)

#define BLOTS_RADIUS 0.6f

//============================================================================================

CVECTOR Blots::clipTriangles[3 * 32];
int32_t Blots::numClipTriangles;
CVECTOR Blots::dir, Blots::normal;

// ============================================================================================
// Construction, destruction
// ============================================================================================

Blots::Blots()
    : model(0), vrt{}
{
    for (int32_t i = 0; i < BLOTS_MAX; i++)
        blot[i].isUsed = 0;
    rs = nullptr;
    textureID = -1;
    useVrt = 0;
    blotsInfo = nullptr;
    pCharAttributeRoot = nullptr;
    updateBlot = 0;
}

Blots::~Blots()
{
    if (rs && textureID >= 0)
        rs->TextureRelease(textureID);
}

// Initialization
bool Blots::Init()
{
    // GUARD(Blots::Init())
    // DX9 render
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    // Layers
    ////core.LayerCreate("realize", true, false);
    // core.SetLayerType(realize, layer_type_t::realize);
    // core.AddToLayer(realize, GetId(), 1000);
    textureID = rs->TextureCreate("blot.tga");
    return true;
    // UNGUARD
}

// Messages
uint64_t Blots::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_BLOTS_SETMODEL:
        model = message.EntityID();
        pCharAttributeRoot = message.AttributePointer();
        if (pCharAttributeRoot)
        {
            blotsInfo = pCharAttributeRoot->CreateSubAClass(pCharAttributeRoot, "ship.blots");
            char buf[32];
            sprintf_s(buf, "%i", BLOTS_MAX);
            blotsInfo->SetValue(buf);
            for (int32_t i = 0; i < BLOTS_MAX; i++)
                LoadBlot(i);
        }
        break;
    case MSG_BLOTS_HIT:
        Hit(message);
        break;
    }
    return 0;
}

void Blots::Hit(MESSAGE &message)
{
    // Model of a ship
    auto *m = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!m)
        return;
    // Position
    CVECTOR pos;
    pos.x = message.Float();
    pos.y = message.Float();
    pos.z = message.Float();
    // Looking for a free blot and proximity to others
    CVECTOR lpos;
    m->mtx.MulToInv(pos, lpos);
    int32_t i, j = -1;
    for (i = 0; i < BLOTS_MAX; i++)
    {
        if (blot[i].isUsed)
        {
            if (~(lpos - blot[i].pos) < 3.0f * 3.0f * BLOTS_RADIUS * BLOTS_RADIUS)
                return;
        }
        else
        {
            if (j < 0)
                j = i;
        }
    }
    if (j < 0)
        return;
    i = j;
    // Fall direction of the cannonball
    dir.x = message.Float();
    dir.y = message.Float();
    dir.z = message.Float();
    CVECTOR ldir;
    m->mtx.MulToInvNorm(dir, ldir);
    AddBlot(i, rand(), lpos, ldir, 0.0f);
}

// Add blot
void Blots::AddBlot(int32_t i, int32_t rnd, const CVECTOR &lpos, const CVECTOR &dir, float time)
{
    // Model of a ship
    auto *m = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!m)
        return;
    blot[i].isUsed = false;
    auto pos = m->mtx * CVECTOR(lpos);
    this->dir = m->mtx * CVECTOR(dir) - m->mtx.Pos();
    // bounding box
    static PLANE p[6];
    p[0].Nx = 0.0f;
    p[0].Ny = 1.0f;
    p[0].Nz = 0.0f;
    p[0].D = (pos.y + BLOTS_RADIUS);
    p[1].Nx = 0.0f;
    p[1].Ny = -1.0f;
    p[1].Nz = 0.0f;
    p[1].D = -(pos.y - BLOTS_RADIUS);
    p[2].Nx = 0.0f;
    p[2].Ny = 0.0f;
    p[2].Nz = 1.0f;
    p[2].D = (pos.z + BLOTS_RADIUS);
    p[3].Nx = 0.0f;
    p[3].Ny = 0.0f;
    p[3].Nz = -1.0f;
    p[3].D = -(pos.z - BLOTS_RADIUS);
    p[4].Nx = 1.0f;
    p[4].Ny = 0.0f;
    p[4].Nz = 0.0f;
    p[4].D = (pos.x + BLOTS_RADIUS);
    p[5].Nx = -1.0f;
    p[5].Ny = 0.0f;
    p[5].Nz = 0.0f;
    p[5].D = -(pos.x - BLOTS_RADIUS);
    // Cut out the triangles in the box
    numClipTriangles = 0;
    normal = -0.1f * dir;
    // don't collide with the patch and masts
    auto *root = m->GetNode(0);
    SetNodesCollision(root, true);
    m->Clip(p, 6, pos, BLOTS_RADIUS, AddPolygon);
    SetNodesCollision(root, false);
    if (numClipTriangles <= 0)
        return;
    // transformation matrix to the local coordinate system of the hole
    CMatrix uvmtx;
    if (!uvmtx.BuildViewMatrix(pos, pos + normal * 1.0f, CVECTOR(0.0f, 1.0f, 0.0f)))
    {
        if (!uvmtx.BuildViewMatrix(pos, pos + normal * 1.0f, CVECTOR(0.0f, 0.0f, 1.0f)))
        {
            if (!uvmtx.BuildViewMatrix(pos, pos + normal * 1.0f, CVECTOR(1.0f, 0.0f, 0.0f)))
                return;
        }
    }
    // Blot information
    blot[i].isUsed = true;
    blot[i].lastAlpha = 0xff;
    blot[i].numTrgs = static_cast<uint16_t>(numClipTriangles);
    blot[i].liveTime = time;
    blot[i].pos = lpos;
    blot[i].dir = dir;
    blot[i].rnd = rnd;
    blot[i].startIndex = useVrt;
    useVrt += numClipTriangles * 3;
    Assert(useVrt < sizeof(vrt) / sizeof(Vertex));
    // Convert triangles to the local coordinate system of the ship
    auto mtx(m->mtx);
    mtx.Transposition();
    auto *v = vrt + blot[i].startIndex;
    numClipTriangles *= 3;
    auto baseU = 0.0f;
    auto baseV = 0.0f;
    if (rnd & 1)
        baseU += 0.5f;
    if (rnd & 2)
        baseV += 0.5f;
    for (int32_t n = 0; n < numClipTriangles; n++)
    {
        v[n].pos = mtx * clipTriangles[n];
        v[n].c = 0xffffffff;
        auto uv = uvmtx * clipTriangles[n];
        uv.x = (0.5f + uv.x * 0.5f / BLOTS_RADIUS);
        uv.y = (0.5f + uv.y * 0.5f / BLOTS_RADIUS);
        if (uv.x < 0.0f)
            uv.x = 0.0f;
        if (uv.x > 1.0f)
            uv.x = 1.0f;
        if (uv.y < 0.0f)
            uv.y = 0.0f;
        if (uv.y > 1.0f)
            uv.y = 1.0f;
        if (rnd & 4)
            uv.x = 1.0f - uv.x;
        if (rnd & 8)
            uv.y = 1.0f - uv.y;
        v[n].u = baseU + uv.x * 0.5f;
        v[n].v = baseV + uv.y * 0.5f;
    }
    // Writing the state
    SaveBlot(i);
}

void Blots::SetNodesCollision(NODE *n, bool isSet)
{
    if (!n)
        return;
    if (isSet)
    {
        n->flags &= 0x00ffffff;
        n->flags |= n->flags << 24;
        const auto *const name = n->GetName();
        if (name && name[0])
        {
            if (name[0] == 'r' || name[0] == 'R')
            {
                if (name[1] == 'e' || name[1] == 'E')
                    if (name[2] == 'y' || name[2] == 'Y')
                        n->flags &= ~NODE::CLIP_ENABLE;
            }
            else if (name[0] == 'm' || name[0] == 'M')
            {
                if (name[1] == 'a' || name[1] == 'A')
                    if (name[2] == 's' || name[2] == 'S')
                        if (name[3] == 't' || name[3] == 'T')
                            n->flags &= ~NODE::CLIP_ENABLE;
            }
            else if (name[0] == 'p' || name[0] == 'P')
            {
                if (name[1] == 'a' || name[1] == 'A')
                    if (name[2] == 't' || name[2] == 'T')
                        if (name[3] == 'h' || name[3] == 'H')
                            n->flags &= ~NODE::CLIP_ENABLE;
            }
            else if (name[0] == 's' || name[0] == 'S')
            {
                if (name[1] == 'h' || name[1] == 'H')
                    if (name[2] == 'a' || name[2] == 'A')
                        if (name[3] == 't' || name[3] == 'T')
                            if (name[4] == 't' || name[4] == 'T')
                                if (name[5] == 'e' || name[5] == 'E')
                                    if (name[6] == 'r' || name[6] == 'R')
                                        n->flags &= ~NODE::CLIP_ENABLE;
            }
        }
    }
    else
    {
        n->flags &= 0xffffff00;
        n->flags |= n->flags >> 24;
        n->flags &= 0x00ffffff;
    }
    for (int32_t i = 0; i < n->nnext; i++)
        SetNodesCollision(n->next[i], isSet);
}

// Save blot parameters
void Blots::SaveBlot(int32_t i)
{
    if (!blotsInfo)
        return;
    // Attribute name
    char name[16];
    sprintf_s(name, "b%.3i", i);
    if (blot[i].isUsed)
    {
        auto *blt = blotsInfo->CreateSubAClass(blotsInfo, name);
        blt->SetAttributeUseDword("rnd", blot[i].rnd);
        blt->SetAttributeUseFloat("x", blot[i].pos.x);
        blt->SetAttributeUseFloat("y", blot[i].pos.y);
        blt->SetAttributeUseFloat("z", blot[i].pos.z);
        blt->SetAttributeUseFloat("vx", blot[i].dir.x);
        blt->SetAttributeUseFloat("vy", blot[i].dir.y);
        blt->SetAttributeUseFloat("vz", blot[i].dir.z);
        blt->SetAttributeUseFloat("time", blot[i].liveTime);
    }
    else
    {
        blotsInfo->DeleteAttributeClassX(blotsInfo->FindAClass(blotsInfo, name));
    }
}

// Load blot parameters
void Blots::LoadBlot(int32_t i)
{
    if (!blotsInfo)
        return;
    // Attribute name
    char name[16];
    sprintf_s(name, "b%.3i", i);
    auto *blt = blotsInfo->FindAClass(blotsInfo, name);
    if (blt)
    {
        if (!blt->GetAttribute("rnd"))
            return;
        if (!blt->GetAttribute("x"))
            return;
        if (!blt->GetAttribute("y"))
            return;
        if (!blt->GetAttribute("z"))
            return;
        if (!blt->GetAttribute("vx"))
            return;
        if (!blt->GetAttribute("vy"))
            return;
        if (!blt->GetAttribute("vz"))
            return;
        if (!blt->GetAttribute("time"))
            return;
        const int32_t rnd = blt->GetAttributeAsDword("rnd");
        const auto x = blt->GetAttributeAsFloat("x");
        const auto y = blt->GetAttributeAsFloat("y");
        const auto z = blt->GetAttributeAsFloat("z");
        const auto vx = blt->GetAttributeAsFloat("vx");
        const auto vy = blt->GetAttributeAsFloat("vy");
        const auto vz = blt->GetAttributeAsFloat("vz");
        const auto time = blt->GetAttributeAsFloat("time");
        AddBlot(i, rnd, CVECTOR(x, y, z), CVECTOR(vx, vy, vz), time);
    }
}

// Update
void Blots::Realize(uint32_t delta_time)
{
    // Updating the state
    blotsInfo = pCharAttributeRoot->FindAClass(pCharAttributeRoot, "ship.blots");
    updateBlot++;
    if (updateBlot >= BLOTS_MAX)
        updateBlot = 0;
    SaveBlot(updateBlot);
    // Model of a ship
    auto *m = static_cast<MODEL *>(core.GetEntityPointer(model));
    if (!m)
        return;
    // Distance from camera
    CVECTOR pos, ang;
    rs->GetCamera(pos, ang, ang.x);
    auto dist = ~(pos - m->mtx.Pos());
    if (dist >= BLOTS_DIST * BLOTS_DIST)
        return;
    // Transparency according to the distance to the ship
    dist = (sqrtf(dist / (BLOTS_DIST * BLOTS_DIST)) - 0.5f) / 0.5f;
    if (dist <= 0.0f)
        dist = 0.0f;
    dist = (1.0f - dist) * 255.0f;
    auto color = static_cast<int32_t>(dist);
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, (color << 24) | (color << 16) | (color << 8) | color);
    // Settings
    rs->SetTransform(D3DTS_WORLD, m->mtx);
    rs->TextureSet(0, textureID);
    // Draw all the added blots
    for (int32_t i = 0; i < BLOTS_MAX; i++)
    {
        // Skip unused
        if (!blot[i].isUsed)
            continue;
        // Lifetime

        // !!!
        // blot[i].liveTime += delta_time*0.001f;

        if (blot[i].liveTime >= BLOTS_TIME)
        {
            blot[i].isUsed = false;
            const auto startIndex = blot[i].startIndex;
            const int32_t numDelVerts = blot[i].numTrgs * 3;

            // -----------------------------------------------
            // !!! begin Checks
            blot[i].startIndex = -10000;
            blot[i].numTrgs = 0;
            static Vertex vr[3 * BLOTS_NTRGS * BLOTS_MAX];
            for (int32_t n = 0; n < BLOTS_MAX; n++)
            {
                if (!blot[n].isUsed)
                    continue;
                auto *const v1 = vr + n * BLOTS_NTRGS * 3;
                auto *const v2 = vrt + blot[n].startIndex;
                for (int32_t v = 0; v < blot[n].numTrgs * 3; v++)
                    v1[v] = v2[v];
            }
            // !!! end Checks
            // -----------------------------------------------

            // Remove triangles from the array
            int32_t j;
            for (j = 0; j < BLOTS_MAX; j++)
            {
                if (!blot[j].isUsed)
                    continue;
                if (blot[j].startIndex > startIndex)
                    blot[j].startIndex -= numDelVerts;
            }
            j = useVrt - (startIndex + numDelVerts);
            if (j > 0)
                memcpy(vrt + startIndex, vrt + startIndex + numDelVerts, j * sizeof(Vertex));
            useVrt -= numDelVerts;
            Assert(useVrt >= 0);

            // -----------------------------------------------
            // !!! begin Checks
            Assert(j % 3 == 0);
            int32_t nnn = 0;
            for (j = 0; j < BLOTS_MAX; j++)
            {
                if (!blot[j].isUsed)
                    continue;
                nnn += blot[j].numTrgs * 3;
                Assert(blot[j].startIndex + blot[j].numTrgs * 3 <= useVrt);
            }
            Assert(nnn == useVrt);
            for (int32_t n = 0; n < BLOTS_MAX; n++)
            {
                if (!blot[n].isUsed)
                    continue;
                auto *const v1 = vr + n * BLOTS_NTRGS * 3;
                auto *const v2 = vrt + blot[n].startIndex;
                for (int32_t v = 0; v < blot[n].numTrgs * 3; v++)
                {
                    Assert(v1[v].pos.x == v2[v].pos.x);
                    Assert(v1[v].pos.y == v2[v].pos.y);
                    Assert(v1[v].pos.z == v2[v].pos.z);
                    Assert(v1[v].c == v2[v].c);
                    Assert(v1[v].u == v2[v].u);
                    Assert(v1[v].v == v2[v].v);
                }
            }
            // !!! end Checks
            // -----------------------------------------------

            continue;
        }
        // Transparency over time
        auto k = blot[i].liveTime / BLOTS_TIME;
        k = (k - 0.5f) / 0.5f;
        if (k < 0.0f)
            k = 0.0f;
        if (k > 1.0f)
            k = 1.0f;
        color = static_cast<int32_t>((1.0f - k) * 255.0f);
        if (color != blot[i].lastAlpha)
        {
            // Update the vertices
            // Colour
            color = 0xff000000 | (color << 16) | (color << 8) | color;
            // amount
            const int32_t numVrt = blot[i].numTrgs * 3;
            // Array
            auto *const v = vrt + blot[i].startIndex;
            for (int32_t j = 0; j < numVrt; j++)
                v[j].c = color;
        }
    }
    // Draw
    if (useVrt > 3)
        rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, useVrt / 3, vrt,
                            sizeof(Vertex), "Blot");
}

bool Blots::AddPolygon(const CVECTOR *v, int32_t nv)
{
    if (numClipTriangles >= BLOTS_NTRGS)
        return false;
    if (nv < 3)
        return true;
    // Normal
    const auto norm = (v[0] - v[1]) ^ (v[0] - v[2]);
    if ((norm | dir) >= 0.0f)
        return true;
    normal += 100.0f * norm;
    // Add
    for (int32_t i = 2; i < nv; i++)
    {
        clipTriangles[numClipTriangles * 3 + 0] = v[0];
        clipTriangles[numClipTriangles * 3 + 1] = v[i - 1];
        clipTriangles[numClipTriangles * 3 + 2] = v[i];
        numClipTriangles++;
        if (numClipTriangles >= BLOTS_NTRGS)
            return false;
    }
    return true;
}
