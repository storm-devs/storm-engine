//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    OctTree
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "oct_tree.h"

#include "storm_assert.h"

//============================================================================================

#define LLOT_MAX 128

OctTree::OTNode::OTNode(const CVECTOR &_min, const CVECTOR &_max)
{
    for (int32_t i = 0; i < 8; i++)
        node[i] = nullptr;
    min = _min;
    max = _max;
    vrt = new Vertex *[LLOT_MAX];
    num = 0;
}

OctTree::OTNode::~OTNode()
{
    delete[] vrt;
    for (int32_t i = 0; i < 8; i++)
        if (node[i])
            delete node[i];
}

// ============================================================================================
// Construction, destruction
// ============================================================================================

OctTree::OctTree()
    : vertsPos(), vertsPosMin(), vertsPosMax(), vertsR(0)
{
    root = nullptr;
    numVerts = 0;
    maxVerts = 0;
    vrt = nullptr;
    numVrt = 0;
}

OctTree::~OctTree()
{
    delete root;
}

// Initialize tree
void OctTree::Init(LGeometry *g)
{
    vrt = g->vrt.data();
    numVrt = g->numVrt;
    root = new OTNode(g->min, g->max);
    for (int32_t i = 0; i < numVrt; i++)
        AddVertex(root, &vrt[i]);
    Optimize(root);
    /*
    for(i = 0; i < numVrt; i++)
    {
      int32_t num = Check(root, vrt + i, 0);
      Assert(num == 1);
    }
    */
}

int32_t OctTree::Check(OTNode *node, Vertex *v, int32_t num)
{
    if (node->vrt)
    {
        for (int32_t i = 0; i < node->num; i++)
            if (node->vrt[i] == v)
                num++;
    }
    else
    {
        for (int32_t i = 0; i < 8; i++)
            if (node->node[i])
                num += Check(node->node[i], v, 0);
    }
    return num;
}

bool OctTree::AddVertex(OTNode *node, Vertex *v)
{
    auto &min = node->min;
    auto &max = node->max;
    if (v->p.x < min.x || v->p.x > max.x)
        return false;
    if (v->p.y < min.y || v->p.y > max.y)
        return false;
    if (v->p.z < min.z || v->p.z > max.z)
        return false;
    if (node->vrt)
    {
        if (node->num < LLOT_MAX)
        {
            // Add to the current node
            node->vrt[node->num++] = v;
            return true;
        }
        // Overflow, must be distributed among children
        const auto cnt = (node->min + node->max) * 0.5f;
        node->node[0] = new OTNode(CVECTOR(min.x, min.y, min.z), CVECTOR(cnt.x, cnt.y, cnt.z));
        node->node[1] = new OTNode(CVECTOR(min.x, min.y, cnt.z), CVECTOR(cnt.x, cnt.y, max.z));
        node->node[2] = new OTNode(CVECTOR(cnt.x, min.y, cnt.z), CVECTOR(max.x, cnt.y, max.z));
        node->node[3] = new OTNode(CVECTOR(cnt.x, min.y, min.z), CVECTOR(max.x, cnt.y, cnt.z));
        node->node[4] = new OTNode(CVECTOR(min.x, cnt.y, min.z), CVECTOR(cnt.x, max.y, cnt.z));
        node->node[5] = new OTNode(CVECTOR(min.x, cnt.y, cnt.z), CVECTOR(cnt.x, max.y, max.z));
        node->node[6] = new OTNode(CVECTOR(cnt.x, cnt.y, cnt.z), CVECTOR(max.x, max.y, max.z));
        node->node[7] = new OTNode(CVECTOR(cnt.x, cnt.y, min.z), CVECTOR(max.x, max.y, cnt.z));
        for (int32_t n = 0; n < node->num; n++)
        {
            int32_t c;
            for (c = 0; c < 8; c++)
            {
                if (AddVertex(node->node[c], node->vrt[n]))
                    break;
            }
            Assert(c < 8);
        }
        delete node->vrt;
        node->vrt = nullptr;
        node->num = 0;
    }
    // Adding children
    int32_t c;
    for (c = 0; c < 8; c++)
    {
        if (AddVertex(node->node[c], v))
            break;
    }
    Assert(c < 8);
    return true;
}

// Optimizing the tree
void OctTree::Optimize(OTNode *node)
{
    if (!node->vrt)
    {
        for (int32_t i = 0; i < 8; i++)
        {
            if (!node->node[i])
                continue;
            if (node->node[i]->vrt)
            {
                if (node->node[i]->num <= 0)
                {
                    delete node->node[i];
                    node->node[i] = nullptr;
                }
            }
            else
                Optimize(node->node[i]);
        }
    }
}

// Find vertices in a given radius
void OctTree::FindVerts(const CVECTOR &pos, float r)
{
    numVerts = 0;
    vertsPos = pos;
    vertsR = r * r;
    r += 0.000001f;
    vertsPosMin = pos - CVECTOR(r);
    vertsPosMax = pos + CVECTOR(r);
    if (root)
        FindVerts(root);
}

// Search
void OctTree::FindVerts(OTNode *node)
{
    auto &min = node->min;
    auto &max = node->max;
    // Preliminary check
    if (vertsPosMin.x > max.x)
        return;
    if (vertsPosMax.x < min.x)
        return;
    if (vertsPosMin.y > max.y)
        return;
    if (vertsPosMax.y < min.y)
        return;
    if (vertsPosMin.z > max.z)
        return;
    if (vertsPosMax.z < min.z)
        return;
    // Refined check

    // If do not have our own array, send it to the children
    if (node->vrt == nullptr)
    {
        for (int32_t i = 0; i < 8; i++)
            if (node->node[i])
                FindVerts(node->node[i]);
    }
    else
    {
        for (int32_t i = 0; i < node->num; i++)
        {
            const auto r = ~(node->vrt[i]->p - vertsPos);
            if (r < vertsR)
            {
                if (numVerts >= maxVerts)
                {
                    maxVerts += 1024;
                    verts.resize(maxVerts);
                }
                verts[numVerts].v = node->vrt[i];
                verts[numVerts++].r2 = r;
            }
        }
    }
}
