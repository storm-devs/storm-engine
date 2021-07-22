#pragma once

#define TEXTURE_MODIFY_NONE 0
#define TEXTURE_MODIFY_HORZFLIP 1
#define TEXTURE_MODIFY_VERTFLIP 2

class VDX9RENDER;
struct FXYRECT;
struct XYRECT;
struct FXYPOINT;
struct XYPOINT;

class VXSERVICE
{
  public:
    virtual ~VXSERVICE() = default;
    virtual void Init(VDX9RENDER *pRS, long lWidth, long lHight) = 0;

    // get texture identificator for image group
    virtual long GetTextureID(const char *sImageListName) = 0;
    virtual bool ReleaseTextureID(const char *sImageListName) = 0;

    // get texture positon for select picture
    virtual bool GetTexturePos(long pictureNum, FXYRECT &texRect) = 0;
    virtual bool GetTexturePos(long pictureNum, XYRECT &texRect) = 0;
    virtual bool GetTexturePos(const char *sImageListName, const char *sImageName, FXYRECT &texRect) = 0;
    virtual bool GetTexturePos(const char *sImageListName, const char *sImageName, XYRECT &texRect) = 0;
    virtual bool GetTexturePos(int nTextureModify, long pictureNum, FXYRECT &texRect) = 0;
    virtual bool GetTexturePos(int nTextureModify, const char *sImageListName, const char *sImageName,
                               FXYRECT &texRect) = 0;

    virtual void GetTextureCutForSize(const char *pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize,
                                      long nSrcWidth, long nSrcHeight, FXYRECT &outUV) = 0;

    virtual long GetImageNum(const char *sImageListName, const char *sImageName) = 0;

    virtual void ReleaseAll() = 0;
};
