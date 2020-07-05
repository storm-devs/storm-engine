#ifndef POINT_EMITTER_H
#define POINT_EMITTER_H

#include "base.h"


class PointEmitter : public BaseEmitter
{

protected:

 
//Получить позицию для рождения новых партиклов  
  virtual Vector GetNewParticlePosition (float DeltaTime);

public:
 
	// Конструктор / деструктор
  PointEmitter(ParticleSystem* pSystem);
  virtual ~PointEmitter();
  
  


};

#endif