#ifndef _SCRSHOTER_H_
#define _SCRSHOTER_H_

#include "..\\..\\common_h\\dx8render.h"

class SCRSHOTER : public ENTITY
{
	VDX8RENDER  *rs;

	struct SAVETEXTURES
	{
		char * fileName;
		char * dataString;
		IDirect3DTexture8 * m_pTex;
		SAVETEXTURES * next;
	};

public:
	 SCRSHOTER();
	~SCRSHOTER();
	void SetDevice();
	bool Init();
    void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
    dword _cdecl ProcessMessage(MESSAGE & message);

private:
	bool MakeScreenShot();
	IDirect3DTexture8 * FindSaveTexture(char * fileName);
	char * FindSaveData(char * fileName);
	IDirect3DTexture8 * AddSaveTexture(char * dirName, char * fileName);
	void DelSaveTexture(char * fileName);
	IDirect3DTexture8 * GetTexFromSave(char * fileName, char **pDatStr);

	IDirect3DTexture8 * m_pScrShotTex;
	SAVETEXTURES * m_list;
};

#endif
