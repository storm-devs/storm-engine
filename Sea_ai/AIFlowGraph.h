#ifndef AI_FLOW_GRAPH_HPP
#define AI_FLOW_GRAPH_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\defines.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

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
		dword	dw1,dw2;	// first and second point index
		float	fLen;		// edge len

		edge_t() {};
		edge_t(dword _dw1, dword _dw2, float _fLen) { dw1 = _dw1; dw2 = _dw2; fLen = _fLen; };
		inline bool operator == (const edge_t & e) const { return ((e.dw1 == dw1 && e.dw2 == dw2) || (e.dw1 == dw2 && e.dw2 == dw1)); }
	};

	struct npoint_t 
	{
		dword		dwPnt;
		float		fDistance;
		float		fTemp;
		inline bool operator < (const npoint_t & n) const { return (fDistance < n.fDistance); };
	};

	struct point_t
	{
		CVECTOR				vPos;
		dword				dwFlags;
		array<dword>		aEdges;

		point_t() : aEdges(_FL_) {};
		point_t(CVECTOR _vPos) : aEdges(_FL_) { vPos = _vPos; };
		inline bool operator == (const point_t & p) const { return ((~(p.vPos - vPos))<1e-5f); }
	};

	class VectorPath
	{
		public:
			array<CVECTOR>		aPoints;

			VectorPath() : aPoints(_FL_) {};
			~VectorPath() {};

			void AddPoint(CVECTOR vPos) { aPoints.Add(vPos); };
	};

	class Path
	{
		private: 
			struct point_t
			{
				dword		dwPnt;
				float		fDistance;
			};

			float			fDistance;

		public:
			array<point_t>	aPoints;

			float	GetPathDistance() { return fDistance; }
			float	GetDistance(dword dwPnt)
			{
				float fDist = 0.0f;
				for (dword i=0;i<aPoints.Size();i++) 
				{
					point_t * pP = &aPoints[i];
					fDist += pP->fDistance;
					if (dwPnt == pP->dwPnt) return fDist;
				}
				return -1.0f;
			}
			bool	FindPoint(dword dwPnt) { return (GetDistance(dwPnt) >= 0.0f); }
			dword	GetLastPoint() { Assert(aPoints.Size()); return aPoints[aPoints.Last()].dwPnt; };
			void	DelLastPoint() { fDistance -= aPoints[aPoints.Last()].fDistance; aPoints.DelIndex(aPoints.Last()); };
			void	AddPoint(dword dwP, float _fDistance)
			{
				point_t *pP = &aPoints[aPoints.Add()];
				pP->dwPnt = dwP;
				pP->fDistance = _fDistance;
				fDistance += _fDistance;
			}

			Path(Path * pPath = null) : aPoints(_FL_)
			{ 
				fDistance = 0.0f; 
				if (pPath)
				{
					aPoints = pPath->aPoints;
					fDistance = pPath->fDistance;
				}
			};
			~Path() {};
	};

private:
	array<Path*>		aPaths;

	bool	FindPath(Path *pPath, dword dwP1, dword dwP2);

protected:
	dword				dwIteration;

	array<edge_t>		aEdges;
	array<point_t>		aPoints;
	//array<npoint_t>		aNearestPoints;
	string				sSectionName;

struct table_t
{
	dword	p;
	float	d;
};

	table_t				* pTable;
public:
	AIFlowGraph() : aPaths(_FL_, 64), aEdges(_FL_), aPoints(_FL_)//, aNearestPoints(200)
	{
		pTable = null;
		sSectionName = "GraphPoints";
	};
	~AIFlowGraph() 
	{
		DELETE(pTable);
	};

	// save/load/release section
	void	ReleaseAll();
	bool	Load(INIFILE * pIni);
	bool	Save(INIFILE * pIni);

	// point/edge/Path function section
	dword	GetNumPoints() { return aPoints.Size(); };
	dword	GetNumEdges() { return aEdges.Size(); };
	CVECTOR	GetPointPos(dword dwPnt)
	{
		Assert(dwPnt < aPoints.Size());
		return aPoints[dwPnt].vPos;
	}
	point_t		* GetPoint(dword dwPntIdx);
	edge_t		* GetEdge(dword dwEdgeIdx);
	VectorPath	* GetVectorPath(dword dwP1, dword dwP2);
	Path		* GetPath(dword dwP1, dword dwP2);
	float		GetPathDistance(dword dwP1, dword dwP2);
	float		GetDistance(dword dwP1, dword dwP2);
	dword		GetOtherEdgePoint(dword dwEdgeIdx, dword dwPnt);
	array<npoint_t> * GetNearestPoints(CVECTOR & vP);

	dword	AddPoint(CVECTOR vPos);
	dword	AddEdge(dword dwEdgePnt1, dword dwEdgePnt2);
	dword	AddEdge2Point(dword dwPnt, dword dwEdgePnt1, dword dwEdgePnt2);

	void	BuildTable();

