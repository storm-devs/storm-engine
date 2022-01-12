//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    PtcData
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "ptc_data.h"

#include <chrono>

#include "core.h"
#include "dx9render.h"
#include "storm_assert.h"
#include "v_file_service.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

PtcData::PtcData()
    : isSlide(false), slideDir(), isBearing(false), stepPos{}
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    data = nullptr;
    triangle = nullptr;
    numTriangles = 0;
    vertex = nullptr;
    numVerteces = 0;
    normal = nullptr;
    numNormals = 0;
    min = max = 0.0f;
    map = nullptr;
    l = w = 0;
    ls = ws = 0.0f;
    indeces = nullptr;
    numIndeces = 0;
    table = nullptr;
    lineSize = 0;
    ctriangle = nullptr;
    numClTriangles = 0;
    maxClTriangles = 0;
    dbgTriangles = nullptr;
    dbgEdges = nullptr;
    numSteps = 0;
    materials = nullptr;
    middle = 0.0f;
}

PtcData::~PtcData()
{
    delete[] data;
    delete ctriangle;
    delete dbgTriangles;
    delete dbgEdges;
}

bool PtcData::Load(const char *path)
{
    Assert(data == nullptr);
    char *buf = nullptr;
    uint32_t size = 0;
    middle = 0.0f;
    // Loading data
    if (!fio->LoadFile(path, &buf, &size))
    {
        core.Trace("Ptc(\"%s\") -> file not found", path);
        return false;
    }
    // Checking the file for correctness
    if (!buf || size < sizeof(PtcHeader))
    {
        core.Trace("Ptc(\"%s\") -> invalide file size", path);
        delete buf;
        return false;
    }
    auto &hdr = *(PtcHeader *)buf;
    if (hdr.id != PTC_ID)
    {
        core.Trace("Ptc(\"%s\") -> invalide file ID", path);
        delete buf;
        return false;
    }
    if (hdr.ver != PTC_VERSION && hdr.ver != PTC_PREVERSION1)
    {
        core.Trace("Ptc(\"%s\") -> invalide file version", path);
        delete buf;
        return false;
    }
    uint32_t tsize = sizeof(PtcHeader);
    tsize += hdr.numTriangles * sizeof(PtcTriangle);
    tsize += hdr.numVerteces * sizeof(PtcVertex);
    tsize += hdr.numNormals * sizeof(PtcNormal);
    tsize += hdr.mapL * hdr.mapW * sizeof(PtcMap);
    tsize += hdr.numIndeces * sizeof(uint16_t);
    tsize += hdr.lineSize * hdr.numTriangles * sizeof(uint8_t);
    if (hdr.ver == PTC_VERSION)
        tsize += sizeof(PtcMaterials);
    if (tsize != size)
    {
        core.Trace("Ptc(\"%s\") -> invalide file size", path);
        delete buf;
        return false;
    }
    if (hdr.numTriangles < 1 || hdr.numVerteces < 3 || hdr.numNormals < 1 || hdr.mapL < 1 || hdr.mapW < 1 ||
        hdr.numIndeces < 1 || hdr.lineSize < 1 || hdr.minX >= hdr.maxX || hdr.minY > hdr.maxY || hdr.minZ >= hdr.maxZ)
    {
        core.Trace("Ptc(\"%s\") -> invalide file header", path);
        delete buf;
        return false;
    }
    // form data structures
    data = buf;
    SFLB_PotectionLoad();
    return true;
}

