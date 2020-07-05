
//==========================================================================================
//Update events
//==========================================================================================

void LAi_Character_FrameUpdate()
{
	float dltTime = GetEventData();
	//Обновляем состояние персонажей
	LAi_AllCharactersUpdate(dltTime);
	//Задержка исполнения квестов
	LAi_QuestDelayProcess(dltTime);
}

void LAi_CharacterUpdate()
{
	//Параметры
	aref chr = GetEventData();
	float dltTime = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterUpdate") == false) return;
	//Процессируем соответствующий тип
	string func = chr.chr_ai.type;
	if(func != "")
	{
		func = "LAi_type_" + func + "_CharacterUpdate";
		call func(chr, dltTime);
	}
	//Процессируем соответствующий шаблон
	func = chr.chr_ai.tmpl;
	if(func == "") return;
	func = "LAi_tmpl_" + func + "_CharacterUpdate";
	call func(chr, dltTime);
}

//==========================================================================================
//EndTask events
//==========================================================================================

void LAi_CharacterEndTask()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterEndTask") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Определяем отработавшую задачу
	bool isProcessed = false;
	switch(endTask)
	{
	case "Goto point":
		func = "LAi_tmpl_" + func + "_EndGoToPoint";
		isProcessed = true;
		break;
	case "Runto point":
		func = "LAi_tmpl_" + func + "_EndRunToPoint";
		isProcessed = true;
		break;
	case "Escape":
		func = "LAi_tmpl_" + func + "_EndEscape";
		isProcessed = true;
		break;
	}
	if(isProcessed != false)
	{
		call func(chr);
	}else{
		Trace("LAi_CharacterEndTask -> unknow end task <" + endTask + ">");
	}
}

//==========================================================================================
//TaskFailure events
//==========================================================================================

void LAi_CharacterTaskFailure()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterTaskFailure") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Определяем невыполнившиюся задачу
	bool isProcessed = false;
	switch(endTask)
	{
	case "Goto point":
		func = "LAi_tmpl_" + func + "_FailureGoToPoint";
		isProcessed = true;
		break;
	case "Runto point":
		func = "LAi_tmpl_" + func + "_FailureRunToPoint";
		isProcessed = true;
		break;
	case "Follow character":
		func = "LAi_tmpl_" + func + "_FailureFollow";
		isProcessed = true;
		break;
	case "Fight":
		func = "LAi_tmpl_" + func + "_FailureFight";
		isProcessed = true;
		break;
	case "Escape":
		func = "LAi_tmpl_" + func + "_FailureEscape";
		isProcessed = true;
		break;
	}
	if(isProcessed != false)
	{
		call func(chr);
	}else{
		Trace("LAi_CharacterTaskFailure -> unknow failure task <" + endTask + ">");
	}
}

void LAi_CharacterBusyPos()
{
	//Параметры
	aref chr = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterTaskFailure") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	func = "LAi_tmpl_" + func + "_BusyPos";
	call func(chr, x, y, z);
}


//==========================================================================================
//Follow character's events
//==========================================================================================

void LAi_CharacterFollowGo()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFollowGo") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FollowGo";
	call func(chr);
}

void LAi_CharacterFollowStay()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFollowStay") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FollowStay";
	call func(chr);
}

//==========================================================================================
//Fight character's events
//==========================================================================================

void LAi_CharacterFightGo()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFightGo") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FightGo";
	call func(chr);
}

void LAi_CharacterFightStay()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterFightStay") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_FightStay";
	call func(chr);
}

void LAi_CharacterAttack()
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	string attackType = GetEventData();
	bool isBlocked = GetEventData();
	/*if(attack.id == pchar.id)
	{
		pchar.combo.target = sti(enemy.index);
	}   */
	if(isBlocked == true)  // to_do
	{
		if(CheckCharacterPerk(attack, "sliding"))
		{
			int iRand = rand(100);
			if(iRand < 20)
			{
				isBlocked = false;
			}
		}
	}
	//Реакция груп на атаку
	LAi_group_Attack(attack, enemy);
	//Начисление повреждений
	LAi_ApplyCharacterAttackDamage(attack, enemy, attackType, isBlocked);
	//Обновим цель сразу
	LAi_group_UpdateTargets(enemy);
	string func = enemy.chr_ai.type;
	if(func == "") return;
	func = "LAi_type_" + func + "_Attacked";
	call func(enemy, attack);
	func = "LAi_type_" + enemy.chr_ai.type + "_CharacterUpdate";
	call func(enemy, 0.0001);
	
}

