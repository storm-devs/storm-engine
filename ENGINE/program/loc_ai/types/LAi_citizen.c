

#define LAI_TYPE_CITIZEN	"citizen"


//Инициализация
void LAi_type_citizen_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}else{
		if(chr.chr_ai.type != LAI_TYPE_CITIZEN) isNew = true;
	}
	if(isNew == true)
	{
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_CITIZEN;
		//Установим шаблон гуляния
		LAi_tmpl_walk_InitTemplate(chr);
	}	
	chr.chr_ai.type.notalk = rand(10);
	chr.chr_ai.type.checkFight = rand(5)+2;
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_citizen_CharacterUpdate(aref chr, float dltTime)
{	
	int idx, i;
	int trg = -1;
	float time;
	ref by;
	if(chr.chr_ai.tmpl == LAI_TMPL_WALK)
	{
		trg = LAi_type_citizen_FindNearEnemy(chr);
		if(trg >= 0)
		{
			if(LAi_IsSetBale(chr))
			{
				//Переходим в боевой режим
				LAi_tmpl_SetFight(chr, &Characters[trg]);
			}else{
				//Боимся
				LAi_tmpl_afraid_SetAfraidCharacter(chr, &Characters[trg], true);
				LAi_SetAfraidDead(chr);
			}
		}else{
			//Думаем о возможности поговорить	
			time = stf(chr.chr_ai.type.notalk) - dltTime;
			int num = FindNearCharacters(chr, 3.0, -1.0, -1.0, 0.001, true, true);	
			//--> eddy. если квестовый ситизен, задача которого поговорить с ГГ
			if (CheckAttribute(chr, "talker") && time < 10.0) 
			{	
				for(i = 0; i < num; i++)
				{						
					idx = sti(chrFindNearCharacters[i].index);
					if (idx == nMainCharacterIndex) //ищем ГГ
					{					
						if (sti(chr.talker) > rand(10) && LAi_Character_CanDialog(chr, pchar))
						{
							LAi_tmpl_SetDialog(chr, pchar, -1.0); //любое время можно ставить, с ГГ базар будет сразу
							DeleteAttribute(chr, "talker");
							break;
						}
						else time = 50.0;
					}
				}		
			}
			//<-- если квестовый ситизен, задача которого поговорить с ГГ
			//--> проверяем не врагов, но дерущихся. 			
			if (stf(chr.chr_ai.type.checkFight) < 0.0)
			{
				for(i = 0; i < num; i++)
				{
					idx = sti(chrFindNearCharacters[i].index);
					by = &Characters[idx];
					chr.chr_ai.type.checkFight = 2.0;
					if (by.chr_ai.tmpl == LAI_TMPL_FIGHT)
					{
						LAi_tmpl_afraid_SetAfraidCharacter(chr, by, true);
					}
				}
			}
			else 
			{
				chr.chr_ai.type.checkFight = stf(chr.chr_ai.type.checkFight) - dltTime;
			}
			//<-- проверяем не врагов, но дерущихся.
			if(time <= 0.0)
			{
				if(rand(100) < 10)
				{
					if (CheckAttribute(&chrFindNearCharacters[0], "index"))  //fix
					{
						idx = sti(chrFindNearCharacters[0].index);
						if(idx >= 0)
						{							
							by = &Characters[idx];
							//Тип
							if(CheckAttribute(by, "chr_ai.type"))
							{
								bool isDialog = false;
								switch(by.chr_ai.type)
								{
								case LAI_TYPE_CITIZEN:
									isDialog = true;
									break;
								case LAI_TYPE_GUARDIAN: 
									//не разговаривать с протекторами
									if (!CheckAttribute(by, "protector"))
									{
										isDialog = true;
									}
									break;
								case LAI_TYPE_PATROL:
									isDialog = true;
									break;
								case LAI_TYPE_MERCHANT: //eddy. с мерчантами пусть тоже базарят
									isDialog = true;
									break;
								}
								//Дистанция
								time = -1.0;
								if(GetCharacterDistByChr(chr, by, &time))
								{
									if(time < 0.0) isDialog = false;								
									if(time > 2.5) isDialog = false;
								}
								if(isDialog)
								{
									chr.chr_ai.type.notalk = 10.0 + rand(20);
									if(LAi_Character_CanDialog(chr, by))
									{
										if(!LAi_tmpl_SetDialog(chr, by, 3.0 + rand(10)))
										{
											LAi_tmpl_walk_InitTemplate(chr);
										}
									}
								}
							}
						}
					}
				}
			}else{
				chr.chr_ai.type.notalk = time;
			}
		}
	}else{
		if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
		{
			//Воюем, посматривая на убийство цели
			bool isValidate = false;
			trg = LAi_tmpl_fight_GetTarget(chr);
			if(trg >= 0)
			{
				if(LAi_group_ValidateTarget(chr, &Characters[trg]))
				{
					if(!LAi_tmpl_fight_LostTarget(chr))
					{
						isValidate = true;
					}
				}
			}
			if(!isValidate)
			{
				//Смотрим остальные цели
				trg = LAi_type_citizen_FindNearEnemy(chr);
				if(trg >= 0)
				{
					LAi_tmpl_SetFight(chr, &Characters[trg]);
				}else{
					LAi_tmpl_walk_InitTemplate(chr);
				}
			}
		}else{
			if(chr.chr_ai.tmpl == LAI_TMPL_AFRAID)
			{
				if(LAi_tmpl_afraid_IsNoActive(chr))
				{
					//Вернёмся к нормальной жизни
					LAi_SetDefaultDead(chr);
					LAi_tmpl_walk_InitTemplate(chr);
				}
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_citizen_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_citizen_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_citizen_TemplateComplite(aref chr, string tmpl)
{

}

//Сообщить о желании завести диалог
void LAi_type_citizen_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_citizen_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	if(LAi_CanNearEnemy(chr, 5.0)) return false;
	//Если просто стоим, то согласимся на диалог
	if(chr.chr_ai.tmpl == LAI_TMPL_WALK)
	{
		if(sti(by.index) != nMainCharacterIndex)
		{
			if(stf(chr.chr_ai.type.notalk) <= 0.0)
			{
				return true;
			}
			return false;
		}
		return true;
	}
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY) return true;
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG)	
	{
		if(sti(by.index) == nMainCharacterIndex)
		{
			if(LAi_tmpl_dialog_StopNPC(chr)) return true;
		}
	}
	return false;
}

//Начать диалог
void LAi_type_citizen_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_citizen_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	LAi_tmpl_walk_InitTemplate(chr);
	chr.chr_ai.type.notalk = 10.0 + rand(20);
}

//Персонаж выстрелил
void LAi_type_citizen_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//Персонаж атакован
void LAi_type_citizen_Attacked(aref chr, aref by)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG)
	{
		LAi_tmpl_dialog_StopNPC(chr);
		LAi_type_citizen_CharacterUpdate(chr, 0.0);
	}
}


int LAi_type_citizen_FindNearEnemy(aref chr)
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
