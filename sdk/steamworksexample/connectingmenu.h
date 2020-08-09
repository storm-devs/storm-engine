//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the main game menu
//
// $NoKeywords: $
//=============================================================================

#ifndef CONNECTINGMENU_H
#define CONNECTINGMENU_H

#include "BaseMenu.h"
#include "GameEngine.h"
#include "SpaceWar.h"
#include "SpaceWarClient.h"
#include <string>
#include <vector>

class CConnectingMenu : public CBaseMenu<EClientGameState>
{
  public:
    // Constructor
    CConnectingMenu(IGameEngine *pGameEngine);

    void Reset();
    void OnConnectFailure();

  private:
    bool m_bInitialLogonFailed;
};

#endif // CONNECTINGMENU_H