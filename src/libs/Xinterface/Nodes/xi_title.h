#pragma once

#include "../inode.h"

class CXI_TITLE : public CINODE
{
  public:
    CXI_TITLE(CXI_TITLE &&) = delete;
    CXI_TITLE(const CXI_TITLE &) = delete;
    CXI_TITLE();
    ~CXI_TITLE();
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

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void FillVertexBuffer() const;

  protected:
    char *m_sGroupName;
    long m_idTex; // texture identity

    long m_idString;
    XYPOINT m_StringCenter;
    uint32_t m_fontColor;
    uint32_t m_backColor;
    float m_fontScale;
    long m_fontID;

    long m_idVBuf; // vertex buffer identificator
    long m_idIBuf; // index buffer identificator
    long m_nVert;  // vertex quantity
    long m_nIndx;  // index quantity

    long m_nStringWidth;
    XYRECT m_mRect, m_tRect;
    long m_nTiledQuantity;
};
