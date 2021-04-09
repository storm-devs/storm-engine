#pragma once

#include "../xdefines.h"
#include "dx9render.h"

class INIFILE;
class VXSERVICE;

class CXI_ToolTip
{
  public:
    CXI_ToolTip(CXI_ToolTip &&) = delete;
    CXI_ToolTip(const CXI_ToolTip &) = delete;
    CXI_ToolTip(VXSERVICE *pPicService, VSTRSERVICE *pStrService, XYPOINT &pntScrSize);
    ~CXI_ToolTip();

    void Draw();
    void SetByFormatString(XYRECT &rectOwner, INIFILE *pDefIni, const char *pFmtStr);

    void MousePos(float fDeltaTime, long nX, long nY);

  protected:
    void ReleaseAll();

    void CreateIndexBuffer();
    void CreateVertexBuffer();
    void UpdateIndexBuffer() const;
    void UpdateVertexBuffer();

    void ReplaceRectangle(long x, long y);

  protected:
    VDX9RENDER *m_rs;
    XI_ONETEX_VERTEX *m_pV;
    uint16_t *m_pI;
    long m_nSquareQ;
    uint32_t m_dwBackColor;
    long m_nLeftSideWidth{};
    long m_nRightSideWidth{};

    FXYRECT m_uvBackLeft;
    FXYRECT m_uvBackRight;
    FXYRECT m_uvBackMiddle;

    std::string m_sGroupName;
    long m_nTextureID;
    long m_nPicIndex_Left{};
    long m_nPicIndex_Right{};
    long m_nPicIndex_Middle{};

    XYPOINT m_pntScreenSize;
    XYPOINT m_pntTextOffset;
    XYRECT m_rPos;
    XYRECT m_rActiveZone;

    long m_nXRectangleOffset{};
    long m_nYRectangleOffsetUp{};
    long m_nYRectangleOffsetDown{};

    std::string m_sText;
    std::vector<std::string> m_aSubText;
    long m_nFontID;
    uint32_t m_dwFontColor{};
    float m_fFontScale{};
    long m_nMaxStrWidth{};
    long m_nUseWidth{};
    long m_nUseHeight{};

    bool m_bDisableDraw;
    float m_fTurnOnDelay;
    float m_fCurTimeLeft;
    long m_nMouseX;
    long m_nMouseY;

    VXSERVICE *m_pPicService;
    VSTRSERVICE *m_pStrService;
};
