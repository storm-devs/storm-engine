//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the main game menu
//
// $NoKeywords: $
//=============================================================================

#ifndef MAINMENU_H
#define MAINMENU_H

#include "BaseMenu.h"
#include "GameEngine.h"
#include "SpaceWar.h"
#include "SpaceWarClient.h"
#include <string>
#include <vector>

class CMainMenu : public CBaseMenu<EClientGameState>
{
  public:
    // Constructor
    CMainMenu(IGameEngine *pGameEngine);

    void SetupMenu();

  private:
    STEAM_CALLBACK(CMainMenu, OnParentalSettingsChanged, SteamParentalSettingsChanged_t);
};

#endif // MAINMENU_H