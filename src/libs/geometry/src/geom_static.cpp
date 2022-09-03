/******************************************************************************
File:  geom_static.cpp

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
Import library main file
******************************************************************************/
#include "geom.h"

#include <cstdint>
#include <cstring>
#include <vector>

#include "../../util/include/string_compare.hpp"

namespace
{
std::vector<uint32_t> getColData(GEOM_SERVICE &srv, const std::string_view &file_name)
{
    std::vector<uint32_t> result;

    auto ltfl = srv.OpenFile(file_name.data());
    if (ltfl.is_open())
    {
        const auto file_size = srv.FileSize(file_name.data());
        if (file_size > 0)
        {
            result.resize(file_size / sizeof(uint32_t));
            srv.ReadFile(ltfl, result.data(), file_size);
        }
    }
    srv.CloseFile(ltfl);

    return result;
}
} // namespace

// create geometry func
GEOS *CreateGeometry(const char *fname, const char *lightname, GEOM_SERVICE &srv, int32_t flags)
{
    return new GEOM(fname, lightname, srv, flags);
}

// geometry constructor does all init
GEOM::GEOM(const char *fname, const char *lightname, GEOM_SERVICE &_srv, int32_t flags) : srv(_srv)
{
    std::vector<uint32_t> colData;
    if (lightname != nullptr)
    {
        colData = getColData(srv, lightname);
    }

    auto file = srv.OpenFile(fname);
    // read header
    srv.ReadFile(file, &rhead, sizeof(RDF_HEAD));
    if (rhead.version != RDF_VERSION)
        throw std::runtime_error("invalid version");

    // read names
    globname = static_cast<char *>(srv.malloc(rhead.name_size));
    srv.ReadFile(file, globname, rhead.name_size);

    names = static_cast<int32_t *>(srv.malloc(rhead.names * sizeof(int32_t)));
    srv.ReadFile(file, names, rhead.names * sizeof(int32_t));

    // load textures
    tname = static_cast<int32_t *>(srv.malloc(rhead.ntextures * sizeof(int32_t)));
    tlookup = static_cast<int32_t *>(srv.malloc(rhead.ntextures * sizeof(int32_t)));
    srv.ReadFile(file, tname, rhead.ntextures * sizeof(int32_t));

    // read materials
    auto *rmat = static_cast<RDF_MATERIAL *>(srv.malloc(sizeof(RDF_MATERIAL) * rhead.nmaterials));
    srv.ReadFile(file, rmat, sizeof(RDF_MATERIAL) * rhead.nmaterials);
    material = static_cast<MATERIAL *>(srv.malloc(sizeof(MATERIAL) * rhead.nmaterials));

    // read lights
    auto *rlig = static_cast<RDF_LIGHT *>(srv.malloc(sizeof(RDF_LIGHT) * rhead.nlights));
    srv.ReadFile(file, rlig, sizeof(RDF_LIGHT) * rhead.nlights);
    light = static_cast<LIGHT *>(srv.malloc(sizeof(LIGHT) * rhead.nlights));
    for (int32_t l = 0; l < rhead.nlights; l++)
    {
        light[l].flags = rlig[l].flags;
        light[l].type = static_cast<LIGHT_TYPE>(rlig[l].type);
        light[l].name = &globname[rlig[l].name];
        light[l].r = rlig[l].r;
        light[l].g = rlig[l].g;
        light[l].b = rlig[l].b;
        light[l].range = rlig[l].range;
        light[l].pos.x = rlig[l].pos.x;
        light[l].pos.y = rlig[l].pos.y;
        light[l].pos.z = rlig[l].pos.z;
        light[l].atten[0] = rlig[l].atten[0];
        light[l].atten[1] = rlig[l].atten[1];
        light[l].atten[2] = rlig[l].atten[2];
        light[l].inner = rlig[l].inner;
        light[l].outer = rlig[l].outer;
        light[l].falloff = rlig[l].falloff;
        light[l].dir.x = rlig[l].dir.x;
        light[l].dir.y = rlig[l].dir.y;
        light[l].dir.z = rlig[l].dir.z;
        light[l].id = srv.CreateLight(light[l]);
        srv.ActivateLight(light[l].id);
    }
    srv.free(rlig);

    // read labels
    auto *lab = static_cast<RDF_LABEL *>(srv.malloc(sizeof(RDF_LABEL) * rhead.nlabels));
    srv.ReadFile(file, lab, sizeof(RDF_LABEL) * rhead.nlabels);
    label = static_cast<LABEL *>(srv.malloc(sizeof(LABEL) * rhead.nlabels));
    for (int32_t lb = 0; lb < rhead.nlabels; lb++)
    {
        label[lb].flags = lab[lb].flags;
        label[lb].name = &globname[lab[lb].name];
        label[lb].group_name = &globname[lab[lb].group_name];
        memcpy(&label[lb].m[0][0], &lab[lb].m[0][0], sizeof(lab[lb].m));
        memcpy(&label[lb].bones[0], &lab[lb].bones[0], sizeof(lab[lb].bones));
        memcpy(&label[lb].weight[0], &lab[lb].weight[0], sizeof(lab[lb].weight));
    }
    srv.free(lab);

    // read objects
    auto *obj = static_cast<RDF_OBJECT *>(srv.malloc(sizeof(RDF_OBJECT) * rhead.nobjects));
    atriangles = static_cast<int32_t *>(srv.malloc(sizeof(int32_t) * rhead.nobjects));
    srv.ReadFile(file, obj, sizeof(RDF_OBJECT) * rhead.nobjects);
    object = static_cast<OBJECT *>(srv.malloc(sizeof(OBJECT) * rhead.nobjects));
    for (int32_t o = 0; o < rhead.nobjects; o++)
    {
        object[o].flags = obj[o].flags;
        object[o].center.x = obj[o].center.x;
        object[o].center.y = obj[o].center.y;
        object[o].center.z = obj[o].center.z;
        object[o].radius = obj[o].radius;
        object[o].material = obj[o].material;
        object[o].ntriangles = obj[o].ntriangles;
        object[o].striangle = obj[o].striangle;
        object[o].name = &globname[obj[o].name];
        object[o].group_name = &globname[obj[o].group_name];
        object[o].vertex_buff = obj[o].vertex_buff;
        object[o].start_vertex = obj[o].svertex;
        object[o].num_vertices = obj[o].nvertices;
        atriangles[o] = obj[o].atriangles;
    }
    srv.free(obj);

    // read triangles
    idx_buff = srv.CreateIndexBuffer(rhead.ntriangles * sizeof(RDF_TRIANGLE));
    auto *trg = static_cast<RDF_TRIANGLE *>(srv.LockIndexBuffer(idx_buff));
    srv.ReadFile(file, trg, sizeof(RDF_TRIANGLE) * rhead.ntriangles);
    srv.UnlockIndexBuffer(idx_buff);

    auto nvertices = 0;
    // read vertex buffers
    auto *rvb = static_cast<RDF_VERTEXBUFF *>(srv.malloc(rhead.nvrtbuffs * sizeof(RDF_VERTEXBUFF)));
    srv.ReadFile(file, rvb, rhead.nvrtbuffs * sizeof(RDF_VERTEXBUFF));
    vbuff = static_cast<VERTEX_BUFFER *>(srv.malloc(rhead.nvrtbuffs * sizeof(VERTEX_BUFFER)));
    int32_t v;
    for (v = 0; v < rhead.nvrtbuffs; v++)
    {
        vbuff[v].type = rvb[v].type;
        vbuff[v].stride = sizeof(RDF_VERTEX0) + (rvb[v].type & 3) * 8 + (rvb[v].type >> 2) * 8;
        vbuff[v].size = rvb[v].size;
        vbuff[v].nverts = vbuff[v].size / vbuff[v].stride;
        vbuff[v].dev_buff = vbuff[v].nverts > 0 ? srv.CreateVertexBuffer(rvb[v].type, rvb[v].size) : -1;
        nvertices += vbuff[v].nverts;
    }
    srv.free(rvb);
    // read vertices
    auto itColData = colData.begin();
    for (v = 0; v < rhead.nvrtbuffs; v++)
    {
        auto *vrt = static_cast<RDF_VERTEX0 *>(srv.LockVertexBuffer(vbuff[v].dev_buff));
        srv.ReadFile(file, vrt, vbuff[v].size);
        for (int32_t vr = 0; vr < vbuff[v].nverts; vr++)
        {
            auto *prv = (RDF_VERTEX0 *)((uint8_t *)(vrt) + vbuff[v].stride * vr);
            if (colData.size() == nvertices)
            {
                prv->color = *itColData;
                ++itColData;
            }
        }

        srv.UnlockVertexBuffer(vbuff[v].dev_buff);
    }

    // read BSP
    // rhead.flags &= ~FLAGS_BSP_PRESENT;
    if (rhead.flags & FLAGS_BSP_PRESENT)
    {
        RDF_BSPHEAD bhead;
        srv.ReadFile(file, &bhead, sizeof(RDF_BSPHEAD));

        sroot = static_cast<BSP_NODE *>(srv.malloc(bhead.nnodes * sizeof(BSP_NODE)));
        srv.ReadFile(file, sroot, bhead.nnodes * sizeof(BSP_NODE));

        vrt = static_cast<CVECTOR *>(srv.malloc(bhead.nvertices * sizeof(RDF_BSPVERTEX)));
        srv.ReadFile(file, vrt, bhead.nvertices * sizeof(RDF_BSPVERTEX));

        btrg = static_cast<RDF_BSPTRIANGLE *>(srv.malloc(bhead.ntriangles * sizeof(RDF_BSPTRIANGLE)));
        srv.ReadFile(file, btrg, bhead.ntriangles * sizeof(RDF_BSPTRIANGLE));
    }

    srv.CloseFile(file);

    for (int32_t t = 0; t < rhead.ntextures; t++)
        tlookup[t] = srv.CreateTexture(&globname[tname[t]]);
    for (int32_t m = 0; m < rhead.nmaterials; m++)
    {
        material[m].group_name = &globname[rmat[m].group_name];
        material[m].name = &globname[rmat[m].name];
        material[m].diffuse = rmat[m].diffuse;
        material[m].specular = rmat[m].specular;
        material[m].gloss = rmat[m].gloss;
        material[m].selfIllum = rmat[m].selfIllum;

        // relink textures
        for (int32_t tl = 0; tl < 4; tl++)
        {
            material[m].texture_type[tl] = static_cast<TEXTURE_TYPE>(rmat[m].texture_type[tl]);
            if (rmat[m].texture_type[tl] != TEXTURE_NONE)
                material[m].texture[tl] = tlookup[rmat[m].texture[tl]];
            else
                material[m].texture[tl] = -1;
        }
    }
    srv.free(rmat);
}

