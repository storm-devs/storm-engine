#define CURSED_PIRATES_ENC_CHANCE 0.3

ref CurLandEnc;
object TempLandEnc;

//Проверить возможность создания энкоунтера на данном месте
bool LEnc_LoginStart(ref location, string encGroup, string encLocator, int maxChrQuant)
{
	/*
	ref player, CurTemplate;
	player = GetMainCharacter();
	string sTempName;

	trace("Генерация энкаунтеров: Локация " + location.id); 
	trace("Генерация энкаунтеров: Группа " + EncGroup); 
	trace("Генерация энкаунтеров: Локатор " + EncLocator); 

	if(!CheckAttribute(location, "type")) return false;

	trace("СИСТЕМА ГЕНЕРАЦИИ ЭНКАУНТЕРОВ СООБЩАЕТ: ТИП ЛОКАЦИИ: " + location.type);
	CurTemplate = GetLandEncTemplateByID(location.type);

	//Проверка вероятности появления энкаунтера
//	if(Rand(1) != 0) return false;

	if(CheckAttribute(location, "DisableEncounters") == true && sti(location.DisableEncounters) != 0 ) return false;

	sTempName = Rand(makeint(CurTemplate.quantity));	
	if(makeint(sTempName) > 0)
	{
		sTempName = "enc" + sTempName;	

		trace("СИСТЕМА ГЕНЕРАЦИИ ЭНКАУНТЕРОВ СООБЩАЕТ: ВЫБРАН ТИП ЭНКАУНТЕРА: " + CurTemplate.(sTempName).type);

		if(CurTemplate.(sTempName).type == "ENC_OFFICER")
		{
			int iNation = sti(colonies[FindColony(location.fastreload)].nation);
			int iChar = GenerateCharacter(iNation, WITH_SHIP, "trader", MAN, 0, TRADER);
			PlaceCharacter(&characters[iChar], "sit", "nonrandom");
			LAi_SetSitTypeNoGroup(&characters[iChar]);
			characters[iChar].Dialog.Filename = "Enc_companion_dialog.c";
			characters[iChar].Dialog.CurrentNode = "First time";
		}
		else
		{

			CopyAttributes(&TempLandEnc, GetLandEncByID(CurTemplate.(sTempName).type));

			CurLandEnc = &TempLandEnc;			
		}
	}
	else
	{
		return false;
	}

	/*
	if(CheckAttribute(CurLandEnc, "StartingQuest1"))
	{
		trace("ПЕРВЫЙ СТАРТОВЫЙ КВЕСТ ДЛЯ ТЕКУЩЕГО ЭНКАУНТЕРА:" + CurLandEnc.StartingQuest1);
		player.quest.LandEnc_StartingQuest1.win_condition.l1 = CurLandEnc.StartingQuest1.QuestType;
		player.quest.LandEnc_StartingQuest1.win_condition.l1.location = location.id;
		player.quest.LandEnc_StartingQuest1.win_condition.l1.locator_group = EncGroup;
		player.quest.LandEnc_StartingQuest1.win_condition.l1.locator = encLocator;
		player.quest.LandEnc_StartingQuest1.win_condition = CurLandEnc.StartingQuest1;
		//log_setstringTolog("!!!!" + CurLandEnc.StartingQuest1);
	}
	if(CheckAttribute(CurLandEnc, "StartingQuest2"))
	{
		trace("ВТОРОЙ СТАРТОВЫЙ КВЕСТ ДЛЯ ТЕКУЩЕГО ЭНКАУНТЕРА:" + CurLandEnc.StartingQuest2);
		player.quest.LandEnc_StartingQuest2.win_condition.l1 = CurLandEnc.StartingQuest2.QuestType;
		player.quest.LandEnc_StartingQuest2.win_condition.l1.location = location.id;
		player.quest.LandEnc_StartingQuest2.win_condition.l1.locator_group = EncGroup;
		player.quest.LandEnc_StartingQuest2.win_condition.l1.locator = encLocator;
		player.quest.LandEnc_StartingQuest2.win_condition = CurLandEnc.StartingQuest2;
	//	log_setstringTolog("!!!!" + CurLandEnc.StartingQuest2);
	}

//	Location.locators_radius.(encGroup).(encLocator) = 5;


	CurLandEnc.LocationID = location.id;
	CurLandEnc.LocGroup = EncGroup;
	CurLandEnc.Locator = EncLocator;
	CurLandEnc.maxCharacters = MaxChrQuant;
	CurLandEnc.CurCharacter = 1;

	if(CheckCharacterItem(player,"coins") == true && Rand(1) <= CURSED_PIRATES_ENC_CHANCE)
	{
	

	}
	

*/
	return false;
}

