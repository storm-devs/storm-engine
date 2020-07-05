#ifndef VCUBEMAP_SERVICE_HPP
#define VCUBEMAP_SERVICE_HPP

#include "vmodule_api.h"
#include "dx8render.h"
#include "defines.h"

#define CM_NUM_FACES	6

#define CM_FACE_POSX	1	// D3DCUBEMAP_FACE_POSITIVE_X
#define CM_FACE_NEGX	2	// D3DCUBEMAP_FACE_NEGATIVE_X
#define CM_FACE_POSY	4	// D3DCUBEMAP_FACE_POSITIVE_Y
#define CM_FACE_NEGY	8	// D3DCUBEMAP_FACE_NEGATIVE_Y
#define CM_FACE_POSZ	16	// D3DCUBEMAP_FACE_POSITIVE_Z
#define CM_FACE_NEGZ	32	// D3DCUBEMAP_FACE_NEGATIVE_Z
#define CM_FACE_ALL		(CM_FACE_POSX | CM_FACE_POSY | CM_FACE_POSZ | CM_FACE_NEGX | CM_FACE_NEGY | CM_FACE_NEGZ)

class VCUBEMAP
{
public:
	virtual ~VCUBEMAP() {};

	virtual bool CreateCubeMap(dword dwSize, D3DFORMAT ColorFormat, D3DFORMAT DepthFormat) = 0;

	// save current cube map to file
	virtual bool Save(const char *cFileName) = 0;
	virtual bool Save(HANDLE hFile) = 0;

	// load cube map from file, delete previous surfaces and z buffer
	virtual bool Load(const char *cFileName) = 0;
	virtual bool Load(HANDLE hFile) = 0;

	// is real cube map, if 
	virtual bool isRealCubeMap() const = 0;

	// render scene to cube map
	virtual bool Render(const char *cLayerName, dword dwFlags = CM_FACE_ALL, CVECTOR * vPos = 0) = 0;

	virtual bool DrawCube(CVECTOR *vPos = 0, float fSize = 512.0f) = 0;

	// set this cube map (or side) to texture stage
	virtual void SetTexture(dword dwStage, dword dwFace = CM_FACE_POSX) = 0;
};

class VCUBE_SERVICE : public SERVICE
{
public:
	virtual ~VCUBE_SERVICE() {};

	virtual VCUBEMAP * CreateCubeMap(dword dwSize = 256, D3DFORMAT ColorFormat = D3DFMT_R5G6B5, D3DFORMAT DepthFormat = D3DFMT_D16, bool bRealCubeMap = false) = 0;
};

#endif