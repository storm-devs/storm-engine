

#define LAI_TYPE_MONKEY		"monkey"


//Инициализация
void LAi_type_monkey_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_MONKEY;
	LAi_tmpl_stay_InitTemplate(chr);
	//Установим анимацию персонажу
	BeginChangeCharacterActions(chr);
	chr.actions.idle.i1 = "idle_1";
	chr.actions.idle.i2 = "idle_2";
	chr.actions.idle.i3 = "idle_3";
	chr.actions.walk = "walk";
	chr.actions.backwalk = "back walk";
	chr.actions.run = "walk";
	chr.actions.backrun = "back walk";
	chr.actions.stsUp = "walk";
	chr.actions.stsDown = "walk";
	chr.actions.stsUpBack = "back walk";
	chr.actions.stsDownBack = "back walk";
	chr.actions.stsUpRun = "walk";
	chr.actions.stsDownRun = "walk";
	chr.actions.stsUpRunBack = "back walk";
	chr.actions.stsDownRunBack = "back walk";
	chr.actions.turnLeft = "skip";
	chr.actions.turnRight = "skip";
	chr.actions.swim = "walk";	
	chr.actions.fightwalk = "walk";
	chr.actions.fightbackwalk = "back walk";
	chr.actions.fightrun = "walk";
	chr.actions.fightbackrun = "back walk";
	chr.actions.fightidle.i1 = "idle_1";
	chr.actions.fightidle.i2 = "idle_2";
	chr.actions.fightidle.i3 = "idle_3";
	chr.actions.attack.a1 = "attack";
	chr.actions.attacktl.a1 = "attack";
	chr.actions.attacktr.a1 = "attack";
	chr.actions.hit.h1 = "hit";
	chr.actions.block = "block";
	chr.actions.blockhit = "block_hit";
	chr.actions.recoil = "Recoil";
	chr.actions.shot = "skip";
	chr.actions.dead.d1 = "death";
	chr.actions.fightdead.d1 = "death";
	EndChangeCharacterActions(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", true);
}

//Процессирование типа персонажа
void LAi_type_monkey_CharacterUpdate(aref chr, float dltTime)
{
	int trg = -1;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
	{
		//Воюем
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
			if(trg < 0)
			{
				//Переходим в режим ожидания
				LAi_type_monkey_Return(chr);
			}else{
				//Натравливаем на новую цель
				LAi_tmpl_SetFight(chr, &Characters[trg]);
				if(rand(100) < 50)
				{
					LAi_CharacterPlaySound(chr, "monkey");
				}
			}
		}else{
			if(rand(4000) == 1224)
			{
				LAi_CharacterPlaySound(chr, "monkey");
			}
		}
	}else{
		//Ищем новую цель
		trg = LAi_group_GetTarget(chr);
		if(trg >= 0)
		{
			//Нападаем на новую цель
			LAi_tmpl_SetFight(chr, &Characters[trg]);
			if(rand(100) < 90)
			{
				LAi_CharacterPlaySound(chr, "monkey");
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_monkey_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_monkey_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_monkey_TemplateComplite(aref chr, string tmpl)
{
	if(tmpl == "goto")
	{
		LAi_tmpl_stay_InitTemplate(chr);
		if(CheckAttribute(chr, "location.group"))
		{
			if(CheckAttribute(chr, "location.locator"))
			{
				CharacterTurnByLoc(chr, chr.location.group, chr.location.locator);
			}
		}
	}
}

//Сообщить о желании завести диалог
void LAi_type_monkey_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_monkey_CanDialog(aref chr, aref by)
{
	return false;
}

//Начать диалог
void LAi_type_monkey_StartDialog(aref chr, aref by)
{
}

//Закончить диалог
void LAi_type_monkey_EndDialog(aref chr, aref by)
{
}
/*
//Персонаж атаковал другого персонажа
void LAi_type_monkey_Attack(aref attack, aref enemy, float attackDmg, float hitDmg)
{
	if(rand(1000) < 100)
	{
		//Отравляем персонажа
		float poison = 0.0;
		if(CheckAttribute(enemy, "chr_ai.poison"))
		{
			poison = stf(enemy.chr_ai.poison);
			if(poison < 1.0) poison = 1.0;
		}		
		enemy.chr_ai.poison = poison + 30 + rand(20);
		Log_SetStringToLog(XI_ConvertString("You've been poisoned"));
	}
}*/

//Персонаж выстрелил
void LAi_type_monkey_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{
}


//Персонаж атакован
void LAi_type_monkey_Attacked(aref chr, aref by)
{
	
}

void LAi_type_monkey_Return(aref chr)
{
	bool isSet = false;
	if(CheckAttribute(chr, "location.group"))
	{
		if(CheckAttribute(chr, "location.locator"))
		{
			isSet = true;
			CharacterTurnByLoc(chr, chr.location.group, chr.location.locator);
		}
	}
	if(isSet)
	{
		LAi_tmpl_goto_InitTemplate(chr);
		LAi_tmpl_goto_SetLocator(chr, chr.location.group, chr.location.locator, -1.0);
	}else{
		LAi_tmpl_stay_InitTemplate(chr);
	}
}