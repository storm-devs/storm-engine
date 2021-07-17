//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmRenderModel
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "geometry.h"

#include "WdmRenderModel.h"

#define WDM_MODEL_TECH "WdmModelDrawStd"
#define WDM_MODEL_TECHA "WdmModelDrawStdA"

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmRenderModel::WdmRenderModel()
{
    geo = nullptr;
    alpha = 1.0f;
    tech = WDM_MODEL_TECH;
    techa = WDM_MODEL_TECHA;
    center = 0.0f;
    radius = 0.0f;

    drawCircle = false;
}

WdmRenderModel::~WdmRenderModel()
{
}

bool WdmRenderModel::Load(const char *modelName)
{
    geo = wdmObjects->CreateGeometry(modelName);
    if (geo)
    {
        GEOS::INFO ginfo;
        geo->GetInfo(ginfo);
        center = CVECTOR(ginfo.boxcenter.x, ginfo.boxcenter.y, ginfo.boxcenter.z);
        radius = sqrtf(ginfo.boxsize.x * ginfo.boxsize.x + ginfo.boxsize.z * ginfo.boxsize.z) * 0.51f;
        return true;
    }
    return false;
}

void WdmRenderModel::PRender(VDX9RENDER *rs)
{
    LRender(rs);
}

void WdmRenderModel::MRender(VDX9RENDER *rs)
{
    auto m(mtx);
    m.m[0][1] = -m.m[0][1];
    m.m[1][1] = -m.m[1][1];
    m.m[2][1] = -m.m[2][1];
    m.m[3][1] = -m.m[3][1];
    rs->SetTransform(D3DTS_WORLD, m);
    Render(rs);
}

void WdmRenderModel::LRender(VDX9RENDER *rs)
{
    rs->SetTransform(D3DTS_WORLD, mtx);
    Render(rs);

    /*
    if(drawCircle)
    {
      CMatrix m(mtx);
      m.Pos() = m*center;
      wdmObjects->DrawCircle(m, radius, 0x2f0fffff);
      //wdmObjects->DrawCircle(mtx, modelRadius, 0x2fffff0f);
    }*/
}

void WdmRenderModel::SetTech(const char *t, const char *ta)
{
    if (t)
        tech = t;
    else
        tech = WDM_MODEL_TECH;
    if (ta)
        techa = ta;
    else
        techa = WDM_MODEL_TECHA;
}

long WdmRenderModel::GetTexture(long stage) const
{
    if (stage >= 4 || stage < 0 || !geo)
        return -1;
    GEOS::MATERIAL mtl;
    geo->GetMaterial(0, mtl);
    return mtl.texture[stage];
}

void WdmRenderModel::SetTexture(long stage, long id) const
{
    if (stage >= 4 || stage < 0 || !geo)
        return;
    GEOS::MATERIAL mtl;
    geo->GetMaterial(0, mtl);
    mtl.texture[stage] = id;
    geo->SetMaterial(0, mtl);
}

void WdmRenderModel::Render(VDX9RENDER *rs) const
{
    if (!geo)
        return;
    auto a = alpha * 255.0f;
    if (wdmObjects->isDebug && a < 80.0f)
        a = 80.0f;
    if (a < 1.0f)
        return;
    // draw
    if (a >= 255.0f)
    {
        a = 255.0f;
        wdmObjects->gs->SetTechnique(tech);
    }
    else
    {
        wdmObjects->gs->SetTechnique(techa);
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, (static_cast<long>(a) << 24) | 0xffffff);
    }
    // Check for visibility
    auto *const plane = rs->GetPlanes();
    static CMatrix mtx;
    rs->GetTransform(D3DTS_WORLD, mtx);
    const auto v = mtx * center;
    for (long i = 0; i < 4; i++)
    {
        auto &p = plane[i];
        const auto dist = v.x * p.Nx + v.y * p.Ny + v.z * p.Nz - p.D;
        if (dist < -radius)
            return;
    }
    geo->Draw(nullptr, 0, nullptr);
    wdmObjects->gs->SetTechnique("");
}
