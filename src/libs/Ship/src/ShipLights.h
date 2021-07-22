#pragma once

#include "collide.h"
#include "defines.h"
#include "dx9render.h"
#include "sea_base.h"
#include <ShipLights.h>

class ShipLights : public IShipLights
{
  private:
    struct Color
    {
        float r, g, b, a;

        friend Color operator*(const Color &c, float fValue)
        {
            Color cc;

            cc.r = c.r * fValue;
            cc.g = c.g * fValue;
            cc.b = c.b * fValue;
            cc.a = c.a;

            return cc;
        }

        void Normalize()
        {
            r = Clamp(r);
            g = Clamp(g);
            b = Clamp(b);
            a = Clamp(a);
        }

        Color &operator*=(float fValue)
        {
            r *= fValue;
            g *= fValue;
            b *= fValue;

            return *this;
        }
    };

    struct LightType
    {
        std::string sLightType;
        Color cLightColor;
        Color cCoronaColor;
        float fRange;
        float fAttenuation0, fAttenuation1, fAttenuation2;
        float fFlicker, fFreq, fFlickerSlow, fFreqSlow;
        float fLifeTime, fUpTime;
        float fCoronaRange, fCoronaSize;
        float fSunRoadFlareFadeDistance;
    };

    struct Oscillator
    {
        float fStep;
        float fAmp;
        float fOneDivAmp;
        float fNewValue;
        float fOldValue;
        float fK;
    };

    struct ShipLight
    {
        VAI_OBJBASE *pObject;
        NODE *pNode;
        bool bDynamicLight;
        CVECTOR vPos, vCurPos;
        D3DLIGHT9 Light;
        float fCoronaIntensity;
        float fCurDistance;
        float fCurTime, fTotalTime, fUpTime;
        float fFlareAlpha, fFlareAlphaMax;
        Oscillator Osc[2];
        bool bCoronaOnly;
        bool bVisible;
        float fTotalBrokenTime, fBrokenTime;
        bool bOff, bBrokenTimeOff;
        bool bDead;
        LightType *pLT;
    };

    struct SelectedLight
    {
        float fDistance;
        uint32_t dwIndex;

        bool operator<(const SelectedLight &other) const
        {
            return fDistance < other.fDistance;
        };
    };

    std::vector<ShipLight> aLights;
    std::vector<SelectedLight> aSelectedLights;
    std::vector<LightType> aLightTypes;
    long iMinLight, iMaxLight;
    uint32_t dwMaxD3DLights;
    bool bLoadLights;
    bool bReflection;
    float fSunRoadFlareSize;

    long iCoronaTex, iFlareSunRoadTex;
    std::string sCoronaTechnique;
    uint32_t dwCoronaSubTexX, dwCoronaSubTexY;

    SEA_BASE *pSea;

    bool LoadLights();
    LightType *FindLightType(std::string sLightType);
    float GetAttributeAsFloat(ATTRIBUTES *pA, const char *pName, float fDefault);
    void AddFlare(VAI_OBJBASE *pObject, bool bLight, MODEL *pModel, const GEOS::LABEL &label);
    bool SetLabel(ShipLight *pL, MODEL *pModel, const char *pStr);

  public:
    static VDX9RENDER *pRS;
    static COLLIDE *pCollide;

    ShipLights();
    ~ShipLights() override;

    void Release(VAI_OBJBASE *pObject) override;

    void AddLights(VAI_OBJBASE *pObject, MODEL *pModel, bool bLights, bool bFlares) override;
    void SetLightsOff(VAI_OBJBASE *pObject, float fTime, bool bLights, bool bFlares, bool bNow) override;
    void KillMast(VAI_OBJBASE *pObject, NODE *pNode, bool bNow) override;
    void AddDynamicLights(VAI_OBJBASE *pObject, const CVECTOR &vPos) override;
    void SetLights(VAI_OBJBASE *pObject) override;
    void UnSetLights(VAI_OBJBASE *pObject) override;

    void SetDead(VAI_OBJBASE *pObject) override;

    bool Init() override;
    void Execute(uint32_t dwDeltaTime);
    void Realize(uint32_t dwDeltaTime);
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
        }
    }
};
