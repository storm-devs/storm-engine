

#define LAI_TYPE_CARRY	"carrier"


//Инициализация
void LAi_type_carrier_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	//Установим анимацию персонажу
	//LAi_SetDefaultStayAnimation(chr);
	string sTemp = chr.model;
	sTemp =	strcut(sTemp, 0, strlen(sTemp)-2);
	BeginChangeCharacterActions(chr);
	chr.actions.idle.i1 = sTemp;
	chr.actions.nfhit	= sTemp;
	chr.actions.walk = sTemp;
	chr.actions.stsUp = "Stairs_" + sTemp;
	chr.actions.stsDown = "Stairs_" + sTemp;
	chr.actions.turnLeft = sTemp;
	chr.actions.turnRight = sTemp;
	EndChangeCharacterActions(chr);
	//Установим шаблон идти
	chr.chr_ai.type = LAI_TYPE_CARRY;
	chr.chr_ai.type.checkFight = rand(5)+2;
	//определим маршрут и отправим в путь	
	if (CheckAttribute(chr, "gotoGroup") && CheckAttribute(chr, "gotoLocator"))
	{
		LAi_tmpl_goto_InitTemplate(chr);
		LAi_tmpl_goto_SetLocator(chr, chr.gotoGroup, chr.gotoLocator, -1);
	}
	else
	{		
		LAi_tmpl_stay_InitTemplate(chr);
	}
	chr.chr_ai.tmpl.wait = 0;  //таймер ожидания
	chr.chr_ai.tmpl.time = 0; 
	chr.chr_ai.tmpl.baseLocator = "";  //откуда идем
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_carrier_CharacterUpdate(aref chr, float dltTime)
{	
	float time;
	if(chr.chr_ai.tmpl == LAI_TMPL_GOTO)
	{
		if (chr.chr_ai.tmpl.state == "goto" && CheckAttribute(chr, "checkMove"))
		{
			time = stf(chr.chr_ai.tmpl.time) + dltTime;
			chr.chr_ai.tmpl.time = time;
			if (time > 1.0)
			{
				float locx, locy, locz;
				GetCharacterPos(chr, &locx, &locy, &locz);
				if (locx == stf(chr.checkMove.locx) && locy == stf(chr.checkMove.locy) && locz == stf(chr.checkMove.locz))
				{
					ChangeCharacterAddressGroup(chr, chr.location, "reload", "gate");
					LAi_tmpl_goto_Complite(chr);
				}
				else
				{
					chr.chr_ai.tmpl.time = 0;
				}
				DeleteAttribute(chr, "checkMove");
			}
		}
		if (chr.chr_ai.tmpl.state == "falure")
		{
			ChangeCharacterAddressGroup(chr, chr.location, "reload", "gate");
			LAi_tmpl_goto_Complite(chr);
		}
		if (chr.chr_ai.tmpl.state == "stay")
		{
			time = stf(chr.chr_ai.tmpl.wait) + dltTime;
			chr.chr_ai.tmpl.wait = time;
			if (time > 40)
			{
				ChangeCharacterAddressGroup(chr, chr.location, "reload", chr.chr_ai.tmpl.baseLocator);
				LAi_tmpl_goto_InitTemplate(chr);
				LAi_type_carrier_GoTo(chr);
			}
		}
		//--> проверяем не врагов, но дерущихся. 
		if (stf(chr.chr_ai.type.checkFight) < 0.0)
		{
			int num = FindNearCharacters(chr, 5.0, -1.0, -1.0, 0.001, false, true);				
			for(int i = 0; i < num; i++)
			{
				int idx = sti(chrFindNearCharacters[i].index);
				ref by = &Characters[idx];
				chr.chr_ai.type.checkFight = 4.0;
				if (by.chr_ai.tmpl == LAI_TMPL_FIGHT)
				{
					if (LAi_grp_alarmactive)
					{
						ChangeCharacterAddress(chr, "none", "");
					}
					else
					{
						ChangeCharacterAddressGroup(chr, chr.location, "reload", "gate");
						LAi_tmpl_goto_Complite(chr);
					}
				}
			}
		}
		else 
		{
			chr.chr_ai.type.checkFight = stf(chr.chr_ai.type.checkFight) - dltTime;
		}
		//<-- проверяем не врагов, но дерущихся.
	}
	//не должно быть, но мало ли..
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
	{
		time = stf(chr.chr_ai.tmpl.wait) + dltTime;
		chr.chr_ai.tmpl.wait = time;
		if (time > 0.5)
		{
			LAi_tmpl_goto_InitTemplate(chr);
			LAi_type_carrier_GoTo(chr);
			chr.chr_ai.tmpl.wait = 0;
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_carrier_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_carrier_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_carrier_TemplateComplite(aref chr, string tmpl)
{

}

//Сообщить о желании завести диалог
void LAi_type_carrier_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_carrier_CanDialog(aref chr, aref by)
{
	return false;
}

//Начать диалог
void LAi_type_carrier_StartDialog(aref chr, aref by)
{

}

//Закончить диалог
void LAi_type_carrier_EndDialog(aref chr, aref by)
{

}

//Персонаж выстрелил
void LAi_type_carrier_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//Персонаж атакован
void LAi_type_carrier_Attacked(aref chr, aref by)
{

}


int LAi_type_carrier_FindNearEnemy(aref chr)
{
	if(LAi_grp_alarmactive == true)
	{
		int num = FindNearCharacters(chr, 15.0, -1.0, -1.0, 0.001, false, true);
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

void LAi_type_carrier_GoTo(aref chr)
{
	float locx, locy, locz;
	GetCharacterPos(chr, &locx, &locy, &locz);
	chr.chr_ai.tmpl.wait = 0;
	chr.checkMove.locx = locx;
	chr.checkMove.locy = locy;
	chr.checkMove.locz = locz;
	chr.chr_ai.tmpl.baseLocator = LAi_type_carrier_SetPath(chr) + "_back";
	LAi_tmpl_goto_InitTemplate(chr);
	LAi_tmpl_goto_SetLocator(chr, "reload", chr.chr_ai.tmpl.baseLocator, -1.0);
}

string LAi_type_carrier_SetPath(aref chr)
{
	//определим маршрут
	string locReload[8];
	locReload[0] = "reload4";
	locReload[1] = "reload5";
	locReload[2] = "reload6";
	locReload[3] = "reload7";
	locReload[4] = "reload8";
	locReload[5] = "reload10";
	locReload[6] = "reload3";
	locReload[7] = "reload9";
	ref loc = &locations[reload_location_index];
	aref aLocator;
	makearef(aLocator, loc.locators.reload);
	int iNum;
	for (int i=0; i<10; i++)
	{
		iNum = rand(7);
		if (CheckAttribute(aLocator, locReload[iNum]) && locReload[iNum] != chr.location.locator) break;
	}
	return locReload[iNum];
}