// Protection function
void PtcData::SFLB_PotectionLoad()
{
    // Data
    auto *const buf = static_cast<char *>(data);
    auto &hdr = *(PtcHeader *)buf;
    // Triangles
    uint32_t tsize = sizeof(PtcHeader);
    triangle = (PtcTriangle *)(buf + tsize);
    numTriangles = hdr.numTriangles;
    // Vertices
    tsize += hdr.numTriangles * sizeof(PtcTriangle);
    vertex = (PtcVertex *)(buf + tsize);
    numVerteces = hdr.numVerteces;
    // Normals
    tsize += hdr.numVerteces * sizeof(PtcVertex);
    normal = (PtcNormal *)(buf + tsize);
    numNormals = hdr.numNormals;
    // Collision map
    tsize += hdr.numNormals * sizeof(PtcNormal);
    map = (PtcMap *)(buf + tsize);
    l = hdr.mapL;
    w = hdr.mapW;
    min.x = hdr.minX;
    min.y = hdr.minY;
    min.z = hdr.minZ;
    max.x = hdr.maxX;
    max.y = hdr.maxY;
    max.z = hdr.maxZ;
    ls = (max.z - min.z) / l;
    ws = (max.x - min.x) / w;
    tsize += hdr.mapL * hdr.mapW * sizeof(PtcMap);
    indeces = (uint16_t *)(buf + tsize);
    // Pathfinding data
    tsize += hdr.numIndeces * sizeof(uint16_t);
    table = (uint8_t *)(buf + tsize);
    lineSize = hdr.lineSize;
    // Materials
    if (hdr.ver == PTC_VERSION)
        materials = (PtcMaterials *)(table + lineSize * numTriangles);
    // Looking for the midpoint
    middle = 0.0f;
    for (int32_t i = 0; i < numVerteces; i++)
    {
        middle.x += vertex[i].x;
        middle.y += vertex[i].y;
        middle.z += vertex[i].z;
    }
    if (numVerteces)
        middle *= 1.0f / numVerteces;
}

// Determine the current position
int32_t PtcData::FindNode(const CVECTOR &pos, float &y)
{
    // Position on the map
    const auto mapX = static_cast<int32_t>((pos.x - min.x) / ws);
    const auto mapZ = static_cast<int32_t>((pos.z - min.z) / ls);
    if (mapX < 0 || mapX >= w)
        return -1;
    if (mapZ < 0 || mapZ >= l)
        return -1;
    // Looping through triangles
    PtcMap &m = map[mapZ * w + mapX];
    int32_t node = -1;
    float h = 0.0f;
    float dist = -1.0f;
    for (int32_t i = 0; i < m.size; i++)
    {
        PtcTriangle &trg = triangle[indeces[m.start + i]];
        // Checking hitting the triangle
        int32_t j;
        for (j = 0; j < 3; j++)
        {
            // Edge vertices
            CVECTOR &vs = *(CVECTOR *)&vertex[trg.i[j]];
            CVECTOR &ve = *(CVECTOR *)&vertex[trg.i[j == 2 ? 0 : j + 1]];
            // Edge normal
            const float nx = -(ve.z - vs.z);
            const float nz = (ve.x - vs.x);
            // If outside the triangle, break
            if (pos.x * nx + pos.z * nz > vs.x * nx + vs.z * nz)
                break;
        }
        if (j == 3)
        {
            // Check the height
            const float d = FindHeight(indeces[m.start + i], pos.x, pos.z);
            if (dist >= 0.0f)
            {
                if (dist > fabsf(pos.y - d))
                {
                    dist = fabsf(pos.y - d);
                    h = d;
                    node = indeces[m.start + i];
                }
            }
            else
            {
                dist = fabsf(pos.y - d);
                h = d;
                node = indeces[m.start + i];
            }
        }
    }
    y = h;
    return node;
}

