#ifndef _TextureSequence_H
#define _TextureSequence_H

#include "..\\..\\common_h\\VideoTexture.h"

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class TextureSequence : public CVideoTexture
{
	VDX8RENDER  *m_pRS;
	long m_xQuantity;
	long m_yQuantity;
	long m_texWidth;
	long m_texHeight;
	bool m_bCicled;
	DWORD m_dwDeltaTime;
	long m_maxCurNum;
	bool m_bHorzFlip;
	bool m_bVertFlip;

	long m_curNum;
	DWORD m_dwCurDeltaTime;

	long m_AllTex;

	void ToTextureRender(float blendValue);
public:
    TextureSequence();
    ~TextureSequence();

    IDirect3DTexture9* Initialize(VDX8RENDER *pRS,const char* cTSfileName, bool bCicled);
    bool FrameUpdate();
	void Release();
	void LostRender();
	void RestoreRender();
};

#endif