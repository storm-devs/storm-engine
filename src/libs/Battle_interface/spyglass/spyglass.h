#pragma once

#include "../bi_defines.h"
#include <string>
#include <vector>

class BIImageRender;
class BIImage;
class VAI_OBJBASE;

class ISPYGLASS : public Entity
{
    struct ImageParam
    {
        BIImage *pImage;
        std::string sTextureName;
        uint32_t dwColor;
        FRECT rUV;
        RECT rPos;

        ImageParam()
        {
            pImage = nullptr;
        };
        ~ImageParam()
        {
            Release();
        }
        void Release();
        void LoadFromAttr(BIImageRender *pImgRender, ATTRIBUTES *pA, const char *pcDefName, long nDefLeftPos,
                          long nDefTopPos, long nDefRightPos, long nDefBottomPos, long nPrior);
        void ChangeIcon(BIImageRender *pImgRender, const char *pcTextureName, FRECT &frUV);
    };

    struct TextParam
    {
        VDX9RENDER *rs;
        long nFontID;
        POINT pos;
        float fScale;
        uint32_t dwColor;
        std::string sText;
        long nAlign;

        TextParam(TextParam &&) = delete;
        TextParam(const TextParam &) = delete;

        TextParam()
        {
            rs = nullptr;
            nFontID = -1;
        }

        ~TextParam()
        {
            Release();
        }

        void Release()
        {
            if (rs && nFontID >= 0)
                rs->UnloadFont(nFontID);
            nFontID = -1;
        }

        void LoadFromAttr(VDX9RENDER *rs, ATTRIBUTES *pA, const char *pcDefText, long nDefXPos, long nDefYPos);
        void Print() const;
    };

    struct SpyGlassCameraParameters
    {
        float fSpyGlassPerspective;
        float fActivateTime;
        float fUpdateTime;

        float fOldPerspective;

        bool bIsActive;
        bool bIsGrow;

        float fCurUpdatingTime;
        float fCurActivateTime;
    };

  public:
    ISPYGLASS();
    ~ISPYGLASS();
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time) const;
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

  protected:
    VDX9RENDER *rs;
    BIImageRender *m_pImgRender;

    // telescope lens
    ImageParam m_Lens;

    // ship sign
    ImageParam m_ShipImage; // ship back
    ImageParam m_ShipBack;  // ship back
    ImageParam m_ShipHP;    // ship HP
    ImageParam m_ShipSP;    // ship SP
    ImageParam m_Class;     // ship class

    // captain sign
    ImageParam m_CaptainBack;    // captain back image
    ImageParam m_CaptainFace;    // captain face image
    TextParam m_TextCaptainName; // captain name
    // captain skills
    ImageParam m_ImgCaptainFencing;
    TextParam m_TextCaptainFencing;
    ImageParam m_ImgCaptainCannon;
    TextParam m_TextCaptainCannon;
    ImageParam m_ImgCaptainAccuracy;
    TextParam m_TextCaptainAccuracy;
    ImageParam m_ImgCaptainNavigation;
    TextParam m_TextCaptainNavigation;
    ImageParam m_ImgCaptainBoarding;
    TextParam m_TextCaptainBoarding;

    // nation flag
    ImageParam m_Nation;
    // cannon icon
    ImageParam m_Cannon;
    // sail icon (ship speed)
    ImageParam m_Sail;
    // cannon charge icon
    ImageParam m_Charge;

    // BOAL distance charge icon
    ImageParam m_SailTo;

    TextParam m_txtShipType;
    TextParam m_txtShipName;
    TextParam m_txtCannons;
    TextParam m_txtShipSpeed;
    TextParam m_txtShipCrew;
    TextParam m_txtSailTo; // boal

    bool m_bIsOn;
    bool m_bIsPresentShipInfo;
    long m_nInfoCharacterIndex;

    SpyGlassCameraParameters m_Camera{};
    std::vector<FRECT> m_aNationUV;
    std::vector<FRECT> m_aChargeUV;
    std::vector<FRECT> m_aSailUV;

    VAI_OBJBASE *m_pFortObj;

    float m_fInfoKeepDelay;
    float m_fMaxInfoKeepDelay;

  protected:
    void Release();
    ATTRIBUTES *GetAttr(const char *pcAttrName) const;
    void TurnOnTelescope(bool bTurnOn);
    void SetShipInfo(long nCharIndex);
    void FindNewTargetShip();
    void TurnOnSpyGlassCamera();
    void ChangeTelescopeType(const char *pcTextureName, float fZoomScale, float fActivateTime, float fUpdateTime);
    void UpdateCamera();
    void ChangeTargetData(const char *pcShipName, const char *pcShipType, float fRelativeHP, float fRelativeSP,
                          long nShipCrew, float fShipSpeed, float fSailTo, long nCurCannons, long nMaxCannons,
                          long nCharge, long nNation, long nSailState, long nFace, long nFencing, long nCannon,
                          long nAccuracy, long nNavigation, long nBoarding, const char *pcCaptainName,
                          const char *pcFaceTexture, long nShipClass);
    void FillUVArrayFromAttributes(std::vector<FRECT> &m_aUV, ATTRIBUTES *pA) const;
    VAI_OBJBASE *GetFort();
};
