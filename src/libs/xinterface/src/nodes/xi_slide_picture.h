#pragma once

#include "../inode.h"

// video
class CXI_SLIDEPICTURE : public CINODE
{
  public:
    CXI_SLIDEPICTURE();
    ~CXI_SLIDEPICTURE() override;
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

    void SetNewPicture(char *sNewTexName);

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void Update(uint32_t Delta_Time);
    int32_t m_idTex;
    XI_ONETEX_VERTEX m_v[4];
    FXYRECT m_texRect;

    float minRotate;
    float deltaRotate;
    float curRotate;
    float curAngle;

    int32_t nLifeTime;
    int32_t nCurSlide;

    struct SLIDE_SPEED
    {
        uint32_t time;
        float xspeed;
        float yspeed;
    } * pSlideSpeedList;

    int32_t nSlideListSize;

    char *strTechniqueName;
};
