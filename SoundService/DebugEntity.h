#ifndef _SOUND_VISUALISATION_H_
#define _SOUND_VISUALISATION_H_


#include "..\common_h\dx8render.h"
#include "..\common_h\entity.h"

class SoundService;

class SoundVisualisationEntity : public ENTITY
{
	SoundService* pSound;

public:

  SoundVisualisationEntity();
  virtual ~SoundVisualisationEntity();

	virtual bool Init();
	virtual void Realize(dword _dTime);
  virtual void Execute(dword _dTime);


	void SetMasterSoundService (SoundService* pSoundService);
	void Wakeup();
  
};


#endif