// Move pos to to, returns a new node
int32_t PtcData::Move(int32_t curNode, const CVECTOR &to, CVECTOR &pos, int32_t depth)
{
    isSlide = false;
    isBearing = false;
    if (curNode < 0)
        return -1;
    Assert(curNode < numTriangles);
    // Direction of movement in 2D
    CVECTOR dir(to.x - pos.x, 0.0f, to.z - pos.z);
    CVECTOR ps = pos;
    int32_t firstNode = curNode;
    // path length in 2D
    float dst = ~dir;
    if (dst == 0.0f || depth > 32)
    {
        if (to.y != pos.y)
        {
            float hy = FindHeight(curNode, pos.x, pos.z);
            pos.y = to.y;
            if (pos.y < hy)
                pos.y = hy;
        }
        return curNode;
    }
    dst = sqrtf(dst);
    dir *= 1.0f / dst;
    // Vertical plane passing through the displacement segment
    CVECTOR nd(dir.z, 0.0f, -dir.x);
    float d = (nd | pos);
    // Trace the way
    int32_t fromNode = -2; // Where we came from
    int32_t loopCounter;
    for (loopCounter = 0; loopCounter < 256; loopCounter++)
    {
        // Check the location of the arrival point on the current triangle
        uint16_t *trg = triangle[curNode].i;
        int32_t j;
        for (j = 0; j < 3; j++)
        {
            // Edge vertices
            CVECTOR &vs = *(CVECTOR *)&vertex[trg[j]];
            CVECTOR &ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
            // Edge normal
            float nx = -(ve.z - vs.z);
            float nz = (ve.x - vs.x);
            float nl = sqrtf(nx * nx + nz * nz);

            if (!nl)
                continue;
            nx /= nl;
            nz /= nl;

            // If outside the triangle, break
            if (to.x * nx + to.z * nz > vs.x * nx + vs.z * nz)
                break;
        }
        if (j == 3)
        {
            // can move
            pos = to;
            // Check the height
            float h = FindHeight(curNode, pos.x, pos.z);
            if (pos.y < h)
            {
                pos.y = h;
                isBearing = true;
            }
            return curNode;
        }
        // Find the edge through which to go further
        short *nb = triangle[curNode].nb;
        int32_t curEdge = -1;
        CVECTOR pnt;
        for (j = 0; j < 3; j++)
        {
            // skip the direction we came from
            if (nb[j] == fromNode)
                continue;
            // Edge vertices
            CVECTOR &vs = *(CVECTOR *)&vertex[trg[j]];
            CVECTOR &ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
            // Distances to the movement line
            float d1 = (nd | vs) - d;
            float d2 = (nd | ve) - d;
            if (d1 * d2 > 0.0f)
                continue;
            if (d1 - d2 == 0.0f)
                continue;
            // Crosses the line
            double k = d1 / static_cast<double>(d1 - d2);
            if (k < 0.0f)
                k = 0.0f;
            if (k > 1.0f)
                k = 1.0f;
            // Intersection point
            CVECTOR p;
            // k += 0.000000001;
            p.x = static_cast<float>(vs.x + (ve.x - vs.x) * k);
            p.y = static_cast<float>(vs.y + (ve.y - vs.y) * k);
            p.z = static_cast<float>(vs.z + (ve.z - vs.z) * k);
            // Checking hitting the path
            if ((d1 = (dir | (p - ps))) < -0.00001f)
                continue;
            if ((d2 = (dir | (to - p))) < -0.00001f)
                continue;
            // Found an intersection
            if (nb[j] >= 0)
            {
                curEdge = j;
                pnt = p;
                break;
            }
            curEdge = j;
            pnt = p;
            if (d1 != 0.0f && d2 != 0.0f)
                break;
        }
        if (curEdge == -1)
        {
            isSlide = true;
            return firstNode;

            Assert(false);
            for (j = 0; j < 3; j++)
            {
                // skip the direction we came from
                if (nb[j] == fromNode)
                    continue;
                // Edge vertices
                CVECTOR &vs = *(CVECTOR *)&vertex[trg[j]];
                CVECTOR &ve = *(CVECTOR *)&vertex[trg[j == 2 ? 0 : j + 1]];
                // Distances to the movement line
                float d1 = (nd | vs) - d;
                float d2 = (nd | ve) - d;
                if (d1 * d2 > 0.0f)
                    continue;
                if (d1 - d2 == 0.0f)
                    continue;
                // Crosses the line
                double k = d1 / static_cast<double>(d1 - d2);
                if (k < 0.0f)
                    k = 0.0f;
                if (k > 1.0f)
                    k = 1.0f;
                // Intersection point
                CVECTOR p;
                // k += 0.000000001;
                p.x = static_cast<float>(vs.x + (ve.x - vs.x) * k);
                p.y = static_cast<float>(vs.y + (ve.y - vs.y) * k);
                p.z = static_cast<float>(vs.z + (ve.z - vs.z) * k);
                // Checking for hitting the path
                d1 = dir | (p - ps);
                d2 = dir | (to - p);
                if (d1 < -0.00001f)
                    continue;
                if (d2 < -0.00001f)
                    continue;
                // Found an intersection
                if (nb[j] >= 0)
                {
                    curEdge = j;
                    pnt = p;
                    break;
                }
                else
                {
                    curEdge = j;
                    pnt = p;
                    if (d1 != 0.0f && d2 != 0.0f)
                        break;
                }
            }
        }

        Assert(curEdge >= 0);

        if (nb[curEdge] >= 0)
        {
            // Go to the next node and continue moving
            // pos = pnt;
            fromNode = curNode;
            curNode = nb[curEdge];
        }
        else
        {
            // The edge along which need to slide
            CVECTOR &vs = *(CVECTOR *)&vertex[trg[curEdge]];
            CVECTOR &ve = *(CVECTOR *)&vertex[trg[curEdge == 2 ? 0 : curEdge + 1]];
            // Edge normal
            float nx = -(ve.z - vs.z);
            float nz = (ve.x - vs.x);
            // Normalize
            float d = nx * nx + nz * nz;
            if (d == 0.0f)
            {
                // Abnormal situation
                core.Trace("Patch have some problem -> triangle edge by zero length");
                // Just stop
                pos = pnt;
                return curNode;
            }
            d = 1.0f / sqrtf(d);
            nx *= d;
            nz *= d;
            // Out-of-triangle vector
            CVECTOR sld = to - pnt;
            sld.y = 0.0f;
            // Sliding distance
            d = nx * sld.x + nz * sld.z;
            if (d > 0.0f)
                d += 0.001f;
            if (d < 0.0f)
                d -= 0.001f;
            // Slide
            CVECTOR newPos(to.x - nx * d, to.y, to.z - nz * d);
            if (newPos.x == to.x && newPos.z == to.z)
            {
                isSlide = true;
                return firstNode;
                Assert(false);
            }
            newPos = pos + (newPos - pos) * 0.99f;
            curNode = Move(firstNode, newPos, pos, depth + 1);
            isSlide = true;
            if (d >= 0.0f)
            {
                slideDir.x = nx;
                slideDir.y = 0.0f;
                slideDir.z = nz;
            }
            else
            {
                slideDir.x = -nx;
                slideDir.y = 0.0f;
                slideDir.z = -nz;
            }
            return curNode;
        }
    }
    if (loopCounter >= 256) //~!~
    {
        if (to.y != pos.y)
        {
            float hy = FindHeight(curNode, pos.x, pos.z);
            pos.y = to.y;
            if (pos.y < hy)
                pos.y = hy;
        }
        return curNode;
    }
    return -1;
}

