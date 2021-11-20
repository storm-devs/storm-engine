/******************************************************************************
File:  vertex.h

Author:   Nick Chirkov
Copyright (C) 2000-2002 Nick Chirkov

Comments:
******************************************************************************/
#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <windows.h>
#include "export.h"

#define MIN_SQUARE 1e-20f
#define VERTEX_DELTA 1e-10f
#define TEXTURE_DELTA 1e-4f
#define NORMAL_DELTA 0.99f
#define WEIGHT_DELTA 1e-3f

//----------------------------------------------------------------------
//vertex types hierarchy
//----------------------------------------------------------------------
class VERTEX0
{
public:
	CVECTOR pos;
	CVECTOR norm;
	long color;
	float tu0, tv0;

	VERTEX0(const EXPORT_STATIC::VERTEX &v)
	{
		pos = CVECTOR(v.x, v.y, v.z);
		norm = CVECTOR(v.nx, v.ny, v.nz);
		color = v.color;
		tu0 = v.tu[0];	tv0 = v.tv[0];
	}

	virtual bool operator==(const VERTEX0 &rv)
	{
		return(
			fabs(rv.tu0 - tu0) < TEXTURE_DELTA &&
			fabs(rv.tv0 - tv0) < TEXTURE_DELTA &&
			~(rv.pos - pos) < VERTEX_DELTA*VERTEX_DELTA &&
			rv.color==color &&
			(rv.norm|norm) > NORMAL_DELTA
			);
	}

	virtual long size()
	{
		return sizeof(RDF_VERTEX0);
	}
	virtual long store(char *buf)
	{
		RDF_VERTEX0 *v = (RDF_VERTEX0*)buf;
		v->pos = pos;
		v->norm = norm;
		v->color = color;
		v->tu0 = tu0;
		v->tv0 = tv0;
		return size();
	}
	virtual long load(char *buf)
	{
		RDF_VERTEX0 *v = (RDF_VERTEX0*)buf;
		pos = v->pos;
		norm = v->norm;
		color = v->color;
		tu0 = v->tu0;
		tv0 = v->tv0;
		return size();
	}

};


//----------------------------------------------------------------------
// 2 texture sets
//----------------------------------------------------------------------
class VERTEX1 : public VERTEX0
{
public:
	float tu1, tv1;

	VERTEX1(const EXPORT_STATIC::VERTEX &v)	: VERTEX0(v)
	{
		tu1 = v.tu[1];	tv1 = v.tv[1];
	}

	virtual bool operator==(const VERTEX0 &_rv)
	{
		VERTEX1 &rv = *(VERTEX1*)&_rv;
		return(
			fabs(rv.tu1 - tu1) < TEXTURE_DELTA && fabs(rv.tv1 - tv1) < TEXTURE_DELTA && 
			VERTEX0::operator==(rv)
			);
	}

	virtual long size()
	{
		return sizeof(RDF_VERTEX1);
	}
	virtual long store(char *buf)
	{
		RDF_VERTEX1 *v = (RDF_VERTEX1*)buf;
		VERTEX0::store(buf);
		v->tu1 = tu1;	v->tv1 = tv1;
		return size();
	}
	virtual long load(char *buf)
	{
		RDF_VERTEX1 *v = (RDF_VERTEX1*)buf;
		VERTEX0::load(buf);
		tu1 = v->tu1;
		tv1 = v->tv1;
		return size();
	}

};


//----------------------------------------------------------------------
// 3 texture sets
//----------------------------------------------------------------------
class VERTEX2 : public VERTEX1
{
public:
	float tu2, tv2;

	VERTEX2(const EXPORT_STATIC::VERTEX &v)	:	VERTEX1(v)
	{
		tu2 = v.tu[2];	tv2 = v.tv[2];
	}

	virtual bool operator==(const VERTEX0 &_rv)
	{
		VERTEX2 &rv = *(VERTEX2*)&_rv;
		return(
			fabs(rv.tu2 - tu2) < TEXTURE_DELTA && fabs(rv.tv2 - tv2) < TEXTURE_DELTA && 
			VERTEX1::operator==(rv)
			);
	}

	virtual long size()
	{
		return sizeof(RDF_VERTEX2);
	}
	virtual long store(char *buf)
	{
		RDF_VERTEX2 *v = (RDF_VERTEX2*)buf;
		VERTEX1::store(buf);
		v->tu2 = tu2;	v->tv2 = tv2;
		return size();
	}
	virtual long load(char *buf)
	{
		RDF_VERTEX2 *v = (RDF_VERTEX2*)buf;
		VERTEX1::load(buf);
		tu2 = v->tu2;
		tv2 = v->tv2;
		return size();
	}

};


//----------------------------------------------------------------------
// 3 texture sets
//----------------------------------------------------------------------
class VERTEX3 : public VERTEX2
{
public:
	float tu3, tv3;

	VERTEX3(const EXPORT_STATIC::VERTEX &v)	:	VERTEX2(v)
	{
		tu3 = v.tu[3];	tv3 = v.tv[3];
	}

	virtual bool operator==(const VERTEX0 &_rv)
	{
		VERTEX3 &rv = *(VERTEX3*)&_rv;
		return(
			fabs(rv.tu3 - tu3) < TEXTURE_DELTA && fabs(rv.tv3 - tv3) < TEXTURE_DELTA && 
			VERTEX2::operator==(rv)
			);
	}

	virtual long size()
	{
		return sizeof(RDF_VERTEX3);
	}
	virtual long store(char *buf)
	{
		RDF_VERTEX3 *v = (RDF_VERTEX3*)buf;
		VERTEX2::store(buf);
		v->tu3 = tu3;	v->tv3 = tv3;
		return size();
	}
	virtual long load(char *buf)
	{
		RDF_VERTEX3 *v = (RDF_VERTEX3*)buf;
		VERTEX2::load(buf);
		tu3 = v->tu3;
		tv3 = v->tv3;
		return size();
	}

};


//----------------------------------------------------------------------
// 1 texture sets with animation
//----------------------------------------------------------------------
class VERTEXA0 : public VERTEX0
{
public:
	float weight;
	unsigned long boneid;

	VERTEXA0(const EXPORT_STATIC::VERTEX &v)	:	VERTEX0(v)
	{
		weight = v.weight[0];
		boneid = (v.boneid[1]<<8)|(v.boneid[0]<<0);
	}

	virtual bool operator==(const VERTEXA0 &_rv)
	{
		VERTEXA0 &rv = *(VERTEXA0*)&_rv;
		return(
			fabs(rv.weight - weight) < WEIGHT_DELTA && rv.boneid==boneid && 
			VERTEX0::operator==(rv)
			);
	}

	virtual long size()
	{
		return sizeof(RDF_AVERTEX0);
	}
	virtual long store(char *buf)
	{
		RDF_AVERTEX0 *v = (RDF_AVERTEX0*)buf;
		v->pos = pos;
		v->weight = weight;
		v->boneid = boneid;
		v->norm = norm;
		v->color = color;
		v->tu0 = tu0;
		v->tv0 = tv0;
		return size();
	}
	virtual long load(char *buf)
	{
		RDF_AVERTEX0 *v = (RDF_AVERTEX0*)buf;
		pos = v->pos;
		weight = v->weight;
		boneid = v->boneid;
		norm = v->norm;
		color = v->color;
		tu0 = v->tu0;
		tv0 = v->tv0;
		return size();
	}
};


#endif