// delete all textures, buffers, memory
GEOM::~GEOM()
{
    if (rhead.flags & FLAGS_BSP_PRESENT)
    {
        // bsp
        srv.free(btrg);
        srv.free(vrt);
        srv.free(sroot);
    }

    for (int32_t v = 0; v < rhead.nvrtbuffs; v++)
        srv.ReleaseVertexBuffer(vbuff[v].dev_buff);
    srv.free(vbuff);

    srv.ReleaseIndexBuffer(idx_buff);

    srv.free(label);
    srv.free(object);
    srv.free(atriangles);
    srv.free(light);

    srv.free(material);

    for (int32_t t = 0; t < rhead.ntextures; t++)
        srv.ReleaseTexture(tlookup[t]);
    srv.free(tlookup);
    srv.free(tname);

    srv.free(names);
    srv.free(globname);
}

// visible analyze and draw all objects
void GEOM::Draw(const PLANE *pl, int32_t np, MATERIAL_FUNC mtf) const
{
    srv.SetIndexBuffer(idx_buff);
    for (int32_t o = 0; o < rhead.nobjects; o++)
    {
        if (!(object[o].flags & VISIBLE))
            continue;
        // clip by external planes
        int32_t cp;
        for (cp = 0; cp < np; cp++)
        {
            const auto dist = object[o].center.x * pl[cp].nrm.x + object[o].center.y * pl[cp].nrm.y +
                              object[o].center.z * pl[cp].nrm.z - pl[cp].d;
            if (dist > object[o].radius)
                break;
            // if(dist<-object[o].radius)    break;
        }
        if (cp < np)
            continue;

        auto *const vb = &vbuff[object[o].vertex_buff];
        srv.SetVertexBuffer(vb->stride, vb->dev_buff);
        srv.SetMaterial(material[object[o].material]);
        if (mtf != nullptr)
            mtf(material[object[o].material]);
        srv.DrawIndexedPrimitive(object[o].start_vertex, object[o].num_vertices, vb->stride, object[o].striangle * 3,
                                 object[o].ntriangles);
    }
}

