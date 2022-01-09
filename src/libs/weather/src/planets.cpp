#include "astronomy.h"
#include "weather_base.h"

Astronomy::PLANETS::PLANETS()
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
    for (int32_t i = 0; i < aPlanets.size(); i++)
    {
        // Astronomy::pGS->DeleteGeometry(aPlanets[i].pGeo);
        if (aPlanets[i].iTexture >= 0)
            pRS->TextureRelease(aPlanets[i].iTexture);
    }

    aPlanets.clear();
}

void Astronomy::PLANETS::Init(ATTRIBUTES *pAP)
{
    ReleasePlanets();
    aPlanets.clear();

    pGS->SetTexturePath("Weather\\Astronomy\\Planets\\");

    // ATTRIBUTES * pAScale = pAP->FindAClass(pAP, "Planets.Scale");
    // fPlanetScale = ((pAScale) ? pAScale->GetAttributeAsFloat() : 1.0f);
    TimeUpdate(pAP);

    auto *pAPlanets = pAP->FindAClass(pAP, "Planets.Planet");
    if (pAPlanets)
        for (uint32_t i = 0; i < pAPlanets->GetAttributesNum(); i++)
        {
            auto *pAPlanet = pAPlanets->GetAttributeClass(i);
            auto *pAMag = pAPlanet->GetAttributeClass("Mag");

            std::string sName = pAPlanet->GetThisName();

            aPlanets.push_back(Planet{});
            // Planet & p = aPlanets[aPlanets.Add()];
            auto &p = aPlanets.back();
            p.fDiameter = pAPlanet->GetAttributeAsFloat("Diameter");
            p.fSpeed = pAPlanet->GetAttributeAsFloat("Speed");
            p.fDistance = pAPlanet->GetAttributeAsFloat("Distance");
            p.fInclination = pAPlanet->GetAttributeAsFloat("Inclination");
            p.fFakeScale = pAPlanet->GetAttributeAsFloat("Scale", 1.0f);
            p.fMagMax = (pAMag) ? pAMag->GetAttributeAsFloat("Max") : 12.0f;
            p.fMagMin = (pAMag) ? pAMag->GetAttributeAsFloat("Min") : 14.0f;
            p.fAngle = PId2 + FRAND(PI);

            // string sFilename = string("Weather\\Planets\\") + pAPlanets->GetAttributeName(i);
            // p.pGeo = Astronomy::pGS->CreateGeometry(sFilename, 0, 0);

            p.iTexture = pRS->TextureCreate(("Weather\\Astronomy\\Planets\\" + sName + ".tga").c_str());
        }

    auto fMaxDistance = 1e-10f;

    for (uint32_t i = 0; i < aPlanets.size(); i++)
        if (aPlanets[i].fDistance > fMaxDistance)
            fMaxDistance = aPlanets[i].fDistance;

    for (uint32_t i = 0; i < aPlanets.size(); i++)
    {
        // aPlanets[i].fDistance /= fMaxDistance;
        aPlanets[i].fRealDistance = 1200.0f + 500.0f * aPlanets[i].fDistance / fMaxDistance;
        aPlanets[i].fScale = static_cast<float>((aPlanets[i].fRealDistance * aPlanets[i].fDiameter) /
                                                (fabs(static_cast<double>(aPlanets[i].fDistance) - 1.0) * 150000000.0));
    }

    pGS->SetTexturePath("");
}

void Astronomy::PLANETS::Execute(double dDeltaTime, double dHour)
{
}

