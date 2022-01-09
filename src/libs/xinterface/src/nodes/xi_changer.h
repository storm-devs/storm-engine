#pragma once

#include "../inode.h"

// picture
class CXI_CHANGER : public CINODE
{
  public:
    CXI_CHANGER();
    ~CXI_CHANGER() override;

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
    XYRECT GetCursorRect() override;

    bool IsGlowChanged() override
    {
        return true;
    }

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetRectanglesToPosition(int nPos);

  protected:
    int m_nPlaceQuantity;
    XYRECT *m_pPlace;

    bool m_bUseBlind;
    uint32_t m_dwFoneColor;
    uint32_t m_dwBlindColor;
    uint32_t m_dwCurColor;
    float m_xOffset, m_yOffset;
    bool m_bUpBlind;
    float m_fCurM;
    float m_fCurM_UpSpeed;
    float m_fCurM_DownSpeed;

    int32_t m_idBackTex;
    CVideoTexture *m_pTex;

    int m_nCurrentPos;

    XI_ONETEX_VERTEX m_pTexVert[8];

    int m_nClickPlaceNum;
    bool m_bIncreaseChange;
};
