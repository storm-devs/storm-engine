
#include "characters\characters_ai.c"
#include "characters\characters_events.c"
#include "characters\characters_login.c"
#include "characters\characters_face.c"
#include "characters\characters.h"
#include "characters\characterUtilite.c"
#include "characters\RPGUtilite.c" //boal
#include "characters\GeneratorUtilite.c" //boal
#include "characters\TravelMap.c" //navy
#include "characters\QuestsUtilite.c" //boal


object chrFindNearCharacters[MAX_CHARS_IN_LOC];
bool isBoardingLoading = false;


void CharactersInit()
{
    if(LoadSegment("characters\Characters_tables.c"))
	{
		InitCharactersTables();
		UnloadSegment("characters\Characters_tables.c");
	}
	ReloadProgressUpdate();

	string sPath = "characters\";
	// + LanguageGetLanguage() + "\";
	
	LoadSegment(sPath + "init\Antigua.c");			ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Barbados.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Jamaica.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Curacao.c");		ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Guadeloupe.c");			ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Santiago.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Panama.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\PuertoPrincipe.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Martinique.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Havana.c");	ReloadProgressUpdate();

	LoadSegment(sPath + "init\Bermudes.c");		ReloadProgressUpdate();
		
	LoadSegment(sPath + "init\Nevis.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\PuertoRico.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\LaVega.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\SantoDomingo.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\PortPax.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\PortoBello.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Trinidad.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\SentMartin.c");				ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Tortuga.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Cartahena.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Maracaibo.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Caracas.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Cumana.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\Beliz.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\SantaCatalina.c");				ReloadProgressUpdate();

	LoadSegment(sPath + "init\StoryCharacters.c");		ReloadProgressUpdate();
	
	LoadSegment(sPath + "init\Other.c");				ReloadProgressUpdate();
	
 	if(LoadSegment("characters\characters_init.c"))
	{
		CreateCharacters();
		UnloadSegment("characters\characters_init.c");
	}

	ReloadProgressUpdate();	   
	sPath = "characters\";
	UnloadSegment(sPath + "init\Antigua.c");
	UnloadSegment(sPath + "init\Barbados.c");
	UnloadSegment(sPath + "init\Jamaica.c");
	UnloadSegment(sPath + "init\Curacao.c");
	UnloadSegment(sPath + "init\Guadeloupe.c");
	UnloadSegment(sPath + "init\Santiago.c");
	UnloadSegment(sPath + "init\Panama.c");
	UnloadSegment(sPath + "init\PuertoPrincipe.c");
	UnloadSegment(sPath + "init\Martinique.c");
	UnloadSegment(sPath + "init\Havana.c");
	UnloadSegment(sPath + "init\Bermudes.c");
	UnloadSegment(sPath + "init\Nevis.c");
	UnloadSegment(sPath + "init\PuertoRico.c");
	UnloadSegment(sPath + "init\LaVega.c");
	UnloadSegment(sPath + "init\SantoDomingo.c");
	UnloadSegment(sPath + "init\PortPax.c");
	UnloadSegment(sPath + "init\PortoBello.c");
	UnloadSegment(sPath + "init\Trinidad.c");
	UnloadSegment(sPath + "init\SentMartin.c");
	UnloadSegment(sPath + "init\Tortuga.c");
	UnloadSegment(sPath + "init\Cartahena.c");
	UnloadSegment(sPath + "init\Maracaibo.c");
	UnloadSegment(sPath + "init\Caracas.c");
	UnloadSegment(sPath + "init\Cumana.c");
	UnloadSegment(sPath + "init\Beliz.c");
	UnloadSegment(sPath + "init\SantaCatalina.c");
	UnloadSegment(sPath + "init\StoryCharacters.c");
	UnloadSegment(sPath + "init\Other.c");

	ReloadProgressUpdate();
	InitCharacterEvents();
	ReloadProgressUpdate();
	LAi_NewGame();
}

