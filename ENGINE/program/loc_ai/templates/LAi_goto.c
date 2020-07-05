

#define LAI_TMPL_GOTO	"goto"

/*
	Возможные состояния:
		"stay"		ожидает задания
		"goto"		идёт к локатору
		"falure"	задача невыполнима
*/

bool LAi_tmpl_goto_InitTemplate(aref chr)
{
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	CharacterPlayAction(chr, "");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if(chr.chr_ai.tmpl != LAI_TMPL_GOTO) isNew = true;
	}else{
		isNew = true;
	}
	if(isNew)
	{
		DeleteAttribute(chr, "chr_ai.tmpl");
		chr.chr_ai.tmpl = LAI_TMPL_GOTO;
		chr.chr_ai.tmpl.state = "stay";
		chr.chr_ai.tmpl.wait = 0;
		chr.chr_ai.tmpl.timeout = -1;
		chr.chr_ai.tmpl.time = 0;
		if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	}else{
		isNew = true;
		if(CheckAttribute(chr, "chr_ai.tmpl.group"))
		{
			if(CheckAttribute(chr, "chr_ai.tmpl."))
			{
				if(chr.chr_ai.tmpl.group != "")
				{
					if(chr.chr_ai.tmpl.locator != "")
					{
						isNew = false;
					}
				}
			}
		}
		if(isNew)
		{
			chr.chr_ai.tmpl.group = "";
			chr.chr_ai.tmpl.locator = "";
			chr.chr_ai.tmpl.state = "stay";
		}else{
			chr.chr_ai.tmpl.state = "goto";
		}
		chr.chr_ai.tmpl.wait = "0";
		if(!CheckAttribute(chr, "chr_ai.tmpl.timeout")) chr.chr_ai.tmpl.timeout = "-1";
		if(!CheckAttribute(chr, "chr_ai.tmpl.time")) chr.chr_ai.tmpl.time = "0";
		LAi_tmpl_goto_Restart(chr);
	}
	return true;
}

//Направить персонажа в заданный локатор
void LAi_tmpl_goto_SetLocator(aref chr, string group, string locator, float timeout)
{
	chr.chr_ai.tmpl.state = "goto";
	chr.chr_ai.tmpl.wait = "0";
	chr.chr_ai.tmpl.group = group;
	chr.chr_ai.tmpl.locator = locator;
	chr.chr_ai.tmpl.timeout = timeout;
	chr.chr_ai.tmpl.time = "0";
	LAi_tmpl_goto_Restart(chr);
}

bool LAi_tmpl_goto_IsStay(aref chr)
{
	if(chr.chr_ai.tmpl.state != "goto") return true;
	return false;
}

bool LAi_tmpl_goto_IsWait(aref chr)
{
	if(stf(chr.chr_ai.tmpl.wait) > 0.0) return true;
	return false;
}

//Процессирование шаблона персонажа
void LAi_tmpl_goto_CharacterUpdate(aref chr, float dltTime)
{
	aref tmpl;
	makearef(tmpl, chr.chr_ai.tmpl);
	if(tmpl.state == "goto")
	{

		float time = stf(tmpl.wait);
		if(time > 0.0)
		{
			//Персонаж ждёт
			if(time > 1.0)
			{
				//Ещё ждём
				tmpl.wait = time - dltTime;
			}else{
				//Пора идти
				tmpl.wait = "0";
				LAi_tmpl_goto_Restart(chr);
			}
		}
		float timeout = stf(tmpl.timeout);
		if(timeout > 0.0)
		{
			time = stf(tmpl.time) + dltTime;
			tmpl.time = time;
			if(time >= timeout)
			{
				//Экстренно переносим персонажа
				Trace("Template <goto> -> timeout for chr.id = " + chr.id);
				if(LAi_tmpl_goto_Teleport(chr))
				{
					LAi_tmpl_goto_Complite(chr);
				}else{
					chr.chr_ai.tmpl.state = "falure";
					SetCharacterTask_Stay(chr);
				}
			}
		}
	}
}

//Персонаж выполнил команду  go to point
void LAi_tmpl_goto_EndGoToPoint(aref chr)
{
	//если тип нпс грузчик
	if (chr.chr_ai.type == LAI_TYPE_CARRY)
	{		
		ChangeCharacterAddressGroup(chr, chr.location, "reload", "gate");
	}
	//закончить таск goto и перевести в stay (с сохранением текущего темплейта!)
	LAi_tmpl_goto_Complite(chr);
	//если это нищий
	if (CheckAttribute(chr, "chr_ai.type.GoAndSit"))
	{
		LAi_SetSitPoorAnimation(chr);
		DeleteAttribute(chr, "chr_ai.type.GoAndSit");
		LAi_tmpl_ani_PlayAnimation(chr, "Ground_SitDown", -1);
	}
	if (CheckAttribute(chr, "chr_ai.type.StandAndGoExit"))
	{
		DeleteAttribute(chr, "chr_ai.type.StandAndGoExit");
		ChangeCharacterAddress(chr, "None", "");
	}
}

