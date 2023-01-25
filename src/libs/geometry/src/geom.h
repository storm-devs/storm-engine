/******************************************************************************
File:  geom.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
Import library class
******************************************************************************/
#pragma once

#include "rdf.h"
#include "geos.h"

#include <vector>

#define EPSILON 4e-7

struct SAVAGE
{
    double dist, dise;
    BSP_NODE *node, *second;
};

class GEOM : public GEOS
{
    static SAVAGE _stack[256];
    std::vector<CVECTOR> vrt{};
    std::vector<RDF_BSPTRIANGLE> btrg{};
    std::vector<BSP_NODE> sroot{};

    CVECTOR res_norm;
    float res_pldist;
    float res_dist;

    struct VERTEX_BUFFER
    {
        int32_t type;
        int32_t size;
        int32_t stride;
        int32_t nverts;
        int32_t dev_buff;
    };

    GEOM_SERVICE &srv;

    RDF_HEAD rhead;
    char *globname = nullptr;
    int32_t *names = nullptr;
    int32_t *tname = nullptr;
    int32_t *tlookup = nullptr;
    MATERIAL *material = nullptr;
    LIGHT *light = nullptr;
    LABEL *label = nullptr;
    OBJECT *object = nullptr;
    int32_t idx_buff;
    VERTEX_BUFFER *vbuff = nullptr;

    int32_t *atriangles = nullptr;
    int32_t traceid;
    DVECTOR src, dst;

  public:
    GEOM(const char *fname, const char *lightname, GEOM_SERVICE &srv, int32_t flags);
    virtual ~GEOM();

    virtual int32_t FindName(const char *name) const;

    virtual int32_t FindLabelN(int32_t start_index, int32_t name_id);
    virtual int32_t FindLabelG(int32_t start_index, int32_t group_name_id);
    virtual void GetLabel(int32_t l, LABEL &lb) const;
    virtual void SetLabel(int32_t l, const LABEL &lb);

    virtual int32_t FindObjN(int32_t start_index, int32_t name_id);
    virtual int32_t FindObjG(int32_t start_index, int32_t group_name_id);
    virtual void GetObj(int32_t o, OBJECT &ob) const;
    virtual void SetObj(int32_t o, const OBJECT &ob);

    virtual int32_t FindMaterialN(int32_t start_index, int32_t name_id);
    virtual int32_t FindMaterialG(int32_t start_index, int32_t group_name_id);
    virtual void GetMaterial(int32_t m, MATERIAL &mt) const;
    virtual void SetMaterial(int32_t m, const MATERIAL &mt);

    virtual int32_t FindLightN(int32_t start_index, int32_t name_id);
    virtual int32_t FindLightG(int32_t start_index, int32_t group_name_id);
    virtual void GetLight(int32_t l, LIGHT &lt) const;
    virtual void SetLight(int32_t l, const LIGHT &lt);

    virtual void GetInfo(INFO &i) const;
    virtual void Draw(const PLANE *pl, int32_t np, MATERIAL_FUNC mtf) const;

    virtual float Trace(VERTEX &src, VERTEX &dst);
    virtual bool Clip(const PLANE *planes, int32_t nplanes, const VERTEX &center, float radius, ADD_POLYGON_FUNC addpoly);
    virtual bool GetCollisionDetails(TRACE_INFO &ti) const;

    virtual int32_t FindTexture(int32_t start_index, int32_t name_id);
    virtual int32_t GetTexture(int32_t tx) const;
    virtual const char *GetTextureName(int32_t tx) const;

    virtual int32_t GetVertexBuffer(int32_t vb) const;

    virtual int32_t GetIndexBuffer() const;
};
