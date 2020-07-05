#include "point.h"

// Конструктор / деструктор
PointEmitter::PointEmitter(ParticleSystem* pSystem) : BaseEmitter(pSystem)
{
}

PointEmitter::~PointEmitter()
{
}

 
//Получить позицию для рождения новых партиклов  
Vector PointEmitter::GetNewParticlePosition (float DeltaTime)
{
	//TODO : Надо рождать на начальной позиции трэка...
	return Vector (0.0f);
}

 
  
  