private:
};

inline void AIFlowGraph::ReleaseAll()
{
	aEdges.DelAll();
	aPoints.DelAll();
}

inline bool AIFlowGraph::Save(INIFILE * pIni)
{
	Assert(pIni);

	string		sTemp,sKey;
	pIni->DeleteSection((char*)sSectionName.GetBuffer());
	for (dword i=0;i<aPoints.Size();i++)
	{
		sTemp = "";
		sTemp += aPoints[i].vPos.x; sTemp += ",";
		sTemp += aPoints[i].vPos.z; sTemp += ",";
		sTemp += (long)aPoints[i].aEdges.Size(); sTemp += ",";
		for (dword j=0;j<aPoints[i].aEdges.Size();j++)
		{
			sTemp += (long)aEdges[aPoints[i].aEdges[j]].dw1; sTemp += ",";
			sTemp += (long)aEdges[aPoints[i].aEdges[j]].dw2; sTemp += ",";
		}
		sKey.Format("pnt%d",i);
		pIni->WriteString((char*)sSectionName.GetBuffer(),(char*)sKey.GetBuffer(),(char*)sTemp.GetBuffer());
	}
	return true;
}

inline bool AIFlowGraph::Load(INIFILE * pIni)
{
	char cTemp[32768];
	Assert(pIni);
	ReleaseAll();

	string sTemp,sKey;
	while(true)
	{
		sKey.Format("pnt%d",aPoints.Size());
		cTemp[0] = 0;
		pIni->ReadString((char*)sSectionName.GetBuffer(),(char*)sKey.GetBuffer(),cTemp,32768,"\0"); if (!cTemp[0]) break;

		point_t *pP = &aPoints[aPoints.Add()];
		pP->vPos.y = 0.0f;
		sscanf(cTemp,"%f,%f",&pP->vPos.x,&pP->vPos.z);
	}
	for (dword i=0;i<aPoints.Size();i++)
	{
		float	x,z;
		dword	dwNum;
		sKey.Format("pnt%d",i);
		cTemp[0] = 0;
		pIni->ReadString((char*)sSectionName.GetBuffer(),(char*)sKey.GetBuffer(),cTemp,32768); if (!cTemp[0]) continue;
		sscanf(cTemp,"%f,%f,%d",&x,&z,&dwNum);
		sTemp = cTemp;
		dword dwShift = 0;
		sTemp.GetToken(cTemp,dwShift,",");
		sTemp.GetToken(cTemp,dwShift,",");
		sTemp.GetToken(cTemp,dwShift,",");
		for (dword j=0;j<dwNum;j++)
		{
			dword dw1,dw2;
			sTemp.GetToken(cTemp,dwShift,",");
			sscanf(cTemp,"%d",&dw1);
			sTemp.GetToken(cTemp,dwShift,",");
			sscanf(cTemp,"%d",&dw2);
			aPoints[i].aEdges.Add(AddEdge(dw1,dw2));
		}
	}
	
	return false;
}

inline dword AIFlowGraph::AddPoint(CVECTOR vPos)
{
	point_t p(vPos);

	dword	dwIdx;
	if ((dwIdx = aPoints.Find(p)) != INVALID_ARRAY_INDEX) return dwIdx;
	return aPoints.Add(p);
}

inline dword AIFlowGraph::AddEdge(dword dwEdgePnt1, dword dwEdgePnt2)
{
	Assert(dwEdgePnt1 < aPoints.Size() && dwEdgePnt2 < aPoints.Size());

	dword	dwIdx;
	edge_t e(dwEdgePnt1,dwEdgePnt2,sqrtf(~(GetPointPos(dwEdgePnt1) - GetPointPos(dwEdgePnt2))));

	if ((dwIdx = aEdges.Find(e)) != INVALID_ARRAY_INDEX) return dwIdx;
	return aEdges.Add(e);
}

inline dword AIFlowGraph::AddEdge2Point(dword dwPnt, dword dwEdgePnt1, dword dwEdgePnt2)
{
	Assert(dwPnt < aPoints.Size() && dwEdgePnt1 < aPoints.Size() && dwEdgePnt2 < aPoints.Size());

	dword dwEdge = AddEdge(dwEdgePnt1,dwEdgePnt2);
	aPoints[dwPnt].aEdges.Add(dwEdge);
	return aPoints[dwPnt].aEdges.Size()-1;
}

inline AIFlowGraph::point_t	* AIFlowGraph::GetPoint(dword dwPntIdx)
{
	Assert(dwPntIdx < aPoints.Size());
	return &aPoints[dwPntIdx];
}

inline AIFlowGraph::edge_t	* AIFlowGraph::GetEdge(dword dwEdgeIdx)
{
	Assert(dwEdgeIdx < aEdges.Size());
	return &aEdges[dwEdgeIdx];
}

