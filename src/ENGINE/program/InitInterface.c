//#include "global.h"
#include "globals.c"
//#include "key_codes.h"
#include "store\goods.h"
#include "store\store.h"
//#include "character\characters.h"
//#include "player\player.h"
#include "interface\interface.h"
#include "interface\interface.c"

extern void InitGoods();
extern void InitStores();
extern void InitBaseCannons();
extern void InitBaseShipsTypes();
extern void InitCharacters();
extern void InitPlayer();
extern void InitBaseInterfaces();

int bUseMain;
//object Screen;

void InterfaceMain()
{
    LoadSegment("init\initGoods.c");
    InitGoods();
    UnloadSegment("init\initGoods.c");

    LoadSegment("init\initStore.c");
    InitStores();
    UnloadSegment("init\initStore.c");

    LoadSegment("init\initBaseShips.c");
    InitBaseShipsTypes();
    UnloadSegment("init\initBaseShips.c");

    LoadSegment("init\initCharacters.c");
    InitCharacters();
    UnloadSegment("init\initCharacters.c");

    LoadSegment("init\initPlayer.c");
    InitPlayer();
    UnloadSegment("init\initPlayer.c");

    LoadSegment("init\BaseInterface.c");
    InitBaseInterfaces();
    UnloadSegment("init\BaseInterface.c");

//    ScreenMake();
//    bUseMain = TRUE;
//    SetEventHandler("key_event","ProcessKeys",1);
}

/*
void InterfaceDoExit()
{
//    UnloadSegment("interface\interface.c");
    bUseMain = 3;
}
*/