//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WideScreen
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WideScreen_H_
#define _WideScreen_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"

class WideScreen : public ENTITY  
{
//--------------------------------------------------------------------------------------------
//���������������, ������������������
//--------------------------------------------------------------------------------------------
public:
	WideScreen();
	virtual ~WideScreen();

	//�������������
	bool Init();
	//���������
	dword _cdecl ProcessMessage(MESSAGE & message);
	//������
	void Realize(dword delta_time);

//--------------------------------------------------------------------------------------------
//������������
//--------------------------------------------------------------------------------------------
private:
	VDX8RENDER * rs;	//
	float w, h;			//������� ������
	float state;		//��������� ������
	float dlt;			//����������� ��������� ������
};

#endif