// Get normal to a node
void PtcData::GetNodeNormal(int32_t curNode, CVECTOR &n) const
{
    if (curNode >= 0)
    {
        Assert(curNode < numTriangles);
        PtcNormal &nrm = normal[triangle[curNode].n];
        n.x = nrm.x;
        n.y = nrm.y;
        n.z = nrm.z;
    }
    else
    {
        n = 0.0f;
    }
}

// Find the direction of the path
bool PtcData::FindPathDir(int32_t curNode, const CVECTOR &cur, int32_t toNode, const CVECTOR &to, int32_t &node, CVECTOR &toPos)
{
    numSteps = 0;
    if (curNode < 0 || toNode < 0)
        return false;
    if (FindPathDir(0, curNode, cur, toNode, to, node, toPos))
        return true;
    toPos = to;
    return false;
}

// Find the direction of the path
bool PtcData::FindPathDir(int32_t step, int32_t curNode, const CVECTOR &cur, int32_t toNode, const CVECTOR &to, int32_t &node,
                          CVECTOR &pos)
{
    if (step > numSteps)
        numSteps = step;
    Assert(curNode >= 0);
    Assert(toNode >= 0);
    // If so, return the destination point
    if (curNode == toNode || step >= PTCDATA_MAXSTEPS)
    {
        pos = to;
        node = toNode;
        return true;
    }
    // Determine in which direction to move (edge)
    Assert(curNode < numTriangles);
    Assert(toNode < numTriangles);
    uint8_t *line = table + curNode * lineSize;
    const uint8_t v = (line[toNode >> 2] >> ((toNode & 3) * 2)) & 3;
    if (v == 3)
        return false;
    // Edge
    CVECTOR &vs = *(CVECTOR *)&vertex[triangle[curNode].i[v]];
    CVECTOR &ve = *(CVECTOR *)&vertex[triangle[curNode].i[v + 1 < 3 ? v + 1 : 0]];
    // Point on the edge
    const CVECTOR p = FindEdgePoint(vs, ve, cur, to);
    // analyze the further path
    const int32_t nb = triangle[curNode].nb[v];
    if (nb < 0)
        return false;
    if (!FindPathDir(step + 1, nb, p, toNode, to, node, pos))
        return false;
    // Refined point on the edge
    pos = FindEdgePoint(vs, ve, cur, pos);
    stepPos[step] = pos;
    node = nb;
    return true;
}

