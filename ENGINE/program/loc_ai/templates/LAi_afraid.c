

#define LAI_TMPL_AFRAID	"afraid"

void LAi_tmpl_afraid_SetAfraidCharacter(aref chr, aref afraid, bool canMove)
{
	DeleteAttribute(chr, "chr_ai.tmpl");
	LAi_tmpl_afraid_InitTemplate(chr);
	chr.chr_ai.tmpl.who = afraid.index;
	chr.chr_ai.tmpl.canmove = canMove;
	LAi_tmpl_afraid_updatetemplate(chr);
}

bool LAi_tmpl_afraid_IsNoActive(aref chr)
{
	if(chr.chr_ai.tmpl.state == "wait") return true;
	if(chr.chr_ai.tmpl.state == "stay") return true;
	return false;
}

bool LAi_tmpl_afraid_InitTemplate(aref chr)
{
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "LockFightMode", false);
	CharacterPlayAction(chr, "");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if(chr.chr_ai.tmpl != LAI_TMPL_AFRAID) isNew = true;
	}else{
		isNew = true;
	}
	if(isNew)
	{
		DeleteAttribute(chr, "chr_ai.tmpl");
		chr.chr_ai.tmpl = LAI_TMPL_AFRAID;
		chr.chr_ai.tmpl.who = "";
		chr.chr_ai.tmpl.state = "wait";
		chr.chr_ai.tmpl.canmove = "1";
		chr.chr_ai.tmpl.afrtime = "-1";
		if(LAi_IsInitedAI) SetCharacterTask_Stay(chr);
	}else{
		if(!CheckAttribute(chr, "chr_ai.tmpl.state")) chr.chr_ai.tmpl.state = "wait";
		if(chr.chr_ai.tmpl.state == "falure") chr.chr_ai.tmpl.state = "wait";
		if(!CheckAttribute(chr, "chr_ai.tmpl.canmove")) chr.chr_ai.tmpl.canmove = "1";
		if(!CheckAttribute(chr, "chr_ai.tmpl.who"))
		{
			chr.chr_ai.tmpl.who = "";
			chr.chr_ai.tmpl.state = "wait";
		}else{
			if(chr.chr_ai.tmpl.who == "") chr.chr_ai.tmpl.state = "wait";
		}
		LAi_tmpl_afraid_updatetemplate(chr);
	}
	return true;
}

