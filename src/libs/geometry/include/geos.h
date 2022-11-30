/******************************************************************************
File:  geos.h

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
Import library main header
******************************************************************************/

#pragma once

#include <cstdint>
#include <fstream>

class GEOS
{
  public:
    using ID = int32_t; // this used w/o any checking

    struct VERTEX
    {
        float x, y, z;
    };

    // vertex
    struct VERTEX0
    {
        VERTEX pos, nrm;
        int32_t color;
        float tu, tv;
    };

    struct VERTEX1
    {
        VERTEX pos, nrm;
        int32_t color;
        float tu, tv;
        float tu1, tv1;
    };

    struct VERTEX2
    {
        VERTEX pos, nrm;
        int32_t color;
        float tu, tv;
        float tu1, tv1;
        float tu2, tv2;
    };

    struct VERTEX3
    {
        VERTEX pos, nrm;
        int32_t color;
        float tu, tv;
        float tu1, tv1;
        float tu2, tv2;
        float tu3, tv3;
    };

    // animated vertex
    struct AVERTEX0
    {
        VERTEX pos;
        float weight;
        uint32_t boneid;
        VERTEX nrm;
        int32_t color;
        float tu0, tv0;
    };

    virtual int32_t FindName(const char *name) const = 0;

    //-----------------------------------------
    // get info of entire object
    //-----------------------------------------
    struct INFO
    {
        int32_t ntextures;
        int32_t nmaterials;
        int32_t nlights;
        int32_t nobjects;
        int32_t ntriangles;
        int32_t nvrtbuffs;
        int32_t nlabels;
        VERTEX boxcenter, boxsize;
        float radius;
    };

    virtual void GetInfo(INFO &i) const = 0;

    //-----------------------------------------
    // label
    //-----------------------------------------
    enum LABEL_FLAGS
    {
        LABLE_FORCEDWORD = 0x7FFFFFFF
    };

    struct LABEL
    {
        alignas(16) float m[4][4];
        int32_t flags; // combination of LABEL_FLAG
        char *group_name;
        char *name;
        int32_t bones[4];
        float weight[4];
    };

    virtual int32_t FindLabelN(int32_t start_index, int32_t name_id) = 0;
    virtual int32_t FindLabelG(int32_t start_index, int32_t group_name_id) = 0;
    virtual void GetLabel(int32_t l, LABEL &lb) const = 0;
    virtual void SetLabel(int32_t l, const LABEL &lb) = 0;

    //-----------------------------------------
    // materials
    //-----------------------------------------
    enum TEXTURE_TYPE
    {
        TEXTURE_NONE = 0,
        TEXTURE_BASE,
        TEXTURE_NORMAL,
        TEXTURE_LIGHTMAP,
        TEXTURE_FORCEDWORD = 0x7FFFFFFF
    };

    struct MATERIAL
    {
        float diffuse;                // 0 - no diffuse material
        float specular, gloss;        // spec=0 - no specular, gloss is a power of cosine
        float selfIllum;              // for area light sources
        TEXTURE_TYPE texture_type[4]; // type of texture
        int32_t texture[4];
        char *group_name;
        char *name;
    };

    virtual int32_t FindMaterialN(int32_t start_index, int32_t name_id) = 0;
    virtual int32_t FindMaterialG(int32_t start_index, int32_t group_name_id) = 0;
    virtual void GetMaterial(int32_t m, MATERIAL &mt) const = 0;
    virtual void SetMaterial(int32_t m, const MATERIAL &mt) = 0;
    using MATERIAL_FUNC = void (*)(const MATERIAL &mt);

    //-----------------------------------------
    // object
    //-----------------------------------------
    enum OBJECT_FLAGS
    {
        VISIBLE = (1 << 0),
        STATIC_LIGHT_ENABLE = (1 << 1),
        // if no - object will not be lited
        DINAMIC_LIGHT_ENABLE = (1 << 2),
        // if no - no dynamic lighting perfomed
        CAST_SHADOWS_ENABLE = (1 << 3),
        // if material makes shadows
        COLLISION_ENABLE = (1 << 4),
        // if yes - object will be stored to BSP
        VERTEX_WEIGHT = (1 << 5),
        // animation
        MERGE = (1 << 6),
        // object is "merged"-type
        CULLENABLE = (1 << 7),
        // single-sided object
        OBJECT_FORCEDWORD = 0x7FFFFFFF
    };

    struct OBJECT
    {
        int32_t flags; // combination of OBJECT_FLAGSS
        VERTEX center;
        float radius;
        int32_t material;
        char *group_name;
        char *name;

        int32_t ntriangles, striangle;
        ID vertex_buff;
        int32_t start_vertex, num_vertices;
        int32_t bones[4];
    };

    virtual int32_t FindObjN(int32_t start_index, int32_t name_id) = 0;
    virtual int32_t FindObjG(int32_t start_index, int32_t group_name_id) = 0;
    virtual void GetObj(int32_t o, OBJECT &ob) const = 0;
    virtual void SetObj(int32_t o, const OBJECT &ob) = 0;