// Find a point on an edge
CVECTOR PtcData::FindEdgePoint(const CVECTOR &vs, const CVECTOR &ve, const CVECTOR &cur, const CVECTOR &to)
{
    CVECTOR pos;
    // Normal to the edge
    CVECTOR edge = ve - vs;
    edge.y = 0.0f;
    CVECTOR nrm(edge.z, 0.0f, -edge.x);
    const float nl = ~nrm;
    if (nl > 0.0f)
    {
        // Plane
        nrm *= 1.0f / sqrtf(nl);
        const float dist = nrm | vs;
        // Destination position
        const float dTo = (to | nrm) - dist;
        if (dTo <= 0.0f)
        {
            // Destination behind the edge
            const float dCur = (cur | nrm) - dist;
            if (dCur - dTo == 0.0f)
                return (vs + ve) * 0.5f;
            float k = dCur / (dCur - dTo);
            pos = cur + (to - cur) * k;
            // Limiting the move point
            CVECTOR p = vs + (ve - vs) * 0.001f;
            k = (ve - p) | (pos - p);
            if (k <= 0.0f)
                pos = p;
            p = vs + (ve - vs) * 0.999f;
            k = (vs - p) | (pos - p);
            if (k <= 0.0f)
                pos = p;
        }
        else
        {
            // Destination in front of the edge
            if (((to - cur) | edge) >= 0.0f)
            {
                // Selecting the end point of the edge
                pos = vs + (ve - vs) * 0.99f;
            }
            else
            {
                // Selecting the starting point of the edge
                pos = vs + (ve - vs) * 0.01f;
            }
        }
    }
    else
    {
        pos = (vs + ve) * 0.5f;
    }
    return pos;
}

