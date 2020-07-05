/*
Тип: воин, бьёт врагов, когда свободен, возвращается к лидеру или стоит

	Используемые шаблоны:
		fight
		follow
		walk
		stay
		dialog
*/



#define LAI_TYPE_WARRIOR	"warrior"



//Установить войну командира
void LAi_warrior_SetCommander(aref chr, aref commander)
{
	chr.chr_ai.type.index = commander.index;
}

//Разрешить или запретить диалоги для война
void LAi_warrior_DialogEnable(aref chr, bool isEnable)
{
	chr.chr_ai.type.dialog = isEnable;
}

//Сказать войну стоять при отсутствие целей
void LAi_warrior_SetStay(aref chr, bool isStay)
{
	chr.chr_ai.type.stay = isStay;
}

//Инициализация
void LAi_type_warrior_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}else{
		if(chr.chr_ai.type != LAI_TYPE_WARRIOR) isNew = true;
	}
	if(isNew == true)
	{
		//Новый тип
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_WARRIOR;
		chr.chr_ai.type.stay = "0";
		chr.chr_ai.type.index = "";
		chr.chr_ai.type.dialog = "1"; 
		chr.chr_ai.type.bottle = rand(10)+2;
		chr.chr_ai.type.checkTarget = 0;
		LAi_tmpl_stay_InitTemplate(chr);
	}else{
		if(!CheckAttribute(chr, "chr_ai.type.stay")) chr.chr_ai.type.stay = "0";
		if(!CheckAttribute(chr, "chr_ai.type.index")) chr.chr_ai.type.index = "";
		if(!CheckAttribute(chr, "chr_ai.type.dialog")) chr.chr_ai.type.dialog = "1"; 
	}
	//Установим анимацию персонажу
	if (chr.model.animation == "mushketer")
	{
        while (FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE) != "")
        {
            TakeItemFromCharacter(chr, FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE));
        }
        while (FindCharacterItemByGroup(chr, GUN_ITEM_TYPE) != "")
        {             
            TakeItemFromCharacter(chr, FindCharacterItemByGroup(chr, GUN_ITEM_TYPE));
        }		
		GiveItem2Character(chr, "unarmed");
		EquipCharacterbyItem(chr, "unarmed");
		GiveItem2Character(chr, "mushket");
		EquipCharacterbyItem(chr, "mushket");
	}
	else
	{
		LAi_SetDefaultStayAnimation(chr);
	}
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_warrior_CharacterUpdate(aref chr, float dltTime)
{
	int trg = -1;
	//Ссылка на ветку с параметрами
	aref type;
	makearef(type, chr.chr_ai.type);

    // boal  лечимся -->
	float fCheck = stf(chr.chr_ai.type.bottle) - dltTime;
	if(fCheck < 0)
	{
		chr.chr_ai.type.bottle = 5.0;
		if (!LAi_IsBottleWork(chr) && MOD_SKILL_ENEMY_RATE > 2)
		{
			string btl = "";		
			float dhlt;
			if(LAi_GetCharacterRelHP(chr) < 0.75)
			{
				dhlt = LAi_GetCharacterMaxHP(chr) - LAi_GetCharacterHP(chr);
				btl = FindHealthForCharacter(&Characters[sti(chr.index)], dhlt);
				DoCharacterUsedItem(&Characters[sti(chr.index)], btl);
				chr.chr_ai.type.bottle = 10.0;
			}
		}
	}
	else chr.chr_ai.type.bottle = fCheck;
	// boal  лечимся <--
	//Анализируем состояние
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
	{
		//Воюем
		bool isValidate = false;
		trg = LAi_tmpl_fight_GetTarget(chr);
		fCheck = stf(chr.chr_ai.type.checkTarget) - dltTime;
		chr.chr_ai.type.checkTarget = fCheck;
		if(trg >= 0)
		{			
			if(LAi_group_ValidateTarget(chr, &Characters[trg]))
			{
				if(!LAi_tmpl_fight_LostTarget(chr))
				{
					isValidate = true;
					if (stf(LAi_grp_relations.distance) > 2.0 && fCheck < 0) //цель далеко, попробуем сменить на ближайшую
					{
						isValidate = false;
					}				
				}
			}
		}
		if(!isValidate)
		{
			//Ищем новую цель
			trg = LAi_group_GetTarget(chr);
			if(trg < 0)
			{
				//Переходим в режим ожидания
				LAi_tmpl_stay_InitTemplate(chr);
				LAi_type_warrior_SetWateState(chr);
			}else{
				//Натравливаем на новую цель
				LAi_tmpl_SetFight(chr, &Characters[trg]);
				chr.chr_ai.type.checkTarget = rand(3) + 2; //таймер на проверялку расстояния до таргета
				if(rand(100) > 95)
				{
					LAi_type_warrior_PlaySound(chr);
				}
			}
		}
	}else{
		//Ищем новую цель
		trg = LAi_group_GetTarget(chr);
		if(trg >= 0)
		{
			//Нападаем на новую цель
			LAi_tmpl_SetFight(chr, &Characters[trg]);
			chr.chr_ai.type.checkTarget = rand(3) + 2; //таймер на проверялку расстояния до таргета
			if(rand(100) > 95)
			{
				LAi_type_warrior_PlaySound(chr);
			}
		}else{
			if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
			{
				//Стоим и ждём цели
				LAi_type_warrior_SetWateState(chr);
			}

		}
	}	
}

