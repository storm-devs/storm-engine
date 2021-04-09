//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    OctTree
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "LGeometry.h"

class OctTree
{

    struct OTNode
    {
        OTNode(const CVECTOR &_min, const CVECTOR &_max);
        ~OTNode();
        OTNode *node[8]; // Children
        CVECTOR min;     // Minimum value
        CVECTOR max;     // Maximum value
        Vertex **vrt;    // Vertex indices
        long num;        // Number of vertices
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    OctTree();
    virtual ~OctTree();

    // Initialize tree
    void Init(LGeometry *g);
    // Find vertices in a given radius
    void FindVerts(const CVECTOR &pos, float r);

    std::vector<OctFndVerts> verts;
    long numVerts;
    long maxVerts;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Adding vertices
    bool AddVertex(OTNode *node, Vertex *v);
    // Optimizing the tree
    void Optimize(OTNode *node);
    // Search
    void FindVerts(OTNode *node);

    long Check(OTNode *node, Vertex *v, long num);

  private:
    Vertex *vrt;
    long numVrt;
    OTNode *root;
    CVECTOR vertsPos, vertsPosMin, vertsPosMax;
    float vertsR;
};
