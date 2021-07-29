//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LightProcessor
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "LightProcessor.h"

#define LIGHTPRC_TRACE_NUM 1000
#define LIGHTPRC_SMOOTH_NUM 1000
#define LIGHTPRC_BLUR_NUM 500

// ============================================================================================
// Construction, destruction
// ============================================================================================

LightProcessor::LightProcessor()
    : lights(nullptr), rs(nullptr), octtree(nullptr)
{
    geometry = nullptr;
    window = nullptr;
    shadowTriangle = -1;
    smoothVertex = -1;
    blurVertex = -1;
}

LightProcessor::~LightProcessor()
{
}

void LightProcessor::SetParams(LGeometry *g, Window *win, LighterLights *lit, OctTree *ot, VDX9RENDER *_rs)
{
    geometry = g;
    window = win;
    lights = lit;
    rs = _rs;
    octtree = ot;
}

void LightProcessor::Process()
{
    if (shadowTriangle >= 0)
    {
        if (geometry->numTrg)
        {
            CalcShadows();
            window->tracePrc = shadowTriangle / static_cast<float>(geometry->numTrg);
        }
        else
            shadowTriangle = -1;
        if (shadowTriangle == -1)
        {
            // Normalizing the result
            auto *const vrt = geometry->vrt.data();
            const auto numVrt = geometry->numVrt;
            const auto numLights = lights->Num();
            for (long i = 0; i < numVrt; i++)
            {
                for (long j = 0; j < numLights; j++)
                {
                    if (vrt[i].shadow[j].nrm > 0.0)
                    {
                        vrt[i].shadow[j].v /= vrt[i].shadow[j].nrm;
                    }
                    else
                    {
                        vrt[i].shadow[j].v = 1.0;
                    }
                    vrt[i].shadow[j].sm = vrt[i].shadow[j].v;
                }
            }
            // indicate that finished
            window->isLockCtrl = false;
            window->tracePrc = 1.0f;
            CalcLights();
        }
        return;
    }
    if (smoothVertex >= 0)
    {
        SmoothShadows();
        if (smoothVertex == -1)
        {
            // indicate that finished
            window->isLockCtrl = false;
            window->smoothPrc = 1.0f;
            CalcLights();
        }
        else
        {
            if (geometry->numVrt)
                window->smoothPrc = smoothVertex / static_cast<float>(geometry->numVrt);
        }
    }
    if (blurVertex >= 0)
    {
        BlurLight();
        if (blurVertex == -1)
        {
            // indicate that finished
            window->isLockCtrl = false;
            window->blurPrc = 1.0f;
            CalcLights();
        }
        else
        {
            if (geometry->numVrt)
                window->blurPrc = blurVertex / static_cast<float>(geometry->numVrt);
        }
    }
    if (window->isTraceShadows)
    {
        // prepare
        shadowTriangle = 0;
        window->isTraceShadows = false;
        window->isLockCtrl = true;
        window->tracePrc = 0.0f;
        // Resetting the color state
        auto *const vrt = geometry->vrt.data();
        const auto numVrt = geometry->numVrt;
        const auto numLights = lights->Num();
        for (long i = 0; i < numVrt; i++)
        {
            for (long j = 0; j < numLights; j++)
            {
                vrt[i].shadow[j].v = 0.0f;
                vrt[i].shadow[j].nrm = 0.0f;
                vrt[i].shadow[j].sm = 0.0f;
            }
        }
    }
    if (window->isSmoothShadows)
    {
        smoothVertex = 0;
        window->isSmoothShadows = false;
        window->isLockCtrl = true;
        window->smoothPrc = 0.0f;
    }
    if (window->isBlurLight)
    {
        CalcLights();
        blurVertex = 0;
        window->isBlurLight = false;
        window->isLockCtrl = true;
        window->blurPrc = 0.0f;
    }
    if (window->isResetBlurLight)
    {
        auto *const vrt = geometry->vrt.data();
        const auto numVrt = geometry->numVrt;
        for (long i = 0; i < numVrt; i++)
            vrt[i].bc = 0.0f;
        window->isResetBlurLight = false;
        CalcLights();
    }
    if (window->isUpdateLight)
    {
        CalcLights(window->updateLight, window->updateLightCos, window->updateLightAtt, window->updateLightSdw);
        window->updateLight = -1;
        window->updateLightCos = true;
        window->updateLightAtt = true;
        window->updateLightSdw = true;
        window->isUpdateLight = false;
    }
}

