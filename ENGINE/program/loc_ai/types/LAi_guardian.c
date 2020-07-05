/*
Тип: охраник

	Используемые шаблоны:
		stay
		talk
		fight		
		goto
*/



#define LAI_TYPE_GUARDIAN	"guardian"


//Инициализация
void LAi_type_guardian_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}else{
		if(chr.chr_ai.type != LAI_TYPE_GUARDIAN) isNew = true;
	}
	if(isNew == true)
	{
		//Новый тип
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_GUARDIAN;
		chr.chr_ai.type.enemy = "";
		chr.chr_ai.type.etime = "0";
		chr.chr_ai.type.wait = "";
		chr.chr_ai.type.dlgwas = "0.0" //для таймера на инициализацию диалога. eddy
		chr.chr_ai.type.bottle = rand(10)+2;
		//Установим шаблон стояния
		LAi_tmpl_stay_InitTemplate(chr);
	}else{
		//Востанавливаем состояние
		if(chr.chr_ai.type.wait == "attack")
		{
			LAi_type_guardian_CmdAttack(chr);
		}else{
			if(chr.chr_ai.type.wait == "return")
			{
				LAi_type_guardian_Return(chr);
			}
		}
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
	//Сохраним адрес как точку охраны
	chr.chr_ai.type.group = chr.location.group;
	chr.chr_ai.type.locator = chr.location.locator;
}

//Процессирование типа персонажа
void LAi_type_guardian_CharacterUpdate(aref chr, float dltTime)
{
	aref type;
	int trg, t;
	float time, dist;
	makearef(type, chr.chr_ai.type);
	//Режим ожидания
	if(type.wait != "") return;
	
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
	
	//Нормальная работа
	string tmpl = chr.chr_ai.tmpl;
	if(tmpl == LAI_TMPL_DIALOG) return;
	if(tmpl == LAI_TMPL_FIGHT)
	{
		bool isValidate = false;
		trg = LAi_tmpl_fight_GetTarget(chr);
		//Пробуем обновить оптимальную цель
		time = stf(type.etime) - dltTime;
		type.etime = time;
		if(trg >= 0)
		{
			if(LAi_group_ValidateTarget(chr, &Characters[trg]))
			{
				if(!LAi_tmpl_fight_LostTarget(chr))
				{
					isValidate = true;
					if (stf(LAi_grp_relations.distance) > 2.0 && time < 0) //цель далеко, попробуем сменить на ближайшую
					{
						isValidate = false;
					}
				}
			}
		}
		//Если текущая цель не действительна, получим новую
		if(!isValidate)
		{
			//Ищем новую цель
			trg = LAi_group_GetTarget(chr);
			if(trg < 0)
			{
				//Переходим в режим ожидания
				LAi_tmpl_fight_SetWaitState(chr);
				LAi_type_guardian_Return(chr);
			}else{
				//Натравливаем на новую цель
				LAi_type_guardian_CmdAttack_Now(chr, trg);
				if(rand(100) < 10)
				{
					LAi_CharacterPlaySound(chr, "warrior");
				}
			}
		}
	}else{
		if(tmpl == LAI_TMPL_STAY)
		{
			if(rand(1000) == 125)
			{
				LAi_CharacterPlaySound(chr, "sigh");
			}
			//Проверим наличие врагов
			trg = LAi_group_GetTarget(chr);
			if(trg >= 0)
			{
				chr.chr_ai.type.enemy = trg;
				LAi_type_guardian_CmdAttack(chr);
			}
			else
			{
				//eddy. активация диалога у гарда c атрибутом протектора если не враг и если это не колония базовой нации.
				if (CheckAttribute(chr, "protector") && !LAi_grp_alarmactive && sti(chr.nation) != PIRATE)
				{
					time = stf(chr.chr_ai.type.dlgwas) - dltTime;
					chr.chr_ai.type.dlgwas = time;					
					//Анализируем окружающих персонажей
					int num = FindNearCharacters(chr, 4.0, -1.0, 180.0, 0.1, true, true);
					if(num > 0)
					{
						for(int i = 0; i < num; i++)
						{
							if(nMainCharacterIndex == sti(chrFindNearCharacters[i].index))
							{
								break;
							}
						}
						if(i < num)
						{
							//Нашли главного персонажа
							if(stf(chr.chr_ai.type.dlgwas) <= 0.0)
							{								
								LAi_type_guardian_TestControl(chr);
								return;
							}
						}
						else
						{
							trg = sti(chrFindNearCharacters[0].index);
						}
					}					
				}
				//Проверяем дистанцию до точки охраны
				dist = -1.0;
				if(GetCharacterDistByLoc(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator, &dist))
				{
					if(dist > 1.0)
					{
						LAi_tmpl_runto_InitTemplate(chr);
						LAi_tmpl_runto_SetLocator(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator, -1.0);
					}else{
						CharacterTurnByLoc(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator);
					}
				}else{
					CharacterTurnByLoc(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator);
				}
			}
		}else{
			//Проверим наличие врагов
			trg = LAi_group_GetTarget(chr);
			if(trg >= 0)
			{
				chr.chr_ai.type.enemy = trg;
				LAi_type_guardian_CmdAttack(chr);
			}else{
				if(tmpl != LAI_TMPL_RUNTO)
				{
					LAi_tmpl_stay_InitTemplate(chr);
				}else{
					if(LAi_tmpl_runto_IsStay(chr))
					{
						LAi_tmpl_stay_InitTemplate(chr);
					}
				}
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_guardian_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_guardian_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_guardian_TemplateComplite(aref chr, string tmpl)
{
	LAi_tmpl_stay_InitTemplate(chr);
	CharacterTurnByLoc(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator);
}


//Сообщить о желании завести диалог
void LAi_type_guardian_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_guardian_CanDialog(aref chr, aref by)
{
	//Если просто стоим, то согласимся на диалог
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
	{
		if(LAi_CanNearEnemy(chr, 5.0)) return false;
		return true;
	}
	if(sti(by.index) == nMainCharacterIndex)
	{
		if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG)
		{
			if(LAi_tmpl_dialog_StopNPC(chr)) return true;
		}
	}
	return false;
}

//Начать диалог
void LAi_type_guardian_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_guardian_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
}

//Персонаж выстрелил
void LAi_type_guardian_Fire(aref chr, aref enemy, float kDist, bool isFindedEnemy)
{

}

//Персонаж атакован
void LAi_type_guardian_Attacked(aref chr, aref by)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG)
	{
		LAi_tmpl_dialog_StopNPC(chr);
	}
	//если наносящий удар уже таргет, нефиг крутить код и переназначать цель
	if (LAi_tmpl_fight_GetTarget(chr) == sti(by.index)) return;	
	//Своих пропускаем
	if(!LAi_group_IsEnemy(chr, by)) return;
    //boal fix ai -->
    float dist = -1.0;
	
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > 20.0) return;
	//Натравливаем
	LAi_tmpl_SetFight(chr, by);
    // boal <--
	if (rand(100) > 90)	LAi_CharacterPlaySound(chr, "warrior");
}


