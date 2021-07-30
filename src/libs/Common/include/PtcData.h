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

#include "math3d.h"
#include "ptc.h"

#include <cstdint>

#define PTCDATA_MAXSTEPS 32

class VDX9RENDER;

class PtcData
{
  public:
    struct Triangle
    {
        long index;
        Vector v[3];
        Vector n;
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
    long FindNode(const Vector &pos, float &y);
    // Moves "pos" to "to", returns a new node
    long Move(long curNode, const Vector &to, Vector &pos, long depth = 0);
    // Get normal to a node
    void GetNodeNormal(long curNode, Vector &n) const;
    // Find the direction of the path
    bool FindPathDir(long curNode, const Vector &cur, long toNode, const Vector &to, long &node, Vector &toPos);
    // Find patch intersection
    float Trace(const Vector &s, const Vector &d) const;
    // Find the force pushing away from the edges
    void FindForce(long curNode, Vector &force) const;
    void FindForce(long curNode, const Vector &pos, float dist, Vector &force) const;
    // Get node material
    const char *GetMaterial(long curNode);

    // Get triangles intersecting a given square
    // Triangle * GetTriangles(float x, float z, float sx, float sz, long & num);

    // Last move results
    bool isSlide;     // Slided
    Vector slideDir; // Sliding direction
    bool isBearing;   // Fell on a bearing

    Vector stepPos[PTCDATA_MAXSTEPS];
    long numSteps;

    Vector middle;

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
    bool FindPathDir(long step, long curNode, const Vector &cur, long toNode, const Vector &to, long &node,
                     Vector &pos);
    // Find a point on an edge
    Vector FindEdgePoint(const Vector &vs, const Vector &ve, const Vector &cur, const Vector &to);
    // Check the intersection of a triangle with a line segment
    float Trace(PtcTriangle &trg, const Vector &s, const Vector &d) const;
    // Add triangle to buffer
    // void AddClTriangle(long i);

    // private:
  public:
    // Data block
    void *data;

    // Geometry
    PtcTriangle *triangle; // Geometry triangles
    long numTriangles;     // Number of geometry triangles

    PtcVertex *vertex; // Vertices of geometry
    long numVerteces;  // Number of geometry vertices

    PtcNormal *normal; // Geometry triangle normals
    long numNormals;   // Number of geometry normals

    PtcMaterials *materials; // Materials

    // Collision data
    Vector min, max; // What to fit into

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
