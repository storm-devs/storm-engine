#ifndef SEA_COMMON_CAMERA_HPP
#define SEA_COMMON_CAMERA_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\defines.h"
#include "..\common_h\templates.h"
#include "..\common_h\sd2_h\VAI_ObjBase.h"
#include "..\common_h\sd2_h\SaveLoad.h"

class COMMON_CAMERA : public ENTITY
{
private:
	bool			bActive;
	bool			bOn;

	float			fPerspective;

	ENTITY_ID		eidObject;
	VAI_OBJBASE		* pAIObj;

protected:
	ATTRIBUTES		* pACharacter;

public:
	bool FindShip()
	{
		Assert(pACharacter);
		ENTITY_ID eidTemp;
		// get entity id from loaded ships
		if (api->FindClass(&eidTemp,"ship",0)) do
		{
			VAI_OBJBASE * pObj = (VAI_OBJBASE*)api->GetEntityPointer(&eidTemp);
			if (pObj->GetACharacter() == pACharacter)
			{
				SetEID(&pObj->GetModelEID());
				SetAIObj(pObj);
				return true;
			}
		} while (api->FindClassNext(&eidTemp));
		return false;
	}

	MODEL *GetModelPointer()
	{
		Assert(_CORE_API->ValidateEntity(&eidObject));
		Assert(eidObject.pointer);
		return (MODEL*)eidObject.pointer;
	}
	void		SetAIObj(VAI_OBJBASE * _pAIObj) { pAIObj = _pAIObj; }
	VAI_OBJBASE * GetAIObj() { return pAIObj; }
	void		SetEID(ENTITY_ID *pEID) { eidObject = *pEID; };
	ENTITY_ID	GetEID() { return eidObject; };
	
	virtual void SetCharacter(ATTRIBUTES *_pACharacter) { pACharacter = _pACharacter; };

	void	SetPerspective(float _fPerspective) { fPerspective = _fPerspective; };
	float	GetPerspective() { return fPerspective; };

	void SetOn(bool bOnOff) { bOn = bOnOff; };
	void SetActive(bool bNewActive) { bActive = bNewActive; };

	bool isOn() { return bOn; };
	bool isActive() { return bActive; };

	virtual void Save(CSaveLoad * pSL) = 0;
	virtual void Load(CSaveLoad * pSL) = 0;

	COMMON_CAMERA() { bOn = false; bActive = false; fPerspective = 1.285f; };
	virtual ~COMMON_CAMERA() {};
};

#endif