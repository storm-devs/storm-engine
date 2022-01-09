#pragma once

#include "../inode.h"

class CXI_IMAGE;

class CXI_BORDER : public CINODE
{
  public:
    CXI_BORDER();
    ~CXI_BORDER() override;

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

    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    void FillIndexBuffers() const;
    void FillVertexBuffers();
    void WriteVertexForSquare(XI_ONETEX_VERTEX *pV, FXYRECT &UVRect, uint32_t dwColor, int32_t left, int32_t top, int32_t right,
                              int32_t bottom);

  protected:
    // back picture
    CXI_IMAGE *m_pBackImage;

    int32_t m_nCaptionHeight;
    CXI_IMAGE *m_pCaptionImage;
    int32_t m_mCaptionDividerHeight;
    XYRECT m_rCapRect;

    // texture parameters
    std::string m_sGroupName;
    int32_t m_idTex;
    uint32_t m_dwColor;

    // picture index & vertex buffers
    int32_t m_idVBuf;   // identificator of the vertex buffer
    int32_t m_idIBuf;   // identificator of the index buffer
    int32_t m_nSquareQ; // square quantity

    int32_t m_nLeftTopPicture;
    FXYRECT m_frLeftTopUV;
    XYPOINT m_pntLeftTopSize;

    int32_t m_nRightTopPicture;
    FXYRECT m_frRightTopUV;
    XYPOINT m_pntRightTopSize;

    int32_t m_nLeftBottomPicture;
    FXYRECT m_frLeftBottomUV;
    XYPOINT m_pntLeftBottomSize;

    int32_t m_nRightBottomPicture;
    FXYRECT m_frRightBottomUV;
    XYPOINT m_pntRightBottomSize;

    int32_t m_nTopLinePicture;
    FXYRECT m_frTopLineUV;
    int32_t m_nTopLineHeight;

    int32_t m_nBottomLinePicture;
    FXYRECT m_frBottomLineUV;
    int32_t m_nBottomLineHeight;

    int32_t m_nLeftLinePicture;
    FXYRECT m_frLeftLineUV;
    int32_t m_nLeftLineWidth;

    int32_t m_nRightLinePicture;
    FXYRECT m_frRightLineUV;
    int32_t m_nRightLineWidth;
};
