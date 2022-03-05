/******************************************************************************
File:  shadow.cpp

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
dynamic shadow cpp file
******************************************************************************/
#include "shadow.h"
#include "shared/messages.h"

CREATE_CLASS(Shadow)

static uint32_t HEAD_DENSITY = 0xFF606060;
static uint32_t DENSITY = 0xFF606040;
static const float nearBlend = 8.0f;
static const float farBlend = 16.0f;

static const int32_t vbuff_size = 1024;
static int32_t refcount = 0;
#define TEXTURE_SIZE 128
IDirect3DTexture9 *shTex = nullptr, *blurTex = nullptr;
IDirect3DVertexBuffer9 *vbuff;

Shadow::Shadow()
{
    shading = 1.0f;
    blendValue = 0xFFFFFFFF;
}

Shadow::~Shadow()
{
    refcount--;
    if (refcount == 0)
    {
        rs->Release(vbuff);
        rs->Release(shTex);
        rs->Release(blurTex);
    }
}

bool Shadow::Init()
{
    // GUARD(Shadow::SHADOW())

    col = static_cast<COLLIDE *>(core.GetService("coll"));
    if (col == nullptr)
        throw std::runtime_error("No service: COLLIDE");

    EntityManager::AddToLayer(REALIZE, GetId(), 900);

    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");

    if (refcount == 0)
    {
        rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &shTex);
        if (shTex == nullptr)
            rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
                              &shTex);

        rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
                          &blurTex);
        if (blurTex == nullptr)
            rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT,
                              &blurTex);

        rs->CreateVertexBuffer(sizeof(SHADOW_VERTEX) * (vbuff_size + 128), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                               SHADOW_FVF, D3DPOOL_DEFAULT, &vbuff);
    }
    refcount++;

    // UNGUARD
    return true;
}

CMatrix trans;
float perspective, atten_start, atten_end;
Shadow::SHADOW_VERTEX *shadvert;
int32_t tot_verts;
CVECTOR lightPos, ObjPos, objPos;
CVECTOR camPos;

bool AddPoly(const CVECTOR *vr, int32_t nverts)
{
    const auto norm = !((vr[1] - vr[0]) ^ (vr[2] - vr[0]));

    const auto d = (norm | vr[0]);
    const auto dc = (norm | camPos) - d;
    const auto dl = (norm | lightPos) - d;
    if (dc * dl < 0.0f)
        return true;

    const auto d0 = (norm | objPos) - d;
    if (d0 * dl < 0.0f)
        return true;

    if (tot_verts + (nverts - 2) * 3 > vbuff_size)
        return false;
    int32_t v;
    for (v = 0; v < 3; v++)
    {
        shadvert[tot_verts].pos = vr[v];
        const auto z = (trans.m[0][2] * vr[v].x + trans.m[1][2] * vr[v].y + trans.m[2][2] * vr[v].z + trans.m[3][2]);
        const auto rhw = perspective * 0.5f / z;
        shadvert[tot_verts].tu =
            rhw * (trans.m[0][0] * vr[v].x + trans.m[1][0] * vr[v].y + trans.m[2][0] * vr[v].z + trans.m[3][0]) + 0.5f;
        shadvert[tot_verts].tv =
            -rhw * (trans.m[0][1] * vr[v].x + trans.m[1][1] * vr[v].y + trans.m[2][1] * vr[v].z + trans.m[3][1]) + 0.5f;
        tot_verts++;
    }
    const auto start = tot_verts - 3;
    for (; v < nverts; v++)
    {
        shadvert[tot_verts + 0] = shadvert[start];
        shadvert[tot_verts + 1] = shadvert[tot_verts - 1];
        tot_verts += 2;
        shadvert[tot_verts].pos = vr[v];
        const auto z = (trans.m[0][2] * vr[v].x + trans.m[1][2] * vr[v].y + trans.m[2][2] * vr[v].z + trans.m[3][2]);
        const auto rhw = perspective * 0.5f / z;
        shadvert[tot_verts].tu =
            rhw * (trans.m[0][0] * vr[v].x + trans.m[1][0] * vr[v].y + trans.m[2][0] * vr[v].z + trans.m[3][0]) + 0.5f;
        shadvert[tot_verts].tv =
            -rhw * (trans.m[0][1] * vr[v].x + trans.m[1][1] * vr[v].y + trans.m[2][1] * vr[v].z + trans.m[3][1]) + 0.5f;
        tot_verts++;
    }
    return true;
}

