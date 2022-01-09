#pragma once

#include "../inode.h"
#include "v_module_api.h"

// static inactive images into one object
class CXI_IMGCOLLECTION : public CINODE
{
  public:
    CXI_IMGCOLLECTION();
    ~CXI_IMGCOLLECTION() override;

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
    bool GetInternalNameList(std::vector<std::string> &aStr) override;
    void SetInternalName(std::string &sName) override;

    void AddImage(const char *pcPicName, uint32_t dwColor, XYRECT pos);

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    // set into vertex&index buffers value for image number <rectNum> : screen&texture position
    void SetBuffers(XI_ONETEX_VERTEX *pVBuf, uint16_t *pIBuf, int rectNum, XYRECT &scrRect, FXYRECT &texRect,
                    uint32_t dwColor);
    void UpdateBuffers();

  protected:
    bool m_bRelativeRect;
    char *sGroupName; // image list name
    int32_t texl;        // texture identificator
    int32_t vBuf;        // vertex buffer identificator
    int32_t iBuf;        // index buffer identificator
    int32_t nVert;       // vertex quantity
    int32_t nIndx;       // index quantity

    struct PicEditInfo
    {
        std::string sName;
        int32_t nLeft, nTop, nRight, nBottom;
        uint32_t dwColor;
        bool bNative;
    };

    std::vector<PicEditInfo> m_aEditInfo;

    struct PicEditSection
    {
        std::string sName;
        int32_t nStartNum;
        int32_t nQuantity;
    };

    std::vector<PicEditSection> m_aSections;
    int32_t m_nCurSection;

    XYPOINT m_xyCommonOffset;
};
