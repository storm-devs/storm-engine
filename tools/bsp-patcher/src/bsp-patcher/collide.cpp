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

vector<CVECTOR> lvert;
extern double TOT_FACES, CUR_FACE;

COLLIDE::COLLIDE()
{
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
	delete vrt;
	delete trg;
	delete _trg;
	delete sroot;
	lvert.clear();
}

void COLLIDE::AddMesh(vector<RDF_VERTEX0> &av, long nav, RDF_TRIANGLE *at, long nat)
{
	long t;

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
				if(av[v0].pos==vrt[v1])
					break;

			//add new vertex
			if(v1==nvrts)
			{
				nvrts++;
				vrt = (CVECTOR*)realloc(vrt, sizeof(CVECTOR)*nvrts);
				vrt[nvrts-1] = av[v0].pos;
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

void COLLIDE::AddMesh(vector<RDF_VERTEX1>& av, long nav, RDF_TRIANGLE* at, long nat)
{
	long t;

	//calculate used vertices
	long* ref = new long[nav];
	long v;
	for (v = 0; v < nav; v++)	ref[v] = 0;
	for (t = 0; t < nat; t++)
		for (v = 0; v < 3; v++)
			ref[at[t].vindex[v]]++;

	for (long v0 = 0; v0 < nav; v0++)
		if (ref[v0] > 0)
		{
			long v1;
			for (v1 = 0; v1 < nvrts; v1++)
				if (av[v0].pos == vrt[v1])
					break;

			//add new vertex
			if (v1 == nvrts)
			{
				nvrts++;
				vrt = (CVECTOR*)realloc(vrt, sizeof(CVECTOR) * nvrts);
				vrt[nvrts - 1] = av[v0].pos;
			}
			ref[v0] = v1;
		}

	//add triangles and relink vrtindex
	trg = (BTRG*)realloc(trg, sizeof(BTRG) * (ntrgs + nat));
	for (t = 0; t < nat; t++)
		for (v = 0; v < 3; v++)
			trg[ntrgs + t].vindex[v] = ref[at[t].vindex[v]];
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

	//fill nodes
	ZeroMemory(ndepth, sizeof(long)*MAX_TREE_DEPTH);
	ssize = 0;

	TOT_FACES = ntrgs;	CUR_FACE=0;
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
}
