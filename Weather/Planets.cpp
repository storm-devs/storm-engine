#include "Astronomy.h"
#include "..\common_h\Weather_Base.h"

Astronomy::PLANETS::PLANETS() : aPlanets(_FL_)
{
	fPlanetScale = 1.0f;

	fPlanetFade = 1.f;
	fFadeTimeStart = -1.f;
	fFadeTime = 1.f;
}

Astronomy::PLANETS::~PLANETS()
{
	ReleasePlanets();
}

void Astronomy::PLANETS::ReleasePlanets()
{
	for (long i=0; i<aPlanets; i++)
	{
		//Astronomy::pGS->DeleteGeometry(aPlanets[i].pGeo);
		if (aPlanets[i].iTexture >= 0) Astronomy::pRS->TextureRelease(aPlanets[i].iTexture);
	}

	aPlanets.DelAll();
}

void Astronomy::PLANETS::Init(ATTRIBUTES * pAP)
{
	ReleasePlanets();
	aPlanets.DelAll();

	Astronomy::pGS->SetTexturePath("Weather\\Astronomy\\Planets\\");

	//ATTRIBUTES * pAScale = pAP->FindAClass(pAP, "Planets.Scale");
	//fPlanetScale = ((pAScale) ? pAScale->GetAttributeAsFloat() : 1.0f);
	TimeUpdate(pAP);

	ATTRIBUTES * pAPlanets = pAP->FindAClass(pAP, "Planets.Planet");
	if (pAPlanets) for (dword i=0; i<pAPlanets->GetAttributesNum(); i++)
	{
		ATTRIBUTES * pAPlanet = pAPlanets->GetAttributeClass(i);
		ATTRIBUTES * pAMag = pAPlanet->GetAttributeClass("Mag");

		string sName = pAPlanet->GetThisName();

		Planet & p = aPlanets[aPlanets.Add()];
		p.fDiameter = pAPlanet->GetAttributeAsFloat("Diameter");
		p.fSpeed = pAPlanet->GetAttributeAsFloat("Speed");
		p.fDistance = pAPlanet->GetAttributeAsFloat("Distance");
		p.fInclination = pAPlanet->GetAttributeAsFloat("Inclination");
		p.fFakeScale = pAPlanet->GetAttributeAsFloat("Scale", 1.0f);
		p.fMagMax = (pAMag) ? pAMag->GetAttributeAsFloat("Max") : 12.0f;
		p.fMagMin = (pAMag) ? pAMag->GetAttributeAsFloat("Min") : 14.0f;
		p.fAngle = PId2 + FRAND(PI);

		//string sFilename = string("Weather\\Planets\\") + pAPlanets->GetAttributeName(i);
		//p.pGeo = Astronomy::pGS->CreateGeometry(sFilename, 0, 0);

		p.iTexture = Astronomy::pRS->TextureCreate(string("Weather\\Astronomy\\Planets\\") + sName + ".tga");
	}

	float fMaxDistance = 1e-10f;

	for (dword i=0; i<aPlanets.Size(); i++)
		if (aPlanets[i].fDistance > fMaxDistance) fMaxDistance = aPlanets[i].fDistance;

	for (dword i=0; i<aPlanets.Size(); i++)
	{
		//aPlanets[i].fDistance /= fMaxDistance;
		aPlanets[i].fRealDistance = 1200.0f + 500.0f * aPlanets[i].fDistance / fMaxDistance;
		aPlanets[i].fScale = float((aPlanets[i].fRealDistance * aPlanets[i].fDiameter) / (fabs(double(aPlanets[i].fDistance) - 1.0) * 150000000.0));
	}

	Astronomy::pGS->SetTexturePath("");
}

void Astronomy::PLANETS::Execute(double dDeltaTime, double dHour)
{
}