void LightProcessor::UpdateLightsParam()
{
    auto *const vrt = geometry->vrt.data();
    const auto numVrt = geometry->numVrt;
    const auto numLights = lights->Num();
    auto &ls = *lights;
    float cs, att;
    float dst;
    CVECTOR nrm;
    for (long i = 0; i < numVrt; i++)
    {
        auto &v = vrt[i];
        for (long j = 0; j < numLights; j++)
        {
            auto &lt = ls[j];
            auto &swh = v.shadow[j];
            switch (lt.type)
            {
            case Light::t_amb:
                swh.cs = 1.0f;
                swh.att = 1.0f;
                break;
            case Light::t_sun:
                cs = lt.p | v.n;
                if (cs < 0.0f)
                    cs = 0.0f;
                swh.cs = cs;
                swh.att = 1.0f;
                break;
            case Light::t_sky:
                cs = v.n.y;
                if (cs < 0.0f)
                    cs = 0.0f;
                swh.cs = v.n.y;
                swh.att = 1.0f;
                break;
            case Light::t_point:
                // Cosine of an angle
                nrm = lt.p - v.p;
                dst = sqrtf(~nrm);
                if (dst > 0.0f)
                {
                    nrm *= 1.0f / dst;
                    cs = nrm | v.n;
                }
                else
                    cs = 1.0f;
                if (cs < 0.0f)
                    cs = 0.0f;
                swh.cs = cs;
                // Attenuation coefficient
                if (dst < lt.range)
                {
                    att = lt.att0 + dst * lt.att1 + dst * dst * lt.att2;
                    if (att > 0.0f)
                        att = 1.0f / att;
                    else
                        att = 0.0f;
                }
                else
                    att = 0.0f;
                swh.cs = cs;
                swh.att = att;
                swh.dst = dst;
                break;
            }
            swh.csatt = static_cast<float>(swh.cs * swh.att);
            swh.shw = 1.0f;
        }
    }
}

// Calculate shading
void LightProcessor::CalcShadows()
{
    // Calculating shading
    for (long i = 0; i < LIGHTPRC_TRACE_NUM && shadowTriangle < geometry->numTrg; i++, shadowTriangle++)
        ApplyTriangleShadows(geometry->trg[shadowTriangle]);
    if (shadowTriangle == geometry->numTrg)
        shadowTriangle = -1;
}