//Персонаж провалил команду  go to point
void LAi_tmpl_goto_FailureGoToPoint(aref chr)
{
	Trace("Template <goto> -> failure for chr.id = " + chr.id);

	if(LAi_tmpl_goto_Teleport(chr))
	{
		LAi_tmpl_goto_Complite(chr);
	}else{
		chr.chr_ai.tmpl.state = "falure";
		SetCharacterTask_Stay(chr);
	}
}


//Персонаж выполнил команду  run to point
void LAi_tmpl_goto_EndRunToPoint(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж провалил команду  run to point
void LAi_tmpl_goto_FailureRunToPoint(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж не может добраться до точки назначения
void LAi_tmpl_goto_BusyPos(aref chr, float x, float y, float z)
{
	if(chr.chr_ai.tmpl.state == "goto" && chr.chr_ai.type != LAI_TYPE_CARRY)
	{
		//Просим освободить локатор
		LAi_Character_FreeLocator(chr, chr.chr_ai.tmpl.group, chr.chr_ai.tmpl.locator);
		//Ждём
		chr.chr_ai.tmpl.wait = 2.0 + rand(4)*0.5;
		SetCharacterTask_Stay(chr);
	}
}

//Персонаж начал перемещение за другим
void LAi_tmpl_goto_FollowGo(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_goto_FollowStay(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж провалил команду  follow character
void LAi_tmpl_goto_FailureFollow(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}


//Персонаж начал перемещение за другим
void LAi_tmpl_goto_FightGo(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_goto_FightStay(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж провалил команду  Fight
void LAi_tmpl_goto_FailureFight(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Можно ли стрелять
bool LAi_tmpl_goto_IsFire(aref chr)
{	
	return false;
}

//Можно ли использовать оружие
bool LAi_tmpl_goto_IsFight(aref chr)
{
	return false;
}


//Персонаж выполнил команду  escape
void LAi_tmpl_goto_EndEscape(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}

//Персонаж скользит вдоль патча
void LAi_tmpl_goto_EscapeSlide(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
	if (chr.chr_ai.type == LAI_TYPE_CARRY)
	{
		LAi_type_carrier_GoTo(chr);
	}
}

//Персонаж провалил команду  escape
void LAi_tmpl_goto_FailureEscape(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}


//Персонаж толкается с другими персонажами
void LAi_tmpl_goto_ColThreshold(aref chr)
{
	if (chr.chr_ai.type == LAI_TYPE_PATROL)
	{
		LAi_type_patrol_Goto(chr);
	}
	if (chr.chr_ai.type == LAI_TYPE_LSCOUT)
	{
		LAI_TYPE_LSCout_Goto(chr);
	}
	else LAi_tmpl_goto_Restart(chr); 
}


//Персонаж закончил проигрывать анимацию
void LAi_tmpl_goto_EndAction(aref chr)
{
	LAi_tmpl_goto_Restart(chr);
}


//Персонажа просят освободить место
void LAi_tmpl_goto_FreePos(aref chr, aref who)
{
}


//Перезапустить задачу
void LAi_tmpl_goto_Restart(aref chr)
{
	if(chr.chr_ai.tmpl.state == "goto")
	{
		if(LAi_IsInitedAI)
		{
			if(stf(chr.chr_ai.tmpl.wait) < 1.0)
			{
				if(SetCharacterTask_GotoPoint(chr, chr.chr_ai.tmpl.group, chr.chr_ai.tmpl.locator) == false)
				{
					Trace("Template 111 <goto> -> path not found chr.id = " + chr.id);
					if(LAi_tmpl_goto_Teleport(chr))
					{
						LAi_tmpl_goto_Complite(chr);
					}else{
						chr.chr_ai.tmpl.state = "falure";
						SetCharacterTask_Stay(chr);
					}				
				}
			}else{
				SetCharacterTask_Stay(chr);
			}
		}
	}else{
		if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	}
}

bool LAi_tmpl_goto_Teleport(aref chr)
{
	if(stf(chr.chr_ai.tmpl.timeout) >= 0)
	{
		//Trace("Template <goto> -> teleport chr.id = " + chr.id + " to <" + chr.chr_ai.tmpl.group + " : " + chr.chr_ai.tmpl.locator + ">");
		TeleportCharacterToLocator(chr, chr.chr_ai.tmpl.group, chr.chr_ai.tmpl.locator);
		return true;
	}
	return false;
}

//Завершить goto
void LAi_tmpl_goto_Complite(aref chr)
{
	if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	chr.chr_ai.tmpl.group = "";
	chr.chr_ai.tmpl.locator = "";
	chr.chr_ai.tmpl.state = "stay";
	chr.chr_ai.tmpl.wait = "0";
	chr.chr_ai.tmpl.timeout = "-1";
	chr.chr_ai.tmpl.time = "0";
	LAi_Character_TemplateComplite(chr, LAI_TMPL_GOTO);
}
