

#define LAI_TMPL_ANI	"ani"


void LAi_tmpl_ani_PlayAnimation(aref chr, string actionName, float timeout)
{
	LAi_tmpl_ani_InitTemplate(chr);
	chr.chr_ai.tmpl.animation = "";
	chr.chr_ai.tmpl.time = "0";
	if (timeout == -1) chr.chr_ai.tmpl.animation.playToEnd = true;
	chr.chr_ai.tmpl.timeout = timeout;
	chr.chr_ai.tmpl.animation = actionName;
	if(LAi_IsInitedAI)
	{
		SetCharacterTask_Stay(chr);
		CharacterPlayAction(chr, actionName);
	}
}

bool LAi_tmpl_ani_InitTemplate(aref chr)
{
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	CharacterPlayAction(chr, "");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if(chr.chr_ai.tmpl != LAI_TMPL_ANI) isNew = true;
	}else{
		isNew = true;
	}
	if(isNew)
	{
		DeleteAttribute(chr, "chr_ai.tmpl");
		chr.chr_ai.tmpl = LAI_TMPL_ANI;
		chr.chr_ai.tmpl.animation = "";
		chr.chr_ai.tmpl.time = "0";
		chr.chr_ai.tmpl.timeout = "-1";
		if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	}else{
		chr.chr_ai.tmpl = LAI_TMPL_ANI;
		if(CheckAttribute(chr, "chr_ai.tmpl.animation") == false)
		{
			chr.chr_ai.tmpl.animation = "";
			chr.chr_ai.tmpl.time = "0";
			chr.chr_ai.tmpl.timeout = "-1";
		}
		if(CheckAttribute(chr, "chr_ai.tmpl.time") == false)
		{
			chr.chr_ai.tmpl.time = "0";
		}
		if(CheckAttribute(chr, "chr_ai.tmpl.timeout") == false)
		{
			chr.chr_ai.tmpl.timeout = "-1";
		}
		LAi_tmpl_ani_UpdateState(chr);
	}
	return true;
}

//Процессирование шаблона персонажа
void LAi_tmpl_ani_CharacterUpdate(aref chr, float dltTime)
{
	if(chr.chr_ai.tmpl.animation != "")
	{
		float timeout = stf(chr.chr_ai.tmpl.timeout);
		if(timeout >= 0)
		{
			float time = stf(chr.chr_ai.tmpl.time) + dltTime;
			chr.chr_ai.tmpl.time = time;
			if(time > timeout)
			{
				chr.chr_ai.tmpl.timeout = "-1";
				LAi_tmpl_ani_EndAnimation(chr);
			}
		}
	}
}

//Персонаж выполнил команду  go to point
void LAi_tmpl_ani_EndGoToPoint(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж провалил команду  go to point
void LAi_tmpl_ani_FailureGoToPoint(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}


//Персонаж выполнил команду  run to point
void LAi_tmpl_ani_EndRunToPoint(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж провалил команду  run to point
void LAi_tmpl_ani_FailureRunToPoint(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж не может добраться до точки назначения
void LAi_tmpl_ani_BusyPos(aref chr, float x, float y, float z)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж начал перемещение за другим
void LAi_tmpl_ani_FollowGo(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_ani_FollowStay(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж провалил команду  follow character
void LAi_tmpl_ani_FailureFollow(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}


//Персонаж начал перемещение за другим
void LAi_tmpl_ani_FightGo(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_ani_FightStay(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж провалил команду  Fight
void LAi_tmpl_ani_FailureFight(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Можно ли стрелять
bool LAi_tmpl_ani_IsFire(aref chr)
{	
	return false;
}

//Можно ли использовать оружие
bool LAi_tmpl_ani_IsFight(aref chr)
{
	return false;
}


//Персонаж выполнил команду  escape
void LAi_tmpl_ani_EndEscape(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж скользит вдоль патча
void LAi_tmpl_ani_EscapeSlide(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Персонаж провалил команду  escape
void LAi_tmpl_ani_FailureEscape(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}


//Персонаж толкается с другими персонажами
void LAi_tmpl_ani_ColThreshold(aref chr)
{
	LAi_tmpl_ani_UpdateState(chr);
}


//Персонаж закончил проигрывать анимацию
void LAi_tmpl_ani_EndAction(aref chr)
{	
	if(chr.chr_ai.tmpl.animation == "") return;
	chr.chr_ai.tmpl.timeout = "-1"; //бага еще та!
	LAi_tmpl_ani_EndAnimation(chr);
}


//Персонажа просят освободить место
void LAi_tmpl_ani_FreePos(aref chr, aref who)
{
	LAi_tmpl_ani_UpdateState(chr);
}

//Обновить состояние шаблона
void LAi_tmpl_ani_UpdateState(aref chr)
{
	if(LAi_IsInitedAI)
	{
		if(CharacterGetTask(chr) != "stay")
		{			
			SetCharacterTask_Stay(chr);
		}
		CharacterPlayAction(chr, chr.chr_ai.tmpl.animation);
	}
}

//Завершение проигрывания анимации
void LAi_tmpl_ani_EndAnimation(aref chr)
{
	chr.chr_ai.tmpl.animation = "";
	//--> нужно только для нищего при смене айдлов сидеть-стоять
	if (chr.chr_ai.type == LAI_TYPE_POOR) CharacterPlayAction(chr, "");
	//<-- нужно только для нищего при смене айдлов сидеть-стоять
	if(stf(chr.chr_ai.tmpl.timeout) < 0.0 && !CheckAttribute(chr, "chr_ai.tmpl.animation.playToEnd"))
	{
		LAi_Character_TemplateComplite(chr, LAI_TMPL_ANI);
	}
}