// Distribute shading from triangle to vertices
void LightProcessor::ApplyTriangleShadows(Triangle &t)
{
    auto &ls = *lights;
    const auto num = ls.Num();
    auto *const vrt = geometry->vrt.data();
    for (long i = 0; i < num; i++)
    {
        // need to trace?
        if (ls[i].type == Light::t_none || ls[i].type == Light::t_amb)
            continue;
        // Standardization coefficient
        vrt[t.i[0]].shadow[i].nrm += t.sq;
        vrt[t.i[1]].shadow[i].nrm += t.sq;
        vrt[t.i[2]].shadow[i].nrm += t.sq;
        // Point from where to trace
        auto pnt = (vrt[t.i[0]].p + vrt[t.i[1]].p + vrt[t.i[2]].p) / 3.0f;
        pnt += t.n * 0.001f;
        // Determining shading
        switch (ls[i].type)
        {
        case Light::t_sun:
            // Sun lighting
            if ((ls[i].p | t.n) >= 0.0f)
            {
                if (geometry->Trace(pnt, pnt + ls[i].p * geometry->radius) > 1.0f)
                {
                    vrt[t.i[0]].shadow[i].v += t.sq;
                    vrt[t.i[1]].shadow[i].v += t.sq;
                    vrt[t.i[2]].shadow[i].v += t.sq;
                }
            }
            break;
        case Light::t_sky:
            // Lighting from the sky
            if (t.n.y >= 0.0f)
            {
                float sky = 0.0;
                const auto rad = geometry->radius;
                const auto rdx = geometry->radius * 0.2f;
                if (geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, 0.0f)) > 1.0f)
                    sky += 1.0f / 5.0f;
                if (geometry->Trace(pnt, pnt + CVECTOR(rdx, rad, 0.0f)) > 1.0f)
                    sky += 1.0f / 5.0f;
                if (geometry->Trace(pnt, pnt + CVECTOR(-rdx, rad, 0.0f)) > 1.0f)
                    sky += 1.0f / 5.0f;
                if (geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, rdx)) > 1.0f)
                    sky += 1.0f / 5.0f;
                if (geometry->Trace(pnt, pnt + CVECTOR(0.0f, rad, -rdx)) > 1.0f)
                    sky += 1.0f / 5.0f;
                sky *= t.sq;
                vrt[t.i[0]].shadow[i].v += sky;
                vrt[t.i[1]].shadow[i].v += sky;
                vrt[t.i[2]].shadow[i].v += sky;
            }
            break;
        case Light::t_point:
            // Sun lighting
            if (((ls[i].p - pnt) | t.n) >= 0.0f)
            {
                if (geometry->Trace(pnt, ls[i].p) > 1.0f)
                {
                    vrt[t.i[0]].shadow[i].v += t.sq;
                    vrt[t.i[1]].shadow[i].v += t.sq;
                    vrt[t.i[2]].shadow[i].v += t.sq;
                }
            }
            break;
        default:
            vrt[t.i[0]].shadow[i].v += t.sq;
            vrt[t.i[1]].shadow[i].v += t.sq;
            vrt[t.i[2]].shadow[i].v += t.sq;
        }
    }
}

// Smooth shading
void LightProcessor::SmoothShadows()
{
    const auto lookNorm = window->smoothNorm;
    const auto smoothRad = window->smoothRad;
    const auto kSmoothRad = 1.0f / smoothRad;
    double smoothRad2 = smoothRad * smoothRad;
    auto &ls = *lights;
    const auto num = ls.Num();
    auto &vrt = geometry->vrt;
    for (long i = 0; i < LIGHTPRC_SMOOTH_NUM && smoothVertex < geometry->numVrt; i++, smoothVertex++)
    {
        auto &v = vrt[smoothVertex];
        // Looking for surrounding vertices
        octtree->FindVerts(v.p, smoothRad);
        auto *const verts = octtree->verts.data();
        const auto numVerts = octtree->numVerts;

        /*if constexpr (false)
        {
          float r2 = smoothRad * smoothRad;
          static Vertex* ov[16384];
          long numV = 0;
          long num = geometry->numVrt;
          long n;
          for (n = 0; n < num; n++)
          {
            float r = ~(vrt[n].p - v.p);
            if (r < r2)
            {
              Assert(numV < sizeof(ov)/sizeof(OctFndVerts));
              ov[numV++] = &vrt[n];
            }
          }
          if (numVerts != numV) core.Trace("numVerts(%i) != numV(%i)", numVerts, numV);
          for (n = 0; n < numVerts; n++)
          {
            long k;
            for (k = 0; k < numV; k++)
              if (verts[n].v == ov[k]) break;
            if (k >= numV) core.Trace("k >= numV");
          }
        }*/
        // Assert(numVerts > 0);
        // go through all the sources
        for (long n = 0; n < num; n++)
        {
            // Set to zero
            auto sm = 0.0;
            double kNorm = 0.0f;
            // All the vertices
            for (long j = 0; j < numVerts; j++)
            {
                if (lookNorm && (v.n | verts[j].v->n) <= 0.6f)
                    continue;
                double k = sqrt(verts[j].r2) * kSmoothRad;
                if (k < 0.0)
                    k = 0.0;
                if (k > 1.0)
                    k = 1.0;
                k = 1.0 - k;
                sm += verts[j].v->shadow[n].v * k;
                kNorm += k;
            }
            if (kNorm > 0.0)
                sm /= kNorm;
            else
                sm = v.shadow[n].v;
            v.shadow[n].sm = sm;
        }
    }
    if (smoothVertex >= geometry->numVrt)
        smoothVertex = -1;
}

