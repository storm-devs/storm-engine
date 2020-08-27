#ifndef BI_SHIPCOMMAND_H
#define BI_SHIPCOMMAND_H

#include "..\image\defines.h"
#include "defines.h"

class BIShipSign;
class BIImage;

class BIShipCommand
{
  public:
    BIShipCommand(BIShipSign *pSign);
    ~BIShipCommand();

  protected:
    void Release();

    BIShipSign *m_pShipSign;
    array<BIImage *> m_apComImage;
};

#endif
