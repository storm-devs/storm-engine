#ifndef _VIDEOTEXTURE_H
#define _VIDEOTEXTURE_H

#include "..\common_h\dx8render.h"

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class CVideoTexture : public ENTITY
{
public:
    virtual IDirect3DTexture8* Initialize(VDX8RENDER *pRS,const char* sFileName, bool bCicled) = 0;
    virtual bool FrameUpdate() = 0;
	virtual void Release() = 0;
	IDirect3DTexture8 *m_pTexture;
	operator IDirect3DTexture8* () {return m_pTexture;}
	operator IDirect3DBaseTexture8* () {return m_pTexture;}
};

#endif