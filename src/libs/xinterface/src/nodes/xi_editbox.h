#pragma once

#include "..//inode.h"

// edit box
class CXI_EDITBOX : public CINODE
{
  public:
    CXI_EDITBOX(CXI_EDITBOX &&) = delete;
    CXI_EDITBOX(const CXI_EDITBOX &) = delete;
    CXI_EDITBOX();
    ~CXI_EDITBOX() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override;
    void MouseThis(float fX, float fY) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetNewCurSymbol(int h, int v);

  protected:
    char m_alpha[512];
    int m_nAlphaQuantity;
    int m_nCurAlphaNum;
    int m_nFirstChar;
    int m_nHorz, m_nVert;
    int m_nMaxSize;
    int m_bUpChrRegistrOffset;

    FXYRECT m_frNormTex;
    FXYRECT m_frSelTex;

    long m_idBtnTex;
    uint32_t m_dwEditBoxColor;
    uint32_t m_dwBorderColor;

    int m_nTopStringPos;
    int m_nTopOffset;
    int m_nLeftOffset;

    long m_nChrFontNum;
    long m_nStrFontNum;
    float m_fChrScale;
    float m_fStrScale;
    float m_fChrTopOffset;

    bool m_bOldSelected;

    long m_idVBRect;
    long m_idVB;
    long m_idIB;
    long m_nVQ;
    long m_nIQ;

    float m_fVAdd;
    float m_fHAdd;
};