//------------------------------------------------------------------------------------
// realize
//------------------------------------------------------------------------------------
void Shadow::Realize(uint32_t Delta_Time)
{
    auto *obj = static_cast<MODEL *>(EntityManager::GetEntityPointer(entity));
    if (!obj)
        return;

    auto *pV = core.Event("EWhr_GetShadowDensity");
    HEAD_DENSITY = ((VDATA *)pV->GetArrayElement(0))->GetInt();
    DENSITY = ((VDATA *)pV->GetArrayElement(1))->GetInt();

    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);

    pV = core.Event("EWhr_GetFogDensity");
    auto fogDensity = pV->GetFloat();

    // MODEL *obj = (MODEL*)EntityManager::GetEntityPointer(entity);
    auto *node = obj->GetNode(0);
    GEOS::INFO gi;
    node->geo->GetInfo(gi);
    objPos = obj->mtx.Pos();
    ObjPos = objPos;
    objPos.y += gi.radius;
    auto headPos = objPos;
    headPos.y += gi.radius;

    D3DLIGHT9 dLight;
    BOOL bOk = false;
    rs->GetLightEnable(0, &bOk);
    if (bOk)
        rs->GetLight(0, &dLight);
    else
        return;

    dLight.Position.y = lightPos.y;

    auto light_pos = !CVECTOR(dLight.Direction.x, dLight.Direction.y, dLight.Direction.z);
    if (light_pos.y > -0.6f)
        light_pos.y = -0.6f;
    light_pos = -1000.0f * (!light_pos);

    light_pos = objPos - !(objPos - light_pos) * 100.0f;

    CVECTOR dir = !(objPos - light_pos);
    lightPos = light_pos;

    // check visibility of shadow
    CMatrix visView, visPoj;
    rs->GetTransform(D3DTS_VIEW, visView);
    camPos.x =
        -visView.m[3][0] * visView.m[0][0] - visView.m[3][1] * visView.m[0][1] - visView.m[3][2] * visView.m[0][2];
    camPos.y =
        -visView.m[3][0] * visView.m[1][0] - visView.m[3][1] * visView.m[1][1] - visView.m[3][2] * visView.m[1][2];
    camPos.z =
        -visView.m[3][0] * visView.m[2][0] - visView.m[3][1] * visView.m[2][1] - visView.m[3][2] * visView.m[2][2];

    rs->GetTransform(D3DTS_PROJECTION, visPoj);
    FindPlanes(visView, visPoj);

    const auto its = EntityManager::GetEntityIdIterators(SHADOW);

    CVECTOR hdest = headPos + !(headPos - light_pos) * 100.0f;
    float ray = col->Trace(its, headPos, hdest, nullptr, 0);
    CVECTOR cen;
    float radius;
    if (ray <= 1.0f)
    {
        if (ray < 4.0f / 100.0f)
            ray = 4.0f / 100.0f;
        if (ray > 10.0f / 100.0f)
            ray = 10.0f / 100.0f;
        cen = headPos + 0.5f * ray * (hdest - headPos);
        radius = ray * 50.0f;
    }
    else
    {
        cen = headPos + 4.0f * !(hdest - headPos);
        radius = 8.0f;
    }

    int32_t p;
    for (p = 0; p < 4; p++)
    {
        float dist = cen.x * planes[p].Nx + cen.y * planes[p].Ny + cen.z * planes[p].Nz - planes[p].D;
        if (dist > radius)
            break;
    }
    if (p < 4)
    {
        return;
    }

    float minVal = 0.0f;
    for (int32_t it = 0; it < 10; it++)
    {
        CVECTOR ps = ObjPos;
        ps.y += gi.radius * 0.111f * static_cast<float>(it);
        if (col->Trace(its, ps, lightPos, nullptr, 0) > 1.0f)
            minVal += 0.1f;
    }

    float dtime = Delta_Time * 0.001f;
    if (minVal <= 0.5f)
        shading -= dtime;
    else
        shading += dtime;

    shading = std::max(0.2f, std::max(minVal, std::min(shading, 1.0f)));
    shading *= (blendValue >> 24) / 255.0f;

    // if(GetAsyncKeyState(0xc0)<0)
    {
        float dist = sqrtf(~(cen - camPos));
        if (dist > farBlend) // too far
        {
            return;
        }
        if (dist > nearBlend) // blend
            shading *= 1.0f - (dist - nearBlend) / (farBlend - nearBlend);
    }

    // view matrix-------------------------------------
    CMatrix lightmtx;
    lightmtx.BuildViewMatrix(CVECTOR(0.0f, 0.0f, 0.0f), dir, CVECTOR(0.0f, 1.0f, 0.0f));
    lightmtx.SetInversePosition(light_pos.x, light_pos.y, light_pos.z);
    // projection matrix-------------------------------------
    CMatrix proj;
    float tanfov = sqrtf(~(objPos - light_pos)) / (1.2f * gi.radius);
    float fov = 2.0f * atanf(1.0f / tanfov);

    proj.BuildProjectionMatrix(fov, TEXTURE_SIZE, TEXTURE_SIZE, 0.1f, 1000.0f);

    // general params-------------------------------------
    trans = lightmtx;
    perspective = std::max(proj.m[0][0], proj.m[1][1]);
    atten_start = 1200.0f;
    atten_end = 1300.0f;
    //---------------------------------------------------------------
    // draw PROJECTOR
    FindPlanes(lightmtx, proj);

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    // draw object into shadow texture
    CMatrix prev_view, prev_proj;
    rs->GetTransform(D3DTS_VIEW, prev_view);
    rs->GetTransform(D3DTS_PROJECTION, prev_proj);

    IDirect3DSurface9 *backbuff, *zbuff;
    rs->GetRenderTarget(&backbuff);
    rs->GetDepthStencilSurface(&zbuff);
    rs->EndScene();

    rs->SetTransform(D3DTS_PROJECTION, proj);
    rs->SetTransform(D3DTS_VIEW, lightmtx);

    IDirect3DSurface9 *texsurf;
    shTex->GetSurfaceLevel(0, &texsurf);
    rs->SetRenderTarget(texsurf, nullptr);
    rs->Release(texsurf);

    rs->Clear(0L, nullptr, D3DCLEAR_TARGET, 0, 0.0f, 0L);
    rs->BeginScene();

    rs->SetRenderState(D3DRS_TEXTUREFACTOR, DENSITY);
    char tech[256];
    strcpy_s(tech, node->GetTechnique());
    node->SetTechnique("shadow_model");
    node->flags &= ~NODE::VISIBLE_TREE;

    rs->SetRenderState(D3DRS_ZENABLE, FALSE);
    obj->ProcessStage(Stage::realize, 0);
    rs->SetRenderState(D3DRS_ZENABLE, TRUE);

    node->flags |= NODE::VISIBLE_TREE;
    node->SetTechnique(tech);

    rs->EndScene();

    Smooth();

    rs->SetRenderTarget(backbuff, zbuff);
    rs->Release(backbuff);
    rs->Release(zbuff);
    rs->SetTransform(D3DTS_VIEW, prev_view);
    rs->SetTransform(D3DTS_PROJECTION, prev_proj);
    rs->BeginScene();

    //---------------------------------------------------------------
    //---------------------------------------------------------------

    // create last plane
    lightmtx.Transposition3X3();
    CVECTOR pdir = -!CVECTOR(dir.x, 0.0f, dir.z);
    planes[4].Nx = pdir.x;
    planes[4].Ny = pdir.y;
    planes[4].Nz = pdir.z;
    planes[4].D = pdir | (objPos + 0.5f * pdir);
    planes[5].Nx = -pdir.x;
    planes[5].Ny = -pdir.y;
    planes[5].Nz = -pdir.z;
    planes[5].D = -pdir | (objPos - 3.5f * pdir);

    rs->SetTexture(0, blurTex);
    rs->SetTransform(D3DTS_WORLD, CMatrix());

    float dist = 3.0f * sqrtf(~(cen - camPos));
    shading *= powf(2.71f, -fogDensity * dist);

    int32_t shade = static_cast<int32_t>(fabsf(shading * 255.0f));

    rs->SetRenderState(D3DRS_TEXTUREFACTOR, (shade << 16) | (shade << 8) | (shade << 0));
    rs->SetFVF(SHADOW_FVF);
    // rs->SetIndices(0,0);
    rs->SetStreamSource(0, vbuff, sizeof(SHADOW_VERTEX));

    tot_verts = 0;
    rs->VBLock(vbuff, 0, 0, (uint8_t **)&shadvert, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
    col->Clip(its, &planes[0], 5, cen, radius, AddPoly, &entity, 1);

    rs->VBUnlock(vbuff);

    if (tot_verts >= 3 && rs->TechniqueExecuteStart("shadow_draw"))
        do
        {
            rs->DrawPrimitive(D3DPT_TRIANGLELIST, 0, tot_verts / 3);
        } while (rs->TechniqueExecuteNext());
    rs->SetViewport(&vp);
}