bool GEOM::GetCollisionDetails(TRACE_INFO &ti) const
{
    if (!(rhead.flags & FLAGS_BSP_PRESENT) || traceid == -1)
    {
        ti.a = ti.b = -1.0;
        ti.obj = ti.trg = -1;
        memset(&ti.plane, 0, sizeof(PLANE));
        return false;
    }

    // triangle-based coord
    int32_t vindex[3];
    vindex[0] =
        (btrg[traceid].vindex[0][0] << 0) | (btrg[traceid].vindex[0][1] << 8) | (btrg[traceid].vindex[0][2] << 16);
    vindex[1] =
        (btrg[traceid].vindex[1][0] << 0) | (btrg[traceid].vindex[1][1] << 8) | (btrg[traceid].vindex[1][2] << 16);
    vindex[2] =
        (btrg[traceid].vindex[2][0] << 0) | (btrg[traceid].vindex[2][1] << 8) | (btrg[traceid].vindex[2][2] << 16);

    const auto ve = dst - src;
    const DVECTOR a = vrt[vindex[1]] - vrt[vindex[0]];
    const DVECTOR b = vrt[vindex[2]] - vrt[vindex[0]];
    const auto pvec = ve ^ b;
    const double det = a | pvec;
    const double invdet = 1.0 / det;
    const DVECTOR c = src - vrt[vindex[0]];

    ti.a = static_cast<float>((c | pvec) * invdet);
    ti.b = static_cast<float>((ve | (c ^ a)) * invdet);

    // plane info
    const DVECTOR nrm = !(a ^ b);
    const double pldist = nrm | vrt[vindex[0]];
    ti.plane.nrm.x = static_cast<float>(nrm.x);
    ti.plane.nrm.y = static_cast<float>(nrm.y);
    ti.plane.nrm.z = static_cast<float>(nrm.z);
    ti.plane.d = static_cast<float>(pldist);

    // object and triangle
    for (int32_t o = 0; o < rhead.nobjects; o++)
        if (atriangles[o] > traceid)
        {
            ti.obj = o;
            ti.trg = traceid;
            if (o > 0)
                ti.trg -= atriangles[o - 1];
            break;
        }

    // vertices
    for (int32_t v = 0; v < 3; v++)
    {
        ti.vrt[v].x = vrt[vindex[v]].x;
        ti.vrt[v].y = vrt[vindex[v]].y;
        ti.vrt[v].z = vrt[vindex[v]].z;
    }
    return true;
}

