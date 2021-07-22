#pragma once

#include "../inode.h"

class INIFILE;
class XINTERFACE;

// video
class CXI_PICTURE : public CINODE
{
    friend XINTERFACE;

  public:
    CXI_PICTURE();
    ~CXI_PICTURE() override;
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
    virtual void ChangeUV(FXYRECT &frNewUV);
    void ChangeColor(uint32_t dwColor);
    void SetPictureSize(long &nWidth, long &nHeight);

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void SetNewPicture(bool video, const char *sNewTexName);
    void SetNewPictureFromDir(const char *dirName);
    void SetNewPictureByGroup(const char *groupName, const char *picName);
    void SetNewPictureByPointer(IDirect3DBaseTexture9 *pTex);
    void ReleasePicture();

    char *m_pcGroupName;
    long m_idTex;
    IDirect3DBaseTexture9 *m_pD3D8Texture;
    CVideoTexture *m_pTex;
    XI_ONETEX_VERTEX m_v[4];

    bool m_bMakeBlind;
    float m_fCurBlindTime;
    bool m_bBlindUp;
    float m_fBlindUpSpeed;
    float m_fBlindDownSpeed;
    uint32_t m_dwBlindMin;
    uint32_t m_dwBlindMax;
};
