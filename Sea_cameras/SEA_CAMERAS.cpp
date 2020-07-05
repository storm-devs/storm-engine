#include "SEA_CAMERAS.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "..\engine\program\sea_ai\Script_Defines.h"

INTERFACE_FUNCTION
CREATE_CLASS(SEA_CAMERAS)
CREATE_CLASS(FREE_CAMERA)
CREATE_CLASS(SHIP_CAMERA)
CREATE_CLASS(DECK_CAMERA)

SEA_CAMERAS::SEA_CAMERAS() : CamerasArray(_FL_)
{
	bActive = true;
//	ShowCursor(false);
}

SEA_CAMERAS::~SEA_CAMERAS()
{
//	ShowCursor(true);
}

void SEA_CAMERAS::ProcessMessage(dword iMsg,dword wParam,dword lParam)
{
/*	GUARD(SEA_CAMERAS::ProcessMessage(dword,dword,dword))
	switch(iMsg)
	{
		case WM_ACTIVATE: 
		{
			WORD wActive = LOWORD(wParam);           
			bActive = (wActive == WA_CLICKACTIVE || wActive == WA_ACTIVE);
			for (dword i=0;i<CamerasArray.Size();i++) CamerasArray[i]->SetActive(bActive);
		}
		break;
	}
	UNGUARD*/
}

dword SEA_CAMERAS::ProcessMessage(MESSAGE & message)
{
	dword i;
	switch (message.Long())
	{
		case AI_CAMERAS_ADD_CAMERA:
		{
			ENTITY_ID eidCamera = message.EntityID();
			COMMON_CAMERA * pCamera = (COMMON_CAMERA*)eidCamera.pointer;
			if (CamerasArray.Find(pCamera) == INVALID_ARRAY_INDEX) CamerasArray.Add(pCamera);
			pCamera->SetOn(false);
			pCamera->SetActive(bActive);
		}
		break;
		case AI_CAMERAS_SET_CAMERA:
		{
			ENTITY_ID eidCamera = message.EntityID();
			ATTRIBUTES * pACharacter = message.AttributePointer();
			COMMON_CAMERA * pCamera = (COMMON_CAMERA*)eidCamera.pointer;
			if (CamerasArray.Find(pCamera) == INVALID_ARRAY_INDEX) CamerasArray.Add(pCamera);
			for (i=0;i<CamerasArray.Size();i++) CamerasArray[i]->SetOn(false);
			pCamera->SetOn(true);
			pCamera->SetActive(bActive);
			pCamera->SetCharacter(pACharacter);
		}
		break;
		case AI_MESSAGE_SEASAVE:
		{
			CSaveLoad * pSL = (CSaveLoad*)message.Pointer();
			for (i=0; i<CamerasArray.Size();i++) CamerasArray[i]->Save(pSL);
		}
		break;
		case AI_MESSAGE_SEALOAD:
		{
			CSaveLoad * pSL = (CSaveLoad*)message.Pointer();
			for (i=0; i<CamerasArray.Size();i++) CamerasArray[i]->Load(pSL);
		}
		break;
	}
	return 0;
}
