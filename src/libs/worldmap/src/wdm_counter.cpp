//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmCounter
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_counter.h"
#include "world_map.h"

//============================================================================================

const char *WdmCounter::skytex[WMD_NUM_SKYS] = {
    "WorldMap\\Sky\\d1.tga", "WorldMap\\Sky\\n1.tga", "WorldMap\\Sky\\d2.tga", "WorldMap\\Sky\\n2.tga",
    "WorldMap\\Sky\\d3.tga", "WorldMap\\Sky\\n3.tga", "WorldMap\\Sky\\d4.tga", "WorldMap\\Sky\\n4.tga",
};

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmCounter::WdmCounter()
{
    sky = nullptr;
    d[0] = d[1] = nullptr;
    m[0] = m[1] = nullptr;
    y[0] = y[1] = y[2] = y[3] = nullptr;

    Assert(wdmObjects->rs);
    const auto kDef = wdmObjects->rs->GetHeightDeformator();
    mtx.BuildMatrix(-0.1f, 0.0f, 0.0f, 1.4f, 0.9f * kDef, 40.0f);
    mtx.Vx() *= -0.25f;
    mtx.Vy() *= 0.25f;
    mtx.Vz() *= -0.25f;

    for (int32_t i = 0; i < WMD_NUM_SKYS; i++)
        skytx[i] = -1;
    lastSkys[0] = lastSkys[1] = -1;
    for (int32_t i = 0; i < sizeof(skyseq) / sizeof(int32_t); i++)
        skyseq[i] = -1;
    skyCounter = 0;
    dayCounter = -1;
}

WdmCounter::~WdmCounter()
{
    if (sky)
    {
        sky->SetTexture(0, lastSkys[0]);
        sky->SetTexture(1, lastSkys[1]);
        delete sky;
    }
    if (d[0])
        delete d[0];
    if (d[1])
        delete d[1];
    if (m[0])
        delete m[0];
    if (m[1])
        delete m[1];
    if (y[0])
        delete y[0];
    if (y[1])
        delete y[1];
    if (y[2])
        delete y[2];
    if (y[3])
        delete y[3];
    if (wdmObjects->rs)
    {
        for (int32_t i = 0; i < WMD_NUM_SKYS; i++)
        {
            if (skytx[i] >= 0)
                wdmObjects->rs->TextureRelease(skytx[i]);
        }
    }
}

