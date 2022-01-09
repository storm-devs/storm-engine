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
    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override;

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
    int32_t m_idTex;
    uint32_t m_dwColor;

    // picture index & vertex buffers
    int32_t m_idVBuf; // identificator of the vertex buffer
    int32_t m_idIBuf; // identificator of the index buffer
    int32_t m_nVert;  // vertex quantity
    int32_t m_nIndx;  // index quantity

    float m_fAngleWidth;
    float m_fAngleHeight;

    int32_t m_idAngle;
    int32_t m_idHorzLine;

    int32_t m_nHorzLineQuantity;
    int32_t m_nVertLineQuantity;
};
