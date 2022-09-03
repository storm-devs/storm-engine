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

#include "c_vector.h"
#include "ptc.h"

#define PTCDATA_MAXSTEPS 32

class VDX9RENDER;

class PtcData
{
  public:
    struct Triangle
    {
        int32_t index;
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
    int32_t FindNode(const CVECTOR &pos, float &y);
    // Moves "pos" to "to", returns a new node
    int32_t Move(int32_t curNode, const CVECTOR &to, CVECTOR &pos, int32_t depth = 0);
    // Get normal to a node
    void GetNodeNormal(int32_t curNode, CVECTOR &n) const;
    // Find the direction of the path
    bool FindPathDir(int32_t curNode, const CVECTOR &cur, int32_t toNode, const CVECTOR &to, int32_t &node, CVECTOR &toPos);
    // Find patch intersection
    float Trace(const CVECTOR &s, const CVECTOR &d) const;
    // Find the force pushing away from the edges
    void FindForce(int32_t curNode, CVECTOR &force) const;
    void FindForce(int32_t curNode, const CVECTOR &pos, float dist, CVECTOR &force) const;
    // Get node material
    const char *GetMaterial(int32_t curNode);

    // Get triangles intersecting a given square
    // Triangle * GetTriangles(float x, float z, float sx, float sz, int32_t & num);

    // Last move results
    bool isSlide;     // Slided
    CVECTOR slideDir; // Sliding direction
    bool isBearing;   // Fell on a bearing

    CVECTOR stepPos[PTCDATA_MAXSTEPS];
    int32_t numSteps;

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
    float FindHeight(int32_t trgID, float x, float z);
    // Find the direction of the path
    bool FindPathDir(int32_t step, int32_t curNode, const CVECTOR &cur, int32_t toNode, const CVECTOR &to, int32_t &node,
                     CVECTOR &pos);
    // Find a point on an edge
    CVECTOR FindEdgePoint(const CVECTOR &vs, const CVECTOR &ve, const CVECTOR &cur, const CVECTOR &to);
    // Check the intersection of a triangle with a line segment
    float Trace(PtcTriangle &trg, const CVECTOR &s, const CVECTOR &d) const;
    // Add triangle to buffer
    // void AddClTriangle(int32_t i);

    // private:
  public:
    // Data block
    char *data;

    // Geometry
    PtcTriangle *triangle; // Geometry triangles
    int32_t numTriangles;     // Number of geometry triangles

    PtcVertex *vertex; // Vertices of geometry
    int32_t numVerteces;  // Number of geometry vertices

    PtcNormal *normal; // Geometry triangle normals
    int32_t numNormals;   // Number of geometry normals

    PtcMaterials *materials; // Materials

    // Collision data
    CVECTOR min, max; // What to fit into

    PtcMap *map;  // Quick search map
    int32_t l, w;    // Quick Search Map Size
    float ls, ws; // Size of one square

    uint16_t *indeces; // Indexes
    int32_t numIndeces;   // Number of indexes

    // Pathfinding data
    uint8_t *table; // Direction table
    int32_t lineSize;  // Line size

    // Triangles after collision
    Triangle *ctriangle;
    int32_t numClTriangles;
    int32_t maxClTriangles;

    // Debug information
    DbgVertex *dbgTriangles;
    DbgVertex *dbgEdges;
};
