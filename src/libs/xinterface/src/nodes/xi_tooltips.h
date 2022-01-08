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

    void MousePos(float fDeltaTime, int32_t nX, int32_t nY);

  protected:
    void ReleaseAll();

    void CreateIndexBuffer();
    void CreateVertexBuffer();
    void UpdateIndexBuffer() const;
    void UpdateVertexBuffer();

    void ReplaceRectangle(int32_t x, int32_t y);

  protected:
    VDX9RENDER *m_rs;
    XI_ONETEX_VERTEX *m_pV;
    uint16_t *m_pI;
    int32_t m_nSquareQ;
    uint32_t m_dwBackColor;
    int32_t m_nLeftSideWidth{};
    int32_t m_nRightSideWidth{};

    FXYRECT m_uvBackLeft;
    FXYRECT m_uvBackRight;
    FXYRECT m_uvBackMiddle;

    std::string m_sGroupName;
    int32_t m_nTextureID;
    int32_t m_nPicIndex_Left{};
    int32_t m_nPicIndex_Right{};
    int32_t m_nPicIndex_Middle{};

    XYPOINT m_pntScreenSize;
    XYPOINT m_pntTextOffset;
    XYRECT m_rPos;
    XYRECT m_rActiveZone;

    int32_t m_nXRectangleOffset{};
    int32_t m_nYRectangleOffsetUp{};
    int32_t m_nYRectangleOffsetDown{};

    std::string m_sText;
    std::vector<std::string> m_aSubText;
    int32_t m_nFontID;
    uint32_t m_dwFontColor{};
    float m_fFontScale{};
    int32_t m_nMaxStrWidth{};
    int32_t m_nUseWidth{};
    int32_t m_nUseHeight{};

    bool m_bDisableDraw;
    float m_fTurnOnDelay;
    float m_fCurTimeLeft;
    int32_t m_nMouseX;
    int32_t m_nMouseY;

    VXSERVICE *m_pPicService;
    VSTRSERVICE *m_pStrService;
};
