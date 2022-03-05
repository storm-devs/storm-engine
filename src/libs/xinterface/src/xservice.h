#pragma once

#include "vx_service.h"
#include "xdefines.h"

class XSERVICE : public VXSERVICE
{
    struct IMAGELISTDESCR
    {
        char *sImageListName;
        char *sTextureName;
        int32_t textureID;
        int textureQuantity;

        int32_t textureWidth;
        int32_t textureHeight;
        int32_t pictureQuantity;
        int32_t pictureStart;
    };

    struct PICTUREDESCR
    {
        char *sPictureName;
        XYRECT pTextureRect;
    };

  public:
    XSERVICE();
    ~XSERVICE() override;

    // initialization of service
    void Init(VDX9RENDER *pRS, int32_t lWidth, int32_t lHight) override;

    // get texture identificator for image group
    int32_t GetTextureID(const char *sImageListName) override;
    int32_t FindGroup(const char *sImageListName) const;
    bool ReleaseTextureID(const char *sImageListName) override;

    // get texture positon for select picture
    bool GetTexturePos(int32_t pictureNum, FXYRECT &texRect) override;
    bool GetTexturePos(int32_t pictureNum, XYRECT &texRect) override;
    bool GetTexturePos(const char *sImageListName, const char *sImageName, FXYRECT &texRect) override;
    bool GetTexturePos(const char *sImageListName, const char *sImageName, XYRECT &texRect) override;
    bool GetTexturePos(int nTextureModify, int32_t pictureNum, FXYRECT &texRect) override;
    bool GetTexturePos(int nTextureModify, const char *sImageListName, const char *sImageName,
                       FXYRECT &texRect) override;

    void GetTextureCutForSize(const char *pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize,
                              int32_t nSrcWidth, int32_t nSrcHeight, FXYRECT &outUV) override;

    int32_t GetImageNum(const char *sImageListName, const char *sImageName) override;

    void ReleaseAll() override;

  protected:
    void LoadAllPicturesInfo();

  protected:
    VDX9RENDER *m_pRS;

    int32_t m_dwListQuantity;
    int32_t m_dwImageQuantity;
    IMAGELISTDESCR *m_pList;
    PICTUREDESCR *m_pImage;

    // Scale factors
    float m_fWScale;
    float m_fHScale;
    // scaling error parameters
    float m_fWAdd;
    float m_fHAdd;
};

class ComboString
{
    VDX9RENDER *m_pRS;

    struct STRING_DESCR
    {
        int x, y;
        int idFont;
        char *str;
    };

    struct PICS_DESCR
    {
        XYRECT pos;
        int idTex;
        FXYRECT texUV;
    };

    struct STRING_COMBINE
    {
        int *pFontsID;
        int fontQ;

        STRING_DESCR *pStr;
        int strQ;

        PICS_DESCR *pPics;
        int picQ;
    };

    STRING_COMBINE *pComboStr;

    void AddToCombo(char *fontName, const XYPOINT &posStrStart, char *str);
    void AddToCombo(XYRECT posPic, char *picTexName, FXYRECT picUV);

  public:
    ComboString();
    ~ComboString();

    void PrintComboString(int comboStrID);
    int GetComboString(int align, int x, int y, int needWidth, int needHeight, int *allHeight, char *formatStr,
                       char *fontlist);
    void ComboStringRelease(int comboStrID);
};
