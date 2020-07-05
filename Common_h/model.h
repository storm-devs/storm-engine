#ifndef _MODEL_H_
#define _MODEL_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\object.h"
#include "..\common_h\matrix.h"
#include "..\common_h\animation.h"
#include "..\common_h\messages.h"
#include "..\common_h\geos.h"

class NODER;

class NODE
{
public:
	CMatrix loc_mtx;
	CMatrix glob_mtx;

	enum FLAGS
	{
		VISIBLE = (1<<0),
		VISIBLE_TREE = (1<<1),
		CLIP_ENABLE = (1<<2),
		CLIP_ENABLE_TREE = (1<<3),
		TRACE_ENABLE = (1<<4),
		TRACE_ENABLE_TREE = (1<<5)
	};
	long flags;

	GEOS *geo;
	GEOS::MATERIAL_FUNC geoMaterialFunc;
	//children
	long nnext;
	NODE **next;
	//0 for root
	NODE *parent;
	virtual ~NODE(){};
	//name of this node - locator's name
	virtual const char *GetName() = 0;

	//unlink node
	virtual NODE *Unlink() = 0;
	//unlink node to model
	virtual ENTITY_ID Unlink2Model() = 0;
	//link node to node
	virtual void Link(NODE *node) = 0;
	//link model to node
	virtual void Link(ENTITY_ID model, bool transform = true) = 0;

	virtual void SetTechnique(const char *name) = 0;
	virtual const char *GetTechnique() = 0;
	virtual bool Init(const char *lightPath, const char *pname, const char *oname, CMatrix &m, CMatrix &globm, NODER *par, const char *lmPath)= 0;

	virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
};

class VDX8RENDER;


class MODEL : public COLLISION_OBJECT
{
public:
	class RenderTuner
	{
		friend MODEL;
	public:
		RenderTuner(){};
		virtual ~RenderTuner(){};
		virtual void Set(MODEL * model, VDX8RENDER * rs){};
		virtual void Restore(MODEL * model, VDX8RENDER * rs){};
	};
	void SetRenderTuner(RenderTuner * rt){ renderTuner = rt; };
	RenderTuner * GetRenderTuner(RenderTuner * rt){ return renderTuner; };
protected:
	RenderTuner * renderTuner;
public:



public:
	MODEL(){ renderTuner = null; };
	virtual ~MODEL(){ renderTuner = null; };

	virtual NODE *GetNode(long n) = 0;
	virtual NODE *FindNode(const char *cNodeName) = 0;
	virtual void Update() = 0;
	virtual Animation * GetAnimation() = 0;

	virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
	virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) = 0;

	virtual const char *GetCollideMaterialName() = 0;
	virtual bool GetCollideTriangle(TRIANGLE &triangle) = 0;

	virtual NODE *GetCollideNode() = 0;
};

#endif