// Smooth lighting
void LightProcessor::BlurLight()
{
    auto isTrace = window->isTraceBlur;
    const auto blurRad = window->blurRad;
    auto kBlurRad = 1.0f / blurRad;
    auto pw = window->blurAtt;
    auto kCos = window->blurCos;
    auto kCos1 = 1.0f - window->blurCos;
    double blurRad2 = blurRad * blurRad;
    auto *const vrt = geometry->vrt.data();
    const auto numVrt = geometry->numVrt;
    for (long i = 0; i < LIGHTPRC_BLUR_NUM && blurVertex < numVrt; i++, blurVertex++)
    {
        auto &v = vrt[blurVertex];
        // Looking for surrounding vertices
        octtree->FindVerts(v.p, blurRad);
        auto *verts = octtree->verts.data();
        auto numVerts = octtree->numVerts;

        continue;

        auto step = (numVerts + 8) >> 4;
        if (step < 1)
            step = 1;
        step = 1;
        auto r = 0.0, g = 0.0, b = 0.0, sum = 0.0;
        // All the vertices
        for (long j = 0; j < numVerts; j += step)
        {
            auto &vs = *verts[j].v;
            if (vs.c.x + vs.c.y + vs.c.z <= 0.0f)
                continue;
            auto n = vs.p - v.p;
            double css = -(n | vs.n);
            if (css <= 0.0)
                continue;
            double csd = n | v.n;
            if (csd <= 0.0)
                continue;
            double dst = sqrt(~n);
            if (dst <= 0.0)
                continue;
            auto k = dst * kBlurRad;
            dst = 1.0f / dst;
            n *= static_cast<float>(dst * 0.001);
            if (isTrace && geometry->Trace(v.p + n, vs.p - n) <= 1.0f)
                continue;
            css *= dst;
            csd *= dst;
            if (css > 1.0)
                css = 1.0;
            if (csd > 1.0)
                csd = 1.0;
            if (k <= 0.0)
                continue;
            if (k > 1.0)
                k = 1.0;
            k = powf(1.0f - k, pw) * (css * csd * kCos + kCos1);
            r += vs.c.x * k;
            g += vs.c.y * k;
            b += vs.c.z * k;
            sum += 1.0f;
        }
        if (sum > 0.0)
        {
            sum = 1.0 / sum;
            r *= sum;
            g *= sum;
            b *= sum;
        }
        auto max = r > g ? r : g;
        if (max < b)
            max = b;
        max = 1.0;
        if (max > 0.0)
        {
            max = 1.0 / max;
            r *= max;
            g *= max;
            b *= max;
        }
        v.bc.x = static_cast<float>(r);
        v.bc.y = static_cast<float>(g);
        v.bc.z = static_cast<float>(b);
    }
    if (blurVertex >= geometry->numVrt)
        blurVertex = -1;
}

