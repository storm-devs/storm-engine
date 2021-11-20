/******************************************************************************
File:  export.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef _RDFEXP_H_
#define _RDFEXP_H_

#pragma warning ( disable : 4786)

#include <cmath>
#include <vector>
#include <stdio.h>
#include "cvector.h"
#include "rdf_exp.h"
#include "rdf.h"
#include "vertex.h"

template <class T> T SQR(T a)	{	return a*a;	};

struct GEOMLINK
{
	long obj;
	long trg;
};

class SEARCH_NODE
{
public:
	long vertex;
	float value;
	long left, right;
};

//----------------------------------------------------------------------
//object support class
//----------------------------------------------------------------------
class EOBJECT
{
public:

	enum FLAGS	{	MERGED = 1	};
	long flags;
	long group_name;
	long name;
	long mtl;
	std::vector<RDF_TRIANGLE> trg;
	std::vector<VERTEX0*> vertex;

	//used for fast vertex search
	long tot_nodes;
	SEARCH_NODE *root;
	long Insert(long n, float v);

	DVECTOR boxmin, boxmax, center;
	double radius;

	EOBJECT();
	EOBJECT(const EOBJECT &o);
	~EOBJECT();
	void AddTriangle(VERTEX0 **vrt);
	void CalculateBoxRadius();
	void Optimize();
};

//----------------------------------------------------------------------
//main export class
//----------------------------------------------------------------------
class GEOM_STATIC : public EXPORT_STATIC
{

	long trgaddby2side, tot_triangles, tot_vertices;
	bool build_bsp, production, animation, geoaccess;

	TEXTURE_FUNC tfn;
	RESULT_FUNC fnresult;
	RESULT_FUNC fnerror;
	FILE *fl;
	float max_elength, merge_size;
	long flags;


	std::vector<std::string> globname;

	std::vector<long> texture;
	std::vector<long> texid;
	std::vector<RDF_MATERIAL> material;
	std::vector<RDF_LIGHT> light;
	std::vector<EOBJECT> object;
	std::vector<RDF_LABEL> label;

	long FindGlobalName(const std::string &s);
	long AddTexture(const TEXTURE &tex);
	void SplitObject(long o);
public:

	GEOM_STATIC(const std::string &fname,
						 TEXTURE_FUNC Textures, RESULT_FUNC fnr, RESULT_FUNC fne,
						 float maxedge, float merge_size, long flags);

	virtual ~GEOM_STATIC();
	virtual void AddObject(const OBJECT &obj);
	virtual void AddLight(const LIGHT &l);
	virtual void AddLabel(const LABEL &l);
	virtual void WriteFile();
};

#endif