bool CreateCharacter(ref character)
{
	CreateEntity(&character, character.model.entity);
	if(!SendMessage(character, "lss", MSG_CHARACTER_SETMODEL, character.model, character.model.animation))
	{
		Trace("CreateCharacter -> character id = '" + character.id + "' idx = " + character.index +" can invalide model("+ character.model +") or animation(" + character.model.animation + ")");
		DeleteClass(&character);
		return false;
	}

	float fCurCharge = 1000.0;
	if ( actLoadFlag == 1 && //fix
		CheckAttribute(character,"chr_ai.charge") ) 
	{
		fCurCharge = stf(character.chr_ai.charge);
	}
	ExecuteCharacterEquip(character);
	if( CheckAttribute(character,"chr_ai.charge") &&
		fCurCharge<stf(character.chr_ai.charge) ) {
			character.chr_ai.charge = fCurCharge;
	}

	//Set fight level
	if(!CheckAttribute(&character, "sex"))
	{
		character.sex = "man";
	}
	//if(character.sex == "man")
	//{
	float fgtlevel = LAi_GetCharacterFightLevel(character); // boal fix

	SendMessage(character, "lf", MSG_CHARACTER_SETFTGLEVEL, fgtlevel);
	//Set character sex
	SendMessage(character, "lsl", MSG_CHARACTER_EX_MSG, "SetSex", character.sex == "man");
	//
	AddCharacterLocatorGroup(character, "goto");
	BeginChangeCharacterActions(character);
	SetDefaultFight(character);
	EndChangeCharacterActions(character);
	//}
	return true;
}

bool DeleteCharacter(ref character)
{
	DeleteClass(character);
	return true;
}

bool TeleportCharacterToPos(ref character, float x, float y, float z)
{
	return SendMessage(character, "lfff", MSG_CHARACTER_TELEPORT, x, y, z);
}

bool TeleportCharacterToPosAy(ref character, float x, float y, float z, float ay)
{
	return SendMessage(character, "lffff", MSG_CHARACTER_TELEPORT_AY, x, y, z, ay);
}

bool TeleportCharacterToLocator(ref character, string group, string locator)
{
	return SendMessage(character, "lss", MSG_CHARACTER_TELEPORT_TO_LOCATOR, group, locator);
}

bool CheckLocationPosition(ref location, float x, float y, float z)
{
	if(IsEntity(loadedLocation) == false) return false;
	return (SendMessage(loadedLocation, "lfff", MSG_LOCATION_CHECKENTRY, x, y, z) != 0);
}

object GetCharacterModel(ref character)
{
	object model;
	SendMessage(character, "le", MSG_CHARACTER_GETMODEL, &model);
	return model;
}

bool GetCharacterPos(ref character, ref float_x, ref float_y, ref float_z)
{
	float cx, cy, cz;
	if(SendMessage(character, "leee", MSG_CHARACTER_GETPOS, &cx, &cy, &cz) == 0) return false;
	float_x = cx; float_y = cy; float_z = cz;
	return true;
}

bool GetCharacterAy(ref character, ref float_ay)
{
	float ay = 0.0;
	if(SendMessage(character, "le", MSG_CHARACTER_GETAY, &ay) == 0) return false;
	float_ay = ay;
	return true;
}

bool PushCharacterAy(ref character)
{
	return SendMessage(character, "l", MSG_CHARACTER_TURNPUSH);
}

bool PopCharacterAy(ref character)
{
	return SendMessage(character, "l", MSG_CHARACTER_TURNPOP);
}

int FindNearCharacters(ref character, float rad, float ax, float testAng, float nearDist, bool isVisibleTest, bool isSort)
{
	int num = 0;
	SendMessage(character, "leeffffll", MSG_CHARACTER_FINDNEAR, &chrFindNearCharacters, &num, rad, ax, testAng, nearDist, isVisibleTest, isSort);
	return num;
}

/*int FindNearNotCompanionCharacters(ref character, float rad, float ax, float testAng, float nearDist, bool isVisibleTest, bool isSort)
{
	int num = 0;
	SendMessage(character, "leeffffll", MSG_CHARACTER_FINDNEAR, &chrFindNearCharacters, &num, rad, ax, testAng, nearDist, isVisibleTest, isSort);
	return num;
}*/

