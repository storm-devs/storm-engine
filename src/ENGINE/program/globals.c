#include "sound.h"
#include "messages.h"
#include "sound.c"

string savefile_info = "ver 1.0.7";
//--------------------------------------------------------------------
// Max characters per location
//--------------------------------------------------------------------
#define MAX_CHARS_IN_LOC	64

//--------------------------------------------------------------------
// World Config section
//--------------------------------------------------------------------

// сетевые штучки -->
int iRealDeltaTime = 0;
int iRealYear = 0;
int iRealMonth = 0;
int iRealDay = 0;
// сетевые штучки <--

int iGrassQuality = 0; //0 full, 1 middle, 2 low, 3 off

int iEncountersRate = 1;   // работает на карте и будет работать для скорости смены отношений и тп
int	iArcadeSails = 1;   // GetRSRollSpeed

//--------------------------------------------------------------------
// Math section
//--------------------------------------------------------------------
#define PI		3.14159265
#define PIm2	6.28318530
#define PId2	1.57079632
#define INVALID_SOUND_ID	0

int ngMusicID = 0;
string sgMusicName = "?";

float FRAND(float _x) { return rand(32768) / 32768.0 * _x; }
float frandSmall(float _x) { return rand(32) / 32.0 * _x; } // boal

ref pchar;

//--------------------------------------------------------------------
// Bool section
//--------------------------------------------------------------------
#define false			0
#define true			1			

//--------------------------------------------------------------------
// Nation section
//--------------------------------------------------------------------
#define	MAX_NATIONS       5
#define MAX_NATIONS_SQR   25

#define ENGLAND		0
#define FRANCE		1
#define SPAIN		2
#define HOLLAND		3
#define PIRATE		4

object	Nations[MAX_NATIONS];
int		NationsRelations[MAX_NATIONS_SQR];
int		NationsRelations2MainCharacter[MAX_NATIONS];

object	Render;

//--------------------------------------------------------------------
// Characters section
//--------------------------------------------------------------------
int     GlobalCharacters = 0;  // номер, с которого идут фантомы
int     MAX_CHARACTERS   = 10;
#define LOC_FANTOM_CHARACTERS		870	//32 max
#define FANTOM_CHARACTERS           940
#define TOTAL_CHARACTERS			1000  // прописать в BOAT_CHARACTER  TOTAL_CHARACTERS - 1
#define BOAT_CHARACTER               999

#define MAX_LOCATIONS		690 // boal
#define MAX_ISLANDS   30
#define MAX_COLONIES  34

#define LSC_MAX_TAVERN		12
#define LSC_MAX_RESIDENCE	3
#define LSC_MAX_CHURCH		7
#define LSC_MAX_PRISON		3
#define LSC_MAX_STORE		3

string 	sEnManNames[10], sEnWomenNames[4], sEnFamilies[108];
string 	sHoManNames[39], sHoWomenNames[14], sHoFamilies[95];
string 	sFrManNames[18], sFrWomenNames[17], sFrFamilies[158];
string 	sSpManNames[42], sSpWomenNames[16], sSpFamilies[66];


object  	NullCharacter;
object	Characters[TOTAL_CHARACTERS];
object	Locations[MAX_LOCATIONS];
object	Islands[MAX_ISLANDS];
object	Colonies[MAX_COLONIES];

object  	Environment;
int		nMainCharacterIndex = 1;//-1;
int		iNumFantoms;

int		nLocationsNum;
int		nTeleportLocation;
string	sTeleportLocName;
string  sTeleportLocator;

object SeaParametrs;

int MaxVertices = 32000;//32000;//16000;//8000;//4000
int MaxIndices = 33200;//33200;//17200;//9200;//5200
float GridStepX = 0.1;
float GridStepPC = 0.1;
float MaxWaveDistance = 4000.0;//4000.0;//3000.0;//2000.0;//1000.0
float LodScale = 0.7;//0.7;//1.2;//2.0;//4.0

ref GetFantomCharacter(int iIndex)
{ 
	return &Characters[FANTOM_CHARACTERS + iIndex]; 
}

ref GetCharacter(int iIndex) 
{ 
	if (iIndex < 0 || iIndex >= TOTAL_CHARACTERS)
	{
		trace("Incorrect Index for a character! Wrong Index is " + iIndex);
		if (MOD_BETTATESTMODE	==	"On") Log_Info("ERROR: Incorrect Index for a character! Wrong Index is " + iIndex);
		return &NullCharacter;
	}
	return &Characters[iIndex]); 
}

ref GetMainCharacter() 
{ 
	return &Characters[nMainCharacterIndex]; 
}

int GetMainCharacterIndex() 
{ 
	return nMainCharacterIndex;
}

int	GetCharacterIndex(string string_id) 
{
	return NativeFindCharacter(&Characters, string_id);
/*	int n;
	for(n=0;n<TOTAL_CHARACTERS;n++)
	{
		if(string_id == Characters[n].id) return n; 
	}
	return -1;*/
}

void SetMainCharacterIndex(int index) 
{ 
	if (nMainCharacterIndex!=-1)
	{
		Characters[nMainCharacterIndex].MainCharacter = "0";
		Characters[nMainCharacterIndex].model.entity = "NPCharacter";
	}
	nMainCharacterIndex = index; 
	Characters[nMainCharacterIndex].MainCharacter = "1";
	Characters[nMainCharacterIndex].model.entity = "Player";
	pchar = GetMainCharacter();
}

#event_handler("GOpt_GetGrassQuality","EventReturn_iGrassQuality");

int EventReturn_iGrassQuality()
{
    //trace("GrassQuality"); - эта хрень работает 5-10 раз в секунду
	return iGrassQuality;
}

// boal 13.11.06 спец прерывание ядра, для выбора шейдера подсветки и без
#event_handler("GOpt_isGrassLightsOn","EventReturn_isGrassLightsOn");

int EventReturn_isGrassLightsOn()
{
	// trace("EventReturn_isGrassLightsOn"); это я перевел на 1 раз за все время создания травы
	// 1 - светится, 0 - темная
	if (bSeaActive || bStorm || bTornado)
	{
		return 0;
	}
	
	if (GetHour() >= 9 && GetHour() <= 19)
	{
	    return 1;
	}
	return 0;
}