void LAi_CharacterFire()
{
	aref attack = GetEventData();
	aref enemy = GetEventData();
	float kDist = GetEventData();	//0..1
	int isFindedEnemy = GetEventData();
	//Заряд персонажа
	if(!CheckAttribute(attack, "chr_ai.charge")) attack.chr_ai.charge = "0";
	float charge = stf(attack.chr_ai.charge) - 1.0;
	// boal gun bullet убираем пулю после выстрела -->
	TakeItemFromCharacter(attack, "bullet");
	// boal gun bullet убираем пулю после выстрела <--
	if(charge <= 0.0)
	{
		charge = 0.0;
		attack.chr_ai.chargeprc = "1";
	}
	// boal fix любой выстрел врубает дозарядку! -->
	attack.chr_ai.chargeprc = "1";
	// boal <--
	attack.chr_ai.charge = charge;
	//Если промахнулись, то ничего не делаем
	if(isFindedEnemy == 0)
	{
		//здесь можно поднимать тревогу в случае близкого выстрела
		return;
	}
	//Реакция груп на атаку
	LAi_group_Attack(attack, enemy);
	//Начисление повреждений
	LAi_ApplyCharacterFireDamage(attack, enemy, kDist);
	//Исполнение типа
	string func = attack.chr_ai.type;
	if(func == "") return;
	func = "LAi_type_" + func + "_Fire";
	call func(attack, enemy, kDist, isFindedEnemy != 0);
	//Обновим цель сразу
	LAi_group_UpdateTargets(enemy);
	func = enemy.chr_ai.type;
	if(func == "") return;
	func = "LAi_type_" + func + "_Attacked";
	call func(enemy, attack);
	func = "LAi_type_" + enemy.chr_ai.type + "_CharacterUpdate";
	call func(enemy, 0.0001);
}


bool LAi_tmp_return_bool;
bool LAi_CharacterIsFire()
{
	aref chr = GetEventData();
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_IsFire";
	LAi_tmp_return_bool = call func(chr);
	return LAi_tmp_return_bool;
}

bool LAi_CharacterIsFight()
{
	aref chr = GetEventData();
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_IsFight";
	LAi_tmp_return_bool = call func(chr);
	return LAi_tmp_return_bool;
}

//==========================================================================================
//Escape events
//==========================================================================================

void LAi_CharacterEscapeSlide()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterEscapeSlide") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_EscapeSlide";
	call func(chr);
}

//==========================================================================================
//Collision events
//==========================================================================================

void LAi_CharacterColThreshold()
{
	//Параметры
	string endTask = GetEventData();
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_CharacterColThreshold") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_ColThreshold";
	call func(chr);
}

//==========================================================================================
//Animation events
//==========================================================================================

void LAi_Character_EndAction()
{
	//Параметры
	aref chr = GetEventData();
	if(LAi_CheckCharacter(chr, "LAi_Character_EndAction") == false) return;
	string func = chr.chr_ai.tmpl;
	if(func == "") return;
	//Исполнение
	func = "LAi_tmpl_" + func + "_EndAction";
	call func(chr);
}


//==========================================================================================
//Dead event
//==========================================================================================

void LAi_Character_Dead_Process(aref chr)
{
	DialogExit();
	LAi_CharacterLogoff(chr);
	if(sti(chr.index) != nMainCharacterIndex)
	{
//navy --> смерть ПГГ
		if (CheckAttribute(chr, "PGGAi"))
		{
			PGG_CheckDead(chr);
		}
//navy <--
		//Подождём, а затем удалим персонажа
		// boal трупы не убирать
        if (MOD_DEAD_CLEAR == "On" && !CheckAttribute(chr, "DontClearDead"))
        {
            PostEvent("LAi_event_Character_Dead", MOD_DEAD_CLEAR_TIME * 1000, "i", chr);
        }
        // убираем совместителя в CharacterDeadProcess
        
		//Реинкарнируем
		LAi_GenerateFantomFromMe(chr);
		//Посмотрим группу
		LAi_group_CheckGroupQuest(chr);
	}
	else
	{
		//Доигрались...
		if(!LAi_IsBoardingProcess())
		{
			PostEvent("LAi_event_GameOver", 5000, "s", "land");
		}
		else
		{
			PostEvent("LAi_event_GameOver", 5000, "s", "boarding");
		}
	}
}

#event_handler("LAi_event_Character_Dead", "LAi_Character_Dead_Event");
void LAi_Character_Dead_Event()
{
	aref chr = GetEventData();
	if (IsEntity(chr))  //fix
	{
		// boal dead can be searched 14.12.2003 -->
		Dead_DelLoginedCharacter(chr); // трем
		// boal dead can be searched 14.12.2003 <--
		CharacterExitFromLocation(chr);
	}
}

