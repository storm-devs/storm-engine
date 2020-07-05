/*
Тип: Бедняг

	Используемые шаблоны:
		stay
		dialog
		ani
*/


#define LAI_TYPE_POOR		"poor"


//Инициализация
void LAi_type_poor_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_POOR;
	LAi_tmpl_stay_InitTemplate(chr);
	chr.chr_ai.type.AttackIndex = -1;
	chr.chr_ai.type.GroundAfraid = false;
	//Флаги
	chr.chr_ai.type.wait = 1.0;
	LAi_SetDefaultStayAnimation(chr);	
	//нищий сидит или стоит. пропишем анимацию.
	if (CheckAttribute(chr, "PoorSit") && sti(chr.PoorSit))
	{
		chr.chr_ai.type.SitState = true; //сидим
		LAi_SetSitPoorAnimation(chr);
	}
	else
	{
		chr.chr_ai.type.SitState = false; //стоим
	}
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_poor_CharacterUpdate(aref chr, float dltTime)
{
	ref by;
	int idx, num;
	string sLoc;

	if (chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return; //режем диалог
	if (chr.chr_ai.tmpl == LAI_TMPL_ANI)
	{
		if (chr.chr_ai.tmpl.animation != "")
		{
			return; //режем анимацию
		}
		else
		{	//тут все действия после проигрывания анимаций
			if (sti(chr.chr_ai.type.SitState))
			{	//сидим
				if (!CheckAttribute(chr, "chr_ai.type.StandAndGoExit"))
				{
					if (sti(chr.chr_ai.type.AttackIndex) > 0 && !sti(chr.chr_ai.type.GroundAfraid))
					{	//после выполнения анимации быстрого вставания от удара
						by = &characters[sti(chr.chr_ai.type.AttackIndex)]; //здесь помним индекс врага с прошлых тактов
						LAi_tmpl_afraid_SetAfraidCharacter(chr, by, true);
						chr.chr_ai.type.SitState = false; //теперь нпс будет стоячим нищий
					}
					else
					{	//после выполнения анимации ground_afraid
						LAi_tmpl_stay_InitTemplate(chr);
						chr.chr_ai.type.GroundAfraid = false;
					}
				}	
				else
				{
					LAi_tmpl_goto_InitTemplate(chr);					
					if (rand(4) == 2)
					{
						LAi_tmpl_goto_SetLocator(chr, "reload", "gate_back", -1);
					}
					else
					{
						LAi_tmpl_goto_SetLocator(chr, "patrol",  LAi_FindRandomLocator("patrol"), -1);
						DeleteAttribute(chr, "chr_ai.type.StandAndGoExit");
					}
					chr.chr_ai.type.SitState = false; //теперь нпс будет стоячим нищим
				}				
			}
			else
			{	//стоим
				LAi_tmpl_stay_InitTemplate(chr);
				chr.chr_ai.type.SitState = true; //теперь нпс будет сидячим нищим
			}
			chr.chr_ai.type.wait = 0.0;
		}
	}
	float time = stf(chr.chr_ai.type.wait) - dltTime;
	if (time > 0)
	{
		chr.chr_ai.type.wait = time;
		return;
	}
	else 
	{
		else chr.chr_ai.type.wait = 2.0;
	}
	// --> сидячий нищий
	if (sti(chr.chr_ai.type.SitState))
	{
		num = FindNearCharacters(chr, 5.0, -1.0, -1.0, 0.001, false, true);
		//если люди в радиусе 5 метров
		if(num > 0)
		{
			for(int i = 0; i < num; i++)
			{
				idx = sti(chrFindNearCharacters[i].index);
				if(LAi_group_IsEnemy(chr, &Characters[idx])) break;
			}
			//есть враги
			if(i < num)
			{
				if (rand(1))
				{
					LAi_tmpl_ani_PlayAnimation(chr, "Ground_StandUp_Fast", -1.0);				
					chr.chr_ai.type.AttackIndex = idx;
					LAi_SetDefaultStayAnimation(chr);
				}
				else
				{			
					LAi_tmpl_ani_PlayAnimation(chr, "Ground_Afraid", -1.0);
					LAi_SetPoorAfraidGroundDead(chr);
					chr.chr_ai.type.GroundAfraid = true;
				}				
			}
			//нет врагов
			else
			{
				if(chr.chr_ai.tmpl != LAI_TMPL_STAY)
				{
					LAi_tmpl_stay_InitTemplate(chr);
					LAi_SetDefaultDead(chr);
				}
				if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG)
				{
					if (rand(50) == 49)
					{
						LAi_tmpl_ani_PlayAnimation(chr, "Ground_StandUp", -1);			
						chr.chr_ai.type.StandAndGoExit = true; //и на релоад
						LAi_SetDefaultStayAnimation(chr);
					}
				}
			}
		}
		//никого нет вокруг
		else
		{
			if(chr.chr_ai.tmpl != LAI_TMPL_STAY)
			{
				LAi_tmpl_stay_InitTemplate(chr);
				LAi_SetDefaultDead(chr);
			}
			if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG)
			{				
				if (rand(50) == 49)
				{
					LAi_tmpl_ani_PlayAnimation(chr, "Ground_StandUp", -1);					
					chr.chr_ai.type.StandAndGoExit = true; //и на релоад
					LAi_SetDefaultStayAnimation(chr);
				}
			}
		}
	}
	// <-- сидячий нищий
	else
	// --> стоячий нищий
	{	
		//если боимся
		if(chr.chr_ai.tmpl == LAI_TMPL_AFRAID)
		{
			if(LAi_tmpl_afraid_IsNoActive(chr))
			{
				//команда идти и сесть в локатор
				LAi_SetDefaultDead(chr);
				chr.chr_ai.type.GoAndSit = true; //чтобы сработала анимация садиться в типе goto
				DeleteAttribute(chr, "chr_ai.type.StandAndGoExit");
				LAi_tmpl_goto_InitTemplate(chr);
				if (!CheckAttribute(chr, "forSit"))
				{					
					LAi_tmpl_goto_SetLocator(chr, "goto", LAi_FindRandomLocator("goto"), -1);
				}
				else 
				{
					sLoc = "locator" + rand(2);
					LAi_tmpl_goto_SetLocator(chr, "goto", chr.forSit.(sLoc), -1);
				}
			}
		}
		//если идем к месту сидения
		if(chr.chr_ai.tmpl == LAI_TMPL_GOTO)
		{
			idx = LAi_type_poor_FindNearEnemy(chr);
			if(idx >= 0)
			{
				LAi_tmpl_afraid_SetAfraidCharacter(chr, &Characters[idx], true);
				LAi_SetAfraidDead(chr);
			}
			//если нпс прошелся и встал после выполнения, то переводим в темплейт stay
			if (chr.chr_ai.tmpl.state == "stay")
			{
				LAi_tmpl_stay_InitTemplate(chr);
			}
		}
		//если стоим ждем
		if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
		{
			idx = LAi_type_poor_FindNearEnemy(chr);
			if(idx >= 0)
			{
				LAi_tmpl_afraid_SetAfraidCharacter(chr, &Characters[idx], true);
				LAi_SetAfraidDead(chr);
			}
			else
			{				
				//команда идти и сесть в локатор
				LAi_SetDefaultDead(chr);
				chr.chr_ai.type.GoAndSit = true; //чтобы сработала анимация садиться в типе goto
				DeleteAttribute(chr, "chr_ai.type.StandAndGoExit");
				LAi_tmpl_goto_InitTemplate(chr);
				if (!CheckAttribute(chr, "forSit"))
				{					
					LAi_tmpl_goto_SetLocator(chr, "goto", LAi_FindRandomLocator("goto"), -1);
				}
				else 
				{
					sLoc = "locator" + rand(2);
					LAi_tmpl_goto_SetLocator(chr, "goto", chr.forSit.(sLoc), -1);
				}				
			}
		}
	}
	// <-- стоячий нищий
}