// Find patch intersection
float PtcData::Trace(const CVECTOR &s, const CVECTOR &d) const
{
    // The area describing the segment
    float k = 2.0f;
    CVECTOR pmin = s;
    if (pmin.x > d.x)
        pmin.x = d.x;
    if (pmin.y > d.y)
        pmin.y = d.y;
    if (pmin.z > d.z)
        pmin.z = d.z;
    CVECTOR pmax = s;
    if (pmax.x < d.x)
        pmax.x = d.x;
    if (pmax.y < d.y)
        pmax.y = d.y;
    if (pmax.z < d.z)
        pmax.z = d.z;
    // Coordinates on the map
    const int32_t mnX = static_cast<int32_t>((pmin.x - min.x) / ws);
    const int32_t mnZ = static_cast<int32_t>((pmin.z - min.z) / ls);
    const int32_t mxX = static_cast<int32_t>((pmax.x - min.x) / ws);
    const int32_t mxZ = static_cast<int32_t>((pmax.z - min.z) / ls);
    // go through all the triangles in the zone
    for (int32_t zi = mnZ; zi <= mxZ; zi++)
    {
        if (zi < 0 || zi >= l)
            continue;
        for (int32_t xi = mnX; xi <= mxX; xi++)
        {
            if (xi < 0 || xi >= w)
                continue;
            PtcMap &m = map[zi * w + xi];
            uint16_t *ids = indeces + m.start;
            for (int32_t i = 0; i < m.size; i++)
            {
                const float kn = Trace(triangle[ids[i]], s, d);
                if (kn < k)
                    k = kn;
            }
        }
    }
    return k;
}

// Check the intersection of a triangle with a line segment
float PtcData::Trace(PtcTriangle &trg, const CVECTOR &s, const CVECTOR &d) const
{
    // Normal to triangle
    CVECTOR &n = *(CVECTOR *)&normal[trg.n];
    // Distance to triangle
    const float dst = n | *(CVECTOR *)&vertex[trg.i[0]];
    // Distance of the segment to the plane
    const float d1 = (n | s) - dst;
    const float d2 = (n | d) - dst;
    // If do not cross the plane, then missed
    if (d1 * d2 > 0.0f)
        return 2.0f;
    // The point of intersection of the ray with the plane
    float k;
    if (d1 - d2 != 0.0f)
        k = d1 / (d1 - d2);
    else
        k = 0.0f;
    if (k < 0.0f)
        k = 0.0f;
    if (k > 1.0f)
        k = 1.0f;
    const CVECTOR pnt = s + (d - s) * k;
    // Vertices
    CVECTOR &v0 = *(CVECTOR *)&vertex[trg.i[0]];
    CVECTOR &v1 = *(CVECTOR *)&vertex[trg.i[1]];
    CVECTOR &v2 = *(CVECTOR *)&vertex[trg.i[2]];
    // check the location of the point in the triangle
    CVECTOR en;
    en = (v1 - v0) ^ n;
    if ((en | pnt) - (en | v0) > 0.0f)
        return 2.0f;
    en = (v2 - v1) ^ n;
    if ((en | pnt) - (en | v1) > 0.0f)
        return 2.0f;
    en = (v0 - v2) ^ n;
    if ((en | pnt) - (en | v2) > 0.0f)
        return 2.0f;
    // Got
    return k;
}

// Find the force pushing away from the edges
void PtcData::FindForce(int32_t curNode, CVECTOR &force) const
{
    force = 0.0f;
    if (curNode < 0 || curNode >= numTriangles)
        return;
    short *nb = triangle[curNode].nb;
    for (int32_t i = 0; i < 3; i++)
    {
        if (nb[i] >= 0)
            continue;
        // Normal to the edge
        const int32_t s = triangle[curNode].i[i];
        const int32_t e = triangle[curNode].i[i < 2 ? i + 1 : 0];
        // Edge vertices
        CVECTOR &vs = *(CVECTOR *)&vertex[s];
        CVECTOR &ve = *(CVECTOR *)&vertex[e];
        // Edge normal
        float nx = (ve.z - vs.z);
        float nz = -(ve.x - vs.x);
        const float nl = sqrtf(nx * nx + nz * nz);
        if (!nl)
            continue;
        nx /= nl;
        nz /= nl;
        // Repulsive force
        force.x += nx;
        force.z += nz;
    }
}

