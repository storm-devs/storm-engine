/******************************************************************************
File:  bsp.cpp

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#include "bsp.h"

extern CVECTOR *vrt;
extern BTRG *trg;
extern DVECTOR bbox;
extern long ssize, max_depth, ndepth[MAX_TREE_DEPTH];
extern BSP_NODE *sroot;
extern long cur_node;
extern long cur_depth;
extern long cdepth;
extern long tnode;

#define LIE_PREC 0.0001

BUILD_BSP_NODE::~BUILD_BSP_NODE()
{
	delete _face;
	if(right!=NULL)	delete right;
	if(left!=NULL)	delete left;
}

double TOT_FACES, CUR_FACE;
long min_l, min_r, min_c, min_m;

#include "rdf_exp.h"
extern RESULT_FUNC resultfn;
//-----------------------------------------------------------------------------------
//fill node
//-----------------------------------------------------------------------------------
BSP_NODE *BUILD_BSP_NODE::FillNode(BUILD_BSP_FACE **list, long nfaces)
{
	static long time = 0;
	if(GetTickCount()-time>1000)
	{
		double prc = 100.0*CUR_FACE/TOT_FACES;
		char ee[256];
		sprintf(ee, "Building BSP: %.2f %%", float(prc));
		if(resultfn(std::string(ee))==false)	throw "Building BSP process was terminated";
		time = GetTickCount();
	}

	long best_plane = BestPlane(list, nfaces);
//######################################__SORT__######################################
	norm = list[best_plane]->normal;
	pld = norm | vrt[trg[list[best_plane]->trg].vindex[0]];

	if(min_m>MAX_PLANE_FACES)
	{
		BestEmptyPlane(list, nfaces, &norm, &pld);
		best_plane = -1;
	}

	BUILD_BSP_FACE **rlist = new BUILD_BSP_FACE*[min_r + min_c];
	BUILD_BSP_FACE **llist = new BUILD_BSP_FACE*[min_l + min_c];

	CUR_FACE += min_m;	TOT_FACES += min_c;

	cur_node++;
	cur_depth++;	if(cur_depth>max_depth)	max_depth = cur_depth;
	if(cur_depth>1000)
		__debugbreak();

	tot_faces=0;
	_face = new long[min_m];

	//	sort all faces
	long l=0,r=0;
	for(long i=0; i<nfaces; i++)
	{
		double res[3];
		res[0] =	(norm | vrt[trg[list[i]->trg].vindex[0]]) - pld;
		res[1] =	(norm | vrt[trg[list[i]->trg].vindex[1]]) - pld;
		res[2] =	(norm | vrt[trg[list[i]->trg].vindex[2]]) - pld;
		if(fabs(res[0])<LIE_PREC && fabs(res[1])<LIE_PREC && fabs(res[2])<LIE_PREC)
		{
			long tf;
			for(tf=0; tf<tot_faces; tf++)
				if(_face[tf]==list[i]->trg)	break;
			//add new trg
			if(tf==tot_faces)	_face[tot_faces++] = list[i]->trg;

			delete list[i]->vertices;
			delete list[i];
			continue;
		}

		double vdist[256];
		long min_did, max_did;
		double min_dist=1e300, max_dist=-1e300;
		for(long v=0; v<list[i]->nvertices; v++)
		{
			vdist[v] =	(norm | list[i]->vertices[v]) - pld;
			if(vdist[v]>max_dist)
			{
				max_dist = vdist[v];
				max_did = v;
			}
			if(vdist[v]<min_dist)
			{
				min_dist = vdist[v];
				min_did = v;
			}
		}
		if(min_dist>=-PRECISION && max_dist>=-PRECISION)
		{
			rlist[r++] = list[i];
			continue;
		}
		if(min_dist<=PRECISION && max_dist<=PRECISION)
		{
			llist[l++] = list[i];
			continue;
		}

		//face is on the both sides
		BUILD_BSP_FACE *right_face = new BUILD_BSP_FACE;
		*right_face = *list[i];
		while(vdist[max_did]>-PRECISION)	//search for a first vertex
		{
			max_did--;
			if(max_did==-1)	max_did=list[i]->nvertices-1;
		}	//max_did - first

		right_face->nvertices=0;
		right_face->vertices=NULL;
		do
		{
			right_face->nvertices++;
			right_face->vertices = (DVECTOR*)realloc(right_face->vertices, right_face->nvertices*sizeof(DVECTOR));

			if(vdist[max_did]<=-PRECISION)	//split edge
			{
				long v = max_did+1;	if(v==list[i]->nvertices)	v=0;

				double d = vdist[max_did]/(vdist[max_did]-vdist[v]);
				right_face->vertices[right_face->nvertices-1] = list[i]->vertices[max_did] + d*(list[i]->vertices[v] - list[i]->vertices[max_did]);
			}
			else	right_face->vertices[right_face->nvertices-1] = list[i]->vertices[max_did];	//copy vertex

			max_did++;	if(max_did==list[i]->nvertices)	max_did=0;
		}while(vdist[max_did]>-PRECISION);

		right_face->nvertices++;
		right_face->vertices = (DVECTOR*)realloc(right_face->vertices, right_face->nvertices*sizeof(DVECTOR));
		//split the last edge
		long v = max_did-1;	if(v==-1)	v=list[i]->nvertices-1;
		double d = vdist[max_did]/(vdist[max_did]-vdist[v]);
		right_face->vertices[right_face->nvertices-1] = list[i]->vertices[max_did] + d*(list[i]->vertices[v] - list[i]->vertices[max_did]);
		//	add to the right list
		rlist[r++] = right_face;

		//------------------------------------LEFT-----------------------------------------
		right_face = new BUILD_BSP_FACE;
		*right_face = *list[i];
		while(vdist[min_did]<PRECISION)	//search for a first vertex
		{
			min_did--;
			if(min_did==-1)	min_did=list[i]->nvertices-1;
		}	//min_did - first

		right_face->nvertices=0;
		right_face->vertices=NULL;
		do
		{
			right_face->nvertices++;
			right_face->vertices = (DVECTOR*)realloc(right_face->vertices, right_face->nvertices*sizeof(DVECTOR));

			if(vdist[min_did]>=PRECISION)	//split edge
			{
				long v = min_did+1;	if(v==list[i]->nvertices)	v=0;

				double d = vdist[min_did]/(vdist[min_did]-vdist[v]);
				right_face->vertices[right_face->nvertices-1] = list[i]->vertices[min_did] + d*(list[i]->vertices[v] - list[i]->vertices[min_did]);
			}
			else	right_face->vertices[right_face->nvertices-1] = list[i]->vertices[min_did];	//copy vertex

			min_did++;	if(min_did==list[i]->nvertices)	min_did=0;
		}while(vdist[min_did]<PRECISION);

		right_face->nvertices++;
		right_face->vertices = (DVECTOR*)realloc(right_face->vertices, right_face->nvertices*sizeof(DVECTOR));
		//split the last edge
		v = min_did-1;	if(v==-1)	v=list[i]->nvertices-1;
		d = vdist[min_did]/(vdist[min_did]-vdist[v]);
		right_face->vertices[right_face->nvertices-1] = list[i]->vertices[min_did] + d*(list[i]->vertices[v] - list[i]->vertices[min_did]);
		//	add to the right list
		llist[l++] = right_face;

		delete list[i]->vertices;
		delete list[i];
	}

	delete list;

	//calculate size of BSP
	ssize += NODESIZE(tot_faces);
	ndepth[cur_depth] += NODESIZE(tot_faces);

	if(r>0)
	{
		right = new BUILD_BSP_NODE();
		right->FillNode(rlist,r);
	}
	else	right = NULL;

	if(l>0)
	{
		left = new BUILD_BSP_NODE();
		left->FillNode(llist,l);
	}
	else	left = NULL;

	cur_depth--;

	return 0;
}


//-----------------------------------------------------------------------------------
//best blane
//-----------------------------------------------------------------------------------
long BUILD_BSP_NODE::BestPlane(BUILD_BSP_FACE **list, long nfaces)
{
	double res0, res1, res2;
	long best_plane;

	double gd = 1e20;
	for(long f=0; f<min(nfaces, MAX_PLANES_CALCULATED); f++)
	{
		long l=0, r=0, c=0, m=0;
		for(long i=0; i<nfaces; i++)
		{
			res0 =	(list[f]->normal | vrt[trg[list[i]->trg].vindex[0]]) - list[f]->plane_distance;
			res1 =	(list[f]->normal | vrt[trg[list[i]->trg].vindex[1]]) - list[f]->plane_distance;
			res2 =	(list[f]->normal | vrt[trg[list[i]->trg].vindex[2]]) - list[f]->plane_distance;
			if(fabs(res0)<LIE_PREC && fabs(res1)<LIE_PREC && fabs(res2)<LIE_PREC)	//lies on the best face
			{
				m++;
				continue;
			}

			double min_dist=1e300, max_dist=-1e300;
			for(long v=0; v<list[i]->nvertices; v++)
			{
				double dist =	(list[f]->normal | list[i]->vertices[v]) - list[f]->plane_distance;
				if(dist>max_dist)	max_dist = dist;
				if(dist<min_dist)	min_dist = dist;
			}

			if(min_dist>=-PRECISION && max_dist>=-PRECISION)
			{
				r++;
				continue;
			}
			if(min_dist<=PRECISION && max_dist<=PRECISION)
			{
				l++;
				continue;
			}
			c++;	//triangle is on the both sides
		}

		double dist = 3.0*c + 1.0*fabs(r-l);
		if(dist<gd)
		{
			gd = dist;
			best_plane = f;
			min_l = l;
			min_r = r;
			min_c = c;
			min_m = m;
		}
	}
	return best_plane;
}


//-----------------------------------------------------------------------------------
//empty plane
//-----------------------------------------------------------------------------------
void BUILD_BSP_NODE::BestEmptyPlane(BUILD_BSP_FACE **list, long nfaces, DVECTOR *epnorm, double *epdist)
{
	double res0, res1, res2;
	long f;

	double gd = 1e20;
	long nf = min(MAX_PLANES_CALCULATED,nfaces);
	for(f=0; f<nf; f++)
	{
		for(long e=0; e<3; e++)
		{
			long e1 = e+1;	if(e1==3)	e1=0;
			DVECTOR edge = list[f]->vertices[e1] - list[f]->vertices[e];
			DVECTOR bnormal = !(((list[f]->normal*sqrt(~edge)) ^ edge));
			double bplane_distance = bnormal | list[f]->vertices[e];

			long l=0, r=0, c=0, m=0;
			long i;
			for(i=0; i<nfaces; i++)
			{
				res0 =	(bnormal | vrt[trg[list[i]->trg].vindex[0]]) - bplane_distance;
				res1 =	(bnormal | vrt[trg[list[i]->trg].vindex[1]]) - bplane_distance;
				res2 =	(bnormal | vrt[trg[list[i]->trg].vindex[2]]) - bplane_distance;
				if(fabs(res0)<LIE_PREC && fabs(res1)<LIE_PREC && fabs(res2)<LIE_PREC)	//lies on the best face
					break;

				double min_dist=1e300, max_dist=-1e300;
				for(long v=0; v<list[i]->nvertices; v++)
				{
					double dist =	(bnormal | list[i]->vertices[v]) - bplane_distance;
					if(dist>max_dist)	max_dist = dist;
					if(dist<min_dist)	min_dist = dist;
				}
				if(min_dist>=-PRECISION && max_dist>=-PRECISION)
				{
					r++;
					continue;
				}
				if(min_dist<=PRECISION && max_dist<=PRECISION)
				{
					l++;
					continue;
				}
				c++;	//triangle is on the both sides
			}

			if(i<nfaces)	continue;

			double dist = 3.0*c + 1.0*fabs(r-l);
			if(r>0 && l>0 && dist<gd)
			{
				gd = dist;
				min_l = l;
				min_r = r;
				min_c = c;
				min_m = m;
				*epnorm = bnormal;
				*epdist = bplane_distance;
			}
		}
	}
}

//-----------------------------------------------------------------------------------
//store
//-----------------------------------------------------------------------------------
long BUILD_BSP_NODE::Store()
{
	tnode++;

	BSP_NODE *node = &sroot[ndepth[cdepth]];

	node->sign = 0;
	node->type = 0;
	node->norm.x = float(norm.x);
	node->norm.y = float(norm.y);
	node->norm.z = float(norm.z);
	node->pd = float(pld);

	//store all faces
	unsigned char *a = (unsigned char*)&node->face;
	for(long f=0; f<tot_faces; f++)
	{
		*(a++) = *(((unsigned char*)&_face[f])+0);
		*(a++) = *(((unsigned char*)&_face[f])+1);
		*(a++) = *(((unsigned char*)&_face[f])+2);
	}

	node->nfaces = tot_faces;
	if(node->nfaces>MAX_PLANE_FACES)	throw "Internal error: too many faces on the BSP node";

	
	ndepth[cdepth] += NODESIZE(node->nfaces);

	cdepth++;

	node->left = 0;
	node->node = 0;
	if(left!=NULL)
	{
		node->node = left->Store();
		//left is present
		node->left = 1;
	}

	if(right!=NULL)	//right is present
		if(left==NULL)
		{
			//left is not present
			node->node = right->Store() - 1;
			node->right = 1;
		}
		else
		{
			//node's size 1..3 blocks of 24 byte
			node->right = right->Store() - node->node;
		}

	cdepth--;
	return long(node-sroot);
}
