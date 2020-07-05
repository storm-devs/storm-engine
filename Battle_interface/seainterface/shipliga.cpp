#include "shipliga.h"
#include "shipsign.h"

BIShipLiga::BIShipLiga( BISeaInterface* pSeaI ) :
	m_apSign(_FL)
{
	m_pSeaInterface = pSeaI;
	m_bDeleteEverythink = false;
}

BIShipLiga::~BIShipLiga()
{
	Release();
}

void BIShipLiga::Release()
{
	m_bDeleteEverythink = true;
	m_apSign.DelAllWithPointers();
}
