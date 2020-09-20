#include "interface\messages.h"
#include "interface\utils\interface.c"
#include "interface\utils\MessageBox.c"
#include "interface\utils\IconSelector.c"
#include "interface\utilite.c"
#include "interface\interface_utils.c"
#include "interface\perks\perks.c"

#define FONT_NORMAL	"interface_normal"
#define FONT_CAPTION	"interface_button"
#define FONT_BOLD_NUMBERS	"bold_numbers"

#define COLOR_DESELECT	4286611584
#define COLOR_NORMAL	4294967295
#define COLOR_MONEY		4292915296
#define COLOR_RED		4294901760

#define BLIND_MIN	4282400832 //argb(255,96,96,96)
#define BLIND_MAX	4286611584 //argb(255,128,128,128)

#define MONEY_SIGN		""
#define MONEY_DELIVER	" "

#define MAX_CHARACTER_SKILL		10 // to_do del

#define DISEASE_ON_SHIP	0
#define DISEASE_ON_COLONY	1



//extern string FindRussianDaysString(int idays); 
//extern string FindRussianMoneyString(int imoney);


#event_handler("LaunchIAfterFrame","ILaunchAfterFrame");
#event_handler("ievent_GameOver","IProcEventGameOver");
#event_handler("ievent_SetGameTime","IProcEventSetGameTime");
#event_handler("evntPreLoad","procPreLoad");

#event_handler("FaderEvent_StartFade","ProcBreakInterface");
#event_handler("FaderEvent_StartFadeIn","ProcBreakInterface");
#event_handler("FaderEvent_EndFade","ProcBreakInterface");
#event_handler("FaderEvent_EndFadeIn","ProcBreakInterface");

#event_handler("Fader_GetTipsPath","procGetTipsPath");
#event_handler("Fader_GetFaderPicture","procGetFaderPicture");

#event_handler("evntOptionsBreak","procOptionsBreak");

#event_handler("evMouseWeel","procMouseWeel");
#event_handler("evGetMouseWeel","procGetMouseWeel");

bool EnableVideoBreak = false;

int CurrentInterface;
int gStoreNum;

extern void InitInterface(string iniFile);
extern void InitInterface_R(string iniFile, ref rParam);
extern void InitInterface_RR(string iniFile, ref rParam1, ref rParam2);
extern void InitInterface_S(string iniFile, string sParam1);
extern void InitInterface_GM(string iniFile);
extern void InitInterface_RS(string iniFile, ref rParam1, string sParam2);
extern void InitInterface_RI(string iniFile, ref rParam1, int iParam2);
extern void InitInterface_I(string iniFile, int iParam1);
extern void InitInterface_B(string iniFile, bool bParam1);
extern void InitInterface_BB(string iniFile, bool bParam1, bool bParam2);
extern void InitInterface_RRS(string iniFile, ref rParam1, ref rParam2, string sParam3);
extern void InitInterface_RIS(string iniFile, ref rParam1, int iParam2, string sParam3);


extern ref GetMyCharacterRef();
extern ref GetEnemyCharacterRef();
string ICurNode;
int	   nPrevInterface;

bool bGamePadChangeEnable = false;
bool bPlayVideoNow = false;
bool bMainMenuLaunchAfterVideo = false;

// boal -->
void LaunchAboutScreen()
{	
	SetEventHandler(EVENT_END_VIDEO,"LaunchMainMenu_afterVideo",0);
	bMainMenuLaunchAfterVideo = true;
	StartPostVideo("credits",1);	
	/*if(procInterfacePrepare(INTERFACE_ABOUT))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ABOUT;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}*/
}