#event_handler("LAi_event_GameOver", "LAi_GameOver");
void LAi_GameOver()
{
	string str = GetEventData();
	GameOver(str);
}



//==========================================================================================
//Internal events
//==========================================================================================

//------------------------------------------------------------------------------------------
//Сообщение об окончании работы темплейта
//------------------------------------------------------------------------------------------

void LAi_Character_TemplateComplite(aref chr, string tmplName)
{

	int index = sti(chr.index);
	PostEvent("LAi_event_Character_TemplateComplite", 1, "ls", index, tmplName);

}

#event_handler("LAi_event_Character_TemplateComplite", "LAi_Character_TemplateComplite_Event");
void LAi_Character_TemplateComplite_Event()
{
	int index = GetEventData();
	string tmpl = GetEventData();
	ref chr = &Characters[index];
	// boal fix -->
	string func = "";
	if ( CheckAttribute(chr, "chr_ai.type"))
	{
	    func = chr.chr_ai.type;
	}
	// boal fix <--
	if(func != "")
	{
		func = "LAi_type_" + func + "_TemplateComplite";
		call func(chr, tmpl);
	}
}

//------------------------------------------------------------------------------------------
//Запрос локатора
//------------------------------------------------------------------------------------------

void LAi_Character_FreeLocator(aref chr, string group, string locator)
{
	int index = sti(chr.index);
	PostEvent("LAi_event_Character_FreePos", 1, "lss", index, group, locator);
}

#event_handler("LAi_event_Character_FreePos", "LAi_Character_FreePos_Event");
void LAi_Character_FreePos_Event()
{
	int index = GetEventData();
	string group = GetEventData();
	string locator = GetEventData();
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int idx = LAi_loginedcharacters[i];
		if(idx >= 0)
		{
			if(idx != index)
			{
				float dist;
				if(GetCharacterDistByLoc(&Characters[idx], group, locator, dist))
				{
					if(dist < 0.8)
					{
						string func = Characters[idx].chr_ai.tmpl;
						if(func == "") return;
						//Исполнение
						func = "LAi_tmpl_" + func + "_FreePos";
						call func(&Characters[idx], &Characters[index]);
					}
				}
			}
		}
	}
}


//------------------------------------------------------------------------------------------
//Запрос на диалог
//------------------------------------------------------------------------------------------

void LAi_Character_NeedDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = chr.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_NeedDialog";
			call func(by, chr);
		}
	}
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_Character_CanDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		bool isDisable = false;
		if(CheckAttribute(chr, "chr_ai.disableDlg"))
		{
			if(sti(chr.chr_ai.disableDlg) != 0) isDisable = true;
		}
		if(isDisable == false)
		{
			string func = by.chr_ai.type;
			if(func != "")
			{
				func = "LAi_type_" + func + "_CanDialog";
				return call func(by, chr);
			}
		}
	}
	return false;
}

//Начать диалог
void LAi_Character_StartDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = by.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_StartDialog";
			call func(by, chr);
		}
	}
}

//Закончить диалог
void LAi_Character_EndDialog(aref chr, aref by)
{
	if(IsEntity(by))
	{
		string func = by.chr_ai.type;
		if(func != "")
		{
			func = "LAi_type_" + func + "_EndDialog";
			call func(by, chr);
		}
	}
}
// Привязать к/отвязать от модели персонажа какую нить модель (предмет)
void LAi_CharacterItemAction()
{
	aref chr = GetEventData();
	string sActionName = GetEventData();
	int nItemIndex = GetEventData();

	switch(sActionName)
	{
	case "reset": LAi_UntieItemFromCharacter(chr,nItemIndex); break;
	case "set": LAi_TieItemToCharacter(chr,nItemIndex); break;
	}
}
void LAi_UntieItemFromCharacter(aref chr, int nItemIndex)
{
	if( nItemIndex<0 ) return;
	SendMessage(chr, "lsl", MSG_CHARACTER_EX_MSG, "UntieItem", nItemIndex);
}
void LAi_TieItemToCharacter(aref chr, int nItemIndex)
{
	if( nItemIndex<0 ) return;

	string sModel = "kocherg";
	string sLocator = "Saber_hand";

	string sitm = "TiedItems.itm"+nItemIndex;
	if( CheckAttribute(chr,sitm) )
	{
		if( CheckAttribute(chr,sitm+".model") ) {
			sModel = chr.(sitm).model;
		}
		if( CheckAttribute(chr,sitm+".locator") ) {
			sLocator = chr.(sitm).locator;
		}
	}

	SendMessage(chr, "lslss", MSG_CHARACTER_EX_MSG, "TieItem", nItemIndex, sModel, sLocator);
}
