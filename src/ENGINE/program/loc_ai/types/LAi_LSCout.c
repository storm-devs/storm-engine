/*
Тип: стоячий, всегда стоит, отвечает на диалоги, боиться, если безоружен

	Используемые шаблоны:
		stay
		dialog
		goto
		afraid
*/



#define LAI_TYPE_LSCOUT		"LSCout"


//Инициализация
void LAI_type_LSCout_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}
	else
	{
		if(chr.chr_ai.type != LAI_TYPE_LSCOUT) isNew = true;
	}
	if(isNew)
	{
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_LSCOUT;
		chr.chr_ai.type.locator = "";
		chr.chr_ai.type.state = "stay";
		chr.chr_ai.type.time = rand(5);
		chr.chr_ai.type.notalk = rand(10);
		chr.chr_ai.type.look_around = 0;
		chr.location.going = true; //флаг перенаправления
		LAi_tmpl_stay_InitTemplate(chr);
	}
	else
	{
		if(!CheckAttribute(chr, "chr_ai.type.locator")) chr.chr_ai.type.locator = "";
		if(!CheckAttribute(chr, "chr_ai.type.state")) chr.chr_ai.type.state = "stay";
		if(!CheckAttribute(chr, "chr_ai.type.time")) chr.chr_ai.type.time = rand(5);
		if(!CheckAttribute(chr, "chr_ai.type.player")) chr.chr_ai.type.player = "0";
	}
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAI_TYPE_LSCout_CharacterUpdate(aref chr, float dltTime)
{
	//Log_SetEternalString("Темплейт: " + chr.chr_ai.tmpl + "   Стейт: " + chr.chr_ai.tmpl.state);
	//Если болтаем, то ничего пока не меняем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return;
	
	if(chr.chr_ai.tmpl != LAI_TMPL_FIGHT && chr.chr_ai.tmpl != LAI_TMPL_AFRAID)
	{
		bool isDialog = false;		
		//Ищем цель
		int trg = LAi_group_GetTarget(chr);
		if(trg < 0)
		{			
			float time = stf(chr.chr_ai.type.time) - dltTime;
			chr.chr_ai.type.time = time;
			chr.chr_ai.type.look_around = stf(chr.chr_ai.type.look_around) - dltTime;
			chr.chr_ai.type.notalk = stf(chr.chr_ai.type.notalk) - dltTime;
			if(stf(chr.chr_ai.type.look_around) < 0.0)
			{
				chr.chr_ai.type.look_around = 1.0; //время частоты сканирования
				int num = FindNearCharacters(chr, 3.0, -1.0, 180.0, 0.1, true, true);
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
						if(CheckAttribute(chr, "talker"))
						{
							LAi_tmpl_SetDialog(chr, pchar, -1.0);
							DeleteAttribute(chr, "talker");
							return;
						}
						else CharacterTurnByChr(chr, pchar);
					}
					else
					{	//если не ГГ, то ближайший нпс (сортировка)				
						trg = sti(chrFindNearCharacters[0].index);
						if (stf(chrFindNearCharacters[0].dist) < 2.5 && stf(chr.chr_ai.type.notalk) <= 0.0) 
						{
							isDialog = true;
						}
					}
				}
			}
			//Анализируем состояние ходьбы
			if(chr.chr_ai.tmpl == LAI_TMPL_GOTO)
			{
				if (isDialog && rand(1))
				{
					if(LAi_Character_CanDialog(chr, &Characters[trg]))
					{
						chr.chr_ai.type.notalk = 15.0 + rand(20);
						Characters[trg].chr_ai.type.notalk = chr.chr_ai.type.notalk;
						if(!LAi_tmpl_SetDialog(chr, &Characters[trg], 3.0 + rand(10)))
						{
							LAI_TYPE_LSCout_Goto(chr);
						}
					}
				}			
				if(LAi_tmpl_goto_IsWait(chr))
				{
					if(rand(100) < 20) LAI_TYPE_LSCout_Goto(chr);
				}
				if (chr.chr_ai.tmpl.state == "wait") LAI_TYPE_LSCout_Goto(chr);
				if (chr.chr_ai.tmpl.state == "falure") LAI_TYPE_LSCout_Goto(chr);
				return;
			}
			//Иногда звучим
			if(rand(1000) == 125)
			{
				LAi_CharacterPlaySound(chr, "sigh");
			}
			//Стоим
			if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
			{
				if(trg >= 0) 
				{
					if (isDialog && rand(1))
					{
						ref by = &Characters[trg];
						if(LAi_Character_CanDialog(chr, by))
						{
							chr.chr_ai.type.notalk = 15.0 + rand(20);
							by.chr_ai.type.notalk = chr.chr_ai.type.notalk;
							if(!LAi_tmpl_SetDialog(chr, by, 3.0 + rand(10)))
							{
								LAI_TYPE_LSCout_Goto(chr);
							}
						}
					}
					else
					{
						CharacterTurnByChr(chr, &Characters[trg]);
					}
				}
				else
				{
					if (time < 0.0)
					{
						LAI_TYPE_LSCout_Goto(chr);
					}
				}
			}

		}else{
			//Начинаем атаку
			if(LAi_IsSetBale(chr))
			{
				chr.chr_ai.type.state = "fight";
				if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
				{
					//Несмогли инициировать шаблон
					LAI_TYPE_LSCout_Stay(chr);
				}
			}else{
				//Боимся
				LAi_tmpl_afraid_SetAfraidCharacter(chr, &Characters[trg], true);
				LAi_SetAfraidDead(chr);
			}
		}
	}else{
		if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
		{
			//Проверим на правильность цель
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
				//Ищем новую цель
				trg = LAi_group_GetTarget(chr);
				if(trg >= 0)
				{
					chr.chr_ai.type.state = "fight";
					if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
					{
						//Несмогли инициировать шаблон
						LAI_TYPE_LSCout_Stay(chr);
					}
				}else{
					LAI_TYPE_LSCout_Stay(chr);
				}
			}
		}
		else
		{
			if(chr.chr_ai.tmpl == LAI_TMPL_AFRAID)
			{
				if(LAi_tmpl_afraid_IsNoActive(chr) && !LAi_grp_alarmactive)
				{
					//Вернёмся к нормальной жизни
					LAi_SetDefaultDead(chr);
					LAI_TYPE_LSCout_Goto(chr);
				}
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAI_TYPE_LSCout_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAI_TYPE_LSCout_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAI_TYPE_LSCout_TemplateComplite(aref chr, string tmpl)
{	
	if(tmpl == "goto")
	{
		LAI_TYPE_LSCout_Stay(chr);
		CharacterTurnByLoc(chr, "goto", chr.chr_ai.type.locator);
		chr.chr_ai.type.locator = "";
	}
}

//Сообщить о желании завести диалог
void LAI_TYPE_LSCout_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAI_TYPE_LSCout_CanDialog(aref chr, aref by)
{
	if(chr.chr_ai.type.state == "dialog")
	{
		if(sti(by.index) == nMainCharacterIndex)
		{
			chr.chr_ai.type.state = "stay";
			return true;
		}
		return false;
	}
	if(LAi_CanNearEnemy(chr, 5.0)) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY) return true;
	if(chr.chr_ai.tmpl == LAI_TMPL_GOTO) return true;
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
void LAI_TYPE_LSCout_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAI_TYPE_LSCout_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
	bool isSet = false;
	if(chr.chr_ai.type.state == "goto")
	{
		if(chr.chr_ai.type.locator != "")
		{
			LAi_tmpl_goto_InitTemplate(chr);
			LAi_tmpl_goto_SetLocator(chr, "goto", chr.chr_ai.type.locator, -1.0);
			isSet = true;
		}
	}
	if(!isSet)
	{
		LAI_TYPE_LSCout_Stay(chr);
	}
}


//Персонаж выстрелил
void LAI_TYPE_LSCout_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAI_TYPE_LSCout_Attacked(aref chr, aref by)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG)
	{
		LAi_tmpl_dialog_StopNPC(chr);
	}	
	//Своих пропускаем
	if(!LAi_group_IsEnemy(chr, by)) return;
    //boal fix ai -->
    float dist = -1.0;
	
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > 20.0) return;
	//Натравливаем
	if(LAi_IsSetBale(chr))
	{
		chr.chr_ai.type.state = "fight";
		if(!LAi_tmpl_SetFight(chr, by))
		{
			//Несмогли инициировать шаблон
			LAI_TYPE_LSCout_Stay(chr);
		}
	}else{
		//Боимся
		LAi_tmpl_afraid_SetAfraidCharacter(chr, by, true);
		LAi_SetAfraidDead(chr);
	}
    // boal <--
}

