#include "Weather.h"

NetWeather::NetWeather()
{
}

NetWeather::~NetWeather()
{
	CleanUP();
} 

void NetWeather::SetDevice()
{
	pRS = (VDX8RENDER *)api->CreateService("dx8render"); Assert(pRS);
}

bool NetWeather::Init()
{
	SetDevice();
	return true;
}

void NetWeather::Move()
{
}

void NetWeather::Realize(dword Delta_Time)
{
}

void NetWeather::Execute(dword Delta_Time)
{
}

void NetWeather::CleanUP()
{
	pRS->SetRenderState(D3DRS_FOGENABLE, false);
	pRS->LightEnable(0, false);
}

void NetWeather::SetCommonStates()
{
	pRS->SetRenderState(D3DRS_FOGENABLE, bFogEnable);

	pRS->SetRenderState(D3DRS_FOGCOLOR, dwFogColor);
	pRS->SetRenderState(D3DRS_FOGDENSITY, *((dword*)&fFogDensity));

	pRS->SetRenderState(D3DRS_AMBIENT, dwSunAmbient);

	// setup sun light
	CVECTOR		vSun, vSunColor, vSunLight;
	D3DLIGHT8	sun;

	vSun = CVECTOR(fSunX, fSunY, fSunZ);

	vSunColor.x = float((dwSunColor >> 0x10) & 0xFF) / 255.0f;
	vSunColor.y = float((dwSunColor >> 0x8) & 0xFF) / 255.0f;
	vSunColor.z = float((dwSunColor) & 0xFF) / 255.0f;

	sun.Type = D3DLIGHT_DIRECTIONAL;
	sun.Diffuse.r = vSunColor.x;	sun.Diffuse.g = vSunColor.y;	sun.Diffuse.b = vSunColor.z;	sun.Diffuse.a = 1.0f;
	sun.Specular.r = vSunColor.x;	sun.Specular.g = vSunColor.y;	sun.Specular.b = vSunColor.z;	sun.Specular.a = 1.0f;
	sun.Ambient.r = sun.Ambient.g = sun.Ambient.b = sun.Ambient.a = 0.0f;
	vSunLight = -(!(CVECTOR(vSun.x, vSun.y, vSun.z)));
	sun.Direction.x = vSunLight.x;	sun.Direction.y = vSunLight.y;	sun.Direction.z = vSunLight.z;
	sun.Range = 100000;
	sun.Attenuation0 = 1;	sun.Attenuation1 = 0;	sun.Attenuation2 = 0;
	sun.Falloff = 0;
	sun.Theta = 1;
	sun.Phi = 1;

	pRS->SetLight(0, &sun);
	pRS->LightEnable(0, true);

	pRS->SetRenderState(D3DRS_LIGHTING, false);
}

dword NetWeather::AttributeChanged(ATTRIBUTES * pAttribute)
{
	ATTRIBUTES * pParent = pAttribute->GetParent(); 
	
	if (*pParent == "fog")
	{
		if (*pAttribute == "Enable")		{ bFogEnable = pAttribute->GetAttributeAsDword() != 0; return 0; }
		if (*pAttribute == "Start")			{ fFogStart = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Density")		{ fFogDensity = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "Color")			{ dwFogColor = pAttribute->GetAttributeAsDword(); return 0; }
	}

	if (*pParent == "sun")
	{
		if (*pAttribute == "Color")			{ dwSunColor = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "Ambient")		{ dwSunAmbient = pAttribute->GetAttributeAsDword(); return 0; }
		if (*pAttribute == "x")				{ fSunX = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "y")				{ fSunY = pAttribute->GetAttributeAsFloat(); return 0; }
		if (*pAttribute == "z")				{ fSunZ = pAttribute->GetAttributeAsFloat(); return 0; }
	}

	if (*pAttribute == "isDone")
	{
		SetCommonStates();
	}
	return 0;
}

