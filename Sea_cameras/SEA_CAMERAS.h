#ifndef SEA_CAMERAS_HPP
#define SEA_CAMERAS_HPP

#include "FreeCamera.h"
#include "ShipCamera.h"
#include "DeckCamera.h"

class SEA_CAMERAS : public ENTITY
{
	array<COMMON_CAMERA*>	CamerasArray;
	bool	bActive;
public:
	SEA_CAMERAS();
	virtual ~SEA_CAMERAS();

	void ProcessMessage(dword iMsg,dword wParam,dword lParam);
	dword _cdecl ProcessMessage(MESSAGE & message);
};
/*
API_MODULE_START("SEA_CAMERAS")
	CREATE_CLASS(0,SEA_CAMERAS)
	CREATE_CLASS(1,FREE_CAMERA)
	CREATE_CLASS(2,SHIP_CAMERA)
	CREATE_CLASS(3,DECK_CAMERA)
API_MODULE_END
*/

#endif