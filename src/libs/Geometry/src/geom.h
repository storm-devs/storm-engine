/******************************************************************************
File:  geom.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
Import library class
******************************************************************************/
#pragma once

#include "Rdf.h"
#include "geos.h"

#define EPSILON 4e-7
struct SAVAGE
{
    double dist, dise;
    BSP_NODE *node, *second;
};

class GEOM : public GEOS
{
    static SAVAGE _stack[256];
    CVECTOR *vrt;
    RDF_BSPTRIANGLE *btrg;
    BSP_NODE *sroot;

    CVECTOR res_norm;
    float res_pldist;
    float res_dist;

    struct VERTEX_BUFFER
    {
        long type;
        long size;
        long stride;
        long nverts;
        long dev_buff;
    };

    GEOM_SERVICE &srv;

    RDF_HEAD rhead;
    char *globname;
    long *names;
    long *tname;
    long *tlookup;
    MATERIAL *material;
    LIGHT *light;
    LABEL *label;
    OBJECT *object;
    long idx_buff;
    VERTEX_BUFFER *vbuff;

    long *atriangles;
    long traceid;
    DVECTOR src, dst;

  public:
    GEOM(const char *fname, const char *lightname, GEOM_SERVICE &srv, long flags);
    virtual ~GEOM();

    virtual long FindName(const char *name) const;

    virtual long FindLabelN(long start_index, long name_id);
    virtual long FindLabelG(long start_index, long group_name_id);
    virtual void GetLabel(long l, LABEL &lb) const;
    virtual void SetLabel(long l, const LABEL &lb);

    virtual long FindObjN(long start_index, long name_id);
    virtual long FindObjG(long start_index, long group_name_id);
    virtual void GetObj(long o, OBJECT &ob) const;
    virtual void SetObj(long o, const OBJECT &ob);

    virtual long FindMaterialN(long start_index, long name_id);
    virtual long FindMaterialG(long start_index, long group_name_id);
    virtual void GetMaterial(long m, MATERIAL &mt) const;
    virtual void SetMaterial(long m, const MATERIAL &mt);

    virtual long FindLightN(long start_index, long name_id);
    virtual long FindLightG(long start_index, long group_name_id);
    virtual void GetLight(long l, LIGHT &lt) const;
    virtual void SetLight(long l, const LIGHT &lt);

    virtual void GetInfo(INFO &i) const;
    virtual void Draw(const PLANE *pl, long np, MATERIAL_FUNC mtf) const;

    virtual float Trace(VERTEX &src, VERTEX &dst);
    virtual bool Clip(const PLANE *planes, long nplanes, const VERTEX &center, float radius, ADD_POLYGON_FUNC addpoly);
    virtual bool GetCollisionDetails(TRACE_INFO &ti) const;

    virtual long FindTexture(long start_index, long name_id);
    virtual long GetTexture(long tx) const;
    virtual const char *GetTextureName(long tx) const;

    virtual long GetVertexBuffer(long vb) const;

    virtual long GetIndexBuffer() const;
};
