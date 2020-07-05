#ifndef AI_SEA_GOODS_HPP
#define AI_SEA_GOODS_HPP

#include "AIHelper.h"
#include "..\common_h\geometry.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\ship_base.h"

class AISeaGoods : public ENTITY
{
private:
	struct item_t
	{
		char		sGoodName[48];
		long		iCharIndex, iQuantity;
		float		fTime;
		CVECTOR		vPos, vNormal;
	};

	struct goods_t
	{
		string			sModel;
		array<item_t>	aItems;
		GEOS			* pGeo;

		goods_t() : aItems(_FL_, 16) {};
	};

	array<goods_t*>		aGoods;
	array<SHIP_BASE*>	aShips;
	
	VGEOMETRY		* pGeoService;
	SEA_BASE		* pSea;
	item_t			TmpItem;
	string			sModelPath, sTmpModel;
	DTimer			dtCheckShips;
	bool			bDeleteGoodAnyway;
	float			fDistanceMultiply;	

public:
	AISeaGoods();
	~AISeaGoods();

	bool	Init();
	void	SetDevice();

	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);

	dword	AttributeChanged(ATTRIBUTES * pAttributeChanged);
};

#endif