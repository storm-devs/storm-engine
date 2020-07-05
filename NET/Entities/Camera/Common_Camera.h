#ifndef SEA_NetCommonCamera_HPP
#define SEA_NetCommonCamera_HPP

#include "..\..\common.h"
#include "..\Ship\Ship.h"

class NetCommonCamera : public ENTITY
{
private:
	bool			bActive;
	bool			bOn;
	float			fPerspective;

	ENTITY_ID		eidObject;
	word			wNetID;

public:
	NetShip * GetShip()
	{
		ENTITY_ID eidTemp;
		// get entity id from loaded ships
		if (api->FindClass(&eidTemp, "NetShip", 0)) do
		{
			NetShip * pE = (NetShip*)api->GetEntityPointer(&eidTemp);
			if (pE->IsClient() && pE->GetNetID() == wNetID) return pE;
		} while (api->FindClassNext(&eidTemp));
		return null;
	}

	bool FindShip()
	{
		ENTITY_ID eidTemp;
		// get entity id from loaded ships
		if (api->FindClass(&eidTemp, "NetShip", 0)) do
		{
			NetShip * pE = (NetShip*)api->GetEntityPointer(&eidTemp);
			if (pE->IsClient() && pE->GetNetID() == wNetID)
			{
				SetEID(&((VAI_OBJBASE*)pE)->GetModelEID());
				return true;
			}
		} while (api->FindClassNext(&eidTemp));
		return false;
	}

	MODEL *GetModelPointer()
	{
		Assert(api->ValidateEntity(&eidObject));
		Assert(eidObject.pointer);
		return (MODEL*)eidObject.pointer;
	}
	void		SetEID(ENTITY_ID *pEID) { eidObject = *pEID; };
	ENTITY_ID	GetEID() { return eidObject; };
	
	virtual void SetNetID(word wNetID){ this->wNetID = wNetID; };

	void	SetPerspective(float _fPerspective) { fPerspective = _fPerspective; };
	float	GetPerspective() { return fPerspective; };

	void SetOn(bool bOnOff) { bOn = bOnOff; };
	void SetActive(bool bNewActive) { bActive = bNewActive; };

	bool isOn() { return bOn; };
	bool isActive() { return bActive; };

	NetCommonCamera() { bOn = false; bActive = false; fPerspective = 1.285f; };
	virtual ~NetCommonCamera() {};
};

#endif