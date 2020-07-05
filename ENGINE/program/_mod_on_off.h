// BOAL 07.09.03  Mods on_off mode
// здесь и далее ник boal - это разработческий ник ALexusB

#define VERSION_NUMBER1       "Корсары: Возвращение Легенды  "
#define VERSION_NUMBER2       "ver. 1.1.8.0608"
#define VERSION_NUMBER3       "Seaward.Ru" // ALexusB&Co

#define VERSION_NUM      80608  // текущий номер
#define VERSION_NUM_PRE  80608  // последний совместмый

//Сайт: http://seadogs.4fans.org
#define VERSION_WWW     "http://www.seaward.ru"

// Show special message for test game statment
#define MOD_BETTATESTMODE			"On"  // On -DEV version; Test - TST ver; Off - relize

// гловальная строковая переменная
string sGlobalTemp;

// quest.c bool    bQuestCheckProcessFreeze = false; // заморозка квестов

// запрет всех случаек на суше (офицеры и клады тоже)
bool    bDisableLandEncounters = false;
// запрет жать ф2
bool    bDisableCharacterMenu  = false;
// флаг - игрок читер
bool    bPlayerIsCheater =  false;  // на перспективу, патчем
// режим супер читов
bool    bBettaTestMode = false;
// показывать quest_reaction Log
bool    bQuestLogShow  = false;
// показывать инфу по балансу
bool    bExpLogShow    = false;
// всегда сдаются капитаны - отладка
bool    TestRansackCaptain = false;
// показ локаторов
bool    bLocatorShow  = false;
// Не жрать, нет крыс
bool    bNoEatNoRats  = false;
// Живой мир на паузе: губер не переезжает от времени, нации не объявляют войну и мир, эскадры не берут город себе, город нельзя взять себе (даже с патентом)
bool    bWorldAlivePause      = true;
// пауза метро
bool bPauseContrabandMetro = false;
// лог для ПГГ
bool bPGGLogShow = false;
// флаг генерации монстров
bool bMonstersGen = false;
// флаг генерации сухопутных энкаунтеров
bool bLandEncountersGen = true;
// флаг "в сундуке"
bool bMainCharacterInBox = false;

// Генератор фантомов - альтернатива
bool    bNewFantomGenerator       = true;  // резерв "новый фехт"
bool    bRechargePistolOnLine  = true;  // пистолеты перезаряжаются без сабли
// упрощение невозможного
bool    bNewCodeOn                = false;  // упрощение абордажа
// нет сайвов, токо в церкви есть
bool    bHardcoreGame             = false;
// платный сайв-лоад
//bool    bPayForSaveLoad           = true;

///////////////////////////////////////////////////
//          complexity
//  Next Level rate
int MOD_EXP_RATE           = 10; // это база... + (сложность)
//int Level_Complexity       = 5; // by default только настройка
//  complexity of fantom skill
int MOD_SKILL_ENEMY_RATE   = 5; // 1- easy; 2; 3; 4;  5-medium; 6; 7; 8; 9;  10 - hard (please, do not set more 10 or less 1 it mays crash game


// Killed body clear
#define MOD_DEAD_CLEAR           "On"
#define MOD_DEAD_CLEAR_TIME      100 // in sec.

#define CHAR_ITEMS_WEIGHT        70

#define InstantRepairRATE        65.0

// дистанция до корабля для разговора
#define DistanceToShipTalk        300.0

//#define ShipyardMoneyInit        190000.0
//#define ShipyardMoneyMax         900000.0
//#define ShipyardMoneyAdd         2000

// boal 23.01.2004 доработка 091004 сколь угодно героев -->
int    startHeroType = 1;
//int    heroFaceId    = 1;
// boal 23.01.2004 <--

int ItemDeadStartCount = 0; // заполняется в itemInit порог для заполения трупов (оптимизация)

#define STARTGAME_YEAR       1665
#define STARTGAME_MONTH      1
#define STARTGAME_DAY        1

// пасхалка Кукурузина
#define FUNY_SHIP_NAME    "Кукурузина"

int TimeScaleCounter = 0;
// запрет перехода
bool chrDisableReloadToLocation = false;
// для видения форта в АИ шипов
bool bIsFortAtIsland = false;
float fFort_x, fFort_z;
int iFortNation, iFortCommander;
//начало и конец предметов для локаторов item
int ItemsForLocators_start, ItemsForLocators_end;

int iScriptVersion = 74632; //движок проверяет эту переменную, должна равной #define ENGINE_SCRIPT_VERSION в core.h
//==> проверка скриптами движка на наличие метода. если он есть, то возможное несоответствие движка и
//==> скриптов отсечет сам двиг, если нет - вылет. 74632 - адонный номер.
GetEngineVersion();
/////////////////////////////