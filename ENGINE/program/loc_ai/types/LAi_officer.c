/*
Тип: офицер

	Используемые шаблоны:
		stay
		dialog
*/



#define LAI_TYPE_OFFICER		"officer"


//Инициализация
void LAi_type_officer_Init(aref chr)
{
	//Новый тип
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_OFFICER;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//команда ждать!
		{
	    	if (!LAi_IsFightMode(GetMainCharacter()))
	        {
	    	    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
	    	}
	    	else
	    	{
	            LAi_tmpl_fight_SetWaitState(chr);
	    	}
		}
	}
	else
	{
     	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	}
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

void LAi_type_follower_Init(aref chr)
{
	//Новый тип
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = "follower";
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_officer_CharacterUpdate(aref chr, float dltTime)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return;
	string btl = "";
	//При отравлении детравимся
	if(CheckAttribute(chr, "chr_ai.poison"))
	{
		if(LAi_GetCharacterRelHP(chr) < 0.8)
		{
			bool antidotUse = false;
			if(LAi_GetCharacterRelHP(chr) < 0.6)
			{
				if(GetCharacterItem(&Characters[sti(chr.index)], "potion4") > 0)
				{
					antidotUse = true;
					DoCharacterUsedItem(&Characters[sti(chr.index)], "potion4");
				}
			}
			if(!antidotUse)
			{
				if(GetCharacterItem(&Characters[sti(chr.index)], "potion3") > 0)
				{
					DoCharacterUsedItem(&Characters[sti(chr.index)], "potion3");
				}
			}
		}
	}
	//Дистанция до главного персонажа
	float dist = 0.0;
	float dhlt;
	if(!GetCharacterDistByChr3D(chr, pchar, &dist)) dist = -1.0;
	if(!LAi_IsBottleWork(chr))
	{
		if(LAi_GetCharacterRelHP(chr) < 0.7)
		{
			dhlt = LAi_GetCharacterMaxHP(chr) - LAi_GetCharacterHP(chr);
			btl = FindHealthForCharacter(&Characters[sti(chr.index)], dhlt);
			DoCharacterUsedItem(&Characters[sti(chr.index)], btl);
		}
	}
	//Log_Info("LAi_type_officer_CharacterUpdate "+chr.chr_ai.tmpl);
	if (chr.chr_ai.tmpl == LAI_TMPL_STAY) return; // приказ ему стоять. Врага не ищем 18.06.05
	
	//Дистанция до главного персонажа
	if(!GetCharacterDistByChr3D(chr, GetMainCharacter(), &dist)) dist = -1.0;
	if (dist < 0.0 || dist > 30)
	{
        //if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
        //{
            LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
    		return;
		//}
	}
	LAi_type_officer_FindTarget(chr);   // fix
	//Текущее действие
	if(chr.chr_ai.tmpl == LAI_TMPL_FOLLOW)
	{
		if(dist < 30.0)
		{
			LAi_type_officer_FindTarget(chr);
		}
	}else{
		if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
		{
			//Смотрим на использование бутылочки
			/*if(!LAi_IsBottleWork(chr)) //никогда ветка не сработает, тк выше пьем уже
			{
				if(rand(100) < (50 + 49*LAi_GetCharacterFightLevel(chr)))
				{
					float hlt = MinHealthPotionForCharacter(&Characters[sti(chr.index)]);
					if(LAi_GetCharacterRelHP(chr) < 0.5) hlt = dhlt;
					dhlt = LAi_GetCharacterMaxHP(chr) - LAi_GetCharacterHP(chr);
					if(hlt <= dhlt)
					{
						btl = FindHealthForCharacter(&Characters[sti(chr.index)], dhlt);
						DoCharacterUsedItem(&Characters[sti(chr.index)], btl);
					}					
				}
			}  */
			//
			if(LAi_tmpl_fight_LostTarget(chr))
			{
				LAi_type_officer_FindTarget(chr);
			}else{
				if(LAi_tmpl_fight_IsGo(chr))
				{
					if(dist > 12.0)
					{
						//Пора возвращаться
						if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
                        {
						    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
						}
					}
				}
			}
		}
		else
		{
		    LAi_type_officer_FindTarget(chr);
		}
	}
}

void LAi_type_follower_CharacterUpdate(aref chr, float dltTime)
{
}

//Загрузка персонажа в локацию
bool LAi_type_officer_CharacterLogin(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_officer_CharacterLogoff(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_officer_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
    {
        if (chr.chr_ai.tmpl != LAI_TMPL_STAY && SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
        {
            LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
        }
    }
}

void LAi_type_follower_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW) LAi_tmpl_SetFollow(chr, pchar, -1.0);
}

//Сообщить о желании завести диалог
void LAi_type_officer_NeedDialog(aref chr, aref by)
{
}

void LAi_type_follower_NeedDialog(aref chr, aref by)
{
}


//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_officer_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//Согласимся на диалог
	return true;
}

bool LAi_type_follower_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_officer_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

void LAi_type_follower_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_officer_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//команда ждать!
	   LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
}

void LAi_type_follower_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
}


//Персонаж выстрелил
void LAi_type_officer_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//Персонаж атакован
void LAi_type_officer_Attacked(aref chr, aref by)
{
	//Своих пропускаем
	if(!LAi_group_IsEnemy(chr, by)) return;
	float dist = -1.0;
	//Проверяем возможность смены цели
// boal всегда менять цель, а не тупо бежать на месте
	/*if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
	{
		bool canChange = false;
		if(LAi_tmpl_fight_IsGo(chr)) canChange = true;
		if(LAi_tmpl_fight_LostTarget(chr)) canChange = true;
		if(LAi_tmpl_fight_NoActive(chr)) canChange = true;
		if(!canChange) return;
		dist = -1.0;
		if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
		if(dist >= 0.0)
		{
			if(dist < 5.0) return;
		}
	}*/
	//Определяем дистанцию до нового противника
	dist = -1.0;
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > 20.0) return;
	//Натравливаем
	if(!LAi_tmpl_SetFight(chr, by))
	{
		LAi_type_officer_FindTarget(chr);
	}
}


void LAi_type_officer_FindTarget(aref chr)
{
	//Проверим наличие врагов
	int trg = LAi_group_GetTarget(chr);
	if(trg >= 0)
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			if(LAi_tmpl_SetFight(chr, &Characters[trg])) return;
		}
	}
	trg = LAi_group_GetTarget(pchar);
	if(trg >= 0)
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
			{
				//Несмогли инициировать шаблон
				//LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
				LAi_tmpl_fight_SetWaitState(chr);
			}
		}
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
		{
            if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
            {
                LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
            }
		}
	}
}

bool LAi_type_officer_CheckDists(aref chr, aref trg)
{
	float dist = -1.0;
	if(!GetCharacterDistByChr3D(pchar, trg, &dist)) dist = -1.0;
	if(dist >= 0.0 && dist < 30.0)
	{
		if(!GetCharacterDistByChr3D(chr, trg, &dist)) dist = -1.0;
		if(dist >= 0.0 && dist < 30.0)
		{
			return true;
		}
	}
	return false;
}

