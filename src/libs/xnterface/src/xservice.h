#pragma once

#include "vxservice.h"
#include "xdefines.h"

class XSERVICE : public VXSERVICE
{
    struct IMAGELISTDESCR
    {
        char *sImageListName;
        char *sTextureName;
        long textureID;
        int textureQuantity;

        long textureWidth;
        long textureHeight;
        long pictureQuantity;
        long pictureStart;
    };

    struct PICTUREDESCR
    {
        char *sPictureName;
        XYRECT pTextureRect;
    };

  public:
    XSERVICE();
    ~XSERVICE();

    // initialization of service
    void Init(VDX9RENDER *pRS, long lWidth, long lHight) override;

    // get texture identificator for image group
    long GetTextureID(const char *sImageListName) override;
    long FindGroup(const char *sImageListName) const;
    bool ReleaseTextureID(const char *sImageListName) override;

    // get texture positon for select picture
    bool GetTexturePos(long pictureNum, FXYRECT &texRect) override;
    bool GetTexturePos(long pictureNum, XYRECT &texRect) override;
    bool GetTexturePos(const char *sImageListName, const char *sImageName, FXYRECT &texRect) override;
    bool GetTexturePos(const char *sImageListName, const char *sImageName, XYRECT &texRect) override;
    bool GetTexturePos(int nTextureModify, long pictureNum, FXYRECT &texRect) override;
    bool GetTexturePos(int nTextureModify, const char *sImageListName, const char *sImageName,
                       FXYRECT &texRect) override;

    void GetTextureCutForSize(const char *pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize,
                              long nSrcWidth, long nSrcHeight, FXYRECT &outUV) override;

    long GetImageNum(const char *sImageListName, const char *sImageName) override;

    void ReleaseAll() override;

  protected:
    void LoadAllPicturesInfo();

  protected:
    VDX9RENDER *m_pRS;

    long m_dwListQuantity;
    long m_dwImageQuantity;
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