void LaunchNationLegend()
{
	if(procInterfacePrepare(INTERFACE_NATION_LEGEND))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NATION_LEGEND;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchPsHeroScreen()
{
	if(procInterfacePrepare(INTERFACE_PS_HERO))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PS_HERO;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchLeaveBattleScreen()
{
	if(procInterfacePrepare(INTERFACE_LEAVE_BATTLE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_LEAVE_BATTLE;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
// boal 15.01.06 -->
void LaunchFrameForm()
{
	if(procInterfacePrepare(INTERFACE_FRAMEFORM))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_FRAMEFORM;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
// boal 15.01.06 <--

// boal 13.05.05 -->
void LaunchCardsGame()
{
	if(procInterfacePrepare(INTERFACE_CARDSGAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CARDSGAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
// boal 13.05.05 <--

// boal 10.06.05 -->
void LaunchDiceGame()
{
	if(procInterfacePrepare(INTERFACE_DICE_GAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_DICE_GAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
// boal 10.06.05 <--
void LaunchDebuderMenu()
{
	if(procInterfacePrepare(INTERFACE_DEBUGER))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_DEBUGER;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchBoalBetaScreen()
{
	if(procInterfacePrepare(INTERFACE_BOAL_BETA))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_BOAL_BETA;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
// boal <--

/*
void LaunchEndGameMenu()
{
	if(procInterfacePrepare(INTERFACE_ENDGAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ENDGAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
*/
void LaunchQuickSaveMenu()
{
	if(procInterfacePrepare(INTERFACE_QUICK_SAVE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_QUICK_SAVE;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void HideQuickSaveMenu()
{
	/*DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("evntDoPostExit","DoPostExit");
	*/

	interfaceResultCommand = RC_INTERFACE_QUICK_SAVE;
	EndCancelInterface(true);
	//SetTimeScale(1.0);
}

void LaunchSalaryScreen(string _tmp)
{
	if(procInterfacePrepare(INTERFACE_SALARY))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SALARY;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}


void LaunchNetMenu()
{
	if(procInterfacePrepare(INTERFACE_NET_NETMENU))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_NETMENU;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetFindGame()
{
	if(procInterfacePrepare(INTERFACE_NET_FINDGAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_FINDGAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetBuyScreen()
{
	if(procInterfacePrepare(INTERFACE_NET_BUY))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_BUY;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetGameStateScreen()
{
	if(procInterfacePrepare(INTERFACE_NET_GAMESTATE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_GAMESTATE;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetPlayerSettings()
{
	if(procInterfacePrepare(INTERFACE_NET_PLAYERSETTINGS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_PLAYERSETTINGS;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetChatString()
{
	if(procInterfacePrepare(INTERFACE_NET_CHATSTRING))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_CHATSTRING;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetEndGameScreen()
{
	if(procInterfacePrepare(INTERFACE_NET_ENDGAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_ENDGAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetTopListScreen()
{
	if(procInterfacePrepare(INTERFACE_NET_TOPLIST))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_TOPLIST;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNetCreateGame()
{
	if(procInterfacePrepare(INTERFACE_NET_CREATEGAME))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NET_CREATEGAME;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
void LaunchMapScreen()
{
	if(procInterfacePrepare(INTERFACE_MAP))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_MAP;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchPaperMapScreen()
{
	if(procInterfacePrepare(INTERFACE_PAPER_MAP))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PAPER_MAP;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchColonyInfoScreen()
{
	if (procInterfacePrepare(INTERFACE_COLONY_INFO))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_COLONY_INFO;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

/*
void LaunchPerksScreen(ref chref)
{
	if(procInterfacePrepare(INTERFACE_PERKS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PERKS;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,chref);
	}
}
*/


void LaunchForTest()
{
	if(procInterfacePrepare(INTERFACE_FOR_TEST))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_FOR_TEST;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}


void LaunchItemsTrade(ref chref)
{
	if(procInterfacePrepare(INTERFACE_ITEMSTRADE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSTRADE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,chref);
	}
}
// boal -->

void LaunchColonyCapture(ref city)
{
	if (procInterfacePrepare(INTERFACE_COLONY_CAPTURE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_COLONY_CAPTURE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, city);
	}
}

void LaunchSelectMenu()
{
	if (CheckAttribute(&InterfaceStack,"SelectMenu_node"))
	{
  		string sTemp = InterfaceStack.SelectMenu_node;
  		call sTemp();
	}
	else
	{
	    LaunchMainCharacter();
	}
}
// boal <--
void LaunchOptionScreen()
{
	if(procInterfacePrepare(INTERFACE_OPTIONSCREEN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_OPTIONSCREEN;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}


void LaunchGameMenuScreen()
{
	SetEventHandler("makescrshot","LaunchGameMenuContinue",0);
	aref arScrShoter;
	if( !FindClass(&arScrShoter,"scrshoter") ) {
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else {
		Event("makescrshot");
	}
}

void LaunchGameMenuContinue()
{
	DelEventHandler("makescrshot","LaunchGameMenuContinue");
	if(procInterfacePrepare(INTERFACE_GAMEMENU))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_GAMEMENU;
		if(LanguageGetLanguage() != "Russian")
		{
			InitInterface_GM(Interfaces[CurrentInterface].ENG_IniFile);
		}
		else
		{
			InitInterface_GM(Interfaces[CurrentInterface].IniFile);
		}
		//InitInterface_GM(Interfaces[CurrentInterface].IniFile);
	}
}

void CreateScreenShoter()
{
	object scrshoter;
	LayerCreate("realize",1);
	LayerSetRealize("realize",1);
	CreateEntity(&scrshoter,"scrshoter");
	scrshoter.SavePath = "SAVE";
	if(bSeaActive && !bAbordageStarted) {
		LayerAddObject(SEA_REALIZE,&scrshoter,-1);
	} else {
		LayerAddObject("realize",&scrshoter,-1);
	}
}


/*
void LaunchHelpScreen(string help_section)
{
	if(procInterfacePrepare(INTERFACE_HELPSCREEN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_HELPSCREEN;
		InitInterface_S(Interfaces[CurrentInterface].IniFile,help_section);
	}
}
*/

void LaunchCharacterItemChange(ref chref)
{
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		aref charef; makearef(charef,chref);
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,&charef,chref.FaceID);
	}
}

void LaunchItemsBox(ref boxRef)
{
	if(procInterfacePrepare(INTERFACE_ITEMSBOX))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMSBOX;
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,boxRef,"");
	}
}

void LaunchFortCapture(ref chref)
{
	if(procInterfacePrepare(INTERFACE_FORTCAPTURE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_FORTCAPTURE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,chref);
	}
}

void LaunchShipState()
{
	if(procInterfacePrepare(INTERFACE_SHIP_CHOOSE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIP_CHOOSE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, pchar);
	}
}

void LaunchShipStateNPC(ref _chr)
{
	if(procInterfacePrepare(INTERFACE_SHIP_CHOOSE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIP_CHOOSE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, _chr);
	}
}

void LaunchStore(int storeNum)
{
  if(storeNum<0)	return;
  if(storeNum>STORE_QUANTITY-1)	return;
	gStoreNum=storeNum;
	if(procInterfacePrepare(INTERFACE_STORE))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_STORE;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,&stores[storeNum]);
	}
}

void LaunchShipyard(ref shipmaster)
{
	if(procInterfacePrepare(INTERFACE_SHIPYARD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPYARD;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,shipmaster);
	}
}

void LaunchDiseaseAlert(int iMode)
{
	if(procInterfacePrepare(INTERFACE_DISEASE_ALERT))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_DISEASE_ALERT;
		InitInterface_I(Interfaces[CurrentInterface].IniFile, iMode);
	}
}


void LaunchCannons(int storeNum) // boal 31.08.06
{
    if(storeNum<0)	return;
  	if(storeNum>STORE_QUANTITY-1)	return;
	if(procInterfacePrepare(INTERFACE_CANNONS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CANNONS;
		InitInterface_R(Interfaces[CurrentInterface].IniFile, &stores[storeNum]);
	}
}

void LaunchCharacter(ref refCharacter)
{
	if(procInterfacePrepare(INTERFACE_CHARACTER))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CHARACTER;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,refCharacter);
	}
}

void LaunchMainCharacter()
{
 	if(procInterfacePrepare(INTERFACE_CHARACTER_ALL))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_CHARACTER_ALL;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchMainMenu_afterVideo()
{
	DelEventHandler(EVENT_END_VIDEO,"LaunchMainMenu_afterVideo");
	InterfaceStates.LaunchAnyway = true;
	InterfaceStates.Launched = false;
	ResetSound();   // fix
	LaunchMainMenu();
}

void LaunchMainMenu()
{
	LayerCreate("realize",1);
	LayerSetRealize("realize",1);

	if(procEnableInterfaceLaunch(INTERFACE_MAINMENU)==false)
	{
		if( CheckAttribute(&InterfaceStates,"LaunchAnyway") && sti(InterfaceStates.LaunchAnyway)==true )
		{
			PostEvent("LaunchIAfterFrame",1,"sl","I_MAINMENU",1);
			InterfaceStates.LaunchAfterFrame = "I_MAINMENU";
		}
		return;
	}
	DeleteAttribute(&InterfaceStates,"LaunchAnyway");
	if(g_ibVideoExecuting) return;
	InterfaceStates.Launched = true;
	InterfaceStates.doUnFreeze = false;

	ISetSoundEvents();
	nPrevInterface = -1;
	CurrentInterface = INTERFACE_MAINMENU;
	EngineLayersOffOn(false);

	bGamePadChangeEnable = true;
	LoadSegment(Interfaces[CurrentInterface].SectionName);
	InitInterface(Interfaces[CurrentInterface].IniFile);
	SetTimeScale(1.0);
	// fix
	TimeScaleCounter = 0;
    DelPerkFromActiveList("TimeSpeed");
}

void LaunchSaveGame()
{
	if(procInterfacePrepare(INTERFACE_SAVELOAD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SAVELOAD;
		InitInterface_BB(Interfaces[CurrentInterface].IniFile,true,false);
	}
}

void LaunchLoadGame(bool isMainMenu)
{
	if(procInterfacePrepare(INTERFACE_SAVELOAD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SAVELOAD;
		InitInterface_BB(Interfaces[CurrentInterface].IniFile,false, isMainMenu);
	}
}

/*
void LaunchShipHold()
{
	if(procInterfacePrepare(INTERFACE_SHIPHOLD))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_SHIPHOLD;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}
*/

void LaunchHireCrew()
{
	if(procInterfacePrepare(INTERFACE_HIRECREW))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_HIRECREW;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

/*
void LaunchPassengers(ref _refCharacter)
{
	if(procInterfacePrepare(INTERFACE_PASSENGERS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_PASSENGERS;
		InitInterface_R(Interfaces[CurrentInterface].IniFile,_refCharacter);
	}
}
*/

/*
void LaunchRansackJoin(ref _refMy,ref _refEnemy)
{
	if(procInterfacePrepare(INTERFACE_RANSACK_JOIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_RANSACK_JOIN;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy);
	}
}

void LaunchRansackCrew(ref _refMy,ref _refEnemy)
{
	if(procInterfacePrepare(INTERFACE_RANSACK_CREW))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_RANSACK_CREW;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy);
	}
}

void LaunchRansackCaptain(ref _refMy,ref _refEnemy)
{
	if(procInterfacePrepare(INTERFACE_RANSACK_CAPTAIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_RANSACK_CAPTAIN;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy);
	}
}
*/

void LaunchRansackMain(ref _refMy,ref _refEnemy,string captureState)
{
	if(procInterfacePrepare(INTERFACE_RANSACK_MAIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_RANSACK_MAIN;
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,_refEnemy,captureState); // что характерно, _refMy не при делах вообще :) Забавно, право слово. Ну да, совместимости для.
	}
	// Это по сути вызов LaunchTransferMain(_refMy, _refEnemy, captureState); , но нужна проверка на	INTERFACE_RANSACK_MAIN 
}

void LaunchTransferMain(ref _refMy,ref _refEnemy, string newCurNod)
{
	if(procInterfacePrepare(INTERFACE_TRANSFER_MAIN))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_TRANSFER_MAIN;
		InitInterface_RS(Interfaces[CurrentInterface].IniFile,_refEnemy,newCurNod);
	}
}

/*
void LaunchTransferGoods(ref _refMy,ref _refOther)
{
	if(procInterfacePrepare(INTERFACE_TRANSFER_GOODS))
	{
		nPrevInterface = CurrentInterface;
		CurrentInterface = INTERFACE_TRANSFER_GOODS;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refOther);
	}
}

void LaunchTransferCharacter(ref _refMy,ref _refEnemy)
{
	if(procInterfacePrepare(INTERFACE_TRANSFER_CHARACTER))
	{
		nPrevInterface = CurrentInterface;
		CurrentInterface = INTERFACE_TRANSFER_CHARACTER;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy);
	}
}

void LaunchTransferCrew(ref _refMy,ref _refEnemy)
{
	if(procInterfacePrepare(INTERFACE_TRANSFER_CREW))
	{
		nPrevInterface = CurrentInterface;
		CurrentInterface = INTERFACE_TRANSFER_CREW;
		InitInterface_RR(Interfaces[CurrentInterface].IniFile,_refMy,_refEnemy);
	}
}
*/

void LaunchQuestBook()
{
	if(procInterfacePrepare(INTERFACE_QUESTBOOK))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_QUESTBOOK;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchTradeBook()
{
	if(procInterfacePrepare(INTERFACE_TRADEBOOK))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_TRADEBOOK;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchNationRelation()
{
	if(procInterfacePrepare(INTERFACE_NATIONRELATION))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_NATIONRELATION;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}


void LaunchItems()
{
	if(procInterfacePrepare(INTERFACE_ITEMS))
	{
		nPrevInterface = -1;
		CurrentInterface = INTERFACE_ITEMS;
		InitInterface(Interfaces[CurrentInterface].IniFile);
	}
}

void LaunchSelectCharacter()
{
	//if(procEnableInterfaceLaunch(INTERFACE_CHARACTER_SELECT)==false) return;
	//InterfaceStates.Launched = true;
	//InterfaceStates.doUnFreeze = false;
	//EngineLayersOffOn(false);

	SetEventHandler(EVENT_END_VIDEO,"ISTART_SelCharacter",0);
	PostEvent(EVENT_END_VIDEO,0);
	//StartVideo("3Ship");
}

void LaunchSelectNation()
{
	//if(procEnableInterfaceLaunch(INTERFACE_CHARACTER_SELECT)==false) return;
	//InterfaceStates.Launched = true;
	//InterfaceStates.doUnFreeze = false;
	//EngineLayersOffOn(false);

	SetEventHandler(EVENT_END_VIDEO,"ISTART_SelNation",0);
	PostEvent(EVENT_END_VIDEO,0);
	//StartVideo("3Ship");
}

void ISTART_SelCharacter()
{
	InterfaceStates.Launched = true;
	DelEventHandler(EVENT_END_VIDEO,"ISTART_SelCharacter");
	ISetSoundEvents();
	nPrevInterface = -1;
	CurrentInterface = INTERFACE_CHARACTER_SELECT;
	LoadSegment(Interfaces[CurrentInterface].SectionName);
	InitInterface(Interfaces[CurrentInterface].IniFile);
	//SetMainCharacterIndex(0);
	//interfaceResultCommand = RC_INTERFACE_CHARACTER_SELECT_EXIT;
	//Start_InterfaceDoExit();
}

/*
void ISTART_SelNation()
{
	DelEventHandler(EVENT_END_VIDEO,"ISTART_SelNation");
	ISetSoundEvents();
	nPrevInterface = -1;
     CurrentInterface = INTERFACE_NATION_SELECT;
     LoadSegment(Interfaces[CurrentInterface].SectionName);
     InitInterface(Interfaces[CurrentInterface].IniFile);
	//SetMainCharacterIndex(0);
	//interfaceResultCommand = RC_INTERFACE_CHARACTER_SELECT_EXIT;
	//Start_InterfaceDoExit();
}
*/

void EndOkInterface()
{
    UnloadSegment(Interfaces[CurrentInterface].SectionName);

	if(nPrevInterface == INTERFACE_STORE)
	{
		int cn = GetCompanionIndex(pchar,sti(GameInterface.FourImage.current));
		if(cn!=-1)
		{
			DeleteAttribute(&GameInterface,"");
			CurrentInterface = INTERFACE_STORE;
			LoadSegment(Interfaces[CurrentInterface].SectionName);
			InitInterface_RR(Interfaces[CurrentInterface].IniFile,GetCharacter(cn),&stores[gStoreNum]);  // to_do рудимент?
		}
		else
		{
			IDeleteSoundEvents();
			Start_InterfaceDoExit();
		}
	}
	else
	{
		IDeleteSoundEvents();
	    Start_InterfaceDoExit();
	}
}

void EndCancelInterface(bool bYesRelease)
{
	KillMessageBox();
	KillIconSelector();

	ref refObj1,refObj2;
	bGamePadChangeEnable = false;
	switch(interfaceResultCommand)
	{
		/*
		case RC_INTERFACE_RANSACK_JOIN_EXIT: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
		break;

		case RC_INTERFACE_RANSACK_CREW_EXIT: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
		break;

		case RC_INTERFACE_RANSACK_CAPTAIN_EXIT: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
		break;
		

		case RC_INTERFACE_TRANSFER_START_GOODS: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
			ICurNode = GetCurrentNode(); 
		break;

		case RC_INTERFACE_TRANSFER_START_CHARACTERS: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
			ICurNode = GetCurrentNode(); 
		break;

		case RC_INTERFACE_TRANSFER_START_CREW: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
			ICurNode = GetCurrentNode(); 
		break;

		case RC_INTERFACE_TRANSFER_MAIN_RETURN: 
			refObj1 = GetMyCharacterRef(); 
			refObj2 = GetEnemyCharacterRef(); 
		break;
        */
		/*
		case RC_INTERFACE_CHARACTER_SELECT_EXIT:
			DeleteEntities();
			ClearEvents();
			SetEventHandler("frame","NewGame",1);
			InterfaceStates.doUnFreeze = false;
		break;
		*/
	}
	InterfaceStates.doUnFreeze = bYesRelease;
   UnloadSegment(Interfaces[CurrentInterface].SectionName);
	IDeleteSoundEvents();
	switch(interfaceResultCommand)
	{
		/*
		case RC_INTERFACE_RANSACK_JOIN_EXIT: 
			InterfaceStates.Launched=false; 
			LaunchRansackCrew(refObj1,refObj2); 
			return; 
		break;
		
		case RC_INTERFACE_RANSACK_CREW_EXIT: 
			if (!CheckAttribute(refObj2, "cannotsurrender"))
			{
				InterfaceStates.Launched=false; 
				LaunchRansackCaptain(refObj1,refObj2); 
				return; 
			}
			else
			{
				InterfaceStates.Launched=false; 
				LaunchRansackMain(refObj1,refObj2,""); 
				return;
			}
		break;

		case RC_INTERFACE_RANSACK_CAPTAIN_EXIT: 
			InterfaceStates.Launched=false; 
			LaunchRansackMain(refObj1,refObj2,""); 
			return; 
		break;
		*/
		
		/*
		case RC_INTERFACE_TRANSFER_START_GOODS: 
			InterfaceStates.Launched=false; 
			LaunchTransferGoods(&Characters[sti(refObj1.index)],&Characters[sti(refObj2.index)]); 
			return; 
		break;

		case RC_INTERFACE_TRANSFER_START_CHARACTERS: 
			InterfaceStates.Launched=false; 
			LaunchTransferCharacter(refObj1,refObj2); 
			return; 
		break;

		case RC_INTERFACE_TRANSFER_START_CREW: 
			InterfaceStates.Launched=false; 
			LaunchTransferCrew(refObj1,refObj2); 
			return; 
		break;
		
		case RC_INTERFACE_TRANSFER_MAIN_RETURN:
			if(nPrevInterface==INTERFACE_RANSACK_MAIN)
			{
				InterfaceStates.Launched=false; LaunchRansackMain(refObj1,refObj2,ICurNode);
			}
			else
			{
				InterfaceStates.Launched=false; LaunchTransferMain(refObj1,refObj2,ICurNode);
			}
			return;
		break;
		*/
	}
	
    Start_InterfaceDoExit();
	if(bYesRelease)	PostEvent("evntQuestsCheck",1);
}

// стек для вложенных интерфейсов
object InterfaceStack;
void ClearInterfaceStack()
{
	DeleteAttribute(InterfaceStack,"");
	InterfaceStack.size=0;
}
void PushInterface(int iIndex,string iTitle,string iStartNod)
{
	if(!CheckAttribute(&InterfaceStack,"size"))	InterfaceStack.size = 0;
	string StackSlot = "Slot"+InterfaceStack.size;
	InterfaceStack.(StackSlot).index = iIndex;
	InterfaceStack.(StackSlot).title = iTitle;
	InterfaceStack.(StackSlot).start = iStartNod;
	InterfaceStack.size = sti(InterfaceStack.size)+1;
}
bool PopInterface()
{
	if(!CheckAttribute(&InterfaceStack,"size"))	InterfaceStack.size = 0;
	if(sti(InterfaceStack.size)<1) return false;
	InterfaceStack.size = sti(InterfaceStack.size)-1;
	string StackSlot = "Slot"+InterfaceStack.size;
	CurrentInterface = sti(InterfaceStack.(StackSlot).index);
	/*switch(CurrentInterface)
	{
	case INTERFACE_STORE:
		break;
	case INTERFACE_RANSACK_MAIN:
		break;
	case INTERFACE_TRANSFER_MAIN:
		break;
	} */
	DeleteAttribute(&InterfaceStack,StackSlot);
	return true;
}
void I_PushString(string str)
{
	aref arStrStack;
	makearef(arStrStack,InterfaceStack.strings);
	int q = GetAttributesNum(arStrStack);
	string stackSlotName = "s"+q;
	InterfaceStack.strings.(stackSlotName) = str;
}
string I_PopString()
{
	aref arStrStack;
	makearef(arStrStack,InterfaceStack.strings);
	int q = GetAttributesNum(arStrStack)-1;
	if(q<0) return "";
	string stackSlotName = "s"+q;
	string retStr = InterfaceStack.strings.(stackSlotName);
	DeleteAttribute(&InterfaceStack,"strings."+stackSlotName);
	return retStr;
}

void ISetSoundEvents()
{
	SetEventHandler(ISOUND_EVENT,"IDoSoundEvent",0);
}
void IDeleteSoundEvents()
{
	DelEventHandler(ISOUND_EVENT,"IDoSoundEvent");
}
void IDoSoundEvent()
{
	int comCode = GetEventData();
	switch( comCode )
	{
	case 1: PlaySound("interface\ok.wav"); break; // любая клавиша
	case 2: PlaySound("interface\menu_select_01.wav"); break; // выбор элемента
	}
}

#event_handler("evntPostVideo","stPostVideo");
void StartPictureAsVideo( string picname, float time )
{
	InterfaceStates.GameOverPicture = picname;
	PostEvent( "DoInfoShower", 1, "sl", "Game Over Picture", true );
	SetEventHandler("PictureAsVideoBreak","PictureAsVideoBreak",0);
	PostEvent( "PictureAsVideoBreak", makeint(time * 1000) );
}
void PictureAsVideoBreak()
{
    ResetSound(); //fix boal
	DelEventHandler("PictureAsVideoBreak","PictureAsVideoBreak");
	Event( "DoInfoShower", "sl", "Game Over Picture", false );
	Event( EVENT_END_VIDEO );
}

object aviVideoObj;
void StartPostVideo(string vidName,int n)
{
	SetEventHandler("evntPostVideo","stPostVideo",0);
	PostEvent("evntPostVideo",n,"s",vidName);
}
void stPostVideo()
{
	DelEventHandler("evntPostVideo","stPostVideo");
	string sVidName = GetEventData();
	StartVideo(sVidName);
}
void PostVideoAndQuest(string vidName,int n,string sQuestName)
{
	aviVideoObj.afterQuestName = sQuestName;
	StartPostVideo(vidName,n);
}
bool g_ibVideoExecuting = false;
void StartVideo(string vidName)
{
	if(IsEntity(&aviVideoObj))
	{
		DeleteClass(&aviVideoObj);
		StartPostVideo(vidName,1);
	}
	g_ibVideoExecuting = true;
	PauseAllSounds();
 //ResetSoundScheme();
	ResetSound(); // new
	if(sti(InterfaceStates.Launched))
	{
		InterfaceStates.InstantExit = true;
		Event("exitCancel");
		DeleteAttribute(&InterfaceStates,"InstantExit");
	}
	SetEventHandler("ievntEndVideo","_Procedure_EndVideoPlay",0);
	EngineLayersOffOn(false);
	if(bSeaActive && !bAbordageStarted)
	{
		aviVideoObj.layer = "sea";
	}
	else
	{
		aviVideoObj.layer = "land";
	}
	if( GetTargetPlatform()=="pc" )	{CreateEntity(&aviVideoObj,"CAviPlayer");}
	else	{CreateEntity(&aviVideoObj,"WMVideoPlay");}
	SendMessage(&aviVideoObj,"ls",MSG_SET_VIDEO_PLAY,GetVideoFileName(vidName));
	bGamePadChangeEnable = true;
	bPlayVideoNow = true;

	if( vidName=="Artefact" ||
		vidName=="Ending" ||
		vidName=="Invasion" ||
		vidName=="Temple"
		) {bBreakVideoDisable = true;}
	else {bBreakVideoDisable = false;}

	DeleteAttribute(&InterfaceStates,"VideoBreakControls");
	SetEventHandler("Control Activation","IVideoBreakPrepare",0);
	SetEventHandler("Control Deactivation","IVideoBreak",0);
	if(!IsEntity(aviVideoObj))
	{
		Trace("Can`t create video player");
		PostEvent("ievntEndVideo",0);
	}
}
bool bBreakVideoDisable = false;
void IVideoBreakPrepare()
{
	string ControlName = GetEventData();
	InterfaceStates.VideoBreakControls.(ControlName) = 1;
}
void IVideoBreak()
{
	string ControlName = GetEventData();
	if( !CheckAttribute(&InterfaceStates,"VideoBreakControls."+ControlName) ) {return;}
	DeleteAttribute(&InterfaceStates,"VideoBreakControls");

	bool bMakeBreak = false;
	if( ControlName=="ICancel" )	{bMakeBreak = true;}
	if( !bBreakVideoDisable )
	{
		if( ControlName=="IStartButton" || ControlName=="IAllCancel" ||
			ControlName=="IAction" ) {bMakeBreak = true;}
	} else {
		if(!EnableVideoBreak) {bMakeBreak = false;}
	}
	if(bMakeBreak)
	{
		DelEventHandler("Control Activation","IVideoBreakPrepare");
		DelEventHandler("Control Deactivation","IVideoBreak");
		Event("ievntEndVideo");
	}
}
void _Procedure_EndVideoPlay()
{
	g_ibVideoExecuting = false;
	bBreakVideoDisable = false;
	DelEventHandler("Control Activation","IVideoBreakPrepare");
	DelEventHandler("Control Deactivation","IVideoBreak");
	//ResumeAllSounds();
	DeleteClass(&aviVideoObj);
	if(bMainMenuLaunchAfterVideo)
	{
		bMainMenuLaunchAfterVideo = false;
		Event("DoInfoShower","sl","MainMenuLaunch",true);
	} else {InfoShowSetting();}
	bGamePadChangeEnable = false;
	bPlayVideoNow = false;
	DelEventHandler("ievntEndVideo","_Procedure_EndVideoPlay");
	if(aviVideoObj.layer == "land")
	{
		LayerFreeze("realize",false);
		LayerFreeze("execute",false);
	}
	if(aviVideoObj.layer == "sea")
	{
		LayerFreeze(SEA_REALIZE,false);
		LayerFreeze(SEA_EXECUTE,false);
	}
	if(sti(InterfaceStates.Launched)) EngineLayersOffOn(false);
	if(CheckAttribute(&aviVideoObj,"afterQuestName"))
	{
		DoQuestCheckDelay(aviVideoObj.afterQuestName,0.1);
		//QuestComplete(aviVideoObj.afterQuestName);
		//QuestsCheck();
		DeleteAttribute(&aviVideoObj,"afterQuestName");
	}
	PostEvent(EVENT_END_VIDEO,1);
}

int GetCurrentInterface()
{
	return CurrentInterface;
} 

bool procEnableInterfaceLaunch(int _interfaceCode)
{
	if(sti(InterfaceStates.Launched)==true) return false;
	if(IsEntity(reload_fader)) return false;
	if(DialogRun!=0)
	{
		//if(_interfaceCode!=INTERFACE_HELPSCREEN)	return false;
		return false;
	}
	return true;
}

/*
bool bRunHelpChooser = false;
void RunHelpChooser()
{
	string chooserName = "";
	if(bSeaActive && !bAbordageStarted)	chooserName = "sea_interface";
	else if( IsEntity(worldMap) )	chooserName = "world_map";
	else if( DialogRun )	chooserName = "Dialog_window";
	else chooserName = "land_interface";

	if(chooserName=="")
	{
		LaunchHelpScreen("");
		return;
	}

	DeleteEntitiesByType("HELPCHOOSER");
	SetEventHandler("EventEndHelpChooser","ProcEndHelpChooser",0);
	object objHelpChooser;
	CreateEntity(&objHelpChooser,"HELPCHOOSER");
	if( !IsEntity(&objHelpChooser) )
	{
		Event("EventEndHelpChooser","s","");
		return;
	}
	if( !SendMessage(&objHelpChooser,"ls",MSG_HELPCHOOSER_START,chooserName) )
	{
		Event("EventEndHelpChooser","s","");
	}
	bRunHelpChooser = true;
	EngineLayersOffOn(false);
	InterfaceStates.Launched=true;
	LayerAddObject("iExecute",&objHelpChooser,10000);
	LayerAddObject("iRealize",&objHelpChooser,10000);
}
*/

/*
void ProcEndHelpChooser()
{
	string HelpSectionName = GetEventData();
	DelEventHandler("EventEndHelpChooser","ProcEndHelpChooser");
	DeleteEntitiesByType("HELPCHOOSER");
	//bRunHelpChooser = false;
	InterfaceStates.Launched=false;
	if(HelpSectionName!="")	LaunchHelpScreen(HelpSectionName);
	else EngineLayersOffOn(true);
}
*/

void ReturnToMainMenu()
{
	/*if(sti(InterfaceStates.Buttons.Resume.enable)==true)
	{
		if(bSeaActive) return;
		if(IsEntity(&worldMap)) return;
		if(FindLoadedLocation()!=-1) return;
	}*/
	DelEventHandler("frame","InterfaceDoExit");
	EngineLayersOffOn(false);
	InterfaceStates.Launched = false;
	InterfaceStates.MakeScreenShot = false;
	LaunchMainMenu();
}

void ILaunchAfterFrame()
{
	string nodName = GetEventData();
	int n = GetEventData();
	if( CheckAttribute(&InterfaceStates,"LaunchAfterFrame") && InterfaceStates.LaunchAfterFrame!=nodName ) {
		return;
	}
	if(n>0 || sti(InterfaceStates.Launched))
	{
		PostEvent("LaunchIAfterFrame",1,"sl", nodName, n-1);
		return;
	}

	switch(nodName)
	{
		case "I_CHARACTER":     LaunchMainCharacter();  return; break;
		case "I_SHIP":			LaunchShipState();	return; break;
		case "I_QUESTBOOK":		LaunchQuestBook();	return; break;
		case "I_TRADEBOOK":		LaunchTradeBook();	return; break;
		case "I_NATIONS":		LaunchNationRelation();	return; break;
		case "I_ITEMS":			LaunchItems();	return; break;
		// boal 220904 <--
		case "TransferMain":
			if( CheckAttribute(pchar,"TransferChar") )
			{
				LaunchTransferMain(pchar, GetCharacter(sti(pchar.TransferChar)), "Transfer");
				DeleteAttribute(pchar, "TransferChar");
			}
			return;
		break;
	}
}

bool procInterfacePrepare(int interfaceCode)
{
	if(procEnableInterfaceLaunch(interfaceCode)==false) return false;
	if(g_ibVideoExecuting) return false;
	//if(IsEntity(wdm_fader)!=0) return false;

	if(interfaceCode != INTERFACE_FORTCAPTURE && interfaceCode != INTERFACE_RANSACK_MAIN)
	{
		aref arFader;
		if( FindClass(arFader,"fader") ) {return false;}
	}

	if( LoadSegment(Interfaces[interfaceCode].SectionName) )
	{
		Telescope_Off();
		Event("Interface_Started");
		InterfaceStates.Launched = true;
		InterfaceStates.doUnFreeze = false;
		EngineLayersOffOn(false);
		ISetSoundEvents();
		DeleteAttribute(&GameInterface,"");
		DeleteAttribute(&InterfaceStates,"tooltip");
		if(IsPerkIntoList("TimeSpeed")) {SetTimeScale(1.0);}
		LayerFreeze("irealize",false);
		LayerFreeze("iexecute",false);
		return true;
	}
	else
	{
		interfaceResultCommand = RC_INTERFACE_ERROR;
		Start_InterfaceDoExit();
		return false;
	}
}

void Start_InterfaceDoExit()
{
	if(IsPerkIntoList("TimeSpeed")) {SetTimeScale(GetSeaTimeScale());}
	SetEventHandler("frame","InterfaceDoExit",1);
}

extern void OSL_WriteGameOption();
extern void OSL_ReadGameOption();

void LoadGameOptions()
{
	if( LoadSegment("interface\option_sl.c") )
	{
		OSL_ReadGameOption();
		UnloadSegment("interface\option_sl.c");
	}
}

void SaveGameOptions()
{
	if( LoadSegment("interface\option_sl.c") )
	{
		OSL_WriteGameOption();
		UnloadSegment("interface\option_sl.c");
	}
}

void ReloadAfterFortCapture()
{
    // debug
	if (!CheckAttribute(pchar,"from_interface"))
	{
		if (MOD_BETTATESTMODE == "On") Log_Info("Error: ReloadAfterFortCapture miss from_interface.fortCharacterIdx");
		return;
	}
	
	int fortChr = sti(pchar.from_interface.fortCharacterIdx);
	// boal -->
	AfterTownBattle();//трем неоконченый захват

    // перезапомним
    pchar.GenQuestFort.fortCharacterIdx = fortChr;

    bool ExitToSea = CheckAttribute(pchar, "from_interface.Exit_to_Sea"); // выход в море to_do
	// boal <--
	ref chref = GetCharacter(fortChr);

	string sToLocation = "";
	string sToLocator = "";

	aref ar_FindLoc = FindIslandReloadLocator(chref.location,chref.location.locator);
	if(CheckAttribute(ar_FindLoc,"GoAfterBoard.location"))
	{
		ar_FindLoc = FindIslandReloadLocator(chref.location,ar_FindLoc.GoAfterBoard.location);
		if( CheckAttribute(ar_FindLoc,"go") )
		{	sToLocation = ar_FindLoc.go;
		}
		if( CheckAttribute(ar_FindLoc,"emerge") )
		{	sToLocator = ar_FindLoc.emerge;
		}
	}

	//sToLocation = pchar.from_interface.town + "_town";
	//sToLocator = "reload1";

	DeleteAttribute(pchar,"from_interface");

	chref.Fort.Mode = FORT_DEAD;
	// data of frot die
	chref.Fort.DieTime.Year = GetDataYear();
	chref.Fort.DieTime.Month = GetDataMonth();
	chref.Fort.DieTime.Day = GetDataDay();
	chref.Fort.DieTime.Time = GetTime();

    pchar.GenQuest.Hunter2Pause = true;  // boal бойня в форте
	// опыт
	// boal -->
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Leadership", 500);//добавка в городе, если  бой непрерывен
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Sailing", 500);
    AddCharacterExpToSkillSquadron(GetMainCharacter(), "Sneak", 300);

    Statistic_AddValue(GetMainCharacter(), "Fort", 1);
    Statistic_AddValue(GetMainCharacter(), NationShortName(sti(chref.nation)) + "_KillFort", 1);
    fOldMaxSeaHeight = 0.6; // fix потоп в порту ФФ boal 03.01.05
    // boal <--
	if(sToLocation != "")
	{
		string sColony = Locations[FindLocation(sToLocation)].fastreload;
		SetTownCapturedState(sColony,true);
		Log_TestInfo("ReloadAfterFortCapture");
		/*  //to_do boal
		int iColony = FindColony(sColony);
		if(sti(Colonies[iColony].capture_flag) == 1)
		{
			SetTownCapturedState(sColony,false);
			//SetLocationCapturedState(sToLocation,false);
		}
		else
		{
			SetTownCapturedState(sColony,true);
			//SetLocationCapturedState(sToLocation,true);
		}
		*/
		Go2LocationAfterAbordage();
		pchar.location.from_sea = sToLocation;
		// boal -->
		if (true)//!ExitToSea)
	    {
	        DoQuestReloadToLocation(sToLocation, "reload", sToLocator, "Capture_Forts");
	    }
	    else
	    {
	        DoQuestReloadToLocation(sToLocation, "reload", sToLocator, "Capture_Forts_ExitToSea");
	    }
	    // boal <--
	}
	else
	{
	    trace("ReloadAfterFortCapture has no sToLocation");
	    Log_info("ReloadAfterFortCapture has no sToLocation");
	}
}

void IProcEventGameOver()
{
	string str = GetEventData();
	if( sti(InterfaceStates.Launched) )
	{
		InterfaceStates.InstantExit = true;
		Event("exitCancel");
		DeleteAttribute(&InterfaceStates,"InstantExit");
	}
	GameOver(str);
}

void IProcEventSetGameTime()
{
	int itmp = GetEventData();
	InterfaceStates.GameTime.hour = itmp;

	itmp = GetEventData();
	InterfaceStates.GameTime.min = itmp;

	itmp = GetEventData();
	InterfaceStates.GameTime.sec = itmp;
}

//==============================================================
//	Секция Показывающая какую либо инфо на экране
//==============================================================
#event_handler("DoInfoShower", "procInfoShow");
object objInfoList[10];

void procInfoShow()
{
	string sInfoID = GetEventData();
	int nInfoSet = false;
	if( sInfoID=="" ) {
		sInfoID = "MainMenuLaunch";
	} else {
		nInfoSet = GetEventData();
	}

	int nInfoIdx = 0;
	switch(sInfoID)
	{
	case "MainMenuLaunch":		nInfoIdx = 0; break;
	case "save game":			nInfoIdx = 1; break;
	case "Low Storage":			nInfoIdx = 2; break;
	case "game prepare":		nInfoIdx = 3; break;
	case "OptionsBreak":		nInfoIdx = 4; break;
	case "lost controller":		nInfoIdx = 5; break;
	case "Game Over Picture":	nInfoIdx = 6; break;
	}

	int prevQ = 0;
	if( CheckAttribute(&objInfoList[nInfoIdx],"refr") && IsEntity(&objInfoList[nInfoIdx]) ) {
		prevQ = sti(objInfoList[nInfoIdx].refr);
	}

	if( nInfoSet!=0 ) prevQ++;
	else prevQ--;
	if(prevQ<0) prevQ=0;
	objInfoList[nInfoIdx].refr = prevQ;

	if( prevQ>0 )
	{
		if( !IsEntity(&objInfoList[nInfoIdx]) )
		{
			objInfoList[nInfoIdx].backColor = 0;
			objInfoList[nInfoIdx].foreColor = 0;
			objInfoList[nInfoIdx].borderWidth = 32;
			//objInfoList[nInfoIdx].scale = 1.0;
			objInfoList[nInfoIdx].offset = 30;

			switch(sInfoID)
			{
			case "save game":
				objInfoList[nInfoIdx].picfilename = "loading\" + LanguageGetLanguage() + "\save_game.tga";
			break;

			case "lost controller":
				objInfoList[nInfoIdx].picfilename = "loading\" + LanguageGetLanguage() + "\lost_controller.tga";
			break;

			case "Low Storage":
				objInfoList[nInfoIdx].picfilename = "loading\" + LanguageGetLanguage() + "\low_storage.tga";
			break;

			case "game prepare":
			    switch (rand(4))
			    {
			        case 0 :
						objInfoList[nInfoIdx].picfilename = "loading\start_loading.tga";
					break;
					case 1 :
						objInfoList[nInfoIdx].picfilename = "loading\load_rat.tga";
					break;
					case 2 :
						objInfoList[nInfoIdx].picfilename = "loading\battle.tga";
					break;
					case 3 :
						objInfoList[nInfoIdx].picfilename = "loading\shipcannon.tga";
					break;
					case 4 :
						objInfoList[nInfoIdx].picfilename = "loading\tavern_fight.tga";
					break;
				}
			break;

			case "MainMenuLaunch":
				objInfoList[nInfoIdx].picfilename = "interfaces\mainmenu_ss.tga";
			break;

			case "OptionsBreak":
				objInfoList[nInfoIdx].picfilename = "loading\" + LanguageGetLanguage() + "\options.tga";
			break;

			case "Game Over Picture":
				objInfoList[nInfoIdx].picfilename = InterfaceStates.GameOverPicture;
			break;
			}

			CreateEntity(&objInfoList[nInfoIdx],"InfoHandler");

			LayerCreate("inf_realize", 1);
			LayerSetRealize("inf_realize", 1);
		}
	}
	else
	{
		if( IsEntity(&objInfoList[nInfoIdx]) )
		{
			DeleteClass(&objInfoList[nInfoIdx]);
		}
	}

	InfoShowSetting();
}

void InfoShowSetting()
{
	bool bAlreadySet = false;
	bool bMakeSet;
	for(int i=6; i>=0; i--)
	{
		bMakeSet = false;
		if( IsEntity(&objInfoList[i]) )
		{
			if(!bAlreadySet) {
				bMakeSet = true;
				if( i==5 )
				{
					if( g_ibVideoExecuting || !CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") ||
						sti(InterfaceStates.Buttons.Resume.enable)==false )
					{
						bMakeSet = false;
					}
				}
			}
		}

		if( bMakeSet ) {
			LayerAddObject("inf_realize",&objInfoList[i],-1);
			bAlreadySet = true;
		} else {
			LayerDelObject("inf_realize",&objInfoList[i]);
		}

		if(i==2) {
			if(bMakeSet) {
				SetEventHandler("Control Activation","InfoShow_Control",0);
			} else {
				DelEventHandler("Control Activation","InfoShow_Control");
			}
		}
		if(i==4) {
			if(bMakeSet) {
				SetEventHandler("Control Activation","InfoShow_Control2",0);
			} else {
				DelEventHandler("Control Activation","InfoShow_Control2");
			}
		}
		if(i==5) {
			if(bMakeSet) {
				SetTimeScale(0.0);
			} else {
				if(IsPerkIntoList("TimeSpeed")) {
					if( sti(InterfaceStates.Launched) == true ) {
						SetTimeScale(GetSeaTimeScale());
					} else { SetTimeScale(1.0); }
				} else {SetTimeScale(1.0);}
			}
		}
	}

	if(bAlreadySet) {
		LayerFreeze("irealize",true);
		LayerFreeze("iexecute",true);
	} else {
		LayerFreeze("irealize",false);
		LayerFreeze("iexecute",false);
	}
}


void InfoShow_Control()
{
	string ControlName = GetEventData();

	if(ControlName=="IAction" || ControlName=="ICancel")
	{
		PostEvent("DoInfoShower",0,"sl","Low Storage",false);
		if(ControlName=="ICancel")
		{
			SendMessage(&GameInterface,"l",MSG_INTERFACE_LAUNCH_DASHBOARD);
		}
		else Event("evntLowStorageBreak");
		LayerFreeze("irealize",false);
		LayerFreeze("iexecute",false);
	}
}

void InfoShow_Control2()
{
	string ControlName = GetEventData();

	if(ControlName=="IAction" || ControlName=="IStartButton")
	{
		PostEvent("DoInfoShower",0,"sl","OptionsBreak",false);
		Event("evntOptionsBreak");
		LayerFreeze("irealize",false);
		LayerFreeze("iexecute",false);
	}
}

void procAttractExit()
{
	PostEvent("ievntEndVideo",1);
	DelEventHandler("Control Activation","procAttractExit");
}

float GetSeaTimeScale()
{
	return 2.0;
}

void SetShowWindowParameters(bool TVused, int w,int h, int l,int t,int r,int b)
{
	showWindow.TVused = TVused;
	showWindow.width = w;
	showWindow.height = h;
	showWindow.aspectRatio = (h*4.0)/(w*3.0);
	showWindow.left = RecalculateHIcon(l);
	showWindow.top = RecalculateVIcon(t);
	showWindow.right = w - RecalculateHIcon(w-r);
	showWindow.bottom = h - RecalculateVIcon(h-b);
	showWindow.sw = r-l;
	showWindow.sh = b-t;
	showWindow.scale =  makefloat(w) / 1024.0;
}

int RecalculateHIcon(int curHSize)
{
	return curHSize;
}

int RecalculateVIcon(int curVSize)
{
	return makeint(stf(showWindow.aspectRatio)*curVSize);
}

int RecalculateHIconScaled(int curHSize)
{
	return makeint(stf(showWindow.scale)*curHSize);
}

int RecalculateVIconScaled(int curVSize)
{
	return makeint(stf(showWindow.scale)*stf(showWindow.aspectRatio)*curVSize);
}

string GetVideoFileName(string baseName)
{
	if( baseName=="Invasion" ) {
		baseName = LanguageGetLanguage() + "\Invasion";
	}
	if(baseName=="Ending" && LanguageGetLanguage() == "english")
	{
		baseName = "Ending_eng";
	}
	if(baseName=="Credits" && LanguageGetLanguage() == "russian")
	{
		baseName = "Credits_ru";
	}

	if( GetTargetPlatform()=="pc" ) return baseName+".wmv";
	return baseName+".xmv";
}

void DisableMenuLaunch(bool bDisable)
{
	if(bDisable) {
		InterfaceStates.LaunchDisable = true;
	} else {
		DeleteAttribute(&InterfaceStates,"LaunchDisable");
	}
}
bool IsEnableMenuLaunch()
{
	if( CheckAttribute(&InterfaceStates,"LaunchDisable") ) {
		return !sti(InterfaceStates.LaunchDisable);
	}
	return true;
}

void DisableFastTravel(bool bDisable)
{
	if(bDisable) {
		InterfaceStates.DisFastTravel = true;
	} else {
		DeleteAttribute(&InterfaceStates,"DisFastTravel");
	}
}
bool IsEnableFastTravel()
{
	if( CheckAttribute(&InterfaceStates,"DisFastTravel") ) {
		return !sti(InterfaceStates.DisFastTravel);
	}
	return true;
}

void procPreLoad()
{
	DeleteAttribute(&objActivePerkShower,"PerkList");
	SetTimeScale(1.0);
}

void ProcBreakInterface()
{
	if( CheckAttribute(&InterfaceStates,"Launched") &&
		sti(InterfaceStates.Launched)==true )
	{
		if( CurrentInterface == INTERFACE_RANSACK_MAIN || CurrentInterface == INTERFACE_FORTCAPTURE)
			/*
			CurrentInterface==INTERFACE_TRANSFER_GOODS ||
			CurrentInterface==INTERFACE_TRANSFER_CHARACTER ||
			CurrentInterface==INTERFACE_TRANSFER_CREW )
			*/
		{
			return;
		}
	}
	InterfaceStates.InstantExit = true;
	Event("exitCancel");
	DeleteAttribute(&InterfaceStates,"InstantExit");
}

bool g_bOptionsBreak = false;
void procOptionsBreak()
{
	g_bOptionsBreak = true;
}

string g_sTipsPath;
ref procGetTipsPath()
{
	string sLngID = LanguageGetLanguage();
	if(sLngID=="")	g_sTipsPath = "tips";
	else g_sTipsPath = "tips\" + sLngID;
	return &g_sTipsPath;
}

string g_sFaderPic;
ref procGetFaderPicture()
{
	string sPicName = GetEventData();
	g_sFaderPic = LanguageGetFaderPic(sPicName);
	return &g_sFaderPic;
}

void StartLanguageSetting(string lngID)
{
	if(lngID!="") LanguageSetLanguage(lngID);

	string segmentName = "Characters\" + LanguageGetLanguage() + "\Characters_names.c";
	if( LoadSegment(segmentName) )
	{
		InitCharactersNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = "Ships\" + LanguageGetLanguage() + "\ships_name.c";
	if ( LoadSegment(segmentName) )
	{
		InitRandomShipsNames();
		UnloadSegment(segmentName);
	} else {
		Trace("Error! Can`t Load Segment: " + segmentName);
	}

	segmentName = LanguageGetLanguage() + "\Greetings_alias.ini";
	SendMessage( &Sound, "ls", MSG_SOUND_ALIAS_ADD,  segmentName);
	//SendMessage( &Sound, "ls", MSG_SOUND_ALIAS_ADD, LanguageGetLanguage() + "\talk_" + LanguageGetLanguage() + ".lng" );
}

int nWeelStep = 0;
void procMouseWeel()
{
	nWeelStep = GetEventData();
}
ref procGetMouseWeel()
{
	return &nWeelStep;
}

int SetAlphaIntoColor(int col, int alpha)
{
	int newCol = and(col,16777215) + shl(alpha,24);
	return newCol;
}

void SetSkillShow(string skillName, int skillVal)
{
	GameInterface.strings.(skillName) = skillVal;
	int color = COLOR_NORMAL;
	if(skillVal==SKILL_MAX)	color = COLOR_MONEY;
	//ChangeStringColor(skillName,SetAlphaIntoColor(color,GetAlphaFromSkill(skillVal)));
	ChangeStringColor(skillName,SetAlphaIntoColor(color, 55 + skillVal*2)); // 55-255
}

void SetSkillShowEx(aref xi_refCharacter, string skillName, int skillVal, int skillTempVal, int skillUpVal, int iX, int iY)
{
	//GameInterface.strings.(skillName) = skillVal;
	string sReal = skillName+"real";
	//GameInterface.strings.(sReal) = skillTempVal;
	int color = COLOR_NORMAL;
	
	if(skillVal >= MAX_CHARACTER_SKILL) color = COLOR_MONEY;
	int iResult = skillVal;
	
	if (skillTempVal > skillVal)
	{
		if (skillVal > 0)
		{
			iresult = skillVal; 
		}
		else
		{
			iResult = 1;
		}
	}
	
	if (skillUPVal > iResult && pchar.id == xi_refCharacter.id)
	{
		color = argb(255,128,255,128);

		if(skillVal < 1)
		{
			iResult = skillUPVal + skillVal;
		}
		else
		{
			iResult = skillUPVal;// - skillVal;
		}
	}
	
	if (skillTempVal > skillVal)
	{
		color = COLOR_RED;
	}
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		skillName, its(iResult), FONT_BOLD_NUMBERS,iX,iY, SetAlphaIntoColor(color,GetAlphaFromSkill(iResult)),0, 
		SCRIPT_ALIGN_RIGHT, true, 0.7, 420);
	
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		sReal, its(skillTempVal), FONT_BOLD_NUMBERS,(iX+37),iY, SetAlphaIntoColor(COLOR_NORMAL,GetAlphaFromSkill(skillTempVal)),0, 
		SCRIPT_ALIGN_RIGHT, true, 0.7, 420);
}

/*int GetAlphaFromSkill(int nskill)
{
	switch(nskill)
	{
	case 0:		return 75;	break;
	case 1:		return 90;	break;
	case 2:		return 105;	break;
	case 3:		return 120;	break;
	case 4:		return 135;	break;
	case 5:		return 150;	break;
	case 6:		return 165;	break;
	case 7:		return 180;	break;
	case 8:		return 195;	break;
	case 9:		return 210;	break;
	case 10:		return 225;	break;
	case 11:		return 240;	break;
	case 12:		return 255;	break;
	}
	return 0;
}  */

int GetAlphaFromSkill(int nskill)
{
	switch (nskill)
	{
    	case 0:		return 55;	break;
    	case 1:		return 75;	break;
    	case 2:		return 95;	break;
    	case 3:		return 115;	break;
    	case 4:		return 135;	break;
    	case 5:		return 155;	break;
    	case 6:		return 175;	break;
    	case 7:		return 195;	break;
    	case 8:		return 215;	break;
    	case 9:		return 235;	break;
    	case 10:	return 255;	break;
    	case 11:	return 255;	break;
    	case 12:	return 255;	break;
	}
	return 55;
}

//==============================
// save load section
//==============================
void MakeQuickLoad()
{
	// boal лишнее if( InterfaceStates.Launched != 0 ) {return;}
	if( bPlayVideoNow ) {return;}
	string curSave = PlayerProfile.name + " QuickSave";
	SetEventHandler("evntLoad","LoadGame",0);
	PostEvent("evntLoad",0,"s", "SAVE\"+PlayerProfile.name+"\"+curSave);
}

void MakeQuickSave()
{
	if( IsNetActive() ) {return;}
	if( bPlayVideoNow ) {return;}

	aref arTmp;
	if( FindClass(arTmp,"fader") ) {return;}

	if (!CheckSaveGameEnabled()) {return;}

	if( InterfaceStates.Launched != 0 ) {return;}

	// boal 09.07.06 -->
	if (!QuickSaveGameEnabledHardcore())
	{
        Log_Info("Сохранение в данной игре возможно только в церквях.");
		return;
	}
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	DelPerkFromActiveList("TimeSpeed");
	DeleteAttribute(pchar, "pause");
	
	// boal 09.07.06 <--
	aref arScrShoter;
	if( !FindClass(&arScrShoter,"scrshoter") ) {
		SetEventHandler("makescrshot","QuickSaveContinue",0);
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else { return; }
}

void QuickSaveContinue()
{
	DelEventHandler("makescrshot","QuickSaveContinue");

	LaunchQuickSaveMenu();
	string curSave = PlayerProfile.name + " QuickSave";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	string sSaveDescriber = GetSaveDataString();

	SetEventHandler("evntSave","SaveGame",1);
	PostEvent("evntSave",0,"ss", "SAVE\"+PlayerProfile.name+"\"+curSave, sSaveDescriber);
}

void MakeAutoSave()
{
	if( InterfaceStates.Launched != 0 ) {return;}

	aref arScrShoter;
	if( !FindClass(&arScrShoter,"scrshoter") ) {
		SetEventHandler("makescrshot","AutoSaveContinue",0);
		CreateScreenShoter();
		PostEvent("makescrshot",1);
	} else { return; }
}

void AutoSaveContinue()
{
	DelEventHandler("makescrshot","AutoSaveContinue");

	string curSave = PlayerProfile.name + " AutoSave";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	string sSaveDescriber = GetSaveDataString();

	SetEventHandler("evntSave","SaveGame",1);
	PostEvent("evntSave",0,"ss", "SAVE\"+PlayerProfile.name+"\"+curSave, sSaveDescriber);
}

string GetSaveDataString()
{
	string fighter1, fighter2, fighter3, fighter4, navigator, boatswain, cannoner, doctor, treasurer, carpenter;

	// officers pictures
	if( GetOfficersIndex(pchar,0) < 0 ) { fighter1 = "*";
	} else { fighter1 = GetFaceGroupName( GetOfficersIndex(pchar,0) );
	}
	if( GetOfficersIndex(pchar,1) < 0 ) { fighter2 = "*";
	} else { fighter2 = GetFaceGroupName( GetOfficersIndex(pchar,1) );
	}
	if( GetOfficersIndex(pchar,2) < 0 ) { fighter3 = "*";
	} else { fighter3 = GetFaceGroupName( GetOfficersIndex(pchar,2) );
	}
	if( GetOfficersIndex(pchar,3) < 0 ) { fighter4 = "*";
	} else { fighter4 = GetFaceGroupName( GetOfficersIndex(pchar,3) );
	}
	// specials pictures
	if( !CheckAttribute(pchar,"Fellows.Passengers.navigator") || sti(pchar.Fellows.Passengers.navigator)<0 ) { navigator = "*";
	} else { navigator = GetFaceGroupName( sti(pchar.Fellows.Passengers.navigator) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.boatswain") || sti(pchar.Fellows.Passengers.boatswain)<0 ) { boatswain = "*";
	} else { boatswain = GetFaceGroupName( sti(pchar.Fellows.Passengers.boatswain) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.cannoner") || sti(pchar.Fellows.Passengers.cannoner)<0 ) { cannoner = "*";
	} else { cannoner = GetFaceGroupName( sti(pchar.Fellows.Passengers.cannoner) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.doctor") || sti(pchar.Fellows.Passengers.doctor)<0 ) { doctor = "*";
	} else { doctor = GetFaceGroupName( sti(pchar.Fellows.Passengers.doctor) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.treasurer") || sti(pchar.Fellows.Passengers.treasurer)<0 ) { treasurer = "*";
	} else { treasurer = GetFaceGroupName( sti(pchar.Fellows.Passengers.treasurer) );
	}
	if( !CheckAttribute(pchar,"Fellows.Passengers.carpenter") || sti(pchar.Fellows.Passengers.carpenter)<0 ) { carpenter = "*";
	} else { carpenter = GetFaceGroupName( sti(pchar.Fellows.Passengers.carpenter) );
	}

	string savedata = GetCurLocationName() +
		"@" + fighter1 + "," + fighter2 + "," + fighter3 + "," + fighter4 + "," +
		navigator + "," + boatswain + "," + cannoner + "," +
		doctor + "," + treasurer + "," + carpenter +
		"@" + GetStringTime(GetTime()) + "  " + GetStringDate( GetDataDay(),GetDataMonth(),GetDataYear() ) + // boal FIX
		"@" + GetPlayTime() +
		"@" + LanguageGetLanguage();

	return savedata;
}

string GetCurLocationName()
{
	if( nMainCharacterIndex < 0 ) return "";
	if( !CheckAttribute(pchar,"location") ) return "";
	string locLabel = pchar.location;
	int locidx = FindLocation(pchar.location);
	if( locidx>=0 )
	{
		// boal -->
		int nLablesFileID = LanguageOpenFile("LocLables.txt");
        locLabel = "";
        if (CheckAttribute(&locations[locidx], "islandId"))
		{
			locLabel = LanguageConvertString(nLablesFileID, Locations[locidx].islandId) + " ";
		}
		if (CheckAttribute(&locations[locidx], "fastreload"))
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].fastreload + " Town") + " ";
		}
		if (CheckAttribute(&Locations[locidx],"id.label") )
		{
			locLabel += LanguageConvertString(nLablesFileID, Locations[locidx].id.label);
		}
		LanguageCloseFile(nLablesFileID);
	}
	else
	{
		if (worldMap.island != "")
		{
            locLabel =  worldMap.island;
			if (locLabel == "Cuba2") locLabel = "Cuba";
            if (locLabel == "Hispaniola2") locLabel = "Hispaniola";
			locLabel = GetConvertStr(locLabel, "LocLables.txt");
			if (locLabel == "")
      		{
      		    locLabel = GetConvertStr("Mein", "LocLables.txt");
      		}
			locLabel  += " - " + XI_ConvertString("Sea");
		}
		else
		{
			locLabel = XI_ConvertString("Open Sea");
		}
		// boal <--
	}
	return locLabel;
}

string GetPlayTime()
{
	int hours = sti( InterfaceStates.GameTime.hour );
	int days = hours / 24;
	hours = hours - days*24;

	string sPlayTime = "";
	if( days>0 ) {
		sPlayTime = days + "days - ";
	}
	sPlayTime += InterfaceStates.GameTime.hour + " hours " + InterfaceStates.GameTime.min + " min.";
	return sPlayTime;
}

string GetStringTime(float time)
{
	int hour = makeint(time);
	int minute = makeint((time - hour) * 60.0 + 0.01);
	string retVal;
	if(hour<10) retVal = "0"+hour;
	else retVal = its(hour);
	retVal += ":";
	if(minute<10) retVal += "0"+minute;
	else retVal += its(minute);
	return retVal;
}

string GetStringDate(int day, int month, int year)
{
	// boal -->
	string result;
	if (day < 10)
	{
	    result = result + "0";
	}
	result = result + day + ".";
	if (month < 10)
	{
	    result = result + "0";
	}
	result = result + month + "." + year;
	// boal <--
	return result;
}

void ISetSaveData(string saveName, string saveData)
{
	SendMessage(&GameInterface,"lss",MSG_INTERFACE_DO_SAVE_DATA,saveName,saveData);
}

string IGetSaveString(string saveName)
{
	string retStr;
	SendMessage(&GameInterface,"lse",MSG_INTERFACE_GET_SAVE_DATA,saveName,&retStr);
	return retStr;
}

// boal -->
string GetConvertStr(string _param, string _file)
{
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile(_file);
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);

    return totalInfo;
}