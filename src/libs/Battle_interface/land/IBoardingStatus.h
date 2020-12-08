#ifndef _IBOARDINGSTATUS_H_
#define _IBOARDINGSTATUS_H_

#include "..\bi_defines.h"
#include "common_defines.h"
#include "dx8render.h"

class IBoardingStatus : public ENTITY
{
    VDX8RENDER *rs;

  public:
    IBoardingStatus();
    ~IBoardingStatus();
    bool Init();
    void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE &message);

  protected:
    void Create();
    void SetCharactersHP(float myHP, float enemyHP);

  protected:
    // show parameters
    //-----------------------
    long m_Width;
    long m_Height;
    FPOINT m_myPos;
    FPOINT m_enemyPos;
    DWORD m_myColor;
    DWORD m_enemyColor;

    BI_COLORONLY_VERTEX m_MyChar[4];
    BI_COLORONLY_VERTEX m_EnemyChar[4];
};

#endif