void Astronomy::PLANETS::Realize(double dDeltaTime, double dHour)
{
	// update fade
	if( fFadeTimeStart>=0.f )
	{
		if(	(fFadeTime>0.f && fPlanetFade<1.f) || (fFadeTime<0.f && fPlanetFade>0.f) )
		{
			ENTITY_ID eid;
			if( api->FindClass( &eid, "Weather", 0) )
			{
				float fTime = ((WEATHER_BASE*)eid.pointer)->GetFloat(whf_time_counter);
				if( fFadeTime > 0.f ) fPlanetFade = (fTime-fFadeTimeStart) / fFadeTime;
				if( fFadeTime < 0.f ) fPlanetFade = 1.f + (fTime-fFadeTimeStart) / fFadeTime;
				if( fPlanetFade < 0.f ) fPlanetFade = 0.f;
				if( fPlanetFade > 1.f ) fPlanetFade = 1.f;
			}
		}
	}

	if( fPlanetFade <= 0.f ) return;

	CVECTOR vCamPos, vCamAng; float fFov;
	Astronomy::pRS->GetCamera(vCamPos, vCamAng, fFov);
	dword bLighting, dwAmbient;
	Astronomy::pRS->GetRenderState(D3DRS_LIGHTING, &bLighting);
	Astronomy::pRS->GetRenderState(D3DRS_AMBIENT, &dwAmbient);

	Astronomy::pRS->SetRenderState(D3DRS_FOGENABLE, false);
	Astronomy::pRS->SetRenderState(D3DRS_LIGHTING, false);
	Astronomy::pRS->SetRenderState(D3DRS_AMBIENT, 0x00FFFFFF);

	for (dword i=0; i<aPlanets.Size(); i++)
	{
		CMatrix mP, m2;
		float fDistance = aPlanets[i].fRealDistance;
		mP.BuildMatrix(0.0f, aPlanets[i].fAngle, 0.0f);
		m2.BuildMatrix(PI / 8.0f, 0.0f, 0.0f);
		CVECTOR vPos = mP * CVECTOR(0.0f, 0.0f, fDistance);
		vPos = m2 * vPos;
		mP.m[0][0] = aPlanets[i].fScale * fPlanetScale;
		mP.m[1][1] = aPlanets[i].fScale * fPlanetScale;
		mP.m[2][2] = aPlanets[i].fScale * fPlanetScale;
	
		//Astronomy::pRS->SetTransform(D3DTS_WORLD, CMatrix());
		//Astronomy::pRS->DrawSphere(vCamPos + vPos, 200.0f * aPlanets[i].fScale, 0xFFFFFFFF);

		RS_RECT p;
		p.vPos = vPos + vCamPos;
		p.dwColor = ((dword)(fPlanetFade*255)<<24) | 0xFFFFFF;
		p.dwSubTexture = 0;
		p.fAngle = 0.0f;
		p.fSize = aPlanets[i].fScale * fPlanetScale * aPlanets[i].fFakeScale * 10.0f;
		Astronomy::pRS->TextureSet(0, aPlanets[i].iTexture);
		Astronomy::pRS->DrawRects(&p, 1, "Planet");

		/*if (aPlanets[i].pGeo)
		{
			mP.SetPosition(vPos + vCamPos);
			Astronomy::pRS->SetTransform(D3DTS_WORLD, mP);
			aPlanets[i].pGeo->Draw((GEOS::PLANE*)Astronomy::pRS->GetPlanes(), 0, null);
		}*/
	}
	Astronomy::pRS->SetRenderState(D3DRS_LIGHTING, bLighting);
	Astronomy::pRS->SetRenderState(D3DRS_AMBIENT, dwAmbient);
	Astronomy::pRS->SetRenderState(D3DRS_FOGENABLE, true);
}

void Astronomy::PLANETS::TimeUpdate(ATTRIBUTES * pAP)
{
	ATTRIBUTES * pAPlan = pAP ? pAP->GetAttributeClass("Planets") : 0;
	fPlanetScale = 1.f;
	fPlanetFade = 1.f;
	fFadeTimeStart = -1.f;
	fFadeTime = 0.1f;
	if( pAPlan )
	{
		fPlanetScale = pAPlan->GetAttributeAsFloat("Scale",fPlanetScale);
		fPlanetFade = pAPlan->GetAttributeAsFloat("FadeValue",fPlanetFade);
		fFadeTimeStart = pAPlan->GetAttributeAsFloat("FadeStartTime",fFadeTimeStart);
		fFadeTime = pAPlan->GetAttributeAsFloat("FadeTime",fFadeTime);
	}
}
