#pragma once

#include "bi_utils.h"

class ILogAndActions : public Entity
{
    VDX9RENDER *rs;

    struct STRING_DESCR
    {
        char *str;
        float offset;
        float alpha;
        STRING_DESCR *next;
    };

  public:
    ILogAndActions(ILogAndActions &&) = delete;
    ILogAndActions(const ILogAndActions &) = delete;
    ILogAndActions();
    ~ILogAndActions() override;
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
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
    void Create(bool bFastComShow, bool bLogStringShow);
    void ActionChange(bool bFastComShow, bool bLogStringShow);
    void Release();

    void SetString(const char *str, bool immortal);
    void SetAction(const char *actionName);

  protected:
    // log stings parameters
    // -----------------------
    int32_t m_nWindowWidth;  // X window size
    int32_t m_nWindowHeight; // Y window size
    int32_t m_nWindowLeft;
    int32_t m_nWindowRight;
    int32_t m_nWindowUp;
    int32_t m_fontID;
    float m_fFontScale;
    uint32_t m_dwColor;
    int32_t m_nStringBegin;
    int32_t m_nStringOffset;
    float m_fShiftSpeed;
    float m_fBlendSpeed;
    STRING_DESCR *m_sRoot;

    // Action parameters
    //-------------------
    bool m_bThatRealAction;
    int32_t m_nIconWidth;  // X icon size
    int32_t m_nIconHeight; // Y icon size
    int32_t m_nIconLeft;
    int32_t m_nIconUp;
    int32_t m_idIconTexture;
    int32_t m_horzDiv;
    int32_t m_vertDiv;
    BI_ONETEXTURE_VERTEX m_IconVertex[4];
    char m_sActionName[64];
    char m_sOldActionName[64];

    bool m_bShowActiveCommand;
    bool m_bShowLogStrings;
    bool m_bDontShowAll;

    int32_t m_nTimeCounter;

    BITextInfo m_ActionHint1;
    BITextInfo m_ActionHint2;
};
