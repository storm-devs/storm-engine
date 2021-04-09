#pragma once

#include "../inode.h"

class CXI_IMAGE;

class CXI_BORDER : public CINODE
{
  public:
    CXI_BORDER();
    ~CXI_BORDER();

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
    void FillIndexBuffers() const;
    void FillVertexBuffers();
    void WriteVertexForSquare(XI_ONETEX_VERTEX *pV, FXYRECT &UVRect, uint32_t dwColor, long left, long top, long right,
                              long bottom);

  protected:
    // back picture
    CXI_IMAGE *m_pBackImage;

    long m_nCaptionHeight;
    CXI_IMAGE *m_pCaptionImage;
    long m_mCaptionDividerHeight;
    XYRECT m_rCapRect;

    // texture parameters
    std::string m_sGroupName;
    long m_idTex;
    uint32_t m_dwColor;

    // picture index & vertex buffers
    long m_idVBuf;   // identificator of the vertex buffer
    long m_idIBuf;   // identificator of the index buffer
    long m_nSquareQ; // square quantity

    long m_nLeftTopPicture;
    FXYRECT m_frLeftTopUV;
    XYPOINT m_pntLeftTopSize;

    long m_nRightTopPicture;
    FXYRECT m_frRightTopUV;
    XYPOINT m_pntRightTopSize;

    long m_nLeftBottomPicture;
    FXYRECT m_frLeftBottomUV;
    XYPOINT m_pntLeftBottomSize;

    long m_nRightBottomPicture;
    FXYRECT m_frRightBottomUV;
    XYPOINT m_pntRightBottomSize;

    long m_nTopLinePicture;
    FXYRECT m_frTopLineUV;
    long m_nTopLineHeight;

    long m_nBottomLinePicture;
    FXYRECT m_frBottomLineUV;
    long m_nBottomLineHeight;

    long m_nLeftLinePicture;
    FXYRECT m_frLeftLineUV;
    long m_nLeftLineWidth;

    long m_nRightLinePicture;
    FXYRECT m_frRightLineUV;
    long m_nRightLineWidth;
};
