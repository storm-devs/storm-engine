#pragma once

#include "typedef.h"
#include <vector>

class Astronomy : public Entity
{
  public:
    static VDX9RENDER *pRS;
    static VGEOMETRY *pGS;

    Astronomy();
    ~Astronomy() override;

    bool Init() override;
    void SetDevice();
    void Realize(uint32_t Delta_Time);
    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    ATTRIBUTES *GetRoot(ATTRIBUTES *pA);

  private:
    class PLANETS
    {
      public:
        struct Planet
        {
            GEOS *pGeo;
            CVECTOR vPos;
            int32_t iTexture;
            float fDistance, fRealDistance;
            float fSpeed;
            float fInclination;
            float fDiameter;
            float fScale, fFakeScale, fAngle;
            float fMagMax, fMagMin;
        };

        std::vector<Planet> aPlanets;
        float fPlanetScale;

        float fPlanetFade;
        float fFadeTimeStart;
        float fFadeTime;

        PLANETS();
        ~PLANETS();

        void ReleasePlanets();

        void Init(ATTRIBUTES *pAP);
        void Execute(double dDeltaTime, double dHour);
        void Realize(double dDeltaTime, double dHour);
        void TimeUpdate(ATTRIBUTES *pAP);
    };

    class STARS
    {
      public:
        STARS();
        ~STARS();

        void Init(ATTRIBUTES *pAP);
        void Execute(double dDeltaTime, double dHour);
        void Realize(double dDeltaTime, double dHour);

        uint32_t AttributeChanged(ATTRIBUTES *pAttribute);
        void TimeUpdate(ATTRIBUTES *pAP);

        bool IsEnable() const
        {
            return bEnable;
        };

      private:
        const char *sCatalog, *sTexture;
        float fRadius, fSize, fHeightFade, fSunFade;
        float fVisualMagnitude, fTelescopeMagnitude;
        int32_t iTexture;
        bool bEnable;
        int32_t iVertexBuffer, iVertexBufferColors;
        IDirect3DVertexDeclaration9 *pDecl;
        float fPrevFov;

        struct Star
        {
            float fRA;
            float fDec;
            float fMag;
            char cSpectr[2];
            uint32_t dwColor;
            float fAlpha;
            CVECTOR vPos;
        };

        uint32_t Spectr[256]{};
        std::vector<Star> aStars;

        float fFadeValue;
        float fFadeTimeStart;
        float fFadeTime;
        float m_fTwinklingTime;
    };

    double dHour, dTimeScale;
    STARS Stars;
    PLANETS Planets;
};
