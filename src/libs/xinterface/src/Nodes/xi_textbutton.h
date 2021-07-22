#pragma once

#include "..//inode.h"

// picture
class CXI_TEXTBUTTON : public CINODE
{
  public:
    CXI_TEXTBUTTON(CXI_TEXTBUTTON &&) = delete;
    CXI_TEXTBUTTON(const CXI_TEXTBUTTON &) = delete;
    CXI_TEXTBUTTON();
    ~CXI_TEXTBUTTON() override;

    void Draw(bool bSelected, uint32_t Delta_Time) override;
    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ReleaseAll() override;
    int CommandExecute(int wActCode) override;
    bool IsClick(int buttonID, long xPos, long yPos) override;

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

    void NotUsingTime(uint32_t Delta_Time)
    {
        m_nPressedDelay = 0;
    }

    void SetUsing(bool bUsing) override;
    void MakeLClickPreaction() override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void FillPositionIntoVertices();

  protected:
    char *m_sGroupName;
    long m_idTex;            // texture identity
    long m_idShadowTex;      // shadow texture
    long m_idUnSelectMiddle; // picture id for middle part of unselect button
    long m_idSelectMiddle;   // picture id for middle part of select button
    long m_idUnSelectLeft;   // picture id for left part of unselect button
    long m_idSelectLeft;     // picture id for left part of select button
    long m_idUnSelectRight;  // picture id for right part of unselect button
    long m_idSelectRight;    // picture id for right part of select button
    bool m_bCurrentSelected;

    CVideoTexture *m_pTex;
    XI_ONETEX_VERTEX m_v[8];

    uint32_t m_dwShadowColor;      // shadow color
    uint32_t m_dwFaceColor;        // unpressed key color
    uint32_t m_dwPressedFaceColor; // active key color
    uint32_t m_dwFontColor;        // font color
    uint32_t m_dwUnselFontColor;   // font color for unavailable key
    float m_fFontScale;

    float m_fXShadow;      // X offset for normal key shadow
    float m_fYShadow;      // Y offset for normal key shadow
    float m_fXShadowPress; // X offset for the shadow of the pressed key
    float m_fYShadowPress; // Y offset for the shadow of the pressed key
    float m_fXDeltaPress;  // X offset on button click
    float m_fYDeltaPress;  // Y offset when button is pressed

    int m_nPressedDelay;
    int m_nMaxDelay;

    // string parameters
    int m_nFontNum;
    long m_idString;
    int m_dwStrOffset;
    char *m_sString;

    // picture index & vertex buffers
    long m_idVBuf; // identificator of the vertex buffer
    long m_idIBuf; // identificator of the index buffer
    long m_nVert;  // vertex quantity
    long m_nIndx;  // index quantity

    bool m_bVideoToBack;
    uint32_t m_dwBackColor;

    float m_fShadowScale;
};