// Find the force pushing away from the edges
void PtcData::FindForce(int32_t curNode, const CVECTOR &pos, float dist, CVECTOR &force) const
{
    force = 0.0f;
    if (curNode < 0 || curNode >= numTriangles)
        return;
    short *nb = triangle[curNode].nb;
    for (int32_t i = 0; i < 3; i++)
    {
        if (nb[i] >= 0)
            continue;
        // Normal to the edge
        const int32_t s = triangle[curNode].i[i];
        const int32_t e = triangle[curNode].i[i < 2 ? i + 1 : 0];
        // Edge vertices
        CVECTOR &vs = *(CVECTOR *)&vertex[s];
        CVECTOR &ve = *(CVECTOR *)&vertex[e];
        // Edge normal
        float nx = (ve.z - vs.z);
        float nz = -(ve.x - vs.x);
        const float nl = sqrtf(nx * nx + nz * nz);
        if (nl < 1e-10f)
            continue;
        nx /= nl;
        nz /= nl;
        // Distance to the edge
        float d = pos.x * nx + pos.z * nz - vs.x * nx - vs.z * nz;
        if (d >= dist)
            continue;
        // Repulsive force
        if (d < 0.5f)
            d = 0.5f;
        d = 1.0f / d;
        force.x += nx * d;
        force.z += nz * d;
    }
}

// Get node material
const char *PtcData::GetMaterial(int32_t curNode)
{
    if (!materials)
        return nullptr;
    if (curNode < 0 || curNode >= numTriangles)
        return nullptr;
    const int32_t mtl = triangle[curNode].mtl;
    if (mtl >= 15 || mtl < 0 || mtl >= materials->numMaterials)
        return nullptr;
    return materials->material[mtl];
}

/*
// Get the triangles intersecting the given square
PtcData::Triangle * PtcData::GetTriangles(float x, float z, float sx, float sz, int32_t & num)
{
    num = 0;
    numClTriangles = 0;
    float minX = x - sx*0.5f;
    float minZ = z - sz*0.5f;
    float maxX = x + sx*0.5f;
    float maxZ = z + sz*0.5f;
    int32_t mnX = int32_t((minX - min.x)/ws);
    int32_t mnZ = int32_t((minZ - min.z)/ls);
    int32_t mxX = int32_t((maxX - min.x)/ws);
    int32_t mxZ = int32_t((maxZ - min.z)/ls);
    for(int32_t zi = mnZ; zi <= mxZ; zi++)
    {
        if(zi < 0 || zi >= l) continue;
        for(int32_t xi = mnX; xi <= mxX; xi++)
        {
            if(xi < 0 || xi >= w) continue;
            PtcMap & m = map[zi*w + xi];
            uint16_t * ids = indeces + m.start;
            for(int32_t i = 0; i < m.size; i++) AddClTriangle(ids[i]);
        }
    }
    num = numClTriangles;
    return ctriangle;
}

// Add triangle to buffer
inline void PtcData::AddClTriangle(int32_t i)
{
    for(int32_t j = 0; j < numClTriangles; j++)
        if(ctriangle[j].index == i) return;
    if(numClTriangles >= maxClTriangles)
    {
        maxClTriangles += 64;
        ctriangle = (Triangle *)RESIZE(ctriangle, maxClTriangles*sizeof(Triangle));
    }
    Triangle & ct = ctriangle[numClTriangles++];
    uint16_t * idx = triangle[i].i;
    ct.index = i;
    ct.v[0].x = vertex[idx[0]].x;
    ct.v[0].y = vertex[idx[0]].y;
    ct.v[0].z = vertex[idx[0]].z;
    ct.v[1].x = vertex[idx[1]].x;
    ct.v[1].y = vertex[idx[1]].y;
    ct.v[1].z = vertex[idx[1]].z;
    ct.v[2].x = vertex[idx[2]].x;
    ct.v[2].y = vertex[idx[2]].y;
    ct.v[2].z = vertex[idx[2]].z;
    ct.n.x = normal[triangle[i].n].x;
    ct.n.y = normal[triangle[i].n].y;
    ct.n.z = normal[triangle[i].n].z;
}*/