// Calculate lighting
void LightProcessor::CalcLights(long lit, bool isCos, bool isAtt, bool isSdw)
{
    lights->UpdateLights(lit);
    auto &ls = *lights;
    const auto num = ls.Num();
    auto *const vrt = geometry->vrt.data();
    const auto kBlur = window->kBlur;
    CVECTOR c;
    for (long i = 0; i < num; i++)
    {
        if (!ls[i].isOn)
            continue;
        if (ls[i].type != Light::t_sun && ls[i].type != Light::t_sky && ls[i].type != Light::t_point)
            continue;
        ls[i].curgm = ls[i].gamma >= 0.5f ? 1.0f + (ls[i].gamma - 0.5f) * 12.0f : ls[i].gamma + 0.5f;
        ls[i].curgm = 1.0f / ls[i].curgm;
        ls[i].curct = ls[i].contr >= 0.5f ? 1.0f + (ls[i].contr - 0.5f) * 20.0f : 0.05f + ls[i].contr * 2.0f * 0.95f;
    }
    for (long n = 0; n < geometry->numVrt; n++)
    {
        auto &v = vrt[n];
        if (!v.shadow)
            continue;
        c = v.bc * (kBlur * kBlur * 2.0f);
        float sw;
        double vl;
        for (long i = 0; i < num; i++)
        {
            auto &lt = ls[i];
            if (!lt.isOn)
                continue;
            auto &shw = v.shadow[i];
            if (!lt.isMark)
            {
                c += shw.c;
                continue;
            }
            switch (lt.type)
            {
            case Light::t_amb:
                shw.c = lt.color;
                c += shw.c;
                break;
            case Light::t_sun:
                // Cosine of an angle
                if (isCos)
                {
                    shw.csatt = lt.cosine * shw.cs + (1.0f - lt.cosine);
                }
                // Shading coefficient
                if (isSdw)
                {
                    vl = (v.shadow[i].sm - 0.5) * lt.curct + 0.5;
                    if (vl < 0.0f)
                        vl = 0.0f;
                    if (vl > 1.0f)
                        vl = 1.0f;
                    sw = static_cast<float>(pow(vl, static_cast<double>(lt.curgm))) + (lt.bright - 0.5f) * 1.8f;
                    if (sw < 0.0f)
                        sw = 0.0f;
                    if (sw > 1.0f)
                        sw = 1.0f;
                    shw.shw = lt.shadow * sw + (1.0f - lt.shadow);
                }
                // Resulting color
                shw.c = lt.color * (shw.csatt * shw.shw);
                c += shw.c;
                break;
            case Light::t_sky:
                // Cosine of an angle
                if (isCos)
                {
                    shw.csatt = lt.cosine * shw.cs + (1.0f - lt.cosine);
                }
                // Shading coefficient
                if (isSdw)
                {
                    vl = (v.shadow[i].sm - 0.5) * lt.curct + 0.5;
                    if (vl < 0.0f)
                        vl = 0.0f;
                    if (vl > 1.0f)
                        vl = 1.0f;
                    sw = static_cast<float>(pow(vl, static_cast<double>(lt.curgm))) + (lt.bright - 0.5f) * 1.8f;
                    if (sw < 0.0f)
                        sw = 0.0f;
                    if (sw > 1.0f)
                        sw = 1.0f;
                    shw.shw = lt.shadow * sw + (1.0f - lt.shadow);
                }
                // Resulting color
                shw.c = lt.color * (shw.csatt * shw.shw);
                c += shw.c;
                break;
            case Light::t_point:
                // Attenuation coefficient
                if (isAtt)
                {
                    if (shw.dst < lt.range)
                    {
                        shw.att = lt.att0 + shw.dst * lt.att1 + shw.dst * shw.dst * lt.att2;
                        if (shw.att > 0.0f)
                            shw.att = 1.0f / shw.att;
                        else
                            shw.att = 0.0f;
                    }
                    else
                        shw.att = 0.0f;
                }
                // Cosine of an angle
                if (isCos || isAtt)
                {
                    shw.csatt = (lt.cosine * shw.cs + (1.0f - lt.cosine)) * shw.att;
                }
                // Shading coefficient
                if (isSdw)
                {
                    vl = (v.shadow[i].sm - 0.5) * lt.curct + 0.5;
                    if (vl < 0.0f)
                        vl = 0.0f;
                    if (vl > 1.0f)
                        vl = 1.0f;
                    sw = static_cast<float>(pow(vl, static_cast<double>(lt.curgm))) + (lt.bright - 0.5f) * 1.8f;
                    if (sw < 0.0f)
                        sw = 0.0f;
                    if (sw > 1.0f)
                        sw = 1.0f;
                    shw.shw = lt.shadow * sw + (1.0f - lt.shadow);
                }
                // Resulting color
                shw.c = lt.color * (shw.csatt * shw.shw);
                c += shw.c;
                break;
            }
        }
        v.c = c;
    }
    geometry->UpdateColors(rs);
}
