

#define LAI_TMPL_STAY	"stay"



bool LAi_tmpl_stay_InitTemplate(aref chr)
{
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	CharacterPlayAction(chr, "");
	DeleteAttribute(chr, "chr_ai.tmpl");
	chr.chr_ai.tmpl = LAI_TMPL_STAY;
	if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	return true;
}

//Процессирование шаблона персонажа
void LAi_tmpl_stay_CharacterUpdate(aref chr, float dltTime)
{
}

//Персонаж выполнил команду  go to point
void LAi_tmpl_stay_EndGoToPoint(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж провалил команду  go to point
void LAi_tmpl_stay_FailureGoToPoint(aref chr)
{
	SetCharacterTask_Stay(chr);
}


//Персонаж выполнил команду  run to point
void LAi_tmpl_stay_EndRunToPoint(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж провалил команду  run to point
void LAi_tmpl_stay_FailureRunToPoint(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж не может добраться до точки назначения
void LAi_tmpl_stay_BusyPos(aref chr, float x, float y, float z)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж начал перемещение за другим
void LAi_tmpl_stay_FollowGo(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_stay_FollowStay(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж провалил команду  follow character
void LAi_tmpl_stay_FailureFollow(aref chr)
{
	SetCharacterTask_Stay(chr);
}


//Персонаж начал перемещение за другим
void LAi_tmpl_stay_FightGo(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_stay_FightStay(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж провалил команду  Fight
void LAi_tmpl_stay_FailureFight(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Можно ли стрелять
bool LAi_tmpl_stay_IsFire(aref chr)
{	
	return false;
}

//Можно ли использовать оружие
bool LAi_tmpl_stay_IsFight(aref chr)
{
	return false;
}


//Персонаж выполнил команду  escape
void LAi_tmpl_stay_EndEscape(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж скользит вдоль патча
void LAi_tmpl_stay_EscapeSlide(aref chr)
{
	SetCharacterTask_Stay(chr);
}

//Персонаж провалил команду  escape
void LAi_tmpl_stay_FailureEscape(aref chr)
{
	SetCharacterTask_Stay(chr);
}


//Персонаж толкается с другими персонажами
void LAi_tmpl_stay_ColThreshold(aref chr)
{
	SetCharacterTask_Stay(chr);
}


//Персонаж закончил проигрывать анимацию
void LAi_tmpl_stay_EndAction(aref chr)
{
	SetCharacterTask_Stay(chr);
}


//Персонажа просят освободить место
void LAi_tmpl_stay_FreePos(aref chr, aref who)
{
	SetCharacterTask_Stay(chr);
}

