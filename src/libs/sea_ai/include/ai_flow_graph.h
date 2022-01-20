#pragma once

#include "c_vector.h"
#include "defines.h"
#include "storm_assert.h"
#include "v_file_service.h"
#include "v_module_api.h"

#include <algorithm>
#include <string>
#include <vector>

#define INVALID_ARRAY_INDEX 0xFFFFFFFF

class AIFlowGraph
{
    friend class IslandED;

    enum AIFLOWGRAPH_FLAGS
    {
        ALREADY_USED_IN_PATHFINDING = 1,
        AIFLOWGRAPH_UNKNOWN = 0x7FFFFFFF
    };

  public:
    struct edge_t
    {
        uint32_t dw1, dw2; // first and second point index
        float fLen;        // edge len

        edge_t()
        {
        }

        edge_t(uint32_t _dw1, uint32_t _dw2, float _fLen)
        {
            dw1 = _dw1;
            dw2 = _dw2;
            fLen = _fLen;
        }

        inline bool operator==(const edge_t &e) const
        {
            return ((e.dw1 == dw1 && e.dw2 == dw2) || (e.dw1 == dw2 && e.dw2 == dw1));
        }
    };

    struct npoint_t
    {
        uint32_t dwPnt;
        float fDistance;
        float fTemp;

        inline bool operator<(const npoint_t &n) const
        {
            return (fDistance < n.fDistance);
        }
    };

    struct point_t
    {
        CVECTOR vPos;
        uint32_t dwFlags;
        std::vector<std::size_t> aEdges;

        point_t()
            : vPos(), dwFlags(0)
        {
        }

        point_t(CVECTOR _vPos)
            : dwFlags(0)
        {
            vPos = _vPos;
        }

        inline bool operator==(const point_t &p) const
        {
            return ((~(p.vPos - vPos)) < 1e-5f);
        }
    };

    class VectorPath
    {
      public:
        std::vector<CVECTOR> aPoints;

        VectorPath()
        {
        }

        ~VectorPath()
        {
        }

        void AddPoint(CVECTOR vPos)
        {
            aPoints.push_back(vPos);
        }
    };

    class Path
    {
      private:
        struct point_t
        {
            uint32_t dwPnt;
            float fDistance;
        };

        float fDistance;

      public:
        std::vector<point_t> aPoints;

        float GetPathDistance()
        {
            return fDistance;
        }

        float GetDistance(uint32_t dwPnt)
        {
            float fDist = 0.0f;
            for (auto &aPoint : aPoints)
            {
                point_t *pP = &aPoint;
                fDist += pP->fDistance;
                if (dwPnt == pP->dwPnt)
                    return fDist;
            }
            return -1.0f;
        }

        bool FindPoint(uint32_t dwPnt)
        {
            return (GetDistance(dwPnt) >= 0.0f);
        }

        uint32_t GetLastPoint()
        {
            Assert(aPoints.size());
            return aPoints.back().dwPnt;
        }

        void DelLastPoint()
        {
            fDistance -= aPoints.back().fDistance;
            aPoints.pop_back();
        }

        void AddPoint(uint32_t dwP, float _fDistance)
        {
            aPoints.push_back(point_t{dwP, _fDistance});
            fDistance += _fDistance;
        }

        Path(Path *pPath = nullptr)
        {
            fDistance = 0.0f;
            if (pPath)
            {
                aPoints = pPath->aPoints;
                fDistance = pPath->fDistance;
            }
        }

        ~Path()
        {
        }
    };

  private:
    std::vector<Path *> aPaths;

    bool FindPath(Path *pPath, uint32_t dwP1, uint32_t dwP2);

  protected:
    uint32_t dwIteration;

    std::vector<edge_t> aEdges;
    std::vector<point_t> aPoints;
    // std::vector<npoint_t>        aNearestPoints;
    std::string sSectionName;

    struct table_t
    {
        uint32_t p;
        float d;
    };