    //-----------------------------------------
    // light sources
    //-----------------------------------------
    enum LIGHT_TYPE
    {
        LIGHT_POINT = 0,
        LIGHT_SPOT,
        LIGHT_DIRECTIONAL,
        LIGHT_FORCE_DWORD = 0x7FFFFFFF
    };

    enum LIGHT_FLAGS
    {
        LIGHT_SHADOW = 1,
        LIGHT_FORCEDWORD = 0x7FFFFFFF
    };

    struct LIGHT
    {
        int32_t flags; // combination of LIGHT_FLAGS
        LIGHT_TYPE type;
        int32_t id;
        char *group_name;
        char *name;
        float r, g, b;
        float range;
        VERTEX pos;
        float atten[3];
        float inner, outer, falloff;
        VERTEX dir;
    };

    virtual int32_t FindLightN(int32_t start_index, int32_t name_id) = 0;
    virtual int32_t FindLightG(int32_t start_index, int32_t group_name_id) = 0;
    virtual void GetLight(int32_t l, LIGHT &lt) const = 0;
    virtual void SetLight(int32_t l, const LIGHT &lt) = 0;

    //-----------------------------------------
    // draw
    //-----------------------------------------
    struct PLANE
    {
        VERTEX nrm;
        float d;
    };

    virtual void Draw(const PLANE *pl, int32_t np, MATERIAL_FUNC mtf) const = 0;

    //-----------------------------------------
    // collision, ray shooting
    //-----------------------------------------
    struct TRACE_INFO
    {
        float a, b; // valid only for ray tracing
        int32_t obj, trg;
        PLANE plane;
        VERTEX vrt[3];
    };

    // Trace a ray in local coord-system
    virtual float Trace(VERTEX &src, VERTEX &dst) = 0;

    // clip in local coord-system
    using ADD_POLYGON_FUNC = bool (*)(const VERTEX *v, int32_t nv);
    virtual bool Clip(const PLANE *planes, int32_t nplanes, const VERTEX &center, float radius,
                      ADD_POLYGON_FUNC addpoly) = 0;

    // Get detail info on last ray hit or clip
    virtual bool GetCollisionDetails(TRACE_INFO &ti) const = 0;

    //-----------------------------------------
    // all other
    //-----------------------------------------

    // Get ID of texture
    virtual int32_t FindTexture(int32_t start_index, int32_t name_id) = 0;
    virtual int32_t GetTexture(int32_t tx) const = 0;
    virtual const char *GetTextureName(int32_t tx) const = 0;

    // GetVertexBuffer
    virtual int32_t GetVertexBuffer(int32_t vb) const = 0;

    // GetIndexBuffer
    virtual int32_t GetIndexBuffer() const = 0;

    //-----------------------------------------
    // unused
    //-----------------------------------------
    virtual ~GEOS(){};
};

class GEOM_SERVICE
{
  public:
    virtual ~GEOM_SERVICE(){};
    virtual std::fstream OpenFile(const char *fname) = 0;
    virtual bool ReadFile(std::fstream &fileS, void *data, int32_t bytes) = 0;
    virtual int FileSize(const char *fname) = 0;
    virtual void CloseFile(std::fstream &fileS) = 0;
    virtual void *malloc(int32_t bytes) = 0;
    virtual void free(void *ptr) = 0;

    virtual GEOS::ID CreateTexture(const char *fname) = 0;
    virtual void ReleaseTexture(GEOS::ID tex) = 0;
    virtual void SetMaterial(const GEOS::MATERIAL &mt) = 0;

    virtual GEOS::ID CreateVertexBuffer(int32_t type, int32_t size) = 0;
    virtual void *LockVertexBuffer(GEOS::ID vb) = 0;
    virtual void UnlockVertexBuffer(GEOS::ID vb) = 0;
    virtual void ReleaseVertexBuffer(GEOS::ID vb) = 0;

    virtual GEOS::ID CreateIndexBuffer(int32_t size) = 0;
    virtual void *LockIndexBuffer(GEOS::ID ib) = 0;
    virtual void UnlockIndexBuffer(GEOS::ID ib) = 0;
    virtual void ReleaseIndexBuffer(GEOS::ID ib) = 0;

    virtual void SetIndexBuffer(GEOS::ID ibuff) = 0;
    virtual void SetVertexBuffer(int32_t vsize, GEOS::ID vbuff) = 0;
    virtual void DrawIndexedPrimitive(int32_t minv, int32_t numv, int32_t vrtsize, int32_t startidx, int32_t numtrg) = 0;

    virtual GEOS::ID CreateLight(GEOS::LIGHT) = 0;
    virtual void ActivateLight(GEOS::ID n) = 0;

    virtual void SetCausticMode(bool bSet = false) = 0;
};

enum GEOS_LOADING_FLAGS
{
    LOAD_VISIBLE = 1,
    LOAD_COLLIDE = 2,
    LOAD_GEOACCESS = 4,
    LOAD_FORCEDWORD = 0x7FFFFFFF
};

GEOS *CreateGeometry(const char *fname, const char *lightname, GEOM_SERVICE &srv, int32_t flags);
