//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmRenderModel
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "..\common_h\geometry.h"

#include "WdmRenderModel.h"

#define WDM_MODEL_TECH	"WdmModelDrawStd"
#define WDM_MODEL_TECHA	"WdmModelDrawStdA"

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

WdmRenderModel::WdmRenderModel()
{
	geo = null;
	alpha = 1.0f;
	tech = WDM_MODEL_TECH;
	techa = WDM_MODEL_TECHA;
	center = 0.0f;
	radius = 0.0f;

	drawCircle = false;
}

WdmRenderModel::~WdmRenderModel()
{
}

bool WdmRenderModel::Load(const char * modelName)
{
	geo = wdmObjects->CreateGeometry(modelName);
	if(geo)
	{	
		GEOS::INFO ginfo;
		geo->GetInfo(ginfo);
		center = CVECTOR(ginfo.boxcenter.x, ginfo.boxcenter.y, ginfo.boxcenter.z);
		radius = sqrtf(ginfo.boxsize.x*ginfo.boxsize.x + ginfo.boxsize.z*ginfo.boxsize.z)*0.51f;
		return true;
	}
	return false;
}

void WdmRenderModel::PRender(VDX8RENDER * rs)
{
	LRender(rs);
}

void WdmRenderModel::MRender(VDX8RENDER * rs)
{
	CMatrix m(mtx);
	m.m[0][1] = -m.m[0][1];
	m.m[1][1] = -m.m[1][1];
	m.m[2][1] = -m.m[2][1];
	m.m[3][1] = -m.m[3][1];
	rs->SetTransform(D3DTS_WORLD, m);
	Render(rs);
}

void WdmRenderModel::LRender(VDX8RENDER * rs)
{
	rs->SetTransform(D3DTS_WORLD, mtx);
	Render(rs);

	/*
	if(drawCircle)
	{
		CMatrix m(mtx);
		m.Pos() = m*center;
		wdmObjects->DrawCircle(m, radius, 0x2f0fffff);
		//wdmObjects->DrawCircle(mtx, modelRadius, 0x2fffff0f);		
	}*/
}

void WdmRenderModel::SetTech(const char * t, const char * ta)
{
	if(t) tech = t; else tech = WDM_MODEL_TECH;
	if(ta) techa = ta; else techa = WDM_MODEL_TECHA;
}

long WdmRenderModel::GetTexture(long stage)
{
	if(stage >= 4 || stage < 0 || !geo) return -1;
	GEOS::MATERIAL mtl;
	geo->GetMaterial(0, mtl);
	return mtl.texture[stage];
}

void WdmRenderModel::SetTexture(long stage, long id)
{
	if(stage >= 4 || stage < 0 || !geo) return;
	GEOS::MATERIAL mtl;
	geo->GetMaterial(0, mtl);
	mtl.texture[stage] = id;
	geo->SetMaterial(0, mtl);
}

void WdmRenderModel::Render(VDX8RENDER * rs)
{
	if(!geo) return;
	float a = alpha*255.0f;
	if(wdmObjects->isDebug && a < 80.0f) a = 80.0f;
	if(a < 1.0f) return;
	//Риссуем
	if(a >= 255.0f)
	{
		a = 255.0f;
		wdmObjects->gs->SetTechnique(tech);
	}else{
		wdmObjects->gs->SetTechnique(techa);
		rs->SetRenderState(D3DRS_TEXTUREFACTOR, (long(a) << 24) | 0xffffff);
	}
	//Проверим на видимость
	PLANE * plane = rs->GetPlanes();
	static CMatrix mtx;
	rs->GetTransform(D3DTS_WORLD, mtx);
	CVECTOR v = mtx*center;
	for(long i = 0; i < 4; i++)
	{
		PLANE & p = plane[i];
		float dist = v.x*p.Nx + v.y*p.Ny + v.z*p.Nz - p.D;
		if(dist < -radius) return;
	}
	geo->Draw(null, 0, null);
	wdmObjects->gs->SetTechnique("");
}

