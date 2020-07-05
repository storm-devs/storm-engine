#pragma once

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\collide.h"

class CameraDialog : public ENTITY
{
	VDX8RENDER * rs;
	COLLIDE *col;
	CVECTOR pos, ang;
	float fov;
	ENTITY_ID person, personId;

	float time;
	struct ANIFRAME
	{
		CVECTOR pos, ang;
	};
	long frames;
	ANIFRAME* track;

public:
	CameraDialog();
	~CameraDialog();
	bool Init();
	void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
	dword _cdecl ProcessMessage(MESSAGE &msg);
};


//API_MODULE_START("CameraDialog")
//	CREATE_CLASS(0,CameraDialog)
//API_MODULE_END