bool WdmCounter::Init()
{
    if (!wdmObjects->rs)
        return false;
    if (!Load("counter\\counter"))
        return false;
    if (!LoadModel(sky, "counter\\sky", "WdmCounterDrawSky"))
        return false;
    if (!LoadModel(d[0], "counter\\d1", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(d[1], "counter\\d2", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(m[0], "counter\\m1", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(m[1], "counter\\m2", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(y[0], "counter\\y1", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(y[1], "counter\\y2", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(y[2], "counter\\y3", "WdmCounterDrawNumber"))
        return false;
    if (!LoadModel(y[3], "counter\\y4", "WdmCounterDrawNumber"))
        return false;
    for (int32_t i = 0; i < WMD_NUM_SKYS; i++)
        skytx[i] = wdmObjects->rs->TextureCreate(skytex[i]);
    lastSkys[0] = sky->GetTexture(0);
    lastSkys[1] = sky->GetTexture(1);
    for (int32_t i = 0; i < sizeof(skyseq) / sizeof(int32_t); i += 2)
    {
        skyseq[i + 0] = skytx[(rand() % (WMD_NUM_SKYS / 2)) * 2 + 0];
        skyseq[i + 1] = skytx[(rand() % (WMD_NUM_SKYS / 2)) * 2 + 1];
    }
    return true;
}

// Calculations
void WdmCounter::Update(float dltTime)
{
}

void WdmCounter::LRender(VDX9RENDER *rs)
{
    // Camera matrix
    static CMatrix view, prj, oldPrj;
    rs->GetTransform(D3DTS_VIEW, view);
    rs->GetTransform(D3DTS_PROJECTION, oldPrj);
    float scrw, scrh;
    wdmObjects->GetVPSize(scrw, scrh);
    // prj.BuildProjectionMatrix(0.1f, scrw, scrh, 10.0f, 100.0f);
    prj.BuildProjectionMatrix(0.1f, 4.0f, 3.0f, 10.0f, 100.0f);
    rs->SetTransform(D3DTS_PROJECTION, prj);
    rs->SetTransform(D3DTS_VIEW, CMatrix());
    // Sky
    const int32_t numSkys = sizeof(skyseq) / sizeof(int32_t);
    if (dayCounter < 0)
        dayCounter = wdmObjects->wm->day;
    if (dayCounter != wdmObjects->wm->day)
    {
        dayCounter = wdmObjects->wm->day;
        skyseq[(skyCounter + 4) & (numSkys - 1)] = skytx[(rand() % (WMD_NUM_SKYS / 2)) * 2 + 0];
        skyseq[(skyCounter + 5) & (numSkys - 1)] = skytx[(rand() % (WMD_NUM_SKYS / 2)) * 2 + 1];
        skyCounter += 2;
        if (skyCounter >= numSkys)
            skyCounter = 0;
    }
    // wdmObjects->wm->day = 31;
    // wdmObjects->wm->mon = 12;
    // wdmObjects->wm->hour = 9.0f;
    auto hr = (wdmObjects->wm->hour + 11.0f) * 2.0f / 24.0f;
    const auto ofs = static_cast<int32_t>(hr);
    auto one = skyCounter + ofs;
    auto two = one + 1;
    one &= numSkys - 1;
    two &= numSkys - 1;
    hr = (hr - ofs - 0.5f);
    if (hr < -0.5f)
        hr = -0.5f;
    if (hr > 0.5f)
        hr = 0.5f;
    hr = sinf(hr * 3.141592f) * 0.5f;
    hr += 0.5f;
    sky->SetTexture(0, skyseq[one]);
    sky->SetTexture(1, skyseq[two]);
    rs->TextureSet(1, skyseq[two]);
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, (static_cast<int32_t>(hr * 255.0f) << 24) | 0x00ffffff);
    sky->LRender(rs);
    // Housing
    WdmRenderModel::LRender(rs);
    // Numbers
    // Day
    const auto day = wdmObjects->wm->day - 1;
    const auto mon = wdmObjects->wm->mon - 1;
    const auto days = wdmObjects->wm->month[mon + 1];
    const auto year = wdmObjects->wm->year;
    const auto kHeight = wdmObjects->wm->hour / 24.0f;
    auto kNext = 0.0f;
    float low, dlt, kMove;
    if (days == 30)
    {
        low = day % 10 + kHeight;
        kMove = 0.0f;
        if (day < 29)
        {
            if (low > 8.0f)
                kMove = low - 8.0f;
            if (kMove > 1.0f)
                kMove = 1.0f;
        }
        else
        {
            kMove = day + kHeight - 28.0f;
            kNext = kMove - 1.0f;
        }
        DrawNum(rs, d[0], 0.5f, (day / 10 + kMove) * 0.1f);
        DrawNum(rs, d[1], 0.0f, (low + 1.0f) * 0.1f);
    }
    else if (days == 31)
    {
        low = day % 10 + kHeight;
        kMove = 0.0f;
        if (day < 30)
        {
            if (low > 8.0f)
                kMove = low - 8.0f;
            if (kMove > 1.0f)
                kMove = 1.0f;
        }
        else
        {
            kMove = day + kHeight - 30.0f;
            kNext = kMove;
        }
        DrawNum(rs, d[0], 0.5f, (day / 10 + kMove) * 0.1f);
        dlt = day < 30 ? 0.0f : 0.5f;
        DrawNum(rs, d[1], dlt, dlt + (low + 1.0f) * 0.1f);
    }
    else if (days == 28)
    {
        low = day % 10 + kHeight;
        kMove = 0.0f;
        if (day < 27)
        {
            if (low > 8.0f)
                kMove = low - 8.0f;
            if (kMove > 1.0f)
                kMove = 1.0f;
        }
        else
        {
            kMove = day + kHeight - 27.0f;
            kNext = kMove;
        }
        DrawNum(rs, d[0], 0.75f, (day / 10 + kMove + 4) * 0.1f);
        dlt = day < 20 ? 0.0f : 0.25f;
        DrawNum(rs, d[1], dlt, (low + 1.0f) * 0.1f);
    }
    // Month
    kMove = 0.0f;
    if (kNext > 1.0f)
        kNext = 1.0f;
    low = mon % 10 + kNext;
    dlt = mon > 8 ? 0.75f : 0.0f;
    if (mon == 8)
        kMove = kNext;
    if (mon == 11)
        kMove = kNext;
    DrawNum(rs, m[0], 0.75f, ((mon + 1) / 10 + kMove + 7.0f) * 0.1f);
    DrawNum(rs, m[1], dlt, (low + 1.0f) * 0.1f);
    // Year
    kMove = 0.0f;
    if (mon == 11 && day == 30)
        kMove = kNext;
    low = static_cast<float>(year % 10);
    DrawNum(rs, y[3], 0.0f, (low + kMove) * 0.1f);
    if (low < 9)
        kMove = 0.0f;
    low = static_cast<float>((year / 10) % 10);
    DrawNum(rs, y[2], 0.0f, (low + kMove) * 0.1f);
    if (low < 9)
        kMove = 0.0f;
    low = static_cast<float>((year / 100) % 10);
    DrawNum(rs, y[1], 0.0f, (low + kMove) * 0.1f);
    if (low < 9)
        kMove = 0.0f;
    low = static_cast<float>((year / 1000) % 10);
    DrawNum(rs, y[0], 0.0f, (low + kMove) * 0.1f);
    rs->SetTransform(D3DTS_PROJECTION, oldPrj);
    rs->SetTransform(D3DTS_VIEW, view);
}

bool WdmCounter::LoadModel(WdmRenderModel *&pnt, const char *name, const char *tech) const
{
    pnt = new WdmRenderModel();
    if (!pnt->Load(name))
        return false;
    pnt->mtx = mtx;
    pnt->SetTech(tech, tech);
    return true;
}

void WdmCounter::DrawNum(VDX9RENDER *rs, WdmRenderModel *m, float u, float v)
{
    CMatrix mtx;
    mtx.m[0][0] = 0.25f;
    mtx.m[2][0] = u;
    mtx.m[2][1] = v;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
    rs->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m->LRender(rs);
    mtx.m[0][0] = 1.0f;
    mtx.m[2][0] = 0.0f;
    mtx.m[2][1] = 0.0f;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
    rs->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}
