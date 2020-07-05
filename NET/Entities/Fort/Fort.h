#ifndef NETFORT_HPP
#define NETFORT_HPP

#include "..\..\common.h"
#include "..\Cannons\Cannon.h"
#include "..\Ship\ShipLights.h"

class NetFort : public VAI_OBJBASE
{
public:
	NetFort();
	~NetFort();

	void	Execute(dword dwDeltaTime);
	void	Realize(dword dwDeltaTime);
	dword _cdecl ProcessMessage(MESSAGE & message);

	void SelectCannonType(dword dwCType);

	virtual ATTRIBUTES * GetACharacter() { return AttributesPointer; };

	virtual float Trace(const CVECTOR & vSrc, const CVECTOR & vDst);
	virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) { return false; };

	virtual const char *GetCollideMaterialName() { return null; };
	virtual bool GetCollideTriangle(TRIANGLE &triangle) { return false; };

	virtual float Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst);

	// VAI_OBJBASE
	virtual CMatrix *	GetMatrix() { return GetModel() ? &GetModel()->GetNode(0)->glob_mtx : null; };
	virtual MODEL *		GetModel() { return (MODEL*)eidModel.pointer; };
	virtual ENTITY_ID	GetModelEID() { return eidModel; };

	virtual void Save(CSaveLoad * pSL) {};
	virtual void Load(CSaveLoad * pSL) {};	

	virtual void Fire(const CVECTOR & vPos) {};

	dword		GetNumCannons() { return dword(iTotalCannons); };
	NetCannon	* GetCannon(dword dwCannonIndex);

private:
	dword				dwCannonType, dwCulverinType, dwMortarType;
	long				iTotalCannons, iDamagedCannons;
	float				fSpeedV0;
	float				fMaxCannonDamageDistance;
	NetShipLights		* pShipsLights;
	ENTITY_ID			eidModel;
	DTimer				dtFiredTimer;
	VIDWALKER			* pVWShips;
	bool				bInitCannons, bInitCulverins, bInitMortars;
	dword				dwFriendlyTeam;

	array<NetCannon>	aCannons;			// fort cannons container
	array<NetCannon>	aCulverins;			// fort culverins container
	array<NetCannon>	aMortars;			// fort mortars container

	bool	AddFort(ATTRIBUTES * pIslandAP, ATTRIBUTES * pFortLabelAP, ENTITY_ID eidModel, ENTITY_ID eidBlot);
	bool	ScanFortForCannons(char * pModelsDir, char * pLocatorsName);
	void	TryFire(NetCannon * pCannon, float fSpeedV0);
	void	CheckCannonForBoom(NetCannon * pC, long iBallOwner, dword dwCType, const CVECTOR & vPnt);
};

#endif