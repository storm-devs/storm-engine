//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the pause game menu
//
// $NoKeywords: $
//=============================================================================

#ifndef QUITMENU_H
#define QUITMENU_H

#include "BaseMenu.h"
#include "GameEngine.h"
#include "SpaceWar.h"
#include "SpaceWarClient.h"
#include <string>
#include <vector>

class CQuitMenu : public CBaseMenu<EClientGameState>
{
  public:
    // Constructor
    CQuitMenu(IGameEngine *pGameEngine);
};

#endif // QUITMENU_H