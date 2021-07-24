#pragma once

#include "Utils.h"
#include "defines.h"

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
    long m_nWindowWidth;  // X window size
    long m_nWindowHeight; // Y window size
    long m_nWindowLeft;
    long m_nWindowRight;
    long m_nWindowUp;
    long m_fontID;
    float m_fFontScale;
    uint32_t m_dwColor;
    long m_nStringBegin;
    long m_nStringOffset;
    float m_fShiftSpeed;
    float m_fBlendSpeed;
    STRING_DESCR *m_sRoot;

    // Action parameters
    //-------------------
    bool m_bThatRealAction;
    long m_nIconWidth;  // X icon size
    long m_nIconHeight; // Y icon size
    long m_nIconLeft;
    long m_nIconUp;
    long m_idIconTexture;
    long m_horzDiv;
    long m_vertDiv;
    BI_ONETEXTURE_VERTEX m_IconVertex[4];
    char m_sActionName[64];
    char m_sOldActionName[64];

    bool m_bShowActiveCommand;
    bool m_bShowLogStrings;
    bool m_bDontShowAll;

    long m_nTimeCounter;

    BITextInfo m_ActionHint1;
    BITextInfo m_ActionHint2;
};