    table_t *pTable;

  public:
    AIFlowGraph() //: aPaths(_FL_, 64), aEdges(_FL_), aPoints(_FL_)//, aNearestPoints(200)
        : dwIteration(0)
    {
        pTable = nullptr;
        sSectionName = "GraphPoints";
    }

    ~AIFlowGraph()
    {
        STORM_DELETE(pTable);
    }

    // save/load/release section
    void ReleaseAll();
    bool Load(INIFILE &pIni);
    bool Save(INIFILE *pIni);

    // point/edge/Path function section
    size_t GetNumPoints()
    {
        return aPoints.size();
    }

    size_t GetNumEdges()
    {
        return aEdges.size();
    }

    CVECTOR GetPointPos(size_t dwPnt)
    {
        Assert(dwPnt < aPoints.size());
        return aPoints[dwPnt].vPos;
    }

    point_t *GetPoint(size_t dwPntIdx);
    edge_t *GetEdge(size_t dwEdgeIdx);
    VectorPath *GetVectorPath(size_t dwP1, size_t dwP2);
    Path *GetPath(size_t dwP1, size_t dwP2);
    float GetPathDistance(size_t dwP1, size_t dwP2);
    float GetDistance(size_t dwP1, size_t dwP2);
    size_t GetOtherEdgePoint(size_t dwEdgeIdx, size_t dwPnt);
    std::vector<npoint_t> *GetNearestPoints(CVECTOR &vP);

    decltype(aPoints)::difference_type AddPoint(CVECTOR vPos);
    decltype(aEdges)::difference_type AddEdge(size_t dwEdgePnt1, size_t dwEdgePnt2);
    size_t AddEdge2Point(size_t dwPnt, size_t dwEdgePnt1, size_t dwEdgePnt2);

    void BuildTable();

  private:
};

inline void AIFlowGraph::ReleaseAll()
{
    aEdges.clear();
    aPoints.clear();
}

inline bool AIFlowGraph::Save(INIFILE *pIni)
{
    Assert(pIni);

    pIni->DeleteSection((char *)sSectionName.c_str());
    for (uint32_t i = 0; i < aPoints.size(); i++)
    {
        std::string sTemp;
        sTemp += std::to_string(aPoints[i].vPos.x);
        sTemp += ",";
        sTemp += std::to_string(aPoints[i].vPos.z);
        sTemp += ",";
        sTemp += std::to_string(aPoints[i].aEdges.size());
        sTemp += ",";
        for (unsigned int aEdge : aPoints[i].aEdges)
        {
            sTemp += std::to_string(aEdges[aEdge].dw1);
            sTemp += ",";
            sTemp += std::to_string(aEdges[aEdge].dw2);
            sTemp += ",";
        }
        std::string sKey = "pnt" + std::to_string(i);
        pIni->WriteString(sSectionName.c_str(), sKey.c_str(), sTemp.c_str());
    }
    return true;
}

inline bool AIFlowGraph::Load(INIFILE &pIni)
{
    char cTemp[32768];
    ReleaseAll();

    std::string sKey;
    while (true) //~!~ Optimize?
    {
        sKey = "pnt" + std::to_string(aPoints.size());
        cTemp[0] = 0;
        pIni.ReadString((char *)sSectionName.c_str(), (char *)sKey.c_str(), cTemp, 32768, "\0");
        if (!cTemp[0])
            break;

        // point_t *pP = &aPoints[aPoints.Add()];
        point_t p;
        p.vPos.y = 0.0f;
        sscanf(cTemp, "%f,%f", &p.vPos.x, &p.vPos.z);
        aPoints.push_back(p);
    }
    for (uint32_t i = 0; i < aPoints.size(); i++)
    {
        float x, z;
        uint32_t dwNum;
        sKey = "pnt" + std::to_string(i);
        cTemp[0] = 0;
        pIni.ReadString((char *)sSectionName.c_str(), (char *)sKey.c_str(), cTemp, 32768);
        if (!cTemp[0])
            continue;

        const char *buf = cTemp;
        int offset;
        sscanf(buf, "%f,%f,%d,%n", &x, &z, &dwNum, &offset);
        buf += offset;

        for (uint32_t j = 0; j < dwNum; j++)
        {
            uint32_t dw1, dw2;
            sscanf(buf, "%d,%d,%n", &dw1, &dw2, &offset);
            ;
            buf += offset;
            aPoints[i].aEdges.push_back(AddEdge(dw1, dw2));
        }
    }

    return false;
}

