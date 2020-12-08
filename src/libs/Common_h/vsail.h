#ifndef SAIL_BASE_H
#define SAIL_BASE_H
#include "object.h"

class SAIL_BASE : public COLLISION_OBJECT
{
  public:
    virtual ENTITY_ID GetShipID() = 0;
};

#endif