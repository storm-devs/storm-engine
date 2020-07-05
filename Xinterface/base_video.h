#ifndef _BASE_VIDEO_H_
#define _BASE_VIDEO_H_

#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\dx8render.h"

class xiBaseVideo : public ENTITY
{
public:
	virtual void SetShowVideo(bool bShowVideo) {}
	virtual IDirect3DTexture8 * GetCurrentVideoTexture() {return null;}
};

#endif
