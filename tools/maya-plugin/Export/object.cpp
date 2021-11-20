/******************************************************************************
File:  object.cpp

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#include "export.h"

//---------------------------------------------------------------
//create new object
//---------------------------------------------------------------
EOBJECT::EOBJECT()
{
	boxmin = 1e20;
	boxmax = -1e20;
	radius = 0.0;
	flags=0;
	tot_nodes = 0;
	root = 0;
}

//---------------------------------------------------------------
//copy constructor needed for <vector> template
//---------------------------------------------------------------
EOBJECT::EOBJECT(const EOBJECT &o)
{
	boxmin = o.boxmin;
	boxmax = o.boxmax;
	center = o.center;
	radius = o.radius;

	flags = o.flags;
	group_name = o.group_name;
	name = o.name;
	trg = o.trg;
	mtl = o.mtl;
	vertex = o.vertex;

	tot_nodes = o.tot_nodes;
	root = (SEARCH_NODE*)realloc(0, sizeof(SEARCH_NODE)*1024*(tot_nodes/1024+1));
	memcpy(root, o.root, sizeof(SEARCH_NODE)*tot_nodes);

}

//---------------------------------------------------------------
//delete
//---------------------------------------------------------------
EOBJECT::~EOBJECT()
{
	if(tot_nodes>0)	
		realloc(root, sizeof(SEARCH_NODE)*0);
}


//----------------------------------------------------------------------
//vertex types hierarchy
//----------------------------------------------------------------------
long sdepth = -1, smax_depth = 0;
double straverse;
VERTEX0 *sver;

long EOBJECT::Insert(long n, float v)
{
	sdepth++;	smax_depth = max(sdepth, smax_depth);

	SEARCH_NODE *node = &root[n];

	//init this node
	if(node->vertex<0)
	{
		node->value = v;
		node->left = node->right = -1;
		node->vertex = vertex.size();
		sdepth--;
		return node->vertex;
	}

	straverse += 1.0;

	//if found the same vertex
	if(fabs(v-node->value)<2.0*VERTEX_DELTA*VERTEX_DELTA)
		if(*sver==*vertex[node->vertex])
		{
			sdepth--;
			return node->vertex;
		}

	//if value less or equal
	if(v<=node->value)
		if(node->left>=0)
		{
			long ret = Insert(node->left, v);
			sdepth--;
			return ret;
		}
		else
		{
			node->left = tot_nodes++;
			if((tot_nodes%1024)==0)	root = (SEARCH_NODE*)realloc(root, sizeof(SEARCH_NODE)*1024*(tot_nodes/1024+1));

			root[root[n].left].vertex = -1;
			long ret = Insert(root[n].left, v);
			sdepth--;
			return ret;
		}
	else
		//if value greater
		if(node->right>=0)
		{
			long ret = Insert(node->right, v);
			sdepth--;
			return ret;
		}
		else
		{
			node->right = tot_nodes++;
			if((tot_nodes%1024)==0)	root = (SEARCH_NODE*)realloc(root, sizeof(SEARCH_NODE)*1024*(tot_nodes/1024+1));

			root[root[n].right].vertex = -1;
			long ret = Insert(root[n].right, v);
			sdepth--;
			return ret;
		}
}

//---------------------------------------------------------------
//add triangle
//---------------------------------------------------------------
void EOBJECT::AddTriangle(VERTEX0 **ver)
{
	RDF_TRIANGLE rdft;
	for(long v=0; v<3; v++)
	{
		sver = ver[v];
		float val = ~sver->pos;

		if(tot_nodes==0)
		{
			tot_nodes++;
			root = (SEARCH_NODE*)realloc(root, sizeof(SEARCH_NODE)*((tot_nodes/1024+1)*1024));

			root->value = val;
			root->vertex = -1;
		}

		rdft.vindex[v] = Insert(0, val);

		if(rdft.vindex[v]==vertex.size())
			vertex.push_back(ver[v]);
	}
	trg.push_back(rdft);//*/

	/*RDF_TRIANGLE rdft;
	for(long v=0; v<3; v++)
	{
		for(long ve = vertex.size()-1; ve>-1; ve--)
			if(*ver[v]==*vertex[ve])	break;
			//add new vertex
		if(ve==-1)
		{
			rdft.vindex[v] = vertex.size();
			vertex.push_back(ver[v]);
		}
		else	rdft.vindex[v] = ve;

	}
	trg.push_back(rdft);//*/
}

//---------------------------------------------------------------
//add triangle
//---------------------------------------------------------------
void EOBJECT::CalculateBoxRadius()
{
	long v;
	for(v=0; v<vertex.size(); v++)
	{
		//calculate center of bounding box
		boxmax.x = max(boxmax.x, vertex[v]->pos.x);
		boxmax.y = max(boxmax.y, vertex[v]->pos.y);
		boxmax.z = max(boxmax.z, vertex[v]->pos.z);
		boxmin.x = min(boxmin.x, vertex[v]->pos.x);
		boxmin.y = min(boxmin.y, vertex[v]->pos.y);
		boxmin.z = min(boxmin.z, vertex[v]->pos.z);
	}

	center = 0.5*(boxmax + boxmin);

	//calculate radius
	for(v=0; v<vertex.size(); v++)
	{
		float rad = sqrtf(~(center - vertex[v]->pos));
		if(rad>radius)	radius = rad;
	}
}
//---------------------------------------------------------------
//add triangle
//---------------------------------------------------------------
void EOBJECT::Optimize()
{
}