bool CharactersVisibleTest(ref character, ref lookTo)
{
	return SendMessage(character, "li", MSG_CHARACTER_VISIBLE, lookTo);
}

//
void BeginChangeCharacterActions(ref character)
{
	DeleteAttribute(character, "actions");
}

//
void EndChangeCharacterActions(ref character)
{
	character.actions = "";
}

//Character is stay where play idle animation
void SetDefaultStayIdle(ref character)
{
	character.actions.idle.i1 = "idle_1";
	character.actions.idle.i2 = "idle_2";
	character.actions.idle.i3 = "idle_3";
	character.actions.idle.i4 = "idle_4";
	character.actions.idle.i5 = "idle_5";
	character.actions.idle.i6 = "idle_6";
	character.actions.idle.i7 = "idle_7";
	character.actions.idle.i8 = "idle_8";
	character.actions.idle.i9 = "idle_9";
	character.actions.idle.i10 = "idle_10";
	character.actions.idle.i11 = "idle_11";
	character.actions.HitNoFight = "HitNoFight";
}

//Character is sit where play idle animation
void SetDefaultSitIdle(ref character)
{		
	character.actions.idle.i1 = "Sit_Look_Around";
	character.actions.idle.i2 = "Sit_Lower_head";
	character.actions.idle.i3 = "Sit_Idle01";
	character.actions.idle.i4 = "Sit_Idle02";
	character.actions.idle.i5 = "Sit_Idle03";
	character.actions.idle.i6 = "Sit_Idle04";
	character.actions.idle.i7 = "Sit_Idle05";
	character.actions.idle.i8 = "Sit_Idle06";
	character.actions.HitNoFight = "HitNoFightSit";
}

void SetDefaultSit2Idle(ref character)
{		
	character.actions.idle.i1 = "Sit2_Idle01";
	character.actions.idle.i2 = "Sit2_Idle02";
	character.actions.idle.i3 = "Sit2_Idle03";
	character.actions.idle.i4 = "Sit2_Idle04";
	character.actions.idle.i5 = "Sit2_Idle05";
	character.actions.idle.i6 = "Sit2_Idle06";
	character.actions.idle.i7 = "Sit2_Idle07";
	character.actions.HitNoFight = "HitNoFightSit";
}

void SetDialogStayIdle(ref character)
{
	character.actions.idle.i1 = "dialog_stay1";
	character.actions.idle.i2 = "dialog_stay2";
	character.actions.idle.i3 = "dialog_stay3";
	character.actions.idle.i4 = "dialog_stay4";
	character.actions.idle.i5 = "dialog_stay5";
	character.actions.idle.i6 = "dialog_stay6";
	character.actions.idle.i7 = "dialog_stay7";
	character.actions.idle.i8 = "dialog_stay8";
}
// boal -->
void SetOverloadNormWalk(ref character)
{
    if(GetItemsWeight(character) > GetMaxItemsWeight(character))
    {
        character.actions.walk = "walk";
	    character.actions.backwalk = "back walk";
	    character.actions.run = "walk";
	    character.actions.backrun = "back run";
        character.actions.stsUp = "stairs up";
        character.actions.stsUpRun = "stairs up";
        character.actions.stsDown = "stairs down";
        character.actions.stsDownRun = "stairs down";
    }
}

