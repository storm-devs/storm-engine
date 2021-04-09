#pragma once

#include "dx9render.h"
#include "matrix.h"
#include "vmodule_api.h"

#include "defines.h"
#include <string>
#include <vector>

#define MAX_LINES 5
#define SCROLL_LINE_TIME 100
#define TILED_LINE_HEIGHT 26
#define SBL 6
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

#define BUTTON_STATE_UPENABLE 1
#define BUTTON_STATE_DOWNENABLE 2
#define BUTTON_STATE_UPLIGHT 4
#define BUTTON_STATE_DOWNLIGHT 8

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

    static void AddToStringArrayLimitedByWidth(const char *pcSrcText, long nFontID, float fScale, long nLimitWidth,
                                               std::vector<std::string> &asOutTextList,
                                               std::vector<long> *panPageIndices, long nPageSize);

  private:
    void EmergencyExit();

    // Nikita data
    std::string m_sTalkPersName;

    struct TextDescribe
    {
        VDX9RENDER *rs;
        POINT offset;
        long nWindowWidth;
        long nFontID;
        uint32_t dwColor;
        uint32_t dwSelColor;
        float fScale;
        long nLineInterval;
        std::vector<std::string> asText;
        long nStartIndex;
        long nShowQuantity;
        long nSelectLine;

        TextDescribe()
        {
            rs = nullptr;
            nFontID = -1;
        }
        ~TextDescribe()
        {
            Release();
        }
        void Release()
        {
            if (rs && nFontID >= 0)
                rs->UnloadFont(nFontID);
            nFontID = -1;
            asText.clear();
        }
    };

    bool m_bDlgChanged;
    void UpdateDlgTexts();
    void UpdateDlgViewport();

    struct DlgTextDescribe : public TextDescribe
    {
        float fScrollTime;
        std::vector<long> anPageEndIndex;

        DlgTextDescribe() : TextDescribe(), fScrollTime(0)
        {
        }

        virtual ~DlgTextDescribe()
        {
            Release();
        }
        virtual void Release()
        {
            TextDescribe::Release();
        }
        void ChangeText(const char *pcText);
        void Init(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni);
        long GetShowHeight();
        void Show(long nY);
        bool IsLastPage();
        void PrevPage();
        void NextPage();
    };
    DlgTextDescribe m_DlgText;

    struct DlgLinkDescribe : public TextDescribe
    {
        std::vector<long> anLineEndIndex;

        long nEditLine;
        long nEditVarIndex;
        long nEditCharIndex;
        float fCursorCurrentTime, fCursorVisibleTime, fCursorInvisibleTime;
        DIALOG *pDlg;

        DlgLinkDescribe() : TextDescribe()
        {
            pDlg = nullptr;
        }
        virtual ~DlgLinkDescribe()
        {
            Release();
        }
        virtual void Release()
        {
            TextDescribe::Release();
        }
        void ChangeText(ATTRIBUTES *pALinks);
        void Init(VDX9RENDER *pRS, D3DVIEWPORT9 &vp, INIFILE *pIni);
        long GetShowHeight();
        void Show(long nY);
        void ShowEditMode(long nX, long nY, long nTextIdx);
        void SetDlg(DIALOG *_pDlg)
        {
            pDlg = _pDlg;
        }
    };
    DlgLinkDescribe m_DlgLinks;

    struct BackParameters
    {
        long m_idBackTex;

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
        long nDividerOffsetY;
    };
    BackParameters m_BackParams;
    long m_idVBufBack;
    long m_idIBufBack;
    long m_nVQntBack;
    long m_nIQntBack;

    struct ButtonParameters
    {
        long m_idTexture;

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
    long m_idVBufButton;
    long m_idIBufButton;
    long m_nVQntButton;
    long m_nIQntButton;
    uint32_t m_dwButtonState;

    long m_nCharNameTextFont;
    uint32_t m_dwCharNameTextColor;
    float m_fCharNameTextScale;
    FPOINT m_fpCharNameTextOffset;

    long m_nScrBaseWidth;
    long m_nScrBaseHeight;
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

    long curSnd;
    char soundName[256];
    char charDefSnd[256];

    bool forceEmergencyClose;
    char selectedLinkName[1024];

    int unfadeTime;

    long play;
    bool start;

    bool bEditMode;
};