void Shadow::FindPlanes(const CMatrix &view, const CMatrix &proj)
{
    CVECTOR v[4];
    // left
    v[0].x = proj.m[0][0];
    v[0].y = 0.0f;
    v[0].z = 1.0f;
    // right
    v[1].x = -proj.m[0][0];
    v[1].y = 0.0f;
    v[1].z = 1.0f;
    // top
    v[2].x = 0.0f;
    v[2].y = -proj.m[1][1];
    v[2].z = 1.0f;
    // bottom
    v[3].x = 0.0f;
    v[3].y = proj.m[1][1];
    v[3].z = 1.0f;
    v[0] = !v[0];
    v[1] = !v[1];
    v[2] = !v[2];
    v[3] = !v[3];

    CVECTOR pos;

    pos.x = -view.m[3][0] * view.m[0][0] - view.m[3][1] * view.m[0][1] - view.m[3][2] * view.m[0][2];
    pos.y = -view.m[3][0] * view.m[1][0] - view.m[3][1] * view.m[1][1] - view.m[3][2] * view.m[1][2];
    pos.z = -view.m[3][0] * view.m[2][0] - view.m[3][1] * view.m[2][1] - view.m[3][2] * view.m[2][2];

    planes[0].Nx = v[0].x * view.m[0][0] + v[0].y * view.m[0][1] + v[0].z * view.m[0][2];
    planes[0].Ny = v[0].x * view.m[1][0] + v[0].y * view.m[1][1] + v[0].z * view.m[1][2];
    planes[0].Nz = v[0].x * view.m[2][0] + v[0].y * view.m[2][1] + v[0].z * view.m[2][2];

    planes[1].Nx = v[1].x * view.m[0][0] + v[1].y * view.m[0][1] + v[1].z * view.m[0][2];
    planes[1].Ny = v[1].x * view.m[1][0] + v[1].y * view.m[1][1] + v[1].z * view.m[1][2];
    planes[1].Nz = v[1].x * view.m[2][0] + v[1].y * view.m[2][1] + v[1].z * view.m[2][2];

    planes[2].Nx = v[2].x * view.m[0][0] + v[2].y * view.m[0][1] + v[2].z * view.m[0][2];
    planes[2].Ny = v[2].x * view.m[1][0] + v[2].y * view.m[1][1] + v[2].z * view.m[1][2];
    planes[2].Nz = v[2].x * view.m[2][0] + v[2].y * view.m[2][1] + v[2].z * view.m[2][2];

    planes[3].Nx = v[3].x * view.m[0][0] + v[3].y * view.m[0][1] + v[3].z * view.m[0][2];
    planes[3].Ny = v[3].x * view.m[1][0] + v[3].y * view.m[1][1] + v[3].z * view.m[1][2];
    planes[3].Nz = v[3].x * view.m[2][0] + v[3].y * view.m[2][1] + v[3].z * view.m[2][2];

    planes[0].Nx = -planes[0].Nx;
    planes[0].Ny = -planes[0].Ny;
    planes[0].Nz = -planes[0].Nz;
    planes[1].Nx = -planes[1].Nx;
    planes[1].Ny = -planes[1].Ny;
    planes[1].Nz = -planes[1].Nz;
    planes[2].Nx = -planes[2].Nx;
    planes[2].Ny = -planes[2].Ny;
    planes[2].Nz = -planes[2].Nz;
    planes[3].Nx = -planes[3].Nx;
    planes[3].Ny = -planes[3].Ny;
    planes[3].Nz = -planes[3].Nz;

    planes[0].D = (pos.x * planes[0].Nx + pos.y * planes[0].Ny + pos.z * planes[0].Nz);
    planes[1].D = (pos.x * planes[1].Nx + pos.y * planes[1].Ny + pos.z * planes[1].Nz);
    planes[2].D = (pos.x * planes[2].Nx + pos.y * planes[2].Ny + pos.z * planes[2].Nz);
    planes[3].D = (pos.x * planes[3].Nx + pos.y * planes[3].Ny + pos.z * planes[3].Nz);
}