void SetOverloadFight(ref character)
{
    if(GetItemsWeight(character) > GetMaxItemsWeight(character))
    {
       character.actions.fightwalk = "fight walk";
	   character.actions.fightbackwalk = "fight back walk";
	   character.actions.fightrun = "fight walk";
	   character.actions.fightbackrun = "fight back walk";
    }
}
void CheckAndSetOverloadMode(ref character)
{
	if (CheckAttribute(character, "actions")) // сундуки не трогаем
	{
        BeginChangeCharacterActions(character);
        if(GetItemsWeight(character) > GetMaxItemsWeight(character))
        {
            character.actions.walk = "walk";
	        character.actions.backwalk = "back walk";
	        character.actions.run = "walk";
	        character.actions.backrun = "back run";
            character.actions.stsUp = "stairs up";
            character.actions.stsUpRun = "stairs up";
            character.actions.stsDown = "stairs down";
            character.actions.stsDownRun = "stairs down";

            character.actions.fightwalk = "fight walk";
	        character.actions.fightbackwalk = "fight back walk";
	        character.actions.fightrun = "fight walk";
	        character.actions.fightbackrun = "fight back walk";
        }
        else
        {
            SetDefaultNormWalk(character);
            SetDefaultFight(character);
        }
        EndChangeCharacterActions(character);
    }
}
// boal <--
void SetDefaultNormWalk(ref character)
{
	
//	if( character.id == pchar.id && CheckAttribute(&InterfaceStates,"alwaysrun") && sti(InterfaceStates.alwaysrun) )
//	{
		character.actions.walk = "walk";
		character.actions.run = "run";
		character.actions.backwalk = "back walk";
		character.actions.backrun = "back run";
		character.actions.stsUp = "stairs up";
		character.actions.stsDown = "stairs down";
		character.actions.stsUpBack = "back stairs up";
		character.actions.stsDownBack = "back stairs down";
		character.actions.stsUpRun = "run stairs up";
		character.actions.stsDownRun = "run stairs down";
		character.actions.stsUpRunBack = "back run stairs up";
		character.actions.stsDownRunBack = "back run stairs down";
/*	}
	else
	{
		character.actions.walk = "run";
		character.actions.run = "walk";
		character.actions.backwalk = "back run";
		character.actions.backrun = "back walk";
		character.actions.stsUp = "run stairs up";
		character.actions.stsDown = "run stairs down";
		character.actions.stsUpBack = "back run stairs up";
		character.actions.stsDownBack = "back run stairs down";
		character.actions.stsUpRun = "stairs up";
		character.actions.stsDownRun = "stairs down";
		character.actions.stsUpRunBack = "back stairs up";
		character.actions.stsDownRunBack = "back stairs down";
	}*/

    // boal 21.01.2004 -->
	SetOverloadNormWalk(character);
	// boal 21.01.2004 <--
	
	character.actions.turnLeft = "turn left";
	character.actions.turnRight = "turn right";
	character.actions.swim = "swim";
	character.actions.fall = "fall";
	character.actions.fall_land = "fall_land";
	character.actions.fall_water = "fall_water";
}

