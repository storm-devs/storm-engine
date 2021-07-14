/******************************************************************************
File:  rdf.h

Author:   Nick Chirkov
Copyright (C) 2000, 2001 Nick Chirkov

Comments:
geometry file format
******************************************************************************/

#pragma once

#pragma pack(push, 1)

#include "cvector.h"

/*

RDF_HEAD;
NAMES;
textures;
materials;
lights;
labels;
objects;
triangles;
vertex_buffers;
vertices;

BSP_HEAD
BSP_NODE - bsp nodes
BSP_VERTEX - bsp vertices
BSP_TRIANGLE - bsp triangles

*/

//------------------------------------------------------------
// header
//------------------------------------------------------------

#define RDF_VERSION '1.05'

enum RDF_FLAGS
{
    FLAGS_VISIBLE_PRESENT = 1,
    FLAGS_BSP_PRESENT = 2,
    RDFFLAGS_FORCEDWORD = 0x7FFFFFFF
};

struct RDF_HEAD
{
    long version; // RDF_VERSION
    long flags;   // combination of RDF_FLAGS
    long name_size;
    long names;
    long ntextures;
    long nmaterials;
    long nlights;
    long nlabels;
    long nobjects;
    long ntriangles;
    long nvrtbuffs;

    CVECTOR bbox_size, bbox_center;
    float radius;
};

//------------------------------------------------------------
// strings
//------------------------------------------------------------
// char NAMES[name_size];

//------------------------------------------------------------
// names of textures used
//------------------------------------------------------------
struct RDF_TEXTURE
{
    long name;
};

//------------------------------------------------------------
// material
//------------------------------------------------------------
enum RDF_TEXTURE_TYPE
{
    TEXTURE_NONE = 0,
    TEXTURE_BASE,
    TEXTURE_NORMAL,
    TEXTURE_FORCE_DWORD = 0x7FFFFFFF
};

struct RDF_MATERIAL
{
    long group_name;
    long name;
    float diffuse;         // 0 - no diffuse material
    float specular, gloss; // spec=0 - no specular, gloss is a power of cosine
    float selfIllum;       // for area light sources
    RDF_TEXTURE_TYPE texture_type[4];
    long texture[4];
};

//------------------------------------------------------------
// dynamic light
//------------------------------------------------------------
enum RDF_LIGHT_TYPE
{
    LIGHT_POINT = 0,
    LIGHT_SPOT,
    LIGHT_DIRECTIONAL,
    LIGHT_FORCE_DWORD = 0x7FFFFFFF
};

enum RDF_LIGHT_FLAGS
{
    LIGHT_SHADOW = 1,
    LIGHT_FORCEDWORD = 0x7FFFFFFF
};

struct RDF_LIGHT
{
    long flags;
    RDF_LIGHT_TYPE type;
    long group_name;
    long name;
    float r, g, b;
    float range;
    CVECTOR pos;
    float atten[3];
    float inner, outer, falloff;
    CVECTOR dir;
};

//------------------------------------------------------------
// label
//------------------------------------------------------------
enum RDF_LABEL_FLAGS
{
    LABEL_FORCEDWORD = 0x7FFFFFFF
};

struct RDF_LABEL
{
    long group_name;
    long name;
    long flags; // combination of LABEL_FLAGS
    float m[4][4];
    long bones[4];
    float weight[4];
};

//------------------------------------------------------------
// object
//------------------------------------------------------------
enum RDF_OBJECT_FLAGS
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

struct RDF_OBJECT
{
    long group_name;
    long name;
    long flags;
    CVECTOR center;
    float radius;
    long vertex_buff;
    long ntriangles, striangle, nvertices, svertex;
    long material;
    long lights[8];
    long bones[4];
    long atriangles;
};

//------------------------------------------------------------
// triangle 16bit indices
//------------------------------------------------------------
struct RDF_TRIANGLE
{
    unsigned short vindex[3];
};

//------------------------------------------------------------
// vertex buffer descriptor
//------------------------------------------------------------
struct RDF_VERTEXBUFF
{
    long type;
    long size;
};

//------------------------------------------------------------
// vertex type
//------------------------------------------------------------
struct RDF_VERTEX0
{
    CVECTOR pos;
    CVECTOR norm;
    long color;
    float tu0, tv0;
};

struct RDF_VERTEX1
{
    CVECTOR pos;
    CVECTOR norm;
    long color;
    float tu0, tv0;
    float tu1, tv1;
};

struct RDF_VERTEX2
{
    CVECTOR pos;
    CVECTOR norm;
    long color;
    float tu0, tv0;
    float tu1, tv1;
    float tu2, tv2;
};

struct RDF_VERTEX3
{
    CVECTOR pos;
    CVECTOR norm;
    long color;
    float tu0, tv0;
    float tu1, tv1;
    float tu2, tv2;
    float tu3, tv3;
};

//-------------------animated vertex---------------
struct RDF_AVERTEX0
{
    CVECTOR pos;
    float weight;
    unsigned long boneid;
    CVECTOR norm;
    long color;
    float tu0, tv0;
};

//------------------------------------------------------------
// bsp
//------------------------------------------------------------
struct RDF_BSPHEAD
{
    long nnodes;
    long nvertices;
    long ntriangles;
};

struct RDF_BSPTRIANGLE
{
    unsigned char vindex[3][3];
};

#define RDF_BSPVERTEX CVECTOR

struct BSP_NODE
{
    CVECTOR norm;
    float pd;
    unsigned long node : 22, sign : 1, left : 1, nfaces : 4, right : 2, type : 2;

    long face;
};

#pragma pack(pop)
