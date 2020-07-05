#ifndef BI_SHIPSIGN_H
#define BI_SHIPSIGN_H

#include "defines.h"
#include "..\image\defines.h"

class BIShipLiga;
class BIShipCommand;

class BIShipSign
{
public:
	BIShipSign( BIShipLiga* pLiga );
	~BIShipSign();

protected:
	void Release();

	BIShipLiga* m_pShipLiga;

	BIShipCommand* m_apCommand;
};

#endif
