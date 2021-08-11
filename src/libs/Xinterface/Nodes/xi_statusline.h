#pragma once

#include "..//inode.h"

// static inactive images into one object
class CXI_STATUSLINE : public CINODE
{
  public:
    CXI_STATUSLINE();
    ~CXI_STATUSLINE();

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

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void Refresh();

  protected:
    char *m_sGroupName;  // image list name
    long m_idTex;        // texture identificator
    std::vector<XI_ONLYONETEX_VERTEX> m_vertices;
    long m_nVert;        // vertex quantity
    float m_fLineOffset; // edge offset for frame
    FXYRECT m_texRect1;
    FXYRECT m_texRect2;
};
