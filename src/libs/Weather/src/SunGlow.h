#pragma once

#include "Typedef.h"
#include "Weather_base.h"
#include <string>
#include <vector>

class SKY;

class SUNGLOW : public Entity
{
  public:
    SUNGLOW();
    ~SUNGLOW() override;

    void SetDevice();
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void DrawSunMoon();

  private:
    enum
    {
        SUNGLOWVERTEX_FORMAT = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
        SUNGLOW_FORCE_DWORD = 0x7FFFFFFF
    };

    struct SUNGLOWVERTEX
    {
        CVECTOR vPos;
        uint32_t dwColor;
        float tu, tv;
    };

    struct sunglow_t
    {
        float fSunSize;
        float fMoonSize;
        float fGlowSize;
        float fDist;
        float fDecayTime;
        float fRotateSpeed;
        uint32_t dwColor;
        std::string sSunTexture, sMoonTexture, sGlowTexture, sTechniqueZ, sTechniqueNoZ;
    };

    struct flare_t
    {
        float fDist;            // distance from far
        float fSize;            // size
        uint32_t dwColor;       // color
        uint32_t dwSubTexIndex; // subtexture index
    };

    struct sunflares_t
    {
        // sunflares_t() : aFlares(_FL_) {}

        float fSize;
        float fDist;
        float fFlareScale;
        uint32_t dwTexSizeX, dwTexSizeY;
        std::string sTexture, sTechnique;
        std::vector<flare_t> aFlares;
    };

    struct overflow_t
    {
        std::string sTexture, sTechnique;
        float fSize;
        float fStart;
        uint32_t dwColor;
    };

    struct reflection_t
    {
        std::string sTexture, sTechnique;
        float fSize;
        float fDist;
        uint32_t dwColor;
    };

    overflow_t Overflow;
    sunglow_t Glow;
    sunflares_t Flares;
    reflection_t Reflection;
    COLLIDE *pCollide;
    WEATHER_BASE *pWeather;
    SKY *pSky;
    VDX9RENDER *pRS;
    long idRectBuf;

    std::vector<RS_RECT> aRSR;

    bool bSimpleSea;
    long iNumFlares;
    float fFadeout, fAlpha, fAlphaFlare, fAlphaOverflow, fMaxOverflowAlphaValue;
    long iSunTex, iMoonTex, iSunGlowTex, iFlareTex, iOverflowTex, iReflTexture;
    bool bVisible, bVisibleFlare;
    bool bHaveGlow, bHaveFlare, bHaveOverflow, bHaveReflection;
    bool bMoon;
    float fMinAlphaValue;

    float fBottomClip;

    float LayerTrace(CVECTOR &vSrc, EntityManager::LayerIterators its) const;
    void GenerateSunGlow();
    void DrawReflection() const;
    void Release();

    void DrawRect(uint32_t dwColor, const CVECTOR &pos, float fSize, float fAngle, const char *pcTechnique,
                  float fBottomClip) const;

    float GetSunFadeoutFactor(const CVECTOR &vSunPos, float fSunSize);
};
