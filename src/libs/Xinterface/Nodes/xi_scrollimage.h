#pragma once

#include "..//inode.h"

// scroll image
class CXI_SCROLLIMAGE : public CINODE
{
    struct SCROLLEntity
    {
        FXYPOINT pCenter;
        float fCurScale;
        int imageNum;
        float colorMul;
        SCROLLEntity *next;
    };

    long m_nSlotsQnt;

    struct IMAGEDESCRIBE
    {
        bool *bUseSpecTechnique;
        long *tex;
        IDirect3DTexture9 **ptex;
        char **saveName;
        long *img;

        long str1, str2; // string identificators into string service
        char *string1, *string2;

        void Release(int nQnt);
        void Clear(int nQnt);
    };

  public:
    CXI_SCROLLIMAGE(CXI_SCROLLIMAGE &&) = delete;
    CXI_SCROLLIMAGE(const CXI_SCROLLIMAGE &) = delete;
    CXI_SCROLLIMAGE();
    ~CXI_SCROLLIMAGE();
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
    XYRECT GetCursorRect() override;
    uint32_t MessageProc(long msgcode, MESSAGE &message) override;

    void ChangeScroll(int nScrollItemNum);
    void DeleteImage(int imgNum);
    void RefreshScroll();

  protected:
    void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) override;
    float ChangeDinamicParameters(float fXDelta);
    int FindClickedImageNum() const;
    int GetRightQuantity() const;
    int GetLeftQuantity() const;
    float GetShiftDistance(int shiftIdx) const;
    void UpdateTexturesGroup();
    int FindTexGroupFromOld(char **pGroupList, char *groupName, int listSize);
    long GetMousePointedPictureNum() const;

  protected:
    // parameters for moving to scrolling
    bool m_bDoMove;
    float m_fDeltaMove;
    float m_fDeltaMoveBase;
    float m_fCurrentDistance;
    float m_fMoveDistance;
    int m_nSpeedMul;
    int m_nNotUsedQuantity;

    long *m_pPicOffset;

    bool m_bShowBorder;
    int m_nShowOrder;

    // size & blend parameters
    XYPOINT m_pCenter;
    XYPOINT m_ImageSize;
    XYPOINT m_pntCenterOffset;
    XYRECT m_rAbsolutePosition;
    float m_fScale;
    long m_lDelta;
    uint32_t *m_dwNormalColor;
    uint32_t *m_dwSelectColor;
    uint32_t m_dwBlendColor;

    // blind parameters
    bool m_bDoBlind;        // blind flag
    bool m_bColorType;      // current type of color for blind (true - ligth, false - dark)
    int m_nBlindCounter;    // last time counter for change of color type
    int m_nMaxBlindCounter; // maximum time counter for change of color type
    uint32_t *m_dwCurColor; // current color for select item show

    // textures parameters
    char **m_sGroupName;
    long *m_nGroupTex;
    int m_nGroupQuantity;
    char *m_sBorderGroupName; //
    long m_texBorder;         // select border texture identificator
    long m_nBorderPicture;    // select border picture identificator
    long *m_idBadTexture;     // image texture to replace nonexistent
    long *m_idBadPic;         // picture to replace nonexistent

    char *m_sSpecTechniqueName;
    uint32_t m_dwSpecTechniqueARGB;

    // one string parameters
    float m_nOneStrScale;
    int m_nOneStrFont;
    int m_nOneStrAlign;           // alignment string
    long m_lOneStrX;              // Offset from rectangle center for X coordinate
    bool m_bUseOneString;         // out to screen the one text string
    long m_lOneStrOffset;         // Offset from top rectangle of list for Y coordinate of string1
    uint32_t m_dwOneStrForeColor; // Font foreground color for first string
    uint32_t m_dwOneStrBackColor; // Font background color for first string
    // two string parameters
    float m_nTwoStrScale;
    int m_nTwoStrFont;
    int m_nTwoStrAlign;           // alignment string
    long m_lTwoStrX;              // Offset from rectangle center for X coordinate
    bool m_bUseTwoString;         // out to screen the two text string
    long m_lTwoStrOffset;         // Offset from top rectangle of list for Y coordinate of string2
    uint32_t m_dwTwoStrForeColor; // Font foreground color for second string
    uint32_t m_dwTwoStrBackColor; // Font background color for second string

    SCROLLEntity *m_pScroll;
    int m_nCurImage;
    int m_nListSize;
    IMAGEDESCRIBE *m_Image;
};