void Shadow::Smooth()
{
    struct SMOOTHVRT
    {
        float sx, sy, sz, rhw;
        int32_t diffuse;
        float tu, tv;
    };

    float dt = 0.0f;
    SMOOTHVRT vrt[4];
    vrt[0].sx = 0.0f;
    vrt[0].sy = 0.0f;
    vrt[0].sz = 0.5f;
    vrt[0].rhw = 0.5f;
    vrt[0].tu = 0.0f;
    vrt[0].tv = 0.0f;
    vrt[1].sx = 0.0f;
    vrt[1].sy = TEXTURE_SIZE - 1.0f;
    vrt[1].sz = 0.5f;
    vrt[1].rhw = 0.5f;
    vrt[1].tu = 0.0f;
    vrt[1].tv = 1.0f;
    vrt[2].sx = TEXTURE_SIZE - 1.0f;
    vrt[2].sy = TEXTURE_SIZE - 1.0f;
    vrt[2].sz = 0.5f;
    vrt[2].rhw = 0.5f;
    vrt[2].tu = 1.0f;
    vrt[2].tv = 1.0f;
    vrt[3].sx = TEXTURE_SIZE - 1.0f;
    vrt[3].sy = 0.0f;
    vrt[3].sz = 0.5f;
    vrt[3].rhw = 0.5f;
    vrt[3].tu = 1.0f;
    vrt[3].tv = 0.0f;
    vrt[0].diffuse = vrt[3].diffuse = HEAD_DENSITY;
    vrt[1].diffuse = vrt[2].diffuse = 0xFFFFFFFF;

    IDirect3DSurface9 *texsurf;
    blurTex->GetSurfaceLevel(0, &texsurf);
    rs->SetRenderTarget(texsurf, nullptr);
    rs->Release(texsurf);

    rs->Clear(0L, nullptr, D3DCLEAR_TARGET, 0, 0.0f, 0L);
    rs->BeginScene();

    rs->SetTexture(0, shTex);

    if (rs->TechniqueExecuteStart("shadow_smooth"))
        do
        {
            static const int32_t nIterations = 3;

            for (int32_t u = 0; u < nIterations; u++)
                for (int32_t v = 0; v < nIterations; v++)
                {
                    const float ud = 1.5f * (u / (nIterations - 1.0f) - 0.5f) / (TEXTURE_SIZE - 1.0f) * 2.0f;
                    const float vd = 1.5f * (v / (nIterations - 1.0f) - 0.5f) / (TEXTURE_SIZE - 1.0f) * 2.0f;
                    vrt[0].tu = 0.0f + ud;
                    vrt[0].tv = 0.0f + vd;
                    vrt[1].tu = 0.0f + ud;
                    vrt[1].tv = 1.0f + vd;
                    vrt[2].tu = 1.0f + ud;
                    vrt[2].tv = 1.0f + vd;
                    vrt[3].tu = 1.0f + ud;
                    vrt[3].tv = 0.0f + vd;
                    const int32_t col = static_cast<int32_t>(1.0f / (nIterations * nIterations) * 255.0f);
                    rs->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000 | (col << 16) | (col << 8) | (col << 0));
                    rs->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,
                                        D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_TEXTUREFORMAT2, 2, &vrt,
                                        sizeof(SMOOTHVRT));
                }
        } while (rs->TechniqueExecuteNext());

    rs->EndScene();
}

uint64_t Shadow::ProcessMessage(MESSAGE &message)
{
    const int32_t code = message.Long();
    switch (code)
    {
    case 0:
        entity = message.EntityID();
        break;

    case MSG_BLADE_ALPHA:
        blendValue = message.Long();
        break;
    }
    return 0;
}

void Shadow::LostRender()
{
    if (--refcount == 0)
    {
        rs->Release(shTex);
        rs->Release(blurTex);
        rs->Release(vbuff);
    }
}

void Shadow::RestoreRender()
{
    if (refcount++ == 0)
    {
        rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &shTex);
        if (shTex == nullptr)
            rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
                              &shTex);

        rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
                          &blurTex);
        if (blurTex == nullptr)
            rs->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT,
                              &blurTex);

        rs->CreateVertexBuffer(sizeof(SHADOW_VERTEX) * (vbuff_size + 128), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                               SHADOW_FVF, D3DPOOL_DEFAULT, &vbuff);
    }
}
