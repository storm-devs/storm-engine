#pragma once

#include "link_describe.hpp"

#include "dx9render.h"
#include "matrix.h"
#include "vma.hpp"

#include <string>
#include <vector>

constexpr auto MAX_LINES = 5;
constexpr auto SCROLL_LINE_TIME = 100;
constexpr auto TILED_LINE_HEIGHT = 26;
constexpr auto SBL = 6;
#define TICK_SOUND "interface\\ok.wav"

#define XI_TEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)
class INIFILE;

struct XI_TEX_VERTEX
{
    CVECTOR pos;
    float rhw;
    uint32_t color;
    float u, v;
};

constexpr auto BUTTON_STATE_UPENABLE = 1;
constexpr auto BUTTON_STATE_DOWNENABLE = 2;
constexpr auto BUTTON_STATE_UPLIGHT = 4;
constexpr auto BUTTON_STATE_DOWNLIGHT = 8;

class VSoundService;

class DIALOG final : public Entity
{
    static VDX9RENDER *RenderService;

  public:
    DIALOG(DIALOG &&) = delete;
    DIALOG(const DIALOG &) = delete;
    DIALOG();
    ~DIALOG();

    bool Init();
    void InitLinks(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni);
    void Realize(uint32_t Delta_Time);
    uint32_t AttributeChanged(ATTRIBUTES *pA);
    uint64_t ProcessMessage(MESSAGE &message);

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

  private:
    void EmergencyExit();

    // Nikita data
    std::string m_sTalkPersName;

    bool m_bDlgChanged;
    void UpdateDlgTexts();
    void UpdateDlgViewport();

    struct DlgTextDescribe
    {
      private:
        POINT offset;
        int32_t nWindowWidth;
        int32_t nFontID;
        uint32_t dwColor;
        float fScale;
        int32_t nLineInterval;
        std::vector<std::string> asText;
        int32_t nShowQuantity;

        std::vector<int32_t> pageBreaks_;

      public:
        int32_t currentLine_;

        ~DlgTextDescribe()
        {
            if (RenderService && nFontID >= 0)
                RenderService->UnloadFont(nFontID);
        }

        void ChangeText(std::string_view text);
        void Init(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni);
        int32_t GetShowHeight();
        void Show(int32_t nY);
        bool IsLastPage();
        void PrevPage();
        void NextPage();
    };

    DlgTextDescribe m_DlgText;

    storm::dialog::DlgLinkDescribe linkDescribe_;

    struct BackParameters
    {
        int32_t m_idBackTex;

        FRECT m_frLeftTopUV;
        FRECT m_frRightTopUV;
        FRECT m_frLeftBottomUV;
        FRECT m_frRightBottomUV;
        FRECT m_frLeftUV;
        FRECT m_frRightUV;
        FRECT m_frTopUV;
        FRECT m_frBottomUV;
        FRECT m_frCenterUV;
        FRECT m_frDividerUV;

        FRECT m_frBorderExt;
        FRECT m_frBorderInt;
        FRECT frBorderRect;

        FRECT frCharacterNameRectLeftUV;
        FRECT frCharacterNameRectRightUV;
        FRECT frCharacterNameRectCenterUV;
        FPOINT fpCharacterNameOffset;
        float fCharacterNameRectHeight;
        float fCharacterNameRectLeftWidth;
        float fCharacterNameRectCenterWidth;
        float fCharacterNameRectRightWidth;

        bool bShowDivider;
        float nDividerHeight;
        float nDividerOffsetX;
        int32_t nDividerOffsetY;
    };

    BackParameters m_BackParams;
    int32_t m_idVBufBack;
    int32_t m_idIBufBack;
    int32_t m_nVQntBack;
    int32_t m_nIQntBack;

    struct ButtonParameters
    {
        int32_t m_idTexture;

        FRECT frUpNormalButtonUV;
        FRECT frDownNormalButtonUV;
        FRECT frUpLightButtonUV;
        FRECT frDownLightButtonUV;

        FPOINT fpButtonSize;

        float fRightOffset;
        float fTopOffset;
        float fBottomOffset;
    };

    ButtonParameters m_ButtonParams;
    int32_t m_idVBufButton;
    int32_t m_idIBufButton;
    int32_t m_nVQntButton;
    int32_t m_nIQntButton;
    uint32_t m_dwButtonState;

    int32_t m_nCharNameTextFont;
    uint32_t m_dwCharNameTextColor;
    float m_fCharNameTextScale;
    FPOINT m_fpCharNameTextOffset;

    int32_t m_nScrBaseWidth;
    int32_t m_nScrBaseHeight;
    static FRECT m_frScreenData;

    static float GetScrX(float fX)
    {
        return fX * m_frScreenData.right + m_frScreenData.left;
    }

    static float GetScrY(float fY)
    {
        return fY * m_frScreenData.bottom + m_frScreenData.top;
    }

    static float GetScrWidth(float fX)
    {
        return fX * m_frScreenData.right;
    }

    static float GetScrHeight(float fY)
    {
        return fY * m_frScreenData.bottom;
    }

    void CreateBack();
    void FillBack();
    void FillDivider();
    void DrawBack();

    void CreateButtons();
    void FillButtons();
    void DrawButtons();

    void LoadFromIni();

    static void GetRectFromIni(INIFILE *ini, const char *pcSection, const char *pcKey, FRECT &frect);
    static void GetPointFromIni(INIFILE *ini, const char *pcSection, const char *pcKey, FPOINT &fpoint);

    VSoundService *snd;
    entid_t charId, persId;
    entid_t charMdl, persMdl;
    D3DVIEWPORT9 textViewport;

    int32_t curSnd;
    char soundName[256];
    char charDefSnd[256];

    bool forceEmergencyClose;
    char selectedLinkName[1024];

    int unfadeTime;

    int32_t play;
    bool start;

    bool bEditMode;
};