//Процессирование шаблона персонажа
void LAi_tmpl_afraid_CharacterUpdate(aref chr, float dltTime)
{
	float ay;
	int idx;
	string loc;
	aref tmpl;
	makearef(tmpl, chr.chr_ai.tmpl);
	float dist = LAi_tmpl_afraid_DistByChr(chr);
	if(dist < 0)
	{
		CharacterPlayAction(chr, "");
		return;
	}
	if(sti(tmpl.canmove))
	{
		//Разрешено перемещение
		switch(tmpl.state)
		{
		case "wait":
			//Стоим и ждем слишком близкого подхода
			if(dist < 10.0)
			{
				//Решаем двигаться
				tmpl.locator = LAi_FindRandomLocator("goto");
				if(tmpl.locator != "")
				{
					tmpl.state = "goto";
					tmpl.panic = rand(10)*0.4;
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}
			break;
		case "stay":
			//Следим за врагом
			idx = sti(chr.chr_ai.tmpl.who);
			CharacterTurnByChr(chr, &Characters[idx]);
			//Стоим и ждем слишком близкого подхода
			if(dist < 5.0)
			{
				//Решаем двигаться
				tmpl.locator = LAi_FindRandomLocator("goto");
				if(tmpl.locator != "")
				{
					tmpl.state = "runto";
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}else{
				if(dist > 12.0)
				{
					tmpl.state = "wait";
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}
			break;
		case "goto":
			if(dist < 5.0)
			{
				//Пора бежать
				tmpl.state = "runto";
				LAi_tmpl_afraid_updatetemplate(chr);
			}else{
				if(dist > 15.0)
				{
					tmpl.state = "wait";
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}
			break;
		case "runto":
			if(dist > 6.0)
			{
				if(dist > 15.0)
				{
					//Можно остановиться
					tmpl.state = "wait";
					LAi_tmpl_afraid_updatetemplate(chr);
				}else{
					//Пора сбавлять скорость
					tmpl.state = "goto";
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}else{
				if(CheckAttribute(tmpl, "panic"))
				{
					if(dist < stf(tmpl.panic))
					{						
						//Слижком близко - паническое убегание
						tmpl.state = "escape";
						LAi_tmpl_afraid_updatetemplate(chr);
					}
				}
			}			
			break;
		case "afraid":
			if(dist > 3.5)
			{
				if(rand(2) != 1)
				{
					tmpl.state = "stay";
				}else{
					tmpl.state = "runto";
					tmpl.panic = stf(tmpl.panic)*0.5;
				}
				LAi_tmpl_afraid_updatetemplate(chr);
			}else{
				ay = stf(chr.chr_ai.tmpl.afrtime) - dltTime;
				chr.chr_ai.tmpl.afrtime = ay;
				if(ay <= 0.0)
				{
					//Осмелели
					tmpl.state = "runto";
					tmpl.panic = "0";
				}
			}
			break;
		}
	}else{
		//Можно только стоять и бояться
		if(dist < 2.5)
		{
			//Пора начать стоять и бояться
			if(tmpl.state != "afraid")
			{
				tmpl.state = "afraid";				
				if(!GetCharacterAy(chr, ay)) ay = 0.0;
				tmpl.angle = ay;
				LAi_tmpl_afraid_updatetemplate(chr);
			}
		}else{
			if(dist > 4.0)
			{
				//Отбоялись, можно и выпендриться
				if(tmpl.state == "afraid")
				{
					tmpl.state = "wait";
					LAi_tmpl_afraid_updatetemplate(chr);
				}
			}
		}
	}
	if(tmpl.state == "afraid")
	{
		idx = sti(chr.chr_ai.tmpl.who);
		CharacterTurnByChr(chr, &Characters[idx]);
	}
}

//Персонаж выполнил команду  go to point
void LAi_tmpl_afraid_EndGoToPoint(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж провалил команду  go to point
void LAi_tmpl_afraid_FailureGoToPoint(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}


//Персонаж выполнил команду  run to point
void LAi_tmpl_afraid_EndRunToPoint(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж провалил команду  run to point
void LAi_tmpl_afraid_FailureRunToPoint(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж не может добраться до точки назначения
void LAi_tmpl_afraid_BusyPos(aref chr, float x, float y, float z)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж начал перемещение за другим
void LAi_tmpl_afraid_FollowGo(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_afraid_FollowStay(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж провалил команду  follow character
void LAi_tmpl_afraid_FailureFollow(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}


//Персонаж начал перемещение за другим
void LAi_tmpl_afraid_FightGo(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж начал дошёл до другого персонажа
void LAi_tmpl_afraid_FightStay(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж провалил команду  Fight
void LAi_tmpl_afraid_FailureFight(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Можно ли стрелять
bool LAi_tmpl_afraid_IsFire(aref chr)
{	
	return false;
}

//Можно ли использовать оружие
bool LAi_tmpl_afraid_IsFight(aref chr)
{
	return false;
}


//Персонаж выполнил команду  escape
void LAi_tmpl_afraid_EndEscape(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж скользит вдоль патча
void LAi_tmpl_afraid_EscapeSlide(aref chr)
{
	int afrTest = 1;
	if(CheckAttribute(chr, "sex"))
	{
		if(chr.sex == "woman") afrTest = 4;
	}
	if(rand(10) > afrTest)
	{
		chr.chr_ai.tmpl.state = "runto";
		chr.chr_ai.tmpl.panic = "0";
	}else{
		chr.chr_ai.tmpl.state = "afraid";
		chr.chr_ai.tmpl.afrtime = afrTest + rand(10);
	}
	LAi_tmpl_afraid_updatetemplate(chr);
}

//Персонаж провалил команду  escape
void LAi_tmpl_afraid_FailureEscape(aref chr)
{
	chr.chr_ai.tmpl.state = "stay";
	LAi_tmpl_afraid_updatetemplate(chr);
}


//Персонаж толкается с другими персонажами
void LAi_tmpl_afraid_ColThreshold(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}


//Персонаж закончил проигрывать анимацию
void LAi_tmpl_afraid_EndAction(aref chr)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}


//Персонажа просят освободить место
void LAi_tmpl_afraid_FreePos(aref chr, aref who)
{
	LAi_tmpl_afraid_updatetemplate(chr);
}


void LAi_tmpl_afraid_updatetemplate(aref chr)
{
	if(sti(chr.chr_ai.tmpl.canmove))
	{
		bool isWait = true;
		switch(chr.chr_ai.tmpl.state)
		{
		case "goto":
			if(SetCharacterTask_GotoPoint(chr, "goto", chr.chr_ai.tmpl.locator)) isWait = false;
			CharacterPlayAction(chr, "");
			break;
		case "runto":
			if(SetCharacterTask_RuntoPoint(chr, "goto", chr.chr_ai.tmpl.locator)) isWait = false;
			CharacterPlayAction(chr, "");
			break;
		case "escape":
			if(chr.chr_ai.tmpl.who != "")
			{
				if(SetCharacterTask_Escape(chr, &Characters[sti(chr.chr_ai.tmpl.who)])) isWait = false;
			}
			CharacterPlayAction(chr, "");
			break;
		case "afraid":
			SetCharacterTask_Stay(chr);
			CharacterPlayAction(chr, "afraid");
			isWait = false;
			break;
		case "stay":
			SetCharacterTask_Stay(chr);
			CharacterPlayAction(chr, "");
			break;
		}
		if(isWait)
		{
			chr.chr_ai.tmpl.state = "wait";
			SetCharacterTask_Stay(chr);
			CharacterPlayAction(chr, "");
		}
	}else{
		SetCharacterTask_Stay(chr);
		if(chr.chr_ai.tmpl.state == "afraid")
		{
			CharacterPlayAction(chr, "afraid");
		}else{
			CharacterPlayAction(chr, "");
		}
	}
}

//Найти дистанцию до преследующего персонажа
float LAi_tmpl_afraid_DistByChr(aref chr)
{
	if(chr.chr_ai.tmpl.who == "") return -1.0;
	//Позиция персонажа которого боимся
	int idx = sti(chr.chr_ai.tmpl.who);
	float dist = 0.0;
	if(!GetCharacterDistByChr3D(chr, &Characters[idx], &dist)) dist = -1.0;
	return dist;
}