//Загрузка персонажа в локацию
bool LAi_type_warrior_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_warrior_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_warrior_TemplateComplite(aref chr, string tmpl)
{
}

//Сообщить о желании завести диалог
void LAi_type_warrior_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_warrior_CanDialog(aref chr, aref by)
{	
	if(sti(chr.chr_ai.type.dialog) == 0) return false;
	//Если просто стоим, то согласимся
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY) return true;
	if(chr.chr_ai.tmpl == LAI_TMPL_FOLLOW) return true;
	if(chr.chr_ai.tmpl == LAI_TMPL_WALK) return true;
	return false;
}

//Начать диалог
void LAi_type_warrior_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения	
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_warrior_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
}


//Персонаж выстрелил
void LAi_type_warrior_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAi_type_warrior_Attacked(aref chr, aref by)
{
	//если наносящий удар уже таргет, нефиг крутить код и переназначать цель
	if (LAi_tmpl_fight_GetTarget(chr) == sti(by.index)) return;
	//Своих пропускаем
	//--> штурм Панамы, смотрим, со спины ли выстрел
	if (chr.id == "Richard_Soukins" && by.id == "Blaze")
	{
		LAi_type_warrior_questPanama(chr);
	}
	//<-- штурм Панамы, смотрим, со спины ли выстрел
	if(!LAi_group_IsEnemy(chr, by)) return;
	LAi_group_UpdateTargets(chr);
    float dist = -1.0;	
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > 20.0) return;
	//Натравливаем
	LAi_tmpl_SetFight(chr, by);
	chr.chr_ai.type.checkTarget = rand(3)+2;
	if(rand(100) > 90)
	{
		LAi_type_warrior_PlaySound(chr);
	}
}

//Переходим в режим ожидания
void LAi_type_warrior_SetWateState(aref chr)
{
	if(sti(chr.chr_ai.type.stay) == 0)
	{
		if(chr.chr_ai.type.index != "")
		{
			int cmd = sti(chr.chr_ai.type.index);
			if(cmd >= 0)
			{
				//Возвращаемся к командиру
				LAi_tmpl_SetFollow(chr, &Characters[cmd], -1.0);
			}else{
				//Гуляем в поисках цели
				LAi_tmpl_walk_InitTemplate(chr);
			}
		}else{
			//Гуляем в поисках цели
			LAi_tmpl_walk_InitTemplate(chr);
		}
	}else{
		//Ожидаем цель стоя на месте
		if(chr.chr_ai.tmpl != LAI_TMPL_STAY)
		{
			LAi_tmpl_stay_InitTemplate(chr);
		}
	}
}

void LAi_type_warrior_PlaySound(aref chr)
{
	if(LAi_IsDead(chr) && !LAi_IsDead(pchar)) return;
	string sname = "";
	if(CheckAttribute(chr, "sex"))
	{
		switch(chr.sex)
		{
		case "man":
			sname = "warrior";
			break;
		case "woman":
			break;
		case "skeleton":
			sname = "skeleton";
			break;
		}
	}
	if(sname == "") return;
	LAi_CharacterPlaySound(chr, sname);
}

void LAi_type_warrior_questPanama(ref chr)
{
	if (pchar.questTemp.piratesLine != "Panama_inJungle" && !LAi_IsDead(chr)) return;
	bool bOk = true;
	int num = FindNearCharacters(chr, 30.0, -1.0, 180.0, 0.1, true, true);
	for(int i = 0; i < num; i++)
	{
		if(CheckAttribute(&chrFindNearCharacters[i], "index") && chrFindNearCharacters[i].index == nMainCharacterIndex)
		{
			bOk = false;
			break;
		}
	}
	if (bOk) 
	{
		LAi_SetImmortal(chr, false);
		LAi_KillCharacter(chr);
	}
	else
	{
		Log_SetStringToLog("Ричард Соукинс заметил, что вы подло напали на него. Теперь его пираты будут атаковать вас.");
		int iTemp;
		LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER_OWN);
		LAi_tmpl_fight_SetTarget(chr, pchar);
		for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
		{
			iTemp = GetCharacterIndex("RSpirate_"+i);
			if(iTemp != -1)
			{
				LAi_group_MoveCharacter(&characters[iTemp], chr.chr_ai.group);
			}
		}
		for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
		{
			iTemp = GetCharacterIndex("RSmush_"+i);
			if(iTemp != -1)
			{
				LAi_group_MoveCharacter(&characters[iTemp], chr.chr_ai.group);
			}
		}
		for (i=1; i<=3; i++)
		{
			iTemp = GetCharacterIndex("RSofficer_"+i);
			if(iTemp != -1)
			{
				LAi_group_MoveCharacter(&characters[iTemp], chr.chr_ai.group);
			}
		}
	}
}