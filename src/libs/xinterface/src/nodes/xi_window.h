#pragma once

#include "../inode.h"

class CXI_WINDOW : public CINODE
{
  public:
    CXI_WINDOW();
    ~CXI_WINDOW() override;

    bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
              XYPOINT &ScreenSize) override;
    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;

    void SetShow(bool bShow);

    bool GetShow() const
    {
        return m_bShow;
    }

    void SetActive(bool bActive);

    bool GetActive() const
    {
        return m_bActive;
    }

    void AddNode(const char *pcNodeName);

    int CommandExecute(int wActCode) override
    {
        return -1;
    };

    void Draw(bool bSelected, uint32_t Delta_Time) override{};

    void ReleaseAll() override{};

    bool IsClick(int buttonID, int32_t xPos, int32_t yPos) override
    {
        return false;
    };

    void MouseThis(float fX, float fY) override{};

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;

  protected:
    std::vector<std::string> m_aNodeNameList;

    bool m_bShow;
    bool m_bActive;
};
