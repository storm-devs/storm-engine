//============================================================================================
//    Spirenkov Maxim, 2002
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    PtcData
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Cvector.h"
#include "ptc.h"
#include "vmodule_api.h"

#define PTCDATA_MAXSTEPS 32

class VDX9RENDER;

class PtcData
{
  public:
    struct Triangle
    {
        long index;
        CVECTOR v[3];
        CVECTOR n;
    };

    struct DbgVertex
    {
        float x, y, z;
        uint32_t c;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    PtcData();
    virtual ~PtcData();

    // load the patch
    bool Load(const char *path);
    // Determine the current position
    long FindNode(const CVECTOR &pos, float &y);
    // Moves "pos" to "to", returns a new node
    long Move(long curNode, const CVECTOR &to, CVECTOR &pos, long depth = 0);
    // Get normal to a node
    void GetNodeNormal(long curNode, CVECTOR &n) const;
    // Find the direction of the path
    bool FindPathDir(long curNode, const CVECTOR &cur, long toNode, const CVECTOR &to, long &node, CVECTOR &toPos);
    // Find patch intersection
    float Trace(const CVECTOR &s, const CVECTOR &d) const;
    // Find the force pushing away from the edges
    void FindForce(long curNode, CVECTOR &force) const;
    void FindForce(long curNode, const CVECTOR &pos, float dist, CVECTOR &force) const;
    // Get node material
    const char *GetMaterial(long curNode);

    // Get triangles intersecting a given square
    // Triangle * GetTriangles(float x, float z, float sx, float sz, long & num);

    // Last move results
    bool isSlide;     // Slided
    CVECTOR slideDir; // Sliding direction
    bool isBearing;   // Fell on a bearing

    CVECTOR stepPos[PTCDATA_MAXSTEPS];
    long numSteps;

    CVECTOR middle;

    // Debug rendering
    void DebugDraw(VDX9RENDER *rs, float dltTime);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Protection function
    void SFLB_PotectionLoad();

    // Calculate the height of a point on the plane of a triangle
    float FindHeight(long trgID, float x, float z);
    // Find the direction of the path
    bool FindPathDir(long step, long curNode, const CVECTOR &cur, long toNode, const CVECTOR &to, long &node,
                     CVECTOR &pos);
    // Find a point on an edge
    CVECTOR FindEdgePoint(const CVECTOR &vs, const CVECTOR &ve, const CVECTOR &cur, const CVECTOR &to);
    // Check the intersection of a triangle with a line segment
    float Trace(PtcTriangle &trg, const CVECTOR &s, const CVECTOR &d) const;
    // Add triangle to buffer
    // void AddClTriangle(long i);

    // private:
  public:
    // Data block
    char *data;

    // Geometry
    PtcTriangle *triangle; // Geometry triangles
    long numTriangles;     // Number of geometry triangles

    PtcVertex *vertex; // Vertices of geometry
    long numVerteces;  // Number of geometry vertices

    PtcNormal *normal; // Geometry triangle normals
    long numNormals;   // Number of geometry normals

    PtcMaterials *materials; // Materials

    // Collision data
    CVECTOR min, max; // What to fit into

    PtcMap *map;  // Quick search map
    long l, w;    // Quick Search Map Size
    float ls, ws; // Size of one square

    uint16_t *indeces; // Indexes
    long numIndeces;   // Number of indexes

    // Pathfinding data
    uint8_t *table; // Direction table
    long lineSize;  // Line size

    // Triangles after collision
    Triangle *ctriangle;
    long numClTriangles;
    long maxClTriangles;

    // Debug information
    DbgVertex *dbgTriangles;
    DbgVertex *dbgEdges;
};
