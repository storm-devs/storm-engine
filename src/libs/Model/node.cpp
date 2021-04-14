//#define SHOW_SPHERES 'Q'

#include "Entity.h"
#include "core.h"
#include "modelr.h"

VGEOMETRY *NODER::gs = nullptr;
VDX9RENDER *NODER::rs = nullptr;
static long nlab = 0;
long NODER::depth = -1;
long NODER::node;
extern long clip_nps;
extern const PLANE *clip_p;
extern const CVECTOR *clip_c;
extern float clip_r;
extern GEOS::ADD_POLYGON_FUNC clip_geosap;
extern ADD_POLYGON_FUNC clip_ap;
char nm[256];
GEOS::PLANE clip_gp[256];
NODE *bestTraceNode = nullptr;
GEOS *clipGeo;

#ifdef SHOW_SPHERES
GEOS *sphere = 0;
#endif

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
CMatrix *backtrans;
CVECTOR clip_v[256];

bool AddPolygon(const GEOS::VERTEX *vr, long nv)
{
    //!!! must be transformed
    for (long p = 0; p < nv; p++)
        clip_v[p] = *backtrans * CVECTOR(vr[p].x, vr[p].y, vr[p].z);
    return clip_ap(clip_v, nv);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
bool NODER::Clip()
{
    if (isReleaed)
        return false;

    // check for bounding spheres intersection
    const auto sph_dist = ~(*clip_c - glob_mtx * center);
    if (sph_dist > (radius + clip_r) * (radius + clip_r))
        return false;

    auto retval = false;

    if (flags & CLIP_ENABLE && sph_dist <= (geo_radius + clip_r) * (geo_radius + clip_r))
    {
        backtrans = &glob_mtx;
        for (long p = 0; p < clip_nps; p++)
        {
            const auto x = clip_p[p].D * clip_p[p].Nx - glob_mtx.m[3][0];
            const auto y = clip_p[p].D * clip_p[p].Ny - glob_mtx.m[3][1];
            const auto z = clip_p[p].D * clip_p[p].Nz - glob_mtx.m[3][2];
            const auto Nx =
                glob_mtx.m[0][0] * clip_p[p].Nx + glob_mtx.m[0][1] * clip_p[p].Ny + glob_mtx.m[0][2] * clip_p[p].Nz;
            const auto Ny =
                glob_mtx.m[1][0] * clip_p[p].Nx + glob_mtx.m[1][1] * clip_p[p].Ny + glob_mtx.m[1][2] * clip_p[p].Nz;
            const auto Nz =
                glob_mtx.m[2][0] * clip_p[p].Nx + glob_mtx.m[2][1] * clip_p[p].Ny + glob_mtx.m[2][2] * clip_p[p].Nz;
            const auto lx = glob_mtx.m[0][0] * x + glob_mtx.m[0][1] * y + glob_mtx.m[0][2] * z;
            const auto ly = glob_mtx.m[1][0] * x + glob_mtx.m[1][1] * y + glob_mtx.m[1][2] * z;
            const auto lz = glob_mtx.m[2][0] * x + glob_mtx.m[2][1] * y + glob_mtx.m[2][2] * z;
            clip_gp[p].nrm.x = Nx;
            clip_gp[p].nrm.y = Ny;
            clip_gp[p].nrm.z = Nz;
            clip_gp[p].d = (Nx * lx + Ny * ly + Nz * lz) * glob_mtx.m[3][3];
        }
        GEOS::VERTEX gc;
        CVECTOR cnt;
        glob_mtx.MulToInv(*clip_c, cnt);
        gc.x = cnt.x;
        gc.y = cnt.y;
        gc.z = cnt.z;

        bestTraceNode = this;
        retval = geo->Clip(clip_gp, clip_nps, gc, clip_r, clip_geosap);
    }

    if (flags & CLIP_ENABLE_TREE)
        for (long l = 0; l < nnext; l++)
            if (next[l] != nullptr && static_cast<NODER *>(next[l])->Clip() == true)
                retval = true;
    return retval;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
float NODER::Update(CMatrix &mtx, CVECTOR &cnt)
{
    glob_mtx.EqMultiply(loc_mtx, mtx);

    center = geo_center;
    radius = geo_radius;

    for (long l = 0; l < nnext; l++)
        if (next[l] != nullptr)
        {
            CVECTOR cnt; //~!~
            const auto r = static_cast<NODER *>(next[l])->Update(glob_mtx, cnt);
            const auto rad = sqrtf(~(cnt - center)) + r;
            if (rad > radius)
                radius = rad;
        }

    cnt = loc_mtx * center;
    return radius;
}

//----------------------------------------------------------
// NODE trace
//----------------------------------------------------------
float NODER::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    if (isReleaed)
        return 2.0f;
    // check for bounding spheres intersection
    const auto lmn = dst - src;
    const auto dist2ray2 = ~((glob_mtx * center - src) ^ lmn);
    const auto dlmn = ~(lmn);
    // hierarchy test
    if (dist2ray2 > dlmn * radius * radius)
        return 2.0f;

    auto best_dist = 2.0f;

    if (flags & TRACE_ENABLE_TREE)
        for (long n = 0; n < nnext; n++)
        {
            if (next[n] == nullptr)
                continue;
            const auto d = static_cast<NODER *>(next[n])->Trace(src, dst);
            if (d < best_dist)
            {
                best_dist = d;
                bestTraceNode = next[n];
            }
        }

    if (flags & TRACE_ENABLE && dist2ray2 < dlmn * geo_radius * geo_radius)
    {
        CVECTOR _src, _dst;
        glob_mtx.MulToInv(src, _src);
        glob_mtx.MulToInv(dst, _dst);
        const auto ds = geo->Trace((GEOS::VERTEX &)_src, (GEOS::VERTEX &)_dst);
        if (ds < best_dist)
        {
            best_dist = ds;
            bestTraceNode = this;
        }
    }
    return best_dist;
}

//----------------------------------------------------------
// NODE constructor
//----------------------------------------------------------
NODER::NODER()
{
#ifdef SHOW_SPHERES
    sphere = 0;
#endif
    geo = nullptr;
    nnext = 0;
    next = nullptr;
    parent = nullptr;
    sys_modelName = nullptr;
    sys_TexPath = nullptr;
    sys_LightPath = nullptr;
    sys_lmPath = nullptr;

    max_view_dist = 0.f;
}

bool NODER::Init(const char *lightPath, const char *pname, const char *oname, const CMatrix &m, const CMatrix &globm,
                 NODER *par, const char *lmPath)
{
    name[0] = 0;
    technique[0] = 0;
    geoMaterialFunc = nullptr;
    flags = VISIBLE | VISIBLE_TREE | CLIP_ENABLE | CLIP_ENABLE_TREE | TRACE_ENABLE | TRACE_ENABLE_TREE;

    loc_mtx = m;
    glob_mtx.EqMultiply(loc_mtx, globm);

    nlab++;
    if (oname[0] == 0)
        sprintf_s(nm, "%s", pname);
    else
        sprintf_s(nm, "%s_%s", pname, oname);

    char lp[256];
    // sprintf_s(lp, "%s\\%s", lightPath, nm);
    sprintf_s(lp, "%s", lightPath);

    auto len = strlen(nm) + 1;
    sys_modelName = new char[len];
    memcpy(sys_modelName, nm, len);

    len = strlen(lp) + 1;
    sys_LightPath = new char[len];
    memcpy(sys_LightPath, lp, len);

    len = strlen(lmPath) + 1;
    sys_lmPath = new char[len];
    memcpy(sys_lmPath, lmPath, len);

    const auto *const tPath = gs->GetTexturePath();
    len = strlen(tPath) + 1;
    sys_TexPath = new char[len];
    memcpy(sys_TexPath, tPath, len);

    isReleaed = false;

    geo = gs->CreateGeometry(nm, lp, 0, lmPath);
    if (!geo)
    {
        delete sys_modelName;
        sys_modelName = nullptr;
        delete sys_LightPath;
        sys_LightPath = nullptr;
        delete sys_lmPath;
        sys_lmPath = nullptr;
        return false;
    }

    // load geometry radius and center
    GEOS::INFO gi;
    geo->GetInfo(gi);
    geo_radius = gi.radius;
    geo_center = CVECTOR(gi.boxcenter.x, gi.boxcenter.y, gi.boxcenter.z);

    parent = par;
    if (parent == nullptr)
        strcpy_s(name, pname);
    // calculate number of labels
    const auto idGeo = geo->FindName("geometry");
    nnext = 0;
    long sti = -1;
    while ((sti = geo->FindLabelG(sti + 1, idGeo)) > -1)
        nnext++;

    if (nnext > 0)
    {
        next = static_cast<NODE **>(malloc(sizeof(NODE *) * nnext));
        for (long ii = 0; ii < nnext; ii++)
            next[ii] = nullptr;
        long sti = -1;
        long l = 0;
        while ((sti = geo->FindLabelG(sti + 1, idGeo)) > -1)
        {
            GEOS::LABEL lb;
            geo->GetLabel(sti, lb);
            CMatrix mt;
            // memcpy(mt.m, lb.m, sizeof(lb.m));
            mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
            mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
            mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
            mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

            next[l] = new NODER();

            if (!next[l]->Init(lightPath, pname, lb.name, mt, glob_mtx, this, lmPath))
                return false;

            strcpy_s(static_cast<NODER *>(next[l])->name, lb.name);
            l++;
        }
    }

    idGeoGroup = -1;
    if (parent)
        idGeoGroup = parent->geo->FindName("geometry");

    return true;
}

//----------------------------------------------------------
// NODE destructor
//----------------------------------------------------------

NODER::~NODER()
{
#ifdef SHOW_SPHERES
    if (sphere != 0)
    {
        delete sphere;
        sphere = 0;
    }
#endif

    delete geo;
    for (long l = 0; l < nnext; l++)
        if (next[l] != nullptr)
            delete next[l];
    if (nnext > 0)
        free(next);
    delete sys_modelName;
    delete sys_LightPath;
    delete sys_lmPath;
    delete sys_TexPath;
}

void NODER::ReleaseGeometry()
{
    if (isReleaed)
        return;
    delete geo;
    geo = nullptr;
    isReleaed = true;
    for (long i = 0; i < nnext; i++)
    {
        if (!next[i])
            continue;
        static_cast<NODER *>(next[i])->ReleaseGeometry();
    }
}

void NODER::RestoreGeometry()
{
    if (!isReleaed)
        return;

    const auto *const tPath = gs->GetTexturePath();
    const auto len = strlen(tPath) + 1;
    auto *const ttPath = new char[len];
    memcpy(ttPath, tPath, len);
    gs->SetTexturePath(sys_TexPath);
    geo = gs->CreateGeometry(sys_modelName, sys_LightPath, 0, sys_lmPath);
    gs->SetTexturePath(ttPath);
    delete[] ttPath;
    if (!geo)
        throw std::runtime_error("No geometry");

    isReleaed = false;
    for (long i = 0; i < nnext; i++)
    {
        if (!next[i])
            continue;
        static_cast<NODER *>(next[i])->RestoreGeometry();
    }
}

//----------------------------------------------------------
// NODE draw
//----------------------------------------------------------
extern GEOS::PLANE ViewPlane[4];
GEOS::PLANE TViewPlane[4];

void NODER::Draw()
{
    if (isReleaed)
        return;

    const auto cnt = glob_mtx * center;

    // visibility check
    long p;
    for (p = 0; p < 4; p++)
    {
        const auto dist =
            cnt.x * ViewPlane[p].nrm.x + cnt.y * ViewPlane[p].nrm.y + cnt.z * ViewPlane[p].nrm.z - ViewPlane[p].d;
        if (dist > radius)
            break;
    }
    if (p < 4)
        return;
    if (max_view_dist > 0.f)
    {
        CVECTOR cpos, cang;
        float cpersp;
        rs->GetCamera(cpos, cang, cpersp);
        const float fdist = ~(cpos - cnt);
        const float fmindist = (max_view_dist + radius) * (max_view_dist + radius);
        const float fmaxdist = (max_view_dist * 1.3f + radius) * (max_view_dist * 1.3f + radius);
        if (fdist > fmaxdist)
            distance_blend = 1.f;
        else if (fdist < fmindist)
            distance_blend = 0.f;
        else
            distance_blend = (fdist - fmindist) / (fmaxdist - fmindist);
        if (distance_blend >= 1.f)
            return;
        if (distance_blend < 0.f)
            distance_blend = 0.f;
    }

#ifdef SHOW_SPHERES
    if (sphere == 0)
    {
        sphere = gs->CreateGeometry("sphere", 0, 0);
    }
    if (core.Controls->GetDebugAsyncKeyState(SHOW_SPHERES) < 0)
    {
        CMatrix sm(0.0f, 0.0f, 0.0f, cnt.x, cnt.y, cnt.z);
        CMatrix sc;
        sc.m[0][0] = sc.m[1][1] = sc.m[2][2] = radius;
        CMatrix rm = sc * sm;
        rs->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&rm);
        sphere->Draw(0, 0, 0);
    }
#endif

    // draw this node
    if (flags & VISIBLE)
    {
        // visibility check for geometry
        long p;
        for (p = 0; p < 4; p++)
        {
            const float dist =
                cnt.x * ViewPlane[p].nrm.x + cnt.y * ViewPlane[p].nrm.y + cnt.z * ViewPlane[p].nrm.z - ViewPlane[p].d;
            if (dist > geo_radius)
                break;
        }
        if (p == 4)
        {
            rs->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&glob_mtx);
            gs->SetTechnique(&technique[0]);
            if (max_view_dist > 0.f && distance_blend > 0.f)
            {
                gs->SetTechnique("geomdistanceblend");
                uint32_t dwTFColor;
                dwTFColor = (static_cast<uint32_t>(255.f - 255.f * distance_blend) << 24) | 0xFFFFFF;
                rs->SetRenderState(D3DRS_TEXTUREFACTOR, dwTFColor);
            }
            // transform viewplanes
            for (p = 0; p < 4; p++)
            {
                const float x = ViewPlane[p].d * ViewPlane[p].nrm.x - glob_mtx.m[3][0];
                const float y = ViewPlane[p].d * ViewPlane[p].nrm.y - glob_mtx.m[3][1];
                const float z = ViewPlane[p].d * ViewPlane[p].nrm.z - glob_mtx.m[3][2];
                const float Nx = glob_mtx.m[0][0] * ViewPlane[p].nrm.x + glob_mtx.m[0][1] * ViewPlane[p].nrm.y +
                                 glob_mtx.m[0][2] * ViewPlane[p].nrm.z;
                const float Ny = glob_mtx.m[1][0] * ViewPlane[p].nrm.x + glob_mtx.m[1][1] * ViewPlane[p].nrm.y +
                                 glob_mtx.m[1][2] * ViewPlane[p].nrm.z;
                const float Nz = glob_mtx.m[2][0] * ViewPlane[p].nrm.x + glob_mtx.m[2][1] * ViewPlane[p].nrm.y +
                                 glob_mtx.m[2][2] * ViewPlane[p].nrm.z;
                const float lx = glob_mtx.m[0][0] * x + glob_mtx.m[0][1] * y + glob_mtx.m[0][2] * z;
                const float ly = glob_mtx.m[1][0] * x + glob_mtx.m[1][1] * y + glob_mtx.m[1][2] * z;
                const float lz = glob_mtx.m[2][0] * x + glob_mtx.m[2][1] * y + glob_mtx.m[2][2] * z;
                TViewPlane[p].nrm.x = Nx;
                TViewPlane[p].nrm.y = Ny;
                TViewPlane[p].nrm.z = Nz;
                TViewPlane[p].d = (Nx * lx + Ny * ly + Nz * lz) * glob_mtx.m[3][3];
            }

            // draw geos
            geo->Draw(&TViewPlane[0], 4, geoMaterialFunc);
        }
    }

    // draw all children
    if (flags & VISIBLE_TREE)
        for (long l = 0; l < nnext; l++)
            if (next[l] != nullptr)
                static_cast<NODER *>(next[l])->Draw();
}

