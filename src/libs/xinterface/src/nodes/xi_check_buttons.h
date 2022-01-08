#pragma once

#include "../inode.h"
#include "../xdefines.h"
#include "xi_image.h"

struct ButtonDescribe
{
    ButtonDescribe()
        : bChoose(false), bDisable(false)
    {
        pImg = nullptr;
        bSetPos = false;
    }

    ~ButtonDescribe()
    {
        STORM_DELETE(pImg);
    }

    struct StrDescribe
    {
        std::string str;
        float fX;
    };

    std::vector<StrDescribe> aStr;
    bool bChoose;
    bool bDisable;

    bool bSetPos;
    FXYPOINT pos;

    CXI_IMAGE *pImg;
};

// picture
class CXI_CHECKBUTTONS : public CINODE
{
  public:
    CXI_CHECKBUTTONS(CXI_CHECKBUTTONS &&) = delete;
    CXI_CHECKBUTTONS(const CXI_CHECKBUTTONS &) = delete;
    CXI_CHECKBUTTONS();
    ~CXI_CHECKBUTTONS() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override;

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;

    bool GetInternalNameList(std::vector<std::string> &aStr) override;
    void SetInternalName(std::string &sName) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void AddButton(const char *pcText, bool bDisable, bool bSelect);
    void ChangeText(int32_t nButtonNum, const char *pcText);

    void CheckMouseClick(const FXYPOINT &pntMouse);
    void SetButtonOn(int32_t nButtonNum);
    void SetButtonOff(int32_t nButtonNum);
    void SetAllButtonsToOff();
    void SetCheckToButton(int32_t nButtonNum, bool bCheck);

    void UpdateAllTextInfo();
    void UpdateTextInfo(int32_t nButtonNum);
    void WriteToAttributeButtonState(int32_t nButtonIndex);

    bool m_bExclusiveChoose;

    int32_t m_nFontNum;
    float m_fFontScale;
    uint32_t m_dwNormalFontColor;
    uint32_t m_dwSelectFontColor;
    uint32_t m_dwDisableFontColor;
    FXYRECT m_frTextOffset;
    float m_fTextLineHeight;
    float m_fTextSectionInterval;
    int32_t m_nFontAlignment;

    bool m_bClickIntoTextActive;
    bool m_bIndividualPos;
    std::vector<ButtonDescribe *> m_aButton;
    int32_t m_nEditableSectionIndex;

    FXYPOINT m_fpIconSize;
    FXYPOINT m_fpIconOffset;
    std::string m_sIconGroupName;
    //
    std::string m_sNormalPicture;
    uint32_t m_dwNormalPicColor;
    //
    std::string m_sSelectPicture;
    uint32_t m_dwSelectPicColor;
    //
    std::string m_sDisablePicture;
    uint32_t m_dwDisablePicColor;
};
