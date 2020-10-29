#ifndef _BI_ENEMY_STAT_SHOWER_H_
#define _BI_ENEMY_STAT_SHOWER_H_

#include "animation.h"
#include "common_defines.h"
#include "dx8render.h"
#include "templates\string.h"

class MODEL;
class NODE;

class EnemyStatShower : public ENTITY
{
  public:
    EnemyStatShower();
    ~EnemyStatShower();

    bool Init();
    void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE &message);

  protected:
    bool ReadAndCreate();
    void SetBeginData();
    void Release();

    ENTITY_ID GetModelEIDFromCharacterEID(ENTITY_ID &chrEID);

  protected: // data
    bool m_bVisible;
    ENTITY_ID m_eidChrEID;
    MODEL *m_pModel;
};

#endif