inline dword AIFlowGraph::GetOtherEdgePoint(dword dwEdgeIdx, dword dwPnt)
{
	Assert(dwEdgeIdx < aEdges.Size());
	if (aEdges[dwEdgeIdx].dw1 == dwPnt) return aEdges[dwEdgeIdx].dw2;
	return aEdges[dwEdgeIdx].dw1;
}

inline void AIFlowGraph::BuildTable()
{
	dword i,j,k,x,y;
	dword dwNumPoints = aPoints.Size();

	DELETE(pTable);
	pTable = new table_t[SQR(dwNumPoints)];
	for (i=0;i<SQR(dwNumPoints);i++) 
	{
		pTable[i].p = INVALID_ARRAY_INDEX;
		pTable[i].d = 1e8f;
	}
	for (i=0;i<dwNumPoints;i++)
	{
		point_t * pP = &aPoints[i];
		table_t * pTableRow = &pTable[i * dwNumPoints];
		for (j=0;j<pP->aEdges.Size();j++)
		{
			dword dwPnt = GetOtherEdgePoint(pP->aEdges[j],i);
			pTableRow[dwPnt].p = dwPnt;
			pTableRow[dwPnt].d = aEdges[pP->aEdges[j]].fLen;
		}
	}
	for (k=0;k<dwNumPoints;k++)
	{
		bool bF = true;
		for (y=0;y<dwNumPoints;y++)
		{
			for (x=0;x<dwNumPoints;x++) if (x != y)
			{
				point_t * pP = &aPoints[y];
				float d = pTable[x + y * dwNumPoints].d;
				for (j=0;j<pP->aEdges.Size();j++)
				{
					dword dwPnt = GetOtherEdgePoint(pP->aEdges[j],y);
					float d1 = pTable[dwPnt + y * dwNumPoints].d;
					float d2 = pTable[x + dwPnt * dwNumPoints].d;
					if (d1+d2 < d && fabsf((d1+d2) - d)>0.01f) 
					{
						d = d1+d2;
						pTable[x + y * dwNumPoints].d = d;
						pTable[x + y * dwNumPoints].p = dwPnt;
						bF = false;
					}
				}
				
			}
		}
		if (bF) break;
	}
}

inline float AIFlowGraph::GetDistance(dword dwP1, dword dwP2)
{
	return sqrtf(~(GetPointPos(dwP2) - GetPointPos(dwP1)));
}

inline float AIFlowGraph::GetPathDistance(dword dwP1, dword dwP2)
{
	Assert(dwP1 < aPoints.Size() && dwP2 < aPoints.Size());
	if (dwP1 == dwP2) return 0.0f;
	dword dwNumPoints = aPoints.Size();

	float fDistance = 0.0f;
	dword dwPnt = pTable[dwP2 + dwP1 * dwNumPoints].p;
	while(dwPnt != INVALID_ARRAY_INDEX)
	{
		fDistance += GetDistance(dwP1, dwPnt);
		dwP1 = dwPnt;
		dwPnt = pTable[dwP2 + dwPnt * dwNumPoints].p;
	}

	return fDistance;
}

inline AIFlowGraph::VectorPath * AIFlowGraph::GetVectorPath(dword dwP1, dword dwP2)
{
	VectorPath	* pVPath = new VectorPath();
	Path		* pPath = GetPath(dwP1,dwP2);
	if (pPath)
	{
		for (dword i=0;i<pPath->aPoints.Size();i++)
			pVPath->AddPoint(GetPointPos(pPath->aPoints[i].dwPnt));
	}
	DELETE(pPath);
	return pVPath;
}

inline AIFlowGraph::Path * AIFlowGraph::GetPath(dword dwP1, dword dwP2)
{
	Assert(dwP1 < aPoints.Size() && dwP2 < aPoints.Size());
	dword dwNumPoints = aPoints.Size();

	Path *pP = new Path(null);
	pP->AddPoint(dwP1,0.0f);
	dword dwPnt = pTable[dwP2 + dwP1 * dwNumPoints].p;
	while(dwPnt != INVALID_ARRAY_INDEX)
	{
		float fDistance = GetDistance(dwP1, dwPnt);
		pP->AddPoint(dwPnt,fDistance);
		dwPnt = pTable[dwP2 + dwPnt * dwNumPoints].p;
	}

	return pP;
}

inline array<AIFlowGraph::npoint_t> * AIFlowGraph::GetNearestPoints(CVECTOR & vP)
{
	dword	i;
	array<npoint_t> * aNearestPoints;

	aNearestPoints = new array<npoint_t>(_FL_, 200);
	for (i=0;i<aPoints.Size();i++)
	{
		npoint_t * pN = &(*aNearestPoints)[(*aNearestPoints).Add()];
		pN->fDistance = sqrtf(~(vP - aPoints[i].vPos));
		pN->dwPnt = i;
	}
	(*aNearestPoints).Sort();

	return aNearestPoints;
}

#endif