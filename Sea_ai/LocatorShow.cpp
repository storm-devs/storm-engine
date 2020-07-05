#include "LocatorShow.h"

SeaLocatorShow::SeaLocatorShow()
{
	fScale = 0.7f;
	bShow = false;
	sphereNumTrgs = 0;
	sphereVertex = null;
	pALocators = 0;
}

SeaLocatorShow::~SeaLocatorShow()
{
	DELETE(sphereVertex);
}

bool SeaLocatorShow::Init()
{
	return true;
}

void SeaLocatorShow::SetDevice()
{
}

bool SeaLocatorShow::isLocator(ATTRIBUTES * pA)
{
	return pA->FindAClass(pA,"z") != 0;
}

CVECTOR SeaLocatorShow::GetLocatorPos(ATTRIBUTES * pA)
{
	CVECTOR v;
	v.x = pA->GetAttributeAsFloat("x");
	v.y = pA->GetAttributeAsFloat("y");
	v.z = pA->GetAttributeAsFloat("z");
	return v;
}

char * SeaLocatorShow::GetRealLocatorName(ATTRIBUTES * pA)
{
	return pA->GetThisName();
}

char * SeaLocatorShow::GetLocatorName(ATTRIBUTES * pA)
{
	char * pName = pA->GetAttribute("name");
	if (!pName) pName = pA->GetThisName();
	return pName;
}

char * SeaLocatorShow::GetLocatorGroupName(ATTRIBUTES * pA)
{
	ATTRIBUTES * pAParent = pA->GetParent(); Assert(pAParent);
	return pAParent->GetThisName();
}

float SeaLocatorShow::GetLocatorRadius(ATTRIBUTES * pA)
{
	return pA->GetAttributeAsFloat("radius",0.0f);
}

float SeaLocatorShow::GetLocatorAng(ATTRIBUTES * pA)
{
	return pA->GetAttributeAsFloat("ay",0.0f);
}

void SeaLocatorShow::PrintLocator(ATTRIBUTES * pA)
{
	MTX_PRJ_VECTOR	vrt;

	if(!sphereNumTrgs) CreateSphere();
	CMatrix mPos;

	float d = view.Vz() | view.Pos();

	long fh = long(AIHelper::pRS->CharHeight(FONT_DEFAULT) * fScale);

	CVECTOR vPos = GetLocatorPos(pA);
	float fAng = GetLocatorAng(pA);
	if((vPos | view.Vz()) < d) return;

	vPos.y += 1.0f;

	CMatrix m;
	m.BuildRotateY(fAng);
	m.SetPosition(vPos);
	//m.BuildPosition(vPos.x,vPos.y,vPos.z);
	AIHelper::pRS->SetTransform(D3DTS_WORLD, m);

	AIHelper::pRS->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE, sphereNumTrgs, sphereVertex, sizeof(SphVertex), "SeaLocatorsShow");

	mtx.Projection(&vPos, &vrt, 1, fWidth, fHeight, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
	vPos = CVECTOR(vrt.x, vrt.y, vrt.z);

	char * pName, * pGName;
	
	if (pGName = GetLocatorGroupName(pA))	AIHelper::Print(vPos.x, vPos.y - fh * 0.8f, fScale, "grp: \"%s\"", pGName);
	if (pName = GetLocatorName(pA))			AIHelper::Print(vPos.x, vPos.y, fScale, "loc: \"%s\"", pName);
	float fRadius = GetLocatorRadius(pA);
	AIHelper::Print(vPos.x, vPos.y + fh * 0.8f, fScale, "rad: %.2f", fRadius);
	if (fRadius > 0.0f)
	{
		SphVertex			* pVrt;
		array<SphVertex>	Vrts(_FL_);
		CVECTOR				vPos1 = GetLocatorPos(pA);
		CVECTOR				vCenter = CVECTOR(vPos1.x, 2.0f, vPos1.z);
		
		pVrt = &Vrts[Vrts.Add()];
		pVrt->v = vCenter;
		pVrt->c = 0x4F00FF00;
		
		for (dword i=0;i<32;i++)
		{
			float fAngle = float(i) / 31.0f * PIm2;
			pVrt = &Vrts[Vrts.Add()];			
			pVrt->v = vCenter + CVECTOR(sinf(fAngle) * fRadius, 0.0f, cosf(fAngle) * fRadius);
			pVrt->c = 0x0F00FF00;
		}
		CMatrix m_ident;
		m_ident.SetIdentity();
		AIHelper::pRS->SetTransform(D3DTS_WORLD, m_ident);
		AIHelper::pRS->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, D3DFVF_XYZ | D3DFVF_DIFFUSE, Vrts.Size()-2, &Vrts[0], sizeof(SphVertex), "SeaLocatorsShow");
	}
}

void SeaLocatorShow::ProcessLocators(ATTRIBUTES * pA)
{
	if (!pA) return;
	if (isLocator(pA)) PrintLocator(pA);
	for (dword i=0;i<pA->GetAttributesNum();i++) ProcessLocators(pA->GetAttributeClass(i));
}