void GEOM::GetMaterial(int32_t m, MATERIAL &mt) const
{
    mt = material[m];
}

void GEOM::SetMaterial(int32_t m, const MATERIAL &mt)
{
    material[m].diffuse = mt.diffuse;
    material[m].gloss = mt.gloss;
    material[m].selfIllum = mt.selfIllum;
    material[m].specular = mt.specular;

    for (int32_t t = 0; t < 4; t++)
    {
        material[m].texture_type[t] = mt.texture_type[t];
        material[m].texture[t] = mt.texture[t];
    }
}

void GEOM::SetObj(int32_t o, const OBJECT &ob)
{
    object[o].flags = ob.flags;
    object[o].material = ob.material;
}

void GEOM::GetObj(int32_t o, OBJECT &ob) const
{
    ob = object[o];
    ob.vertex_buff = vbuff[object[o].vertex_buff].dev_buff;
}

void GEOM::GetInfo(INFO &i) const
{
    i.ntextures = rhead.ntextures;
    i.nmaterials = rhead.nmaterials;
    i.nlights = rhead.nlights;
    i.nobjects = rhead.nobjects;
    i.ntriangles = rhead.ntriangles;
    i.nvrtbuffs = rhead.nvrtbuffs;
    i.nlabels = rhead.nlabels;
    i.boxcenter.x = rhead.bbox_center.x;
    i.boxcenter.y = rhead.bbox_center.y;
    i.boxcenter.z = rhead.bbox_center.z;
    i.boxsize.x = rhead.bbox_size.x;
    i.boxsize.y = rhead.bbox_size.y;
    i.boxsize.z = rhead.bbox_size.z;
    i.radius = rhead.radius;
}

