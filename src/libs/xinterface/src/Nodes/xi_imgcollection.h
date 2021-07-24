#pragma once

#include "../inode.h"
#include "vmodule_api.h"

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
    bool IsClick(int buttonID, long xPos, long yPos) override;

    void MouseThis(float fX, float fY) override
    {
    }

    void ChangePosition(XYRECT &rNewPos) override;
    void SaveParametersToIni() override;
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;
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
    long texl;        // texture identificator
    long vBuf;        // vertex buffer identificator
    long iBuf;        // index buffer identificator
    long nVert;       // vertex quantity
    long nIndx;       // index quantity

    struct PicEditInfo
    {
        std::string sName;
        long nLeft, nTop, nRight, nBottom;
        uint32_t dwColor;
        bool bNative;
    };

    std::vector<PicEditInfo> m_aEditInfo;

    struct PicEditSection
    {
        std::string sName;
        long nStartNum;
        long nQuantity;
    };

    std::vector<PicEditSection> m_aSections;
    long m_nCurSection;

    XYPOINT m_xyCommonOffset;
};
