/******************************************************************************
File:  bsp.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef __BSP_H__
#define __BSP_H__

#include "cvector.h"
#include "rdf.h"
#include <vector>
using namespace std;

#define MAX_TREE_DEPTH 1024
#define MAX_PLANES_CALCULATED 256l
//#define PRECISION 5e-7
#define PRECISION 0.00001
#define KDISTANCE 0.0
#define KBALANCE 1.0
#define KCLIP 30.0
//calculate size of node in blocks
#define NODESIZE(a) (1+(a*3+sizeof(BSP_NODE)-sizeof(long))/sizeof(BSP_NODE))
//max faces on node
#define MAX_PLANE_FACES 15l

#pragma pack(push)
#pragma pack(1)

struct BTRG
{
	long vindex[3];
};

//-----------BSP_FACE-----------
typedef struct
{
	long trg;
	DVECTOR *vertices;
	long nvertices;
	DVECTOR normal;	//temporary
	double plane_distance;	//temporary
}BUILD_BSP_FACE;

//	builder
class BUILD_BSP_NODE;
class BUILD_BSP_NODE
{
public:
	BUILD_BSP_NODE *right,*left;

	long current;
	DVECTOR norm;
	double pld;
	long tot_faces;
	long *_face;

	BUILD_BSP_NODE(){};
	~BUILD_BSP_NODE();
	BSP_NODE *FillNode(BUILD_BSP_FACE **list, long nfaces);
	long BestPlane(BUILD_BSP_FACE **list, long nfaces);
	void BestEmptyPlane(BUILD_BSP_FACE **list, long nfaces, DVECTOR *epnorm, double *epdist);
	long Store();
};
#pragma pack(pop)

#endif