//Остановится
void LAI_TYPE_LSCout_Stay(aref chr)
{
	chr.chr_ai.type.time = 2 + rand(20);
	chr.chr_ai.type.state = "stay";
	LAi_tmpl_stay_InitTemplate(chr);
}

//Отправить персонажа в новую точку
void LAI_TYPE_LSCout_Goto(aref chr)
{
	//Идём в новую точку в городе
	string newloc = chr.location.locator;
	int locQty = 9;
	newloc = strcut(newloc, 4, 5);
	//переадресация на другой корабль
	if (rand(10) == 5 && sti(chr.location.baseShip.going)) 
	{
		int iTemp = rand(15)+1;
		if (iTemp < 10) newloc = "0" + iTemp;
		else newloc = "" + iTemp;
	}
	if (newloc == "06" || newloc == "07" || newloc == "10" || newloc == "12" || newloc == "13" || newloc == "16")
	{
		locQty = 4;
	}
	newloc = "goto" + newloc + "_" + rand(locQty);
	if(newloc != "")
	{
		LAi_tmpl_goto_InitTemplate(chr);
		LAi_tmpl_goto_SetLocator(chr, "goto", newloc, -1.0);
		chr.chr_ai.type.state = "goto";
		chr.chr_ai.type.locator = newloc;
	}	
}


//Установка локаторов в ГПК
// - На каждый корабль устанавливается по 5 или 10 локаторов goto
// - формат локаторов - goto01_0. goto - база в имени локатора, 01 - номер судна (от 01 до 16), 1 - номер локатора (0-5 или 0-9)
// Нумерация кораблей:
// 01 - Бэттлшип Тартарус   TartarusPrison
// 02 - Фрегат Каролина   CarolineBank
// 03 - Флейт Церес Смити   CeresSmithy
// 04 - Галеон Эсмеральда   EsmeraldaStoreBig
// 05 - Каравелла Флерон   FleuronTavern
// 06,07 - Пинас Фурия  FurieShipInside2
// 08 - Галеон Глория  GloriaChurch
// 09 - Флейт  PlutoStoreSmall
// 10 - Корвет Протектор  ProtectorFisher
// 11 - Варшип Сан Августин  SanAugustineResidence
// 12,13 - Барк Сан Габриель   SanGabrielMechanic
// 14 - Бриг Санта Флорентина  SantaFlorentinaShipInside4
// 15 - Галеон Эва AvaShipInside3
// 16 - Галеон Веласко  VelascoShipInside1
