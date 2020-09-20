#include "shipcommand.h"
#include "..\image\image.h"
#include "shipsign.h"

BIShipCommand::BIShipCommand(BIShipSign *pSign) : m_apComImage(_FL)
{
    m_pShipSign = pSign;
}

BIShipCommand::~BIShipCommand()
{
    Release();
}

void BIShipCommand::Release()
{
    m_apComImage.DelAllWithPointers();
}
