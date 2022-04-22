#pragma once

#include "../inode.h"

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

    int32_t m_nSlotsQnt;

    struct ImageSlot
    {
        bool useSpecTechnique = false;
        int32_t tex = -1;
        int32_t ptex = -1;
        int32_t img = -1;
        std::string saveName;
    };

    struct IMAGEDESCRIBE
    {
        std::vector<ImageSlot> slots;

        int32_t str1, str2; // string identificators into string service
        std::string string1;
        std::string string2;

        void Clear();
    };

  public:
    CXI_SCROLLIMAGE(CXI_SCROLLIMAGE &&) = delete;
    CXI_SCROLLIMAGE(const CXI_SCROLLIMAGE &) = delete;
    CXI_SCROLLIMAGE();
    ~CXI_SCROLLIMAGE() override;
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
    XYRECT GetCursorRect() override;
    uint32_t MessageProc(int32_t msgcode, MESSAGE &message) override;

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
    int FindTexGroupFromOld(char **pGroupList, const char *groupName, int listSize);
    int32_t GetMousePointedPictureNum() const;

  protected:
    // parameters for moving to scrolling
    bool m_bDoMove;
    float m_fDeltaMove;
    float m_fDeltaMoveBase;
    float m_fCurrentDistance;
    float m_fMoveDistance;
    int m_nSpeedMul;
    int m_nNotUsedQuantity;

    int32_t *m_pPicOffset;

    bool m_bShowBorder;
    int m_nShowOrder;

    // size & blend parameters
    XYPOINT m_pCenter;
    XYPOINT m_ImageSize;
    XYPOINT m_pntCenterOffset;
    XYRECT m_rAbsolutePosition;
    float m_fScale;
    int32_t m_lDelta;
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
    int32_t *m_nGroupTex;
    int m_nGroupQuantity;
    char *m_sBorderGroupName; //
    int32_t m_texBorder;         // select border texture identificator
    int32_t m_nBorderPicture;    // select border picture identificator
    int32_t *m_idBadTexture;     // image texture to replace nonexistent
    int32_t *m_idBadPic;         // picture to replace nonexistent

    char *m_sSpecTechniqueName;
    uint32_t m_dwSpecTechniqueARGB;

    // one string parameters
    float m_nOneStrScale;
    int m_nOneStrFont;
    int m_nOneStrAlign;           // alignment string
    int32_t m_lOneStrX;              // Offset from rectangle center for X coordinate
    bool m_bUseOneString;         // out to screen the one text string
    int32_t m_lOneStrOffset;         // Offset from top rectangle of list for Y coordinate of string1
    uint32_t m_dwOneStrForeColor; // Font foreground color for first string
    uint32_t m_dwOneStrBackColor; // Font background color for first string
    // two string parameters
    float m_nTwoStrScale;
    int m_nTwoStrFont;
    int m_nTwoStrAlign;           // alignment string
    int32_t m_lTwoStrX;              // Offset from rectangle center for X coordinate
    bool m_bUseTwoString;         // out to screen the two text string
    int32_t m_lTwoStrOffset;         // Offset from top rectangle of list for Y coordinate of string2
    uint32_t m_dwTwoStrForeColor; // Font foreground color for second string
    uint32_t m_dwTwoStrBackColor; // Font background color for second string

    SCROLLEntity *m_pScroll;
    int m_nCurImage;
    std::vector<IMAGEDESCRIBE> m_Image;
};
