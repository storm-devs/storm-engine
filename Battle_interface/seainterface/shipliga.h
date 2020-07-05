#ifndef BI_SHIPLIGA_H
#define BI_SHIPLIGA_H

#include "defines.h"
#include "..\image\defines.h"

class BISeaInterface;
class BIShipSign;

class BIShipLiga
{
public:
	BIShipLiga( BISeaInterface* pSeaI );
	~BIShipLiga();

protected:
	void Release();

	BISeaInterface* m_pSeaInterface;
	array<BIShipSign*> m_apSign;

	bool m_bDeleteEverythink;
};

#endif
