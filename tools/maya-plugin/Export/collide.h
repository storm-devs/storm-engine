/******************************************************************************
File:  collide.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef _COLLIDE_H_
#define _COLLIDE_H_

#include "bsp.h"
#include "rdf.h"
#include "cvector.h"
#include "rdf_exp.h"
#include "export.h"

class COLLIDE
{

	//strip section
	struct STRIP_EDGE
	{
		long vertex[2];
		std::vector<long> trg;
	};

	std::vector<STRIP_EDGE> sedge;

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
	DWORD ticks;

	CVECTOR *rvertex;

	long RecStrip(long cur_edge, long cur_vrt);
	void MakeStrip(CVECTOR *_cvec, long nvrt, long *trg, long ntrgs);
	long FillStrip(long cur_edge, long cur_vrt, long nnodes);
	//------end strip section------

	std::vector<long> objStrg;
	RESULT_FUNC rfn;

	bool production;
public:

	COLLIDE(RESULT_FUNC rfn);
	~COLLIDE();
	void AddMesh(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat);
	void AddCollideVertices(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat);
	void AddCollideTriangles(long obj, std::vector<VERTEX0*> &av, long nav, RDF_TRIANGLE *at, long nat);
	void BuildBSP(bool pr);

};

#endif