inline decltype(AIFlowGraph::aPoints)::difference_type AIFlowGraph::AddPoint(CVECTOR vPos)
{
    const point_t p(vPos);

    const auto it = std::find(aPoints.begin(), aPoints.end(), p);
    if (it != aPoints.end())
        return it - aPoints.begin();

    aPoints.push_back(p);
    return aPoints.size() - 1;
}

inline decltype(AIFlowGraph::aEdges)::difference_type AIFlowGraph::AddEdge(size_t dwEdgePnt1, size_t dwEdgePnt2)
{
    Assert(dwEdgePnt1 < aPoints.size() && dwEdgePnt2 < aPoints.size());

    const edge_t e(dwEdgePnt1, dwEdgePnt2, sqrtf(~(GetPointPos(dwEdgePnt1) - GetPointPos(dwEdgePnt2))));

    const auto it = std::find(aEdges.begin(), aEdges.end(), e);
    if (it != aEdges.end())
        return it - aEdges.begin();

    aEdges.push_back(e);
    return aEdges.size() - 1;
}

inline size_t AIFlowGraph::AddEdge2Point(size_t dwPnt, size_t dwEdgePnt1, size_t dwEdgePnt2)
{
    Assert(dwPnt < aPoints.size() && dwEdgePnt1 < aPoints.size() && dwEdgePnt2 < aPoints.size());

    const auto dwEdge = AddEdge(dwEdgePnt1, dwEdgePnt2);
    aPoints[dwPnt].aEdges.push_back(dwEdge);
    return aPoints[dwPnt].aEdges.size() - 1;
}

inline AIFlowGraph::point_t *AIFlowGraph::GetPoint(size_t dwPntIdx)
{
    Assert(dwPntIdx < aPoints.size());
    return &aPoints[dwPntIdx];
}

inline AIFlowGraph::edge_t *AIFlowGraph::GetEdge(size_t dwEdgeIdx)
{
    Assert(dwEdgeIdx < aEdges.size());
    return &aEdges[dwEdgeIdx];
}

inline size_t AIFlowGraph::GetOtherEdgePoint(size_t dwEdgeIdx, size_t dwPnt)
{
    Assert(dwEdgeIdx < aEdges.size());
    if (aEdges[dwEdgeIdx].dw1 == dwPnt)
        return aEdges[dwEdgeIdx].dw2;
    return aEdges[dwEdgeIdx].dw1;
}

