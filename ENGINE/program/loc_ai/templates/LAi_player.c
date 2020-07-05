

#define LAI_TMPL_PLAYER	"player"


bool LAi_tmpl_player_InitTemplate(aref chr)
{
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	CharacterPlayAction(chr, "");
	DeleteAttribute(chr, "chr_ai.tmpl");
	chr.chr_ai.tmpl = LAI_TMPL_PLAYER;
	if(LAi_IsInitedAI) SetCharacterTask_None(chr);
	return true;
}

//Процессирование шаблона персонажа
void LAi_tmpl_player_CharacterUpdate(aref chr, float dltTime)
{
	SetCharacterTask_None(chr);
}

//Персонаж выполнил команду  go to point
void LAi_tmpl_player_EndGoToPoint(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж провалил команду  go to point
void LAi_tmpl_player_FailureGoToPoint(aref chr)
{
	SetCharacterTask_None(chr);
}


//Персонаж выполнил команду  run to point
void LAi_tmpl_player_EndRunToPoint(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж провалил команду  run to point
void LAi_tmpl_player_FailureRunToPoint(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж не может добраться до точки назначения
void LAi_tmpl_player_BusyPos(aref chr, float x, float y, float z)
{
	SetCharacterTask_None(chr);
}

//Персонаж начал перемещение за другим
void LAi_tmpl_player_FollowGo(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_player_FollowStay(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж провалил команду  follow character
void LAi_tmpl_player_FailureFollow(aref chr)
{
	SetCharacterTask_None(chr);
}


//Персонаж начал перемещение за другим
void LAi_tmpl_player_FightGo(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_player_FightStay(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж провалил команду  Fight
void LAi_tmpl_player_FailureFight(aref chr)
{
	SetCharacterTask_None(chr);
}

//Можно ли стрелять
bool LAi_tmpl_player_IsFire(aref chr)
{	
	return LAi_CharacterCanFrie(chr);
}

//Можно ли использовать оружие
bool LAi_tmpl_player_IsFight(aref chr)
{
	if(LAi_LocationCanFight()) return true;
	Log_SetStringToLog(XI_ConvertString("CantFight"));
	return false;
}


//Персонаж выполнил команду  escape
void LAi_tmpl_player_EndEscape(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж скользит вдоль патча
void LAi_tmpl_player_EscapeSlide(aref chr)
{
	SetCharacterTask_None(chr);
}

//Персонаж провалил команду  escape
void LAi_tmpl_player_FailureEscape(aref chr)
{
	SetCharacterTask_None(chr);
}


//Персонаж толкается с другими персонажами
void LAi_tmpl_player_ColThreshold(aref chr)
{
	SetCharacterTask_None(chr);
}


//Персонаж закончил проигрывать анимацию
void LAi_tmpl_player_EndAction(aref chr)
{
	SetCharacterTask_None(chr);
}


//Персонажа просят освободить место
void LAi_tmpl_player_FreePos(aref chr, aref who)
{
	SetCharacterTask_None(chr);
}