void SetDefaultFight(ref character)
{
	
	//if( character.id == pchar.id && CheckAttribute(&InterfaceStates,"alwaysrun") && sti(InterfaceStates.alwaysrun) )
	//{
		character.actions.fightwalk = "fight walk";
		character.actions.fightrun = "fight run";
		character.actions.fightbackwalk = "fight back walk";
		character.actions.fightbackrun = "fight back run";
	/*}
	else
	{
		character.actions.fightwalk = "fight run";
		character.actions.fightrun = "fight walk";
		character.actions.fightbackwalk = "fight back run";
		character.actions.fightbackrun = "fight back walk";
	}*/
	
	// boal 21.01.2004 -->
    SetOverloadFight(character);
    // boal 21.01.2004 <--
    
	//Действия в режиме боя
	//Fast (max 3) --------------------------------------------------
	character.actions.attack_fast.a1 = "attack_fast_1";
	character.actions.attack_fast.a2 = "attack_fast_2";
	character.actions.attack_fast.a3 = "attack_fast_3";
	//Force (max 4) --------------------------------------------------
	character.actions.attack_force.a1 = "attack_force_1";
	character.actions.attack_force.a2 = "attack_force_2";
	character.actions.attack_force.a3 = "attack_force_3";
	character.actions.attack_force.a4 = "attack_force_4";
	//Round (max 2) --------------------------------------------------
	character.actions.attack_round.a1 = "attack_round_1";
	character.actions.attack_round.a2 = "attack_round_1";
	//Break (max 4) --------------------------------------------------
	character.actions.attack_break.a1 = "attack_break_1";
	character.actions.attack_break.a2 = "attack_break_1";
	character.actions.attack_break.a3 = "attack_break_1";
	character.actions.attack_break.a4 = "attack_break_1";
	//Feint (max 4) --------------------------------------------------
	character.actions.attack_feint.a1 = "attack_feint_1";
	character.actions.attack_feintc.a1 = "attack_feintc_1";
	character.actions.attack_feint.a2 = "attack_feint_2";
	character.actions.attack_feintc.a2 = "attack_feintc_2";
	character.actions.attack_feint.a3 = "attack_feint_3";
	character.actions.attack_feintc.a3 = "attack_feintc_3";
	character.actions.attack_feint.a4 = "attack_feint_4";
	character.actions.attack_feintc.a4 = "attack_feintc_4";
	//Shot--------------------------------------------------
	character.actions.shot = "Shot";
	//Normal hit (max 3) --------------------------------------------------
	character.actions.hit_attack.h1 = "hit_attack_1";
	character.actions.hit_attack.h2 = "hit_attack_2";
	character.actions.hit_attack.h3 = "hit_attack_3";
	//Ohter hits --------------------------------------------------------------------
	character.actions.hit_feint = "hit_feint";
	character.actions.hit_parry = "hit_parry";
	character.actions.hit_round = "hit_round";
	character.actions.hit_shot = "hit_fire";
	//Block--------------------------------------------------------------------
	character.actions.block = "block";
	character.actions.blockaxe = "block_axe";
	character.actions.blockhit = "block_hit";
	character.actions.blockaxehit = "block_axe_hit";
	character.actions.blockbreak = "block_break";
	//Parry--------------------------------------------------------------------
	character.actions.parry.p1 = "parry_1";
	character.actions.parry.p2 = "parry_2";
	character.actions.parry.p3 = "parry_3";
	character.actions.parry.p4 = "parry_4";
	//Strafes--------------------------------------------------------------------
	character.actions.recoil = "recoil";					//Назад
	character.actions.strafeleft = "straif_weapon_left";	//Влево
	character.actions.straferight = "straif_weapon_right";	//Вправо
	//Death
	SetDefaultFightDead(character);
	//Idle анимация в режиме боя
	character.actions.fightidle.i1 = "fight stand_1";
	character.actions.fightidle.i2 = "fight stand_2";
	character.actions.fightidle.i3 = "fight stand_3";
	character.actions.fightidle.i4 = "fight stand_4";
	character.actions.fightidle.i5 = "fight stand_5";
	character.actions.fightidle.i6 = "fight stand_6";
	character.actions.fightidle.i7 = "fight stand_7";
	character.actions.fightidle.i8 = "fight stand_8";
}

void SetDefaultDead(ref character)
{
	character.actions.dead.d1 = "death_citizen_1";
	character.actions.dead.d2 = "death_citizen_2";
}

void SetDefaultSitDead(ref character)
{
	character.actions.dead.d1 = "Sit_Death";
}

void SetAfraidDead(ref character)
{
	character.actions.dead.d1 = "death_afraid_1";
	character.actions.dead.d2 = "death_afraid_2";
}

void SetDefaultFightDead(ref character)
{
	character.actions.fightdead.d1 = "death_0";
	character.actions.fightdead.d2 = "death_1";
	character.actions.fightdead.d3 = "death_2";
	character.actions.fightdead.d4 = "death_3";
}

void SetHuberAnimation(ref character)
{
	character.actions.idle.i1 = "Gov_ObserveHands";
	character.actions.idle.i2 = "Gov_LegOnLeg";
	character.actions.idle.i3 = "Gov_Look_Around";
	character.actions.idle.i4 = "Gov_think_1";
	character.actions.idle.i5 = "Gov_think_2";
	character.actions.idle.i6 = "Gov_think_3";
}