//Указать модельку для создаваемого персонажа или прервать создание персонажей
bool LEnc_LoginSelectModel(ref location, aref modelInfo, string group, string locator)
{
	/*
	string CurrentChar, CurModel;
	CurrentChar = "char" + CurLandEnc.CurCharacter;
	CurModel = "model" + (Rand(Makeint(CurLandEnc.(CurrentChar).modelQuantity)-1) + 1);

	modelInfo.model = CurLandEnc.(CurrentChar).(CurModel);
	modelInfo.ani = CurLandEnc.(CurrentChar).(CurModel).ani;
	ModelInfo.HeadModel = "h_" + modelInfo.model;

	return makeint(CurLandEnc.(CurrentChar).cont);
	*/
	return false;
}

//Скорректировать параметры созданного монстра
void LEnc_LoginCorrectParams(ref location, aref chr, string group, string locator)
{
	/*
	ref player;
	bool IsPistol, IsBlade;
	player = GetMainCharacter();
	string CurrentChar, CurModel, curAttr;
	CurrentChar = "char" + CurLandEnc.CurCharacter;
	CurModel = "model" + (Rand(Makeint(CurLandEnc.(CurrentChar).modelQuantity)-1) + 1);
	chr.id = "Enc_Char" + CurLandEnc.CurCharacter;
	trace("ГЕНЕРАЦИЯ ЭНКАУНТЕРОВ: Персонаж " + CurLandEnc.CurCharacter + " присвоен ID " + chr.id);

	//Назначение ранга
	chr.rank = makeint(player.rank) - 2 + rand(4);	
	if(makeint(chr.rank) <= 0) chr.rank = 1;

	//назначение экспы
	chr.experience = CalculateExperienceFromRank(sti(chr.rank));

	//Назначение лица!
	facemaker(chr);
	//	Назначение оружия, если необходимо
	curAttr = "char" + CurLandEnc.CurCharacter + ".pistol";
	if(CheckAttribute(CurLandEnc, curAttr)) IsPistol = true;

	curAttr = "char" + CurLandEnc.CurCharacter + ".sword";
	if(CheckAttribute(CurLandEnc, curAttr)) IsBlade = true;

	LAi_NPC_Equip(chr, makeint(player.rank), IsBlade, IsPistol);

	// назначение типа поведения
	switch(CurLandEnc.(CurrentChar).type)
	{
		case "warrior":
			LAi_SetWarriorTypeNoGroup(chr);
			LAi_warrior_SetStay(chr, true);
			LAi_warrior_DialogEnable(chr, true);
		break;

		case "guardian":
			LAi_SetGuardianTypeNoGroup(chr);
		break;

		case "stay":
			LAi_SetStayTypeNoGroup(chr);
		break;

		case "Sit":
			LAi_SetSitTypeNoGroup(chr);
		break;

		case "Citizen":
			LAi_SetCitizenTypeNoGroup(chr);
		break;

		case "Actor":
			LAi_SetActorTypeNoGroup(chr);
		break;
	}
	// назначение скиллов

	CalculateAppropriateSkills(chr);	

	// назначение здоровья
	LAi_SetHP(chr, LAI_DEFAULT_HP + (makeint(player.rank)*5), LAI_DEFAULT_HP_MAX + (makeint(player.rank)*5));

	// назначение диалога
	chr.Dialog.Filename = CurLandEnc.(CurrentChar).Dialog;
	chr.Dialog.CurrentNode = "First time";
	*/
}

bool LEnc_IsContinueLogin(ref location, aref chr, string group, string locator)
{
	/*
	string CurrentChar;
	CurrentChar = "char" + CurLandEnc.CurCharacter;

	bool retValue = makeint(CurLandEnc.(CurrentChar).cont);

	CurLandEnc.CurCharacter = makeint(CurLandEnc.CurCharacter) + 1;
 
	return retValue;
	*/
	return false;
}

//Завершить загрузку энкоунтера в локацию
void LEnc_LoginEnd(ref location, string encGroup, string encLocator)
{
	//DeleteAttribute(CurLandEnc, "");
}

bool CheckEncounterChance(float Chance)
{
	/*
	if(Rand(1) <= Chance)
	{
		return true;
	}
	else
	{
		return false;
	}
	*/
}

string SelectEncounterblade()
{
	/*
	ref player;
	player = getMainCharacter();
	switch(player.rank)
	{
	
		case 1:
			return "blade1";
		break;

		case 2:
			return "blade2";
		break;

		case 3:
			return "blade3";
		break;

		case 4:
			return "blade4";
		break;

		case 5:
			return "blade5";
		break;
	
		case 6:
			return "blade6";
		break;
	
		case 7:
			return "blade7";
		break;

		case 8:
			return "blade8";
		break;

		case 9:
			return "blade9";
		break;

		case 10:
			return "blade10";
		break;

		case 11:
			return "blade11";
		break;

		case 12:
			return "blade12";
		break;
	}
	if(makeint(player.rank) > 12) return "blade13";

	return "blade1";
	*/
}
