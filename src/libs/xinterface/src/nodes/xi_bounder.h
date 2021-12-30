#pragma once

#include "../inode.h"

class CXI_BOUNDER : public CINODE
{
  public:
    CXI_BOUNDER();
    ~CXI_BOUNDER() override;

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

  protected:
    // texture parameters
    char *m_sGroupName;
    long m_idTex;
    uint32_t m_dwColor;

    // picture index & vertex buffers
    long m_idVBuf; // identificator of the vertex buffer
    long m_idIBuf; // identificator of the index buffer
    long m_nVert;  // vertex quantity
    long m_nIndx;  // index quantity

    float m_fAngleWidth;
    float m_fAngleHeight;

    long m_idAngle;
    long m_idHorzLine;

    long m_nHorzLineQuantity;
    long m_nVertLineQuantity;
};
