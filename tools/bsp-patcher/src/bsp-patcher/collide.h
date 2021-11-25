/******************************************************************************
File:  collide.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef _COLLIDE_H_
#define _COLLIDE_H_

#include <windows.h>

#include "bsp.h"
#include "rdf.h"
#include "cvector.h"

class COLLIDE
{

	//strip section
	struct STRIP_EDGE
	{
		long vertex[2];
		vector<long> trg;
	};
	vector<STRIP_EDGE> sedge;

	struct STRIP_TRIANGLE
	{
		long edge[3];
	};
	STRIP_TRIANGLE *striangle;

	long depth, maxdepth;
	long *direction;
	__int64 iterations;
	long triangles;
	long edge_iterations, max_iter;

	CVECTOR *rvertex;

	long RecStrip(long cur_edge, long cur_vrt);
	void MakeStrip(CVECTOR *_cvec, long nvrt, long *trg, long ntrgs);
	long FillStrip(long cur_edge, long cur_vrt, long nnodes);
	//------end strip section------

	vector<long> objStrg;

	bool production;
public:

	COLLIDE();
	~COLLIDE();
	void AddMesh(vector<RDF_VERTEX0> &av, long nav, RDF_TRIANGLE *at, long nat);
	void AddMesh(vector<RDF_VERTEX1>& av, long nav, RDF_TRIANGLE* at, long nat);
	void BuildBSP(bool pr);

};

#endif
