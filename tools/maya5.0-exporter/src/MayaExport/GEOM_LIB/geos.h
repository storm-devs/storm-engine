/******************************************************************************
File:  geos.h

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
Import library main header
******************************************************************************/

#ifndef _GEOLIB_I_H_
#define _GEOLIB_I_H_

class GEOS
{
public:
	typedef unsigned long ID;	//this used w/o any checking

	struct VERTEX
	{
		float x,y,z;
	};
	struct TRIANGLE
	{
		long v[3];
	};
	struct EDGE
	{
		long v[2];
	};
	//vertex
	struct VERTEX0
	{
		VERTEX pos, nrm;
		long color;
		float tu,tv;
	};
	struct VERTEX1 : public VERTEX0
	{
		float tu1,tv1;
	};
	struct VERTEX2 : public VERTEX1
	{
		float tu2,tv2;
	};
	struct VERTEX3 : public VERTEX0
	{
		float tu3,tv3;
	};
	//animated vertex
	struct AVERTEX0
	{
		VERTEX pos;
		float weight;
		unsigned long boneid;
		VERTEX nrm;
		long color;
		float tu0, tv0;
	};
	struct AVERTEX1 : public AVERTEX0
	{
		float tu1,tv1;
	};
	struct AVERTEX2 : public AVERTEX1
	{
		float tu2,tv2;
	};
	struct AVERTEX3 : public AVERTEX0
	{
		float tu3,tv3;
	};

	virtual long FindName(const char *name) const = 0;

	//-----------------------------------------
	//get info of entire object
	//-----------------------------------------
	struct INFO
	{
		long ntextures;
		long nmaterials;
		long nlights;
		long nobjects;
		long ntriangles;
		long nvrtbuffs;
		long nlabels;
		VERTEX boxcenter, boxsize;
		float radius;
		long ga_vertices, ga_vertex_triangles, ga_edges, ga_edge_triangles;
	};
	virtual void GetInfo(INFO &i) const = 0;

	//-----------------------------------------
	//label
	//-----------------------------------------
	enum LABEL_FLAGS
	{
		LABLE_FORCEDWORD = 0x7FFFFFFF
	};
	struct LABEL
	{
		long flags;	//combination of LABEL_FLAG
		float m[4][4];
		char *group_name;
		char *name;
		long bones[4];
		float weight[4];
	};
	virtual long FindLabelN(long start_index, long name_id) = 0;
	virtual long FindLabelG(long start_index, long group_name_id) = 0;
	virtual void GetLabel(long l, LABEL &lb) const = 0;
	virtual void SetLabel(long l, const LABEL &lb) = 0;

