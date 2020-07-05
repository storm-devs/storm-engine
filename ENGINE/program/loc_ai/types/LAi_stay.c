/*
Тип: стоячий, всегда стоит, отвечает на диалоги, никогда не боится

	Используемые шаблоны:
		stay
		dialog
*/



#define LAI_TYPE_STAY		"stay"


//Инициализация
void LAi_type_stay_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_STAY;
	chr.chr_ai.type.time = 0;
	LAi_tmpl_stay_InitTemplate(chr);
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
void LAi_type_stay_CharacterUpdate(aref chr, float dltTime)
{
	int num = FindNearCharacters(chr, 5.0, -1.0, -1.0, 0.001, false, true);
	int idx;
	bool bVis = false;	
	if(num > 0)
	{
		for(int i = 0; i < num; i++)
		{
			idx = sti(chrFindNearCharacters[i].index);
			if(LAi_group_IsEnemy(chr, &Characters[idx])) break;
			//квест штурма Панамы -->>
			if (chr.id == "Richard_Soukins") 
			{
				if (idx == nMainCharacterIndex)
				{
					CharacterTurnByChr(chr, &Characters[idx]);
					bVis = true;
				}
			}
			//<<--штурм Панамы
			if (CheckAttribute(chr, "talker") && idx == nMainCharacterIndex && LAi_Character_CanDialog(chr, pchar)) 
			{	
				LAi_tmpl_SetDialog(chr, pchar, 1.0); 
				DeleteAttribute(chr, "talker");
				return;
			}
		}
		//квест штурма Панамы -->>
		if (chr.id == "Richard_Soukins" && sti(!bVis)) 
		{
			LAi_SetWarriorTypeNoGroup(chr);
		}
		//<<--штурм Панамы
		if(i < num)
		{
			if(chr.chr_ai.tmpl != LAI_TMPL_ANI)
			{
				LAi_tmpl_ani_PlayAnimation(chr, "afraid", -1.0);
				LAi_SetAfraidDead(chr);
			}
		}else{
			if(chr.chr_ai.tmpl != LAI_TMPL_STAY)
			{
				LAi_tmpl_stay_InitTemplate(chr);
				LAi_SetDefaultDead(chr);
			}
		}
	}
	else
	{
		if(chr.chr_ai.tmpl != LAI_TMPL_STAY)
		{
			LAi_tmpl_stay_InitTemplate(chr);
			LAi_SetDefaultDead(chr);
		}
		//квест штурма Панамы -->>
		if (chr.id == "Richard_Soukins") 
		{
			float dist = -1.0;	
			if(!GetCharacterDistByChr3D(chr, pchar, &dist)) return;
			if (dist > 12.5) LAi_SetWarriorTypeNoGroup(chr);
		}
		//<<--штурм Панамы		
	}
}

//Загрузка персонажа в локацию
bool LAi_type_stay_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_stay_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_stay_TemplateComplite(aref chr, string tmpl)
{
}

//Сообщить о желании завести диалог
void LAi_type_stay_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_stay_CanDialog(aref chr, aref by)
{
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl != LAI_TMPL_STAY && by.id != "Blaze") return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_stay_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr); 
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_stay_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
}


//Персонаж выстрелил
void LAi_type_stay_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAi_type_stay_Attacked(aref chr, aref by)
{
	//--> штурм Панамы, смотрим, со спины ли выстрел
	if (chr.id == "Richard_Soukins" && pchar.questTemp.piratesLine == "Panama_inJungle")
	{
		if (by.id == "Blaze")
		{
			LAi_SetWarriorType(chr);
			LAi_type_warrior_questPanama(chr);
			DeleteAttribute(pchar, "PostEventQuest.questDelay.PQ8_RichardIsWarrior");
		}
		else
		{
			LAi_SetWarriorTypeNoGroup(chr);
		}
	}
	//<-- штурм Панамы, смотрим, со спины ли выстрел	
}