//Загрузка персонажа в локацию
bool LAi_type_poor_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_poor_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_poor_TemplateComplite(aref chr, string tmpl)
{
}

//Сообщить о желании завести диалог
void LAi_type_poor_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_poor_CanDialog(aref chr, aref by)
{
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG || chr.chr_ai.tmpl == LAI_TMPL_ANI) return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_poor_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	if (!sti(chr.chr_ai.type.SitState))
	{
		LAi_CharacterSaveAy(chr);
		CharacterTurnByChr(chr, by);
	}
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_poor_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
}

//Персонаж выстрелил
void LAi_type_poor_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAi_type_poor_Attacked(aref chr, aref by)
{
	
}

int LAi_type_poor_FindNearEnemy(aref chr)
{
	if(LAi_grp_alarmactive == true)
	{
		int num = FindNearCharacters(chr, 5.0, -1.0, -1.0, 0.001, false, true);
		if(num <= 0)
		{
			chrFindNearCharacters[0].index = "-1";
			return -1;
		}
		int cnt = 0;
		for(int i = 0; i < num; i++)
		{
			int idx = sti(chrFindNearCharacters[i].index);
			if(LAi_group_IsEnemy(chr, &Characters[idx])) return idx;
		}
	}
	return -1;
}

void LAi_SetSitPoorAnimation(aref chr)
{
	//Установим анимацию сидячему нищему
	BeginChangeCharacterActions(chr);
	chr.actions.idle.i1 = "Ground_sitting";
	chr.actions.idle.i2 = "Ground_look_around";
	chr.actions.idle.i3 = "Ground_Scratch";
	chr.actions.idle.i4 = "Ground_Beg";
	chr.actions.dead.d1 = "Ground_Death";
	chr.actions.dead.d2 = "Ground_Death1";
	chr.actions.HitNoFight = "Ground_HitNoFight";
	EndChangeCharacterActions(chr);
}

void LAi_SetPoorAfraidGroundDead(ref chr)
{
	BeginChangeCharacterActions(chr);
	chr.actions.dead.d1 = "Ground_Afraid_Death";
	EndChangeCharacterActions(chr);
}