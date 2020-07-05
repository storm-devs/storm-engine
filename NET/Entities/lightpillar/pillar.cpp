#include "Pillar.h"

NetLightPillar::NetLightPillar()
{
	vPos = 0.0f;
}

NetLightPillar::~NetLightPillar()
{
}

bool NetLightPillar::Init()
{
	return true;
}

void NetLightPillar::Realize(dword DeltaTime)
{
	// draw pillar
	
	for (long i=0; i<NUM_SEGMENTS; i++)
	{
		float fAng = float(i) / (NUM_SEGMENTS - 1) * PIm2;
		float x = 0.0f, z = fRadius;
		RotateAroundY(x, z, cosf(fAng), sinf(fAng));
		Vertex[i * 2 + 0].vPos = vPos + CVECTOR(x, 0.0f, z);
		Vertex[i * 2 + 0].dwColor = dwColor1;

		Vertex[i * 2 + 1].vPos = vPos + CVECTOR(x, fHeight, z);
		Vertex[i * 2 + 1].dwColor = dwColor2;
	}

	CMatrix mWorld;	mWorld.SetIdentity();
	Render().SetWorld(mWorld);
	Render().DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZ | D3DFVF_DIFFUSE, (NUM_SEGMENTS - 1) * 2, &Vertex[0], sizeof(Vertex[0]), "LightPillar");
}

dword NetLightPillar::AttributeChanged(ATTRIBUTES * pAttribute)
{
	ATTRIBUTES * pParent = pAttribute->GetParent();

	if (*pAttribute == "Radius")	fRadius = pAttribute->GetAttributeAsFloat();
	if (*pAttribute == "Height")	fHeight = pAttribute->GetAttributeAsFloat();
	if (*pAttribute == "Color1")	dwColor1 = pAttribute->GetAttributeAsDword();
	if (*pAttribute == "Color2")	dwColor2 = pAttribute->GetAttributeAsDword();

	if (*pParent == "Pos")
	{
		if (*pAttribute == "x")	{ vPos.x = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "z")	{ vPos.z = pAttribute->GetAttributeAsFloat(); return 0; }
	}

	return 0;
}
