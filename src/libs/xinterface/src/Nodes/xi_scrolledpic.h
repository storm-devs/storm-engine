#pragma once

#include "xi_image.h"
#include "xi_picture.h"

class CXI_SCROLLEDPICTURE : public CXI_PICTURE
{
  public:
    CXI_SCROLLEDPICTURE();
    ~CXI_SCROLLEDPICTURE() override;
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
    void MoveMouseOutScreen(float fX, float fY) override;
    void ChangeUV(FXYRECT &frNewUV) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetNewPicture(bool video, char *sNewTexName);
    void SetNewPictureFromDir(char *dirName);
    void RecalculateTexPerPixel();
    void UpdateBuildenImages();
    void SetPosToCenter(float fX, float fY);
    void SetScale(long nScaleIdx);
    void SetScale(float fsx, float fsy);

    float m_fUTexPerPixel;
    float m_fVTexPerPixel;

    FXYPOINT m_fpBaseSize;

    struct BuildinImage
    {
        CXI_IMAGE *pImg;
        FXYPOINT fpPos;
        FXYPOINT fpSize;
        bool bShow;
    };

    long m_nScaleNum;
    std::vector<FXYPOINT> m_aScale;
    std::vector<BuildinImage> m_aImg;
};