// Calculate the height of a point on the plane of a triangle
inline float PtcData::FindHeight(int32_t trgID, float x, float z)
{
    Assert(trgID >= 0 && trgID < numTriangles);
    CVECTOR &n = *(CVECTOR *)&normal[triangle[trgID].n];
    float d = n | *(CVECTOR *)&vertex[triangle[trgID].i[0]];
    d = static_cast<float>((d - n.x * x - n.z * z) / static_cast<double>(n.y) + 0.0001);
    return d;
}

// Debug rendering
void PtcData::DebugDraw(VDX9RENDER *rs, float dltTime)
{
    if (numTriangles <= 0)
        return;
    if (!dbgTriangles)
    {
        dbgTriangles = new DbgVertex[numTriangles * 3];
        for (int32_t i = 0; i < numTriangles; i++)
        {
            dbgTriangles[i * 3 + 0].x = vertex[triangle[i].i[0]].x;
            dbgTriangles[i * 3 + 0].y = vertex[triangle[i].i[0]].y;
            dbgTriangles[i * 3 + 0].z = vertex[triangle[i].i[0]].z;
            dbgTriangles[i * 3 + 0].c = 0xff00008f;
            dbgTriangles[i * 3 + 1].x = vertex[triangle[i].i[1]].x;
            dbgTriangles[i * 3 + 1].y = vertex[triangle[i].i[1]].y;
            dbgTriangles[i * 3 + 1].z = vertex[triangle[i].i[1]].z;
            dbgTriangles[i * 3 + 1].c = dbgTriangles[i * 3 + 0].c;
            dbgTriangles[i * 3 + 2].x = vertex[triangle[i].i[2]].x;
            dbgTriangles[i * 3 + 2].y = vertex[triangle[i].i[2]].y;
            dbgTriangles[i * 3 + 2].z = vertex[triangle[i].i[2]].z;
            dbgTriangles[i * 3 + 2].c = dbgTriangles[i * 3 + 0].c;
        }
    }
    if (!dbgEdges)
    {
        dbgEdges = new DbgVertex[numTriangles * 3 * 2];
        for (int32_t i = 0; i < numTriangles; i++)
        {
            for (int32_t j = 0; j < 3; j++)
            {
                float y = 0.05f;
                if (triangle[i].nb[j] >= 0 && triangle[i].nb[j] < i)
                    y += 0.01f;
                dbgEdges[i * 6 + j * 2 + 0].x = vertex[triangle[i].i[j]].x;
                dbgEdges[i * 6 + j * 2 + 0].y = vertex[triangle[i].i[j]].y + 0.001f;
                dbgEdges[i * 6 + j * 2 + 0].z = vertex[triangle[i].i[j]].z;
                dbgEdges[i * 6 + j * 2 + 0].c = triangle[i].nb[j] >= 0 ? 0xff008f00 : 0xff8f0000;
                dbgEdges[i * 6 + j * 2 + 1].x = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].x;
                dbgEdges[i * 6 + j * 2 + 1].y = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].y + 0.001f;
                dbgEdges[i * 6 + j * 2 + 1].z = vertex[triangle[i].i[j + 1 < 3 ? j + 1 : 0]].z;
                dbgEdges[i * 6 + j * 2 + 1].c = dbgEdges[i * 6 + j * 2 + 0].c;
            }
        }
    }
    auto tech = "DbgPatchViewZ";
    rs->SetTransform(D3DTS_WORLD, CMatrix());
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, numTriangles, dbgTriangles, sizeof(DbgVertex),
                        tech);
    rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, numTriangles * 3, dbgEdges, sizeof(DbgVertex),
                        tech);
}