//------------------------------------------------------------------------------------------
//Внутреннии функции
//------------------------------------------------------------------------------------------

void LAi_type_guardian_CmdAttack(aref chr)
{
	chr.chr_ai.type.wait = "attack";
	PostEvent("Event_type_guardian_Attacked", rand(5)*100, "i", chr);
	if (rand(100) > 80 && !LAi_IsDead(chr) && !LAi_IsDead(pchar))	LAi_CharacterPlaySound(chr, "warrior");
}

#event_handler("Event_type_guardian_Attacked", "LAi_type_guardian_CmdAttack_Event");
void LAi_type_guardian_CmdAttack_Event()
{
	aref chr = GetEventData();
	if(!TestRef(chr)) return;
	chr.chr_ai.type.wait = "";
	if(chr.chr_ai.type.enemy == "") return;
	int trg = sti(chr.chr_ai.type.enemy);
	LAi_type_guardian_CmdAttack_Now(chr, trg);
}

bool LAi_type_guardian_CmdAttack_Now(aref chr, int trg)
{
	if(trg < 0)
	{
		LAi_tmpl_stay_InitTemplate(chr);
		return false;
	}
	chr.chr_ai.type.etime = 3 + rand(5);
	if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
	{
		//Несмогли инициировать шаблон
		LAi_tmpl_stay_InitTemplate(chr);
		return false;
	}
	if (rand(100) > 90 && !LAi_IsDead(chr) && !LAi_IsDead(pchar)) LAi_CharacterPlaySound(chr, "warrior");
	return true;
}

void LAi_type_guardian_Return(aref chr)
{
	chr.chr_ai.type.wait = "return";
	chr.chr_ai.type.enemy = "";
	PostEvent("Event_type_guardian_Return", rand(10)*100, "i", chr);
}

#event_handler("Event_type_guardian_Return", "LAi_type_guardian_Return_Event");
void LAi_type_guardian_Return_Event()
{
	aref chr = GetEventData();
	if(!TestRef(chr)) return;
	chr.chr_ai.type.wait = "";	
	chr.chr_ai.type.enemy = "";
	LAi_tmpl_runto_InitTemplate(chr);
	LAi_tmpl_runto_SetLocator(chr, chr.chr_ai.type.group, chr.chr_ai.type.locator, -1.0);
}

//Проверить персонажа с заданной вероятностью
void LAi_type_guardian_TestControl(aref chr)
{
	if (!CheckAttribute(chr, "protector.CheckAlways")) //флаг "опрашивать всегда" через паузу, не один раз.
	{						
		if (GetBaseHeroNation() == sti(chr.nation) && GetRelation2BaseNation(sti(chr.nation)) != RELATION_ENEMY && GetNationRelation2MainCharacter(sti(chr.nation)) != RELATION_ENEMY) return;
		if (!CheckAttribute(pchar, "CheckStateOk")) pchar.CheckStateOk = true; //флаг "уже проверили на входе"
		else return;
	}
	//Пытаемся начать диалог
	LAi_SetFightMode(pchar, false);
	if(LAi_Character_CanDialog(chr, pchar))
	{
		chr.chr_ai.type.state = "dialog";
		LAi_tmpl_SetDialog(chr, pchar, -1.0);
		chr.chr_ai.type.dlgwas = "100"; //уже поговорили
	}
}