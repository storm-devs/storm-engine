/******************************************************************************
File:  collide.cpp

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#include "collide.h"


long nvrts, ntrgs, _ntrgs;
long cur_node, cur_depth;
BTRG *trg, *_trg;	CVECTOR *vrt;
long cdepth, tnode;

DVECTOR bbox;
BSP_NODE *sroot;
long max_depth, ssize, ndepth[MAX_TREE_DEPTH];

std::vector<CVECTOR> lvert;
std::vector<GEOMLINK> geolink;
RESULT_FUNC resultfn;
extern double TOT_FACES, CUR_FACE;

COLLIDE::COLLIDE(RESULT_FUNC _rfn)
{
	resultfn = _rfn;
	rfn = _rfn;
	nvrts=0;
	ntrgs = _ntrgs = 0;
	cur_node=-1;
	cur_depth=0;
	cdepth=0;
	tnode=0;
	max_depth = 0;

	iterations = 0;
	triangles=0;
	trg = _trg = 0;	vrt = 0;
}

COLLIDE::~COLLIDE()
{
	delete vrt; vrt = nullptr;
	delete trg; trg = nullptr;
	delete _trg; _trg = nullptr;
	delete sroot; sroot = nullptr;
	lvert.clear();
	geolink.clear();
}

void COLLIDE::AddCollideVertices(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat)
{
	for(long v0=0; v0<nav; v0++)
	{
		long v1;
		for(v1=0; v1<nvrts; v1++)
			if(av[v0]->pos==vrt[v1])
				break;

		//add new vertex
		if(v1==nvrts)
		{
			nvrts++;
			vrt = (CVECTOR*)realloc(vrt, sizeof(CVECTOR)*nvrts);
			vrt[nvrts-1] = av[v0]->pos;
		}
	}
}

void COLLIDE::AddCollideTriangles(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat)
{
	long t;
	for(t=0; t<nat; t++)	objStrg.push_back(obj);

	//calculate used vertices
	long *ref = new long[nav];
	long v;
	for(v=0; v<nav; v++)	ref[v] = 0;
	for(t=0; t<nat; t++)
		for(v=0; v<3; v++)
			ref[at[t].vindex[v]]++;

	for(long v0=0; v0<nav; v0++)
		if(ref[v0]>0)
		{
			long v1;
			for(v1=0; v1<nvrts; v1++)
				if(av[v0]->pos==vrt[v1])
					break;

			//add new vertex
			if(v1==nvrts)
			{
				nvrts++;
				vrt = (CVECTOR*)realloc(vrt, sizeof(CVECTOR)*nvrts);
				vrt[nvrts-1] = av[v0]->pos;
			}
			ref[v0] = v1;
		}

	//add triangles and relink vrtindex
	_trg = (BTRG*)realloc(_trg, sizeof(BTRG)*(_ntrgs+nat));
	for(t=0; t<nat; t++)
		for(v=0; v<3; v++)
			_trg[_ntrgs+t].vindex[v] = ref[at[t].vindex[v]];
	_ntrgs += nat;

	delete ref;
}

void COLLIDE::AddMesh(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat)
{
	long t;
	for(t=0; t<nat; t++)	objStrg.push_back(obj);

	//calculate used vertices
	long *ref = new long[nav];
	long v;
	for(v=0; v<nav; v++)	ref[v] = 0;
	for(t=0; t<nat; t++)
		for(v=0; v<3; v++)
			ref[at[t].vindex[v]]++;

	for(long v0=0; v0<nav; v0++)
		if(ref[v0]>0)
		{
			long v1;
			for(v1=0; v1<nvrts; v1++)
				if(av[v0]->pos==vrt[v1])
					break;

			//add new vertex
			if(v1==nvrts)
			{
				nvrts++;
				vrt = (CVECTOR*)realloc(vrt, sizeof(CVECTOR)*nvrts);
				vrt[nvrts-1] = av[v0]->pos;
			}
			ref[v0] = v1;
		}

	//add triangles and relink vrtindex
	trg = (BTRG*)realloc(trg, sizeof(BTRG)*(ntrgs+nat));
	for(t=0; t<nat; t++)
		for(v=0; v<3; v++)
			trg[ntrgs+t].vindex[v] = ref[at[t].vindex[v]];
	ntrgs += nat;

	delete ref;
}


//---------------------------------------------------------------------------
//build bsp
//---------------------------------------------------------------------------
void COLLIDE::BuildBSP(bool pr)
{
	production = pr;

	BUILD_BSP_FACE **faces = new BUILD_BSP_FACE*[ntrgs];
	for(long t=0; t<ntrgs; t++)
	{
		BUILD_BSP_FACE *face = new BUILD_BSP_FACE;
		faces[t] = face;

		face->vertices = (DVECTOR*)realloc(NULL, sizeof(DVECTOR)*3);
		face->nvertices = 3;
		for(long v=0; v<3; v++)	face->vertices[v] = vrt[trg[t].vindex[v]];

		face->normal = !( face->vertices[1]-face->vertices[0] ^ face->vertices[2]-face->vertices[0]);
		face->plane_distance = face->vertices[0] | face->normal;
		face->trg = t;
	}

	DVECTOR bmin = 1e20f;
	DVECTOR bmax = -1e20f;
	for(long bv=0; bv<nvrts; bv++)
	{
		bmin.x = min(bmin.x, vrt[bv].x);
		bmin.y = min(bmin.y, vrt[bv].y);
		bmin.z = min(bmin.z, vrt[bv].z);
		bmax.x = max(bmax.x, vrt[bv].x);
		bmax.y = max(bmax.y, vrt[bv].y);
		bmax.z = max(bmax.z, vrt[bv].z);
	}
	bbox = bmax-bmin;
	//to be sure that the box's size >0 in all axis
	float minb = 0.001*sqrt(~bbox);
	bbox.x = max(bbox.x, minb);
	bbox.y = max(bbox.y, minb);
	bbox.z = max(bbox.z, minb);

	//fill nodes
	ZeroMemory(ndepth, sizeof(long)*MAX_TREE_DEPTH);
	ssize = 0;

	TOT_FACES = ntrgs;	CUR_FACE=0;
	DWORD curti = GetTickCount();
	BUILD_BSP_NODE *root = new BUILD_BSP_NODE();
	root->FillNode(faces, ntrgs);

	long prv=0;
	for(long d=0; d<max_depth; d++)
	{
		prv += ndepth[d];
		ndepth[d] = prv;
	}

	sroot = new BSP_NODE[ssize];
	memset(sroot, 0, sizeof(BSP_NODE)*ssize);
	//ZeroMemory(sroot, ssize);
	root->Store();


	FILE *fl = fopen("stat.txt", "a");
	fprintf(fl, "trg: %d, nodes: %d, depth: %d, byte/trg: %d, time: %d\n", 
		ntrgs, tnode, max_depth, ssize*sizeof(BSP_NODE)/ntrgs, (GetTickCount()-curti)/1000);
	fclose(fl);

}