	//-----------------------------------------
	//materials
	//-----------------------------------------
	enum BRDF_TYPE
	{
		BRDF_PHONG = 0,
		BRDF_METALL,
		BRDF_BLINN,
		BRDF_FORCEDWORD = 0x7FFFFFFF
	};
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
		BRDF_TYPE brdf;
		float diffuse;	//0 - no diffuse material
		float specular, gloss;	//spec=0 - no specular, gloss is a power of cosine
		float selfIllum;	//for area light sources
		float transparency, refraction_id;	//0 - no refraction, refid is a K of refraction
		float reflection;	//0 for non-mirrors
		TEXTURE_TYPE texture_type[4];	//type of texture
		long texture[4];
		char *group_name;
		char *name;
	};
	virtual long FindMaterialN(long start_index, long name_id) = 0;
	virtual long FindMaterialG(long start_index, long group_name_id) = 0;
	virtual void GetMaterial(long m, MATERIAL &mt) const = 0;
	virtual void SetMaterial(long m, const MATERIAL &mt) = 0;
	typedef void (*MATERIAL_FUNC)(const MATERIAL &mt);

	//-----------------------------------------
	//object
	//-----------------------------------------
	enum OBJECT_FLAGS
	{
		VISIBLE = (1<<0),
		STATIC_LIGHT_ENABLE = (1<<1),	//if no - object will not be lited
		DINAMIC_LIGHT_ENABLE = (1<<2),	//if no - no dynamic lighting perfomed
		CAST_SHADOWS_ENABLE = (1<<3),	//if material makes shadows
		COLLISION_ENABLE = (1<<4),	//if yes - object will be stored to BSP

		//geometry access flags
		GEO_VERTEX = (1<<5),	//vertices will be stored to geoaccess
		GEO_VERTEX_TRIANGLE = (1<<6),	//triangles and vertices will be stored to geoaccess
		GEO_EDGE = (1<<7),	//edges and vertices will be stored to geoaccess
		GEO_EDGE_TRIANGLE = (1<<8),	//triangles, edges and vertices will be stored to geoaccess

		VERTEX_WEIGHT = (1<<9),	//animation
		MERGE = (1<<10),	//object is "merged"-type
		CULLENABLE = (1<<11),	//single-sided object
		OBJECT_FORCEDWORD = 0x7FFFFFFF
	};
	struct OBJECT
	{
		long flags;	//combination of OBJECT_FLAGSS
		VERTEX center;
		float radius;
		long material;
		char *group_name;
		char *name;

		long ntriangles, striangle;
		ID vertex_buff;
		long start_vertex, num_vertices;
		long bones[4];
	};
	virtual long FindObjN(long start_index, long name_id) = 0;
	virtual long FindObjG(long start_index, long group_name_id) = 0;
	virtual void GetObj(long o, OBJECT &ob) const = 0;
	virtual void SetObj(long o, const OBJECT &ob) = 0;

	//-----------------------------------------
	//light sources
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
		long flags;	//combination of LIGHT_FLAGS
		LIGHT_TYPE type;
		long id;
		char *group_name;
		char *name;
		float r,g,b;
		float range;
		VERTEX pos;
		float atten[3];
		float inner, outer, falloff;
		VERTEX dir;
	};
	virtual long FindLightN(long start_index, long name_id) = 0;
	virtual long FindLightG(long start_index, long group_name_id) = 0;
	virtual void GetLight(long l, LIGHT &lt) const = 0;
	virtual void SetLight(long l, const LIGHT &lt) = 0;

	//-----------------------------------------
	//draw
	//-----------------------------------------
	struct PLANE
	{
		VERTEX nrm;
		float d;
	};
	virtual void Draw(const PLANE *pl, long np, MATERIAL_FUNC mtf) const = 0;

	//-----------------------------------------
	//collision, ray shooting
	//-----------------------------------------
	struct TRACE_INFO
	{
		float a, b;	//valid only for ray tracing
		long obj, trg;
		PLANE plane;
		VERTEX vrt[3];
	};
	//Trace a ray in local coord-system
	virtual float Trace(VERTEX &src, VERTEX &dst) = 0;

	//clip in local coord-system
	typedef bool (*ADD_POLYGON_FUNC)(const VERTEX *v, long nv);
	virtual bool Clip(const PLANE *planes, long nplanes, const VERTEX &center, float radius, ADD_POLYGON_FUNC addpoly) = 0;

	//Get detail info on last ray hit or clip
	virtual void GetCollisionDetails(TRACE_INFO &ti) const = 0;

	//geoaccess functions
	virtual void GetVertex(long v, VERTEX &vr) const = 0;	//vertices; no storage cost if BSP enabled
	virtual void GetVertexTriangle(long t, TRIANGLE &tr) const = 0;	//triangles->vertices; no storage cost if BSP enabled
	virtual void GetEdge(long e, EDGE &ed) const = 0;	//edge->vertices, costs 6 bytes/edge
	virtual void GetEdgeTriangle(long t, TRIANGLE &tr) const = 0;	//triangle->edge->vertices, costs 9 bytes/edge

	//-----------------------------------------
	//all other
	//-----------------------------------------

	//Get ID of texture
	virtual long FindTexture(long start_index, long name_id) = 0;
	virtual long GetTexture(long tx) const = 0;
	virtual const char *GetTextureName(long tx) const = 0;

	//GetVertexBuffer
	virtual long GetVertexBuffer(long vb) const = 0;

	//GetIndexBuffer
	virtual long GetIndexBuffer() const = 0;

	//-----------------------------------------
	//unused
	//-----------------------------------------
	virtual ~GEOS(){};

};


class GEOM_SERVICE
{
public:
	virtual ~GEOM_SERVICE(){};
	virtual GEOS::ID OpenFile(const char *fname) = 0;
	virtual void ReadFile(GEOS::ID file, void *data, long bytes) = 0;
	virtual void CloseFile(GEOS::ID file) = 0;
	virtual void *malloc(long bytes) = 0;
	virtual void free(void *ptr) = 0;

	virtual GEOS::ID CreateTexture(const char *fname) = 0;
	virtual void ReleaseTexture(GEOS::ID tex) = 0;
	virtual void SetMaterial(const GEOS::MATERIAL &mt) = 0;

	virtual GEOS::ID CreateVertexBuffer(long type, long size) = 0;
	virtual void *LockVertexBuffer(GEOS::ID vb) = 0;
	virtual void UnlockVertexBuffer(GEOS::ID vb) = 0;
	virtual void ReleaseVertexBuffer(GEOS::ID vb) = 0;

	virtual GEOS::ID CreateIndexBuffer(long size) = 0;
	virtual void *LockIndexBuffer(GEOS::ID ib) = 0;
	virtual void UnlockIndexBuffer(GEOS::ID ib) = 0;
	virtual void ReleaseIndexBuffer(GEOS::ID ib) = 0;

	virtual void SetIndexBuffer(GEOS::ID ibuff) = 0;
	virtual void SetVertexBuffer(long vsize, GEOS::ID vbuff) = 0;
	virtual void DrawIndexedPrimitive(long minv, long numv, long vrtsize, long startidx, long numtrg) = 0;

	virtual GEOS::ID CreateLight(const GEOS::LIGHT) = 0;
	virtual void ActivateLight(GEOS::ID n) = 0;
};

enum GEOS_LOADING_FLAGS
{
	LOAD_VISIBLE = 1,
	LOAD_COLLIDE = 2,
	LOAD_GEOACCESS = 4,
	LOAD_FORCEDWORD = 0x7FFFFFFF
};

GEOS* _cdecl CreateGeometry(const char *fname, const char *lightname, GEOM_SERVICE &srv, long flags);

#endif