int32_t GEOM::GetVertexBuffer(int32_t vb) const
{
    return vbuff[vb].dev_buff;
}

int32_t GEOM::GetIndexBuffer() const
{
    return idx_buff;
}

int32_t GEOM::GetTexture(int32_t tx) const
{
    return tlookup[tx];
}

const char *GEOM::GetTextureName(int32_t tx) const
{
    return &globname[tname[tx]];
}

// ~!~
// leave it like that for now
// will think later
auto unbelievable_workaround(void *ptr)
{
    return reinterpret_cast<uint64_t>(ptr) & 0x7FFFFFFF;
}

int32_t GEOM::FindName(const char *name) const
{
    for (int32_t n = 0; n < rhead.names; n++)
        if (storm::iEquals(&globname[names[n]], name))
            return unbelievable_workaround(&globname[names[n]]);
    return -1;
}

//--------------------------------------------------
// label functions
//--------------------------------------------------
int32_t GEOM::FindLabelN(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nlabels; start_index++)
        if (unbelievable_workaround(label[start_index].name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindLabelG(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nlabels; start_index++)
        if (unbelievable_workaround(label[start_index].group_name) == name_id)
            return start_index;
    return -1;
}

void GEOM::GetLabel(int32_t l, LABEL &lb) const
{
    lb.flags = label[l].flags;
    lb.group_name = label[l].group_name;
    lb.name = label[l].name;
    memcpy(&lb.bones[0], &label[l].bones[0], sizeof(lb.bones));
    memcpy(&lb.weight[0], &label[l].weight[0], sizeof(lb.weight));
    memcpy(&lb.m[0][0], &label[l].m[0][0], sizeof(lb.m));
}

void GEOM::SetLabel(int32_t l, const LABEL &lb)
{
    label[l].flags = lb.flags;
    memcpy(&label[l].bones[0], &lb.bones[0], sizeof(lb.bones));
    memcpy(&label[l].weight[0], &lb.weight[0], sizeof(lb.weight));
    memcpy(&label[l].m[0][0], &lb.m[0][0], sizeof(lb.m));
}

//--------------------------------------------------
// object functions
//--------------------------------------------------
int32_t GEOM::FindObjN(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nobjects; start_index++)
        if (unbelievable_workaround(object[start_index].name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindObjG(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nobjects; start_index++)
        if (unbelievable_workaround(object[start_index].group_name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindMaterialN(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nmaterials; start_index++)
        if (unbelievable_workaround(material[start_index].name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindMaterialG(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nmaterials; start_index++)
        if (unbelievable_workaround(material[start_index].group_name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindLightN(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nlights; start_index++)
        if (unbelievable_workaround(light[start_index].name) == name_id)
            return start_index;
    return -1;
}

int32_t GEOM::FindLightG(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.nlights; start_index++)
        if (unbelievable_workaround(light[start_index].group_name) == name_id)
            return start_index;
    return -1;
}

void GEOM::SetLight(int32_t l, const LIGHT &lt)
{
    light[l].atten[0] = lt.atten[0];
    light[l].atten[1] = lt.atten[1];
    light[l].atten[2] = lt.atten[2];
    light[l].r = lt.r;
    light[l].g = lt.g;
    light[l].b = lt.b;
    light[l].dir = lt.dir;
    light[l].falloff = lt.falloff;
    light[l].flags = lt.flags;
    light[l].inner = lt.inner;
    light[l].outer = lt.outer;
    light[l].pos = lt.pos;
    light[l].range = lt.range;
    light[l].type = lt.type;
}

int32_t GEOM::FindTexture(int32_t start_index, int32_t name_id)
{
    for (; start_index < rhead.ntextures; start_index++)
        if (globname[tlookup[start_index]] == name_id)
            return start_index;
    return -1;
}

void GEOM::GetLight(int32_t l, LIGHT &lt) const
{
    lt = light[l];
}