void SeaLocatorShow::Realize(dword Delta_Time)
{
#ifndef _XBOX
	if (api->Controls->GetDebugAsyncKeyState('6')<0) fScale -= float(Delta_Time) * 0.001f * 0.5f;
	if (api->Controls->GetDebugAsyncKeyState('7')<0) fScale += float(Delta_Time) * 0.001f * 0.5f;
	if (api->Controls->GetDebugAsyncKeyState('5')<0) 
	{
		bShow ^= 1;
		Sleep(200);
	}
#endif
	if (!bShow || !pALocators) return;
	CMatrix prj;
	AIHelper::pRS->GetTransform(D3DTS_VIEW, view);
	AIHelper::pRS->GetTransform(D3DTS_PROJECTION, prj);
	mtx.EqMultiply(view, prj);

	D3DVIEWPORT8	vp;

	AIHelper::pRS->GetViewport(&vp);
	fWidth = vp.Width*0.5f;
	fHeight = vp.Height*0.5f;
	view.Transposition();
	// Рисуем

	//Уберём текстуры	
	AIHelper::pRS->TextureSet(0, -1);
	AIHelper::pRS->TextureSet(1, -1);

	ProcessLocators(pALocators);
}

void SeaLocatorShow::Execute(dword Delta_Time)
{
	if (!pALocators) return;
}
	
bool SeaLocatorShow::CreateState(ENTITY_STATE_GEN * state_gen)
{
	return true;
}

bool SeaLocatorShow::LoadState(ENTITY_STATE * state)
{
	return true;
}
	
void SeaLocatorShow::ProcessMessage(dword iMsg,dword wParam,dword lParam)
{
}

dword _cdecl SeaLocatorShow::ProcessMessage(MESSAGE & message)
{
	pALocators = message.AttributePointer();
	return 0;
}

dword SeaLocatorShow::AttributeChanged(ATTRIBUTES *pAttribute)
{
	return 0;
}

void SeaLocatorShow::CreateSphere()
{
#define CalcKColor(ind) {kColor = light | !CVECTOR(sphereVertex[t*3 + ind].v.x, sphereVertex[t*3 + ind].v.y, sphereVertex[t*3 + ind].v.z); if(kColor < 0.0f) kColor = 0.0f; }
#define CLerp(c, min) (dword(c*(kColor*(1.0f - min) + min)))
#define Color ((CLerp(255.0f, 0.5f) << 24) | (CLerp(255.0f, 0.7f) << 16) | (CLerp(255.0f, 0.7f) << 8) | (CLerp(255.0f, 0.7f) << 0));
	
	if(sphereVertex) return;

	const float myPI = 3.1415926535897932f;
	const long a1 = 32;
	const long a2 = (a1/2);

	sphereNumTrgs = a1*a2*2;
	sphereVertex = NEW SphVertex[sphereNumTrgs*6];

	CVECTOR light = !CVECTOR(0.0f, 0.0f, 1.0f);
	float kColor;
	//Заполняем вершины
	for(long i = 0, t = 0; i < a2; i++)
	{
		float r1 = sinf(myPI*i/float(a2));
		float y1 = cosf(myPI*i/float(a2));
		float r2 = sinf(myPI*(i + 1)/float(a2));
		float y2 = cosf(myPI*(i + 1)/float(a2));
		for(long j = 0; j < a1; j++)
		{
			float x1 = sinf(2.0f*myPI*j/float(a1));
			float z1 = cosf(2.0f*myPI*j/float(a1));
			float x2 = sinf(2.0f*myPI*(j + 1)/float(a1));
			float z2 = cosf(2.0f*myPI*(j + 1)/float(a1));
			//0
			sphereVertex[t*3 + 0].v.x = r1*x1;
			sphereVertex[t*3 + 0].v.y = y1;
			sphereVertex[t*3 + 0].v.z = r1*z1;
			CalcKColor(0);
			sphereVertex[t*3 + 0].c = Color;
			//1
			sphereVertex[t*3 + 1].v.x = r2*x1;
			sphereVertex[t*3 + 1].v.y = y2;
			sphereVertex[t*3 + 1].v.z = r2*z1;
			CalcKColor(1);
			sphereVertex[t*3 + 1].c = Color;
			//2
			sphereVertex[t*3 + 2].v.x = r1*x2;
			sphereVertex[t*3 + 2].v.y = y1;
			sphereVertex[t*3 + 2].v.z = r1*z2;
			CalcKColor(2);
			sphereVertex[t*3 + 2].c = Color;
			//3 = 2
			sphereVertex[t*3 + 3] = sphereVertex[t*3 + 2];
			//4 = 1
			sphereVertex[t*3 + 4] = sphereVertex[t*3 + 1];
			//5
			sphereVertex[t*3 + 5].v.x = r2*x2;
			sphereVertex[t*3 + 5].v.y = y2;
			sphereVertex[t*3 + 5].v.z = r2*z2;
			CalcKColor(5);
			sphereVertex[t*3 + 5].c = Color;
			//Добавили 2 треугольника
			t += 2;
		}
	}
}
