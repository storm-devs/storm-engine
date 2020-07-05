#ifndef BI_SEADATABASE_H
#define BI_SEADATABASE_H

#include "defines.h"
#include "..\image\defines.h"

class BISeaDatabase
{
public:
	struct ShipDescr
	{
		long nCharacter;
		long nShipType;
		long nShipImage;
		bool bShowCommand;
	};
	struct LigaDescr
	{
		array<ShipDescr> aShip;
		LigaDescr() : aShip(_FL) {}
	};

	struct ImgDescr
	{
		FRECT uv;
		long nImgGroup;
	};
	struct ImgGroupDescr
	{
		string sFile;
	};

public:
	BISeaDatabase();
	~BISeaDatabase();

	void Initialization();

protected:
	void Release();

	LigaDescr m_Liga;

	array<ImgGroupDescr> m_aImgGroup;
	array<ImgDescr> m_aImage;
};

#endif