//----------------------------------------------------------
// NODE get node by number
//----------------------------------------------------------
NODER *NODER::FindNode(const char *cNodeName)
{
    if (_stricmp(cNodeName, name) == 0)
        return this;
    for (long i = 0; i < nnext; i++)
    {
        if (!next[i])
            continue;
        NODER *nd = static_cast<NODER *>(next[i])->FindNode(cNodeName);
        if (nd)
            return nd;
    }
    return nullptr;
}

//----------------------------------------------------------
// NODE get node by number
//----------------------------------------------------------
NODER *NODER::GetNode(long n)
{
    depth++;
    if (depth == 0)
        node = 0;
    if (node == n)
    {
        depth--;
        return this;
    }
    node++;
    for (long l = 0; l < nnext; l++)
    {
        if (next[l] == nullptr)
            continue;
        NODER *g = static_cast<NODER *>(next[l])->GetNode(n);
        if (g != nullptr)
        {
            depth--;
            return g;
        }
    }
    depth--;
    return nullptr;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
const char *NODER::GetName()
{
    return &name[0];
}

//-------------------------------------------------------------------
// unlink node
//-------------------------------------------------------------------
NODE *NODER::Unlink()
{
    return nullptr;
}

//-------------------------------------------------------------------
// link node to node
//-------------------------------------------------------------------
void NODER::Link(NODE *node)
{
}

//-------------------------------------------------------------------
// unlink node to model
//-------------------------------------------------------------------
entid_t NODER::Unlink2Model()
{
    const entid_t id = EntityManager::CreateEntity("modelr");
    auto *mdl = static_cast<MODELR *>(EntityManager::GetEntityPointer(id));

    // link node to as root
    mdl->root = this;

    // for non-root
    if (parent != nullptr)
    {
        // get parent matrix
        mdl->mtx = parent->glob_mtx;

        for (long l = 0; l < parent->nnext; l++)
            if (parent->next[l] == this)
            {
                parent->next[l] = nullptr;
                break;
            }
    }
    //!!! else get model's mtx

    return id;
}

//-------------------------------------------------------------------
// link model to node
//-------------------------------------------------------------------
void NODER::Link(entid_t id, bool transform)
{
    auto *mdl = static_cast<MODELR *>(EntityManager::GetEntityPointer(id));
    if (mdl == nullptr)
        return;

    // increment number of children
    nnext++;
    next = static_cast<NODE **>(realloc(next, sizeof(NODE *) * nnext));
    next[nnext - 1] = mdl->root;

    // modify loc_mtx of node
    if (transform)
    {
        CMatrix glob_parent = next[nnext - 1]->glob_mtx;
        // glob_parent.Transposition();
        // next[nnext-1]->loc_mtx = mdl->mtx * glob_parent;
    }

    // prevent self-deleting
    mdl->root = nullptr;
    // delete model
    EntityManager::EraseEntity(id);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void NODER::SetTechnique(const char *name)
{
    strcpy_s(technique, name);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
const char *NODER::GetTechnique()
{
    return &technique[0];
}

void NODER::SetMaxViewDist(float fDist)
{
    max_view_dist = fDist;
    for (long n = 0; n < nnext; n++)
        if (next[n])
            static_cast<NODER *>(next[n])->SetMaxViewDist(fDist);
}
