/*
Тип: ходячий губернатор

	Используемые шаблоны:
		stay
		dialog
		goto
*/



#define LAI_TYPE_HUBERSTAY		"huberstay"


//Инициализация
void LAi_type_huberstay_Init(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}else{
		if(chr.chr_ai.type != LAI_TYPE_HUBERSTAY) isNew = true;
	}
	if(isNew == true)
	{
		//Новый тип
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_HUBERSTAY;
		chr.chr_ai.type.state = "stay";
		chr.chr_ai.type.time = "0";
		chr.chr_ai.type.win_group = "";
		chr.chr_ai.type.win_locator = "";
		chr.chr_ai.type.map_group = "";
		chr.chr_ai.type.map_locator = "";
		chr.chr_ai.type.curpos = "main";
		LAi_tmpl_stay_InitTemplate(chr);
	}else{
		if(!CheckAttribute(chr, "chr_ai.type.state")) chr.chr_ai.type.state = "stay";
		if(!CheckAttribute(chr, "chr_ai.type.time")) chr.chr_ai.type.time = "0";
		if(!CheckAttribute(chr, "chr_ai.type.win_group")) chr.chr_ai.type.win_group = "";
		if(!CheckAttribute(chr, "chr_ai.type.win_locator")) chr.chr_ai.type.win_locator = "";
		if(!CheckAttribute(chr, "chr_ai.type.map_group")) chr.chr_ai.type.map_group = "";
		if(!CheckAttribute(chr, "chr_ai.type.map_locator")) chr.chr_ai.type.map_locator = "";
		if(!CheckAttribute(chr, "chr_ai.type.curpos")) chr.chr_ai.type.curpos = "main";
	}
	//Установим анимацию персонажу
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
	//Точка основного стояния
	chr.chr_ai.type.main_group = chr.location.group;
	chr.chr_ai.type.main_locator = chr.location.locator;
}

//Процессирование типа персонажа
void LAi_type_huberstay_CharacterUpdate(aref chr, float dltTime)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_STAY)
	{
		//Иногда звучим
		if(rand(1000) == 525)
		{
			LAi_CharacterPlaySound(chr, "governor");
		}
		//Посчитаем время стояния
		float time = stf(chr.chr_ai.type.time) + dltTime;
		chr.chr_ai.type.time = time;
		//Проверим удалённость от локатора стояния
		string grp, loc;
		float maxTime = 15.0;
		if (CheckAttribute(chr, "chr_ai.type.curpos"))
		{
			switch(chr.chr_ai.type.curpos)
			{
			case "main":
				grp = chr.chr_ai.type.main_group;
				loc = chr.chr_ai.type.main_locator;
				maxTime = 40.0;
				break;
			case "map":
				grp = chr.chr_ai.type.map_group;
				loc = chr.chr_ai.type.map_locator;
				maxTime = 30.0;
				break;
			case "win":
				grp = chr.chr_ai.type.win_group;
				loc = chr.chr_ai.type.win_locator;
				maxTime = 20.0;
				break;
			}
		}
		float dist = -1.0;
		if(GetCharacterDistByLoc(chr, grp, loc, &dist))
		{
			//Вернёмся на локатор
			if(dist > 1.0)
			{
				LAi_tmpl_goto_InitTemplate(chr);
				LAi_tmpl_goto_SetLocator(chr, grp, loc, -1.0);
				return;
			}
		}else{
			chr.chr_ai.type.curpos = "main";
			chr.chr_ai.type.time = "0";
			time = 0;
		}
		//Ориентируем по локатору
		CharacterTurnByLoc(chr, grp, loc);
		//Проверяем возможность
		if(time > maxTime)
		{
			//Выбираем новую точку
			int rpnt = rand(6);
			string pnt = "main";
			if(rpnt < 3)
			{
				if(chr.chr_ai.type.map_group != "")
				{
					if(chr.chr_ai.type.map_locator != "")
					{
						pnt = "map";
					}
				}
			}
			if(rpnt == 4)
			{
				if(chr.chr_ai.type.win_group != "")
				{
					if(chr.chr_ai.type.win_locator != "")
					{
						pnt = "win";
					}
				}
			}
			if(pnt != chr.chr_ai.type.curpos)
			{
				//Установим новую точку
				chr.chr_ai.type.curpos = pnt;
				chr.chr_ai.type.time = "0";
			}
		}
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG)
		{
			if(chr.chr_ai.tmpl != LAI_TMPL_GOTO)
			{
				LAi_tmpl_stay_InitTemplate(chr);
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_huberstay_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_huberstay_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_huberstay_TemplateComplite(aref chr, string tmpl)
{
	LAi_tmpl_stay_InitTemplate(chr);
}

//Сообщить о желании завести диалог
void LAi_type_huberstay_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_huberstay_CanDialog(aref chr, aref by)
{
	//Если уже говорим, то откажем
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	//Согласимся на диалог
	return true;
}

//Начать диалог
void LAi_type_huberstay_StartDialog(aref chr, aref by)
{
	//Если мы пасивны, запускаем шаблон без времени завершения
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//Закончить диалог
void LAi_type_huberstay_EndDialog(aref chr, aref by)
{
	LAi_tmpl_stay_InitTemplate(chr);
	LAi_CharacterRestoreAy(chr);
}

//Персонаж выстрелил
void LAi_type_huberstay_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//Персонаж атакован
void LAi_type_huberstay_Attacked(aref chr, aref by)
{
	
}

//------------------------------------------------------------------------------------------
//Scripters ai program interface
//------------------------------------------------------------------------------------------

//Указать губернатору точку возле карты
void LAi_SetStayHuberPointMap(aref chr, string group, string locator)
{
	if(chr.chr_ai.type == LAI_TYPE_HUBERSTAY)
	{
		chr.chr_ai.type.map_group = group;
		chr.chr_ai.type.map_locator = locator;
	}
}

//Указать губернатору точку возле окна
void LAi_SetStayHuberPointWindow(aref chr, string group, string locator)
{
	if(chr.chr_ai.type == LAI_TYPE_HUBERSTAY)
	{
		chr.chr_ai.type.win_group = group;
		chr.chr_ai.type.win_locator = locator;
	}
}