void Astronomy::PLANETS::Realize(double dDeltaTime, double dHour)
{
    // update fade
    if (fFadeTimeStart >= 0.f)
    {
        if ((fFadeTime > 0.f && fPlanetFade < 1.f) || (fFadeTime < 0.f && fPlanetFade > 0.f))
        {
            if (const auto eid = EntityManager::GetEntityId("weather"))
            {
                auto fTime =
                    static_cast<WEATHER_BASE *>(EntityManager::GetEntityPointer(eid))->GetFloat(whf_time_counter);
                if (fFadeTime > 0.f)
                    fPlanetFade = (fTime - fFadeTimeStart) / fFadeTime;
                if (fFadeTime < 0.f)
                    fPlanetFade = 1.f + (fTime - fFadeTimeStart) / fFadeTime;
                if (fPlanetFade < 0.f)
                    fPlanetFade = 0.f;
                if (fPlanetFade > 1.f)
                    fPlanetFade = 1.f;
            }
        }
    }

    if (fPlanetFade <= 0.f)
        return;

    CVECTOR vCamPos, vCamAng;
    float fFov;
    pRS->GetCamera(vCamPos, vCamAng, fFov);
    uint32_t bLighting, dwAmbient;
    pRS->GetRenderState(D3DRS_LIGHTING, &bLighting);
    pRS->GetRenderState(D3DRS_AMBIENT, &dwAmbient);

    pRS->SetRenderState(D3DRS_FOGENABLE, false);
    pRS->SetRenderState(D3DRS_LIGHTING, false);
    pRS->SetRenderState(D3DRS_AMBIENT, 0x00FFFFFF);

    for (uint32_t i = 0; i < aPlanets.size(); i++)
    {
        CMatrix mP, m2;
        auto fDistance = aPlanets[i].fRealDistance;
        mP.BuildMatrix(0.0f, aPlanets[i].fAngle, 0.0f);
        m2.BuildMatrix(PI / 8.0f, 0.0f, 0.0f);
        auto vPos = mP * CVECTOR(0.0f, 0.0f, fDistance);
        vPos = m2 * vPos;
        mP.m[0][0] = aPlanets[i].fScale * fPlanetScale;
        mP.m[1][1] = aPlanets[i].fScale * fPlanetScale;
        mP.m[2][2] = aPlanets[i].fScale * fPlanetScale;

        // Astronomy::pRS->SetTransform(D3DTS_WORLD, CMatrix());
        // Astronomy::pRS->DrawSphere(vCamPos + vPos, 200.0f * aPlanets[i].fScale, 0xFFFFFFFF);

        RS_RECT p;
        p.vPos = vPos + vCamPos;
        p.dwColor = (static_cast<uint32_t>(fPlanetFade * 255) << 24) | 0xFFFFFF;
        p.dwSubTexture = 0;
        p.fAngle = 0.0f;
        p.fSize = aPlanets[i].fScale * fPlanetScale * aPlanets[i].fFakeScale * 10.0f;
        pRS->TextureSet(0, aPlanets[i].iTexture);
        pRS->DrawRects(&p, 1, "planet");

        /*if (aPlanets[i].pGeo)
        {
          mP.SetPosition(vPos + vCamPos);
          Astronomy::pRS->SetTransform(D3DTS_WORLD, mP);
          aPlanets[i].pGeo->Draw((GEOS::PLANE*)Astronomy::pRS->GetPlanes(), 0, null);
        }*/
    }
    pRS->SetRenderState(D3DRS_LIGHTING, bLighting);
    pRS->SetRenderState(D3DRS_AMBIENT, dwAmbient);
    pRS->SetRenderState(D3DRS_FOGENABLE, true);
}

void Astronomy::PLANETS::TimeUpdate(ATTRIBUTES *pAP)
{
    auto *pAPlan = pAP ? pAP->GetAttributeClass("Planets") : nullptr;
    fPlanetScale = 1.f;
    fPlanetFade = 1.f;
    fFadeTimeStart = -1.f;
    fFadeTime = 0.1f;
    if (pAPlan)
    {
        fPlanetScale = pAPlan->GetAttributeAsFloat("Scale", fPlanetScale);
        fPlanetFade = pAPlan->GetAttributeAsFloat("FadeValue", fPlanetFade);
        fFadeTimeStart = pAPlan->GetAttributeAsFloat("FadeStartTime", fFadeTimeStart);
        fFadeTime = pAPlan->GetAttributeAsFloat("FadeTime", fFadeTime);
    }
}