inline void AIFlowGraph::BuildTable()
{
    uint32_t i, j, k, x, y;
    const auto dwNumPoints = aPoints.size();

    STORM_DELETE(pTable);
    pTable = new table_t[SQR(dwNumPoints)];
    for (i = 0; i < SQR(dwNumPoints); i++)
    {
        pTable[i].p = INVALID_ARRAY_INDEX;
        pTable[i].d = 1e8f;
    }
    for (i = 0; i < dwNumPoints; i++)
    {
        point_t *pP = &aPoints[i];
        table_t *pTableRow = &pTable[i * dwNumPoints];
        for (j = 0; j < pP->aEdges.size(); j++)
        {
            const uint32_t dwPnt = GetOtherEdgePoint(pP->aEdges[j], i);
            pTableRow[dwPnt].p = dwPnt;
            pTableRow[dwPnt].d = aEdges[pP->aEdges[j]].fLen;
        }
    }
    for (k = 0; k < dwNumPoints; k++)
    {
        bool bF = true;
        for (y = 0; y < dwNumPoints; y++)
        {
            for (x = 0; x < dwNumPoints; x++)
                if (x != y)
                {
                    point_t *pP = &aPoints[y];
                    float d = pTable[x + y * dwNumPoints].d;
                    for (j = 0; j < pP->aEdges.size(); j++)
                    {
                        const uint32_t dwPnt = GetOtherEdgePoint(pP->aEdges[j], y);
                        const float d1 = pTable[dwPnt + y * dwNumPoints].d;
                        const float d2 = pTable[x + dwPnt * dwNumPoints].d;
                        if (d1 + d2 < d && fabsf((d1 + d2) - d) > 0.01f)
                        {
                            d = d1 + d2;
                            pTable[x + y * dwNumPoints].d = d;
                            pTable[x + y * dwNumPoints].p = dwPnt;
                            bF = false;
                        }
                    }
                }
        }
        if (bF)
            break;
    }
}

inline float AIFlowGraph::GetDistance(size_t dwP1, size_t dwP2)
{
    return sqrtf(~(GetPointPos(dwP2) - GetPointPos(dwP1)));
}

inline float AIFlowGraph::GetPathDistance(size_t dwP1, size_t dwP2)
{
    Assert(dwP1 < aPoints.size() && dwP2 < aPoints.size());
    if (dwP1 == dwP2)
        return 0.0f;
    const auto dwNumPoints = aPoints.size();

    float fDistance = 0.0f;
    uint32_t dwPnt = pTable[dwP2 + dwP1 * dwNumPoints].p;
    while (dwPnt != INVALID_ARRAY_INDEX)
    {
        fDistance += GetDistance(dwP1, dwPnt);
        dwP1 = dwPnt;
        dwPnt = pTable[dwP2 + dwPnt * dwNumPoints].p;
    }

    return fDistance;
}

inline AIFlowGraph::VectorPath *AIFlowGraph::GetVectorPath(size_t dwP1, size_t dwP2)
{
    auto pVPath = new VectorPath();
    Path *pPath = GetPath(dwP1, dwP2);
    if (pPath)
    {
        for (auto &aPoint : pPath->aPoints)
            pVPath->AddPoint(GetPointPos(aPoint.dwPnt));
    }
    STORM_DELETE(pPath);
    return pVPath;
}

inline AIFlowGraph::Path *AIFlowGraph::GetPath(size_t dwP1, size_t dwP2)
{
    Assert(dwP1 < aPoints.size() && dwP2 < aPoints.size());
    const auto dwNumPoints = aPoints.size();

    auto pP = new Path(nullptr);
    pP->AddPoint(dwP1, 0.0f);
    uint32_t dwPnt = pTable[dwP2 + dwP1 * dwNumPoints].p;
    while (dwPnt != INVALID_ARRAY_INDEX)
    {
        const float fDistance = GetDistance(dwP1, dwPnt);
        pP->AddPoint(dwPnt, fDistance);
        dwPnt = pTable[dwP2 + dwPnt * dwNumPoints].p;
    }

    return pP;
}

inline std::vector<AIFlowGraph::npoint_t> *AIFlowGraph::GetNearestPoints(CVECTOR &vP)
{
    auto aNearestPoints = new std::vector<npoint_t>(aPoints.size());
    for (uint32_t i = 0; i < aPoints.size(); i++)
    {
        // npoint_t * pN = &(*aNearestPoints)[(*aNearestPoints).Add()];
        // pN->fDistance = sqrtf(~(vP - aPoints[i].vPos));
        // pN->dwPnt = i;
        (*aNearestPoints)[i].fDistance = sqrtf(~(vP - aPoints[i].vPos));
        (*aNearestPoints)[i].dwPnt = i;
    }
    //(*aNearestPoints).Sort();
    std::sort(aNearestPoints->begin(), aNearestPoints->end());

    return aNearestPoints;
}
