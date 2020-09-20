// Модуль методов для квестов BOAL 07.06.05
// строковая дата, день_мес
string LastSpeakDate()
{
    return environment.date.day + " " + environment.date.month;
}
// Для диалогов НПС -->
string GetNPCQuestDate(ref npchar, string _type)
{
    if (!CheckAttribute(npchar, "QuestDate." + _type))
    {
        npchar.QuestDate.(_type) = "";
    }
    return npchar.QuestDate.(_type);
}
void SetNPCQuestDate(ref npchar, string _type)
{
    npchar.QuestDate.(_type) = LastSpeakDate();
}
bool CheckNPCQuestDate(ref npchar, string _type)
{
    if (GetNPCQuestDate(npchar, _type) != LastSpeakDate())
    {
        return true;
    }
    return false;
}
// Для ГГ <--
void SaveCurrentQuestDateParam(string _quest)
{ // запись даты получения квеста
    aref  arQ;
    makearef(arQ, PChar.(_quest));
    
    arQ.control_day = GetDataDay();
    arQ.control_month = GetDataMonth();
    arQ.control_year = GetDataYear();
    arQ.control_time = GetTime();
}
int GetQuestPastMonthParam(string _quest)
{
    aref  arQ;
    makearef(arQ, PChar.(_quest));

    return GetPastTime("month", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), stf(arQ.control_time),GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
}
int GetQuestPastDayParam(string _quest)
{
    aref  arQ;
    makearef(arQ, PChar.(_quest));
    if (CheckAttribute(PChar, _quest + ".control_year"))
    {
    	return GetPastTime("day", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), stf(arQ.control_time),GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
	} 
	return 0;   	
}
int GetQuestPastTimeParam(string _quest)
{
    aref  arQ;
    makearef(arQ, PChar.(_quest));
    if (CheckAttribute(PChar, _quest + ".control_year"))
    {
    	return GetPastTime("hour", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), stf(arQ.control_time), GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
	}  
	return 0;
}
//navy -->
//Ввел еще одну функцию, т.к. нужны прошедшие минуты.
int GetQuestPastMinutesParam(string _quest)
{
    aref  arQ;
    makearef(arQ, PChar.(_quest));
    if (CheckAttribute(PChar, _quest + ".control_year"))
    {
    	return GetPastTime("minute", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), stf(arQ.control_time), GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
	}
	return 0;
}
//navy <--

// eddy
// ************* для неписей.
void SaveCurrentNpcQuestDateParam(ref _character, string _quest)
{ // запись даты получения квеста
    aref  arQ;
    makearef(arQ, _character.(_quest));

    arQ.control_day = GetDataDay();
    arQ.control_month = GetDataMonth();
    arQ.control_year = GetDataYear();
    arQ.control_time = GetTime();
}

int GetNpcQuestPastDayParam(ref _character, string _quest)
{
    aref  arQ;
    makearef(arQ, _character.(_quest));
    if (CheckAttribute(_character, _quest + ".control_year"))
    {
        return GetPastTime("day", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), 0,GetDataYear(), GetDataMonth(), GetDataDay(), 0);
    }
    else {return 0;}
}

int GetNpcQuestPastDayWOInit(ref _character, string _quest)
{
    aref  arQ;
    makearef(arQ, _character.(_quest));
    if (CheckAttribute(_character, _quest + ".control_year"))
    {
        return GetPastTime("day", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), 0,GetDataYear(), GetDataMonth(), GetDataDay(), 0);
    }
    else {return 1000;}  // если нет ветки, то это значит черти когда было дело
}

int GetNpcQuestPastTimeParam(ref _character, string _quest)
{
    aref  arQ;
    makearef(arQ, _character.(_quest));
    if (CheckAttribute(_character, _quest + ".control_year"))
    {
    	return GetPastTime("hour", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), sti(arQ.control_time), GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
	}
	return 0;
}
int GetNpcQuestPastMinutesParam(ref _character, string _quest)
{
    aref  arQ;
    makearef(arQ, _character.(_quest));
    if (CheckAttribute(_character, _quest + ".control_year"))
    {
    	return GetPastTime("minute", sti(arQ.control_year), sti(arQ.control_month), sti(arQ.control_day), stf(arQ.control_time), GetDataYear(), GetDataMonth(), GetDataDay(), GetTime());
	}
	return 0;
}
// *************


void initStartState2Character(ref ch)
{// метод вызывается в момент новой игры, заполняет главного героя атрибутами, чтоб убрать лишние проверки в if
    NullCharacter.Siege = "";
    
	ch.SystemInfo.SaveCount = 0;
    ch.SystemInfo.LoadCount = 0;
    ch.SystemInfo.DeadCount = 0;

    ch.SystemInfo.CabinType          = ""; // boal 28.03.05 каюты нет  My_Cabin_Small
    
    ch.questTemp.Contraband.relation = 60;
    DeleteAttribute(ch, "quest.meeting");
    ch.ShipInStock = 0; // кораблей в порту
	ch.questTemp.abordage = 0;   // двойной абордаж
	ch.questTemp.donate   = 0; // пожертвование церкви

	ch.questTemp.friend_in_tavern = ""; // оптимизация boal
	ch.questTemp.RelationAgentMet = "0"; // оптимизация boal

	ch.questTemp.NationQuest = ""; // взятая линейка квестов

	ch.GenQuest.BrothelCount = "0"; // счетчик посещений девочек
	//ch.GenQuest.Hunter2Pause   = false; // признак бойни в форте на суше - не делать ОЗГов
	ch.I_MAP_SHOW = false;  // boal карты смотрим из предметов
	ch.CameraHoldPos = false;
	ch.CameraShipNPC = 0;// номер компаньона для камеры 0 - ГГ
	ch.GenQuestFort.FarLocator = true; // для методов пихания фантомов в город

	// Черная жемчужина
	ch.GenQuest.GhostShip.DeadByMe       = 0;
	ch.GenQuest.GhostShip.KillMe         = 0;
	ch.GenQuest.GhostShip.AskAbout       = 0; // в таверне три раза спросить до появления
	ch.GenQuest.GhostShip.lastspeak_date = "";
    GhostShipInit();
    
    ch.GenQuest.Find_Merchant.lastspeak_date = "";
    ch.GenQuest.ConvoyPassenger = ""; //структура недовольных по квесту доставки пассажира

	// ==> состояние квеста текущей линейки
	ch.questTemp.State = "";
	// ==> кол-во дней ожидания выдачи следующего квеста губером, стандарт - два дня
	ch.questTemp.Waiting_time = "2";
	// ==> номер текущего квеста, присваиваем сразу второй, т.к. первый берется на момент первого обращения к губеру.
	ch.questTemp.CurQuestNumber = "2";
	//параметры островов и бухт для корсарского метро, ПГГ, и много еще чего :)
	InitTravelMap();
	// ==> проверка не посещение борделей после свадьбы
	pchar.RomanticQuest.HorseCheck = -1;
	// ==> Квест Аскольда.
    ch.questTemp.Ascold = "canTakeQuest"; // состояние, если равен "", то пока не даем квест.
	// ==> общий индекс Аззи.
    ch.questTemp.Azzy = "1";
	// ==> выбор хозяна магазина и уличного торговца по Аскольду.
	SelectAscoldRendom();
	// ==> квест Изабеллы
	IsabellaInit()
	// ==>  флаги и иниты бергларов 
	BerglarsInit();
	// ==>  нищие
	PoormansInit();
	// ==>  кол-во лута, отданного шлюхами
	pchar.questTemp.HorseLoot = 0;
	// ==>  кол-во перетраха со шлюхами
	pchar.questTemp.HorseQty = 0;
	// ==> прерывания на ход к жемчужному промыслу
	PearlGen_SetSantaCatalina("none");
	PearlGen_SetTeno("none");
	// ==> спрашивальщики
	QuestionsInit();
	// ==> тюремный флаг на проход внутрь
	ch.questTemp.jailCanMove = false;
	// ==> линейка ГПК
	ch.questTemp.LSC = false;
	ch.questTemp.LSC.checkBoxes = false; //флаг проверки сундуков в полночь
	pchar.quest.FirstLoginLostShipsCity.win_condition.l1 = "location";
	pchar.quest.FirstLoginLostShipsCity.win_condition.l1.location = "LostShipsCity_town";
	pchar.quest.FirstLoginLostShipsCity.function = "FirstLoginLostShipsCity";
	// ==> квест получения Синей Птицы
	ch.questTemp.BlueBird = "begin";
	ch.questTemp.BlueBird.City = "";
	ch.questTemp.BlueBird.count = 0;
	// ==> генератор Шарпа по жемчугу
	ch.questTemp.Sharp = "begin";
	ch.questTemp.Sharp.City = "";
	ch.questTemp.Sharp.count = 0; //счетчик кол-ва поисков
	ch.questTemp.Sharp.brothelChance = 1; //античит, шанс встретить Шарпа прямо в борделе
	ch.questTemp.Sharp.SeekSpy = "begin"; //состояние квест на поиски информатора Шарпа
	// ==> пиратская линейка
	PiratesLineInit();
	// ==> набор мелких квестов, дается по одному до закрытия
	ch.questTemp.different = "free";
	// ==> линейка Блада homo
	ch.questTemp.CapBloodLine = true;
	ch.questTemp.CapBUnarmed = false; //ГГ сабля не положена - false
}

// Поместить непися в компаньоны и тут же отправить его на выход из текущей локации.
void CharacterIntoCompanionAndGoOut(ref _mainCharacter, ref _compCharacter, string _group, string _locator, int _timeToGoOut, bool _compRemove)
{
    if (!bDisableFastReload) //чтобы не снимать запрет, если он установлен другим методом
	{
		bDisableFastReload = true;
		_mainCharacter.GenQuest.CallFunctionParam.CharacterIntoCompanion.FastReload = true;
	}
	if (_group == "")    {_group = "reload";}
    if (_locator == "")
    {
    	float locx, locy, locz;
        GetCharacterPos(_compCharacter, &locx, &locy, &locz);
        _locator = LAi_FindNearestFreeLocator(_group, locx, locy, locz);
    }
    LAi_ActorGoToLocation(_compCharacter, _group, _locator, "none", "", "", "", _timeToGoOut);
    _mainCharacter.GenQuest.CallFunctionParam = "Character_into_companion";
    _mainCharacter.GenQuest.CallFunctionParam.CharacterIntoCompanion.Id = _compCharacter.id;
    _mainCharacter.GenQuest.CallFunctionParam.CharacterIntoCompanion.Remove = _compRemove;
    DoQuestCheckDelay("CallFunctionParam", _timeToGoOut);
}

void Character_into_companion()    // относится к методу выше.
{
    ref NPChar = characterFromID(PChar.GenQuest.CallFunctionParam.CharacterIntoCompanion.Id);
    SetCompanionIndex(pchar, -1, getcharacterIndex(PChar.GenQuest.CallFunctionParam.CharacterIntoCompanion.Id));
    if (PChar.GenQuest.CallFunctionParam.CharacterIntoCompanion.Remove == false)    {SetCharacterRemovable(npchar, false);}
	//снимать запрет, только если он установлен этим методом
	if (CheckAttribute(PChar, "GenQuest.CallFunctionParam.CharacterIntoCompanion.FastReload")) bDisableFastReload = false;
}

string GetBookData(int day, int mn, int year)
{
	string result = "";

	if (day < 10)
	{
	    result = result + "0";
	}
	result = result + day + ".";
	if (mn < 10)
	{
	    result = result + "0";
	}
	result = result + mn + "." + year;

    return result;
}

// ========>>>>>> квест Аззи, подсчет контрольных сумм по неуязвимости =================
void AzzyCheckSumControl()
{
    int   QtyKilled, Result;
    float CurHP, MaxHP;

	if (CheckAttribute(pchar, "questTemp.Azzy.Immortal"))
    {
        QtyKilled = Statistic_AddValue(pchar, "Solder_s", 0) +
                    Statistic_AddValue(pchar, "Solder_g", 0) +
                    Statistic_AddValue(pchar, "Warrior_s", 0) +
                    Statistic_AddValue(pchar, "Warrior_g", 0) +
                    Statistic_AddValue(pchar, "Citizen_s", 0) +
                    Statistic_AddValue(pchar, "Citizen_g", 0) +
                    Statistic_AddValue(pchar, "Monster_s", 0) +
                    Statistic_AddValue(pchar, "Monster_g", 0) -
                    sti(pchar.questTemp.Azzy.Immortal.EarlyKilled);
        pchar.questTemp.Azzy.Immortal.AllQuota = makeint(pchar.questTemp.Azzy.Immortal.AllQuota) + sti(pchar.questTemp.Azzy.Immortal.DayQuota);
        Result = QtyKilled - sti(pchar.questTemp.Azzy.Immortal.AllQuota);
        if (Result < 0)
        {
            MaxHP = LAi_GetCharacterMaxHP(pchar);
            CurHP = LAi_GetCharacterHP(pchar);
            MaxHP = MaxHP - 5;
            pchar.questTemp.Azzy.Immortal.Penalty = makeint(pchar.questTemp.Azzy.Immortal.Penalty) + 5;
            if (!CheckAttribute(pchar, "questTemp.Azzy.OK"))
            {
                pchar.questTemp.Azzy = "HowToKnowAzzy";
                pchar.questTemp.Azzy.OK = 1;
            }
            if (MaxHP < 1)
            {
                LAi_KillCharacter(pchar);
                Log_SetStringToLog("Сообщение из Ада: ваши жизненные силы были окончательно отобраны в счет оплаты наших услуг. Добро пожаловать в Ад.");
            }
            else
            {
                Log_SetStringToLog("Адское уведомление о штрафных санкциях. Пожалуйста, ознакомьтесь!");
            }
            LAi_SetHP(pchar, CurHP, MaxHP);
        }
        if (GetQuestPastDayParam("questTemp.Azzy.Immortal") > 30)
        {
            pchar.questTemp.Azzy.Immortal.DayQuota = makeint(pchar.questTemp.Azzy.Immortal.DayQuota) * 2;
            SaveCurrentQuestDateParam("questTemp.Azzy.Immortal");
        }
        pchar.questTemp.Azzy.Immortal.CurKilled = QtyKilled;
        pchar.questTemp.Azzy.Immortal.CurBalance = Result;
        DeleteQuestHeader("Azzy_HellSign");
		if (sti(pchar.questTemp.Azzy.Immortal.Penalty) > 0)
		{		
			AddQuestRecord("Azzy_HellSign", "2");	
			AddQuestUserData("Azzy_HellSign", "Penalty", pchar.questTemp.Azzy.Immortal.Penalty);
		}
		AddQuestRecord("Azzy_HellSign", "1");
		AddQuestUserData("Azzy_HellSign", "sName", GetMainCharacterNameGen());
		AddQuestUserData("Azzy_HellSign", "DayQuota", pchar.questTemp.Azzy.Immortal.DayQuota);
		AddQuestUserData("Azzy_HellSign", "AllQuota", pchar.questTemp.Azzy.Immortal.AllQuota);
		AddQuestUserData("Azzy_HellSign", "CurKilled", pchar.questTemp.Azzy.Immortal.CurKilled);
		AddQuestUserData("Azzy_HellSign", "CurBalance", pchar.questTemp.Azzy.Immortal.CurBalance);
    }
}
// <<<<<======== квест Аззи, подсчет контрольныйх сумм по неуязвимости =================

// репутация плохого парня с границами
bool isBadReputation(ref _pchar, int _rep)
{
    if (GetCharacterPerkUsing(_pchar, "Trustworthy") || sti(_pchar.reputation) >= _rep)
    {
        return false;// good guy
    }
    return true;// bad guy
}
// заполнить сундук каюты во время абордажа
void FillAboardCabinBox(ref _location, ref _npchar)
{
    int     iTemp;
    bool    ok;
	
	_location.box1 = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()); // нужно, чтоб не перетерлось
	// нужно отметить, что в сундуке сгенерятся рандомные вещи, этот код срабатывает потом и правит токо деньги
	
	ok = true;
	// заготовка под квест
	if (_npchar.id == "GhostCapt" && CheckAttribute(pchar , "GenQuest.GhostShip.LastBattle"))
	{
        // трем все в сундуке
        DeleteAttribute(_location, "box1");
        // кладем супер хрень
        _location.box1.items.indian11 = 1;
        _location.box1.money = 66666;
        if (CheckAttribute(PChar, "GenQuest.GhostShip.TakeShotgun"))
        {
            DeleteAttribute(PChar, "GenQuest.GhostShip.TakeShotgun");
            _location.box1.items.pistol7 = 1;
        }
        ok = false;
	}
	// пасхалка Кукурузина
	/*if (CheckAttribute(_npchar, "Ship.Name") && _npchar.Ship.Name == FUNY_SHIP_NAME)
	{
        // не трем все в сундуке
        _location.box1.items.mineral4 = 25;
        ok = false;
        PChar.questTemp.FUNY_SHIP_FIND = true;
	}       */
	// квест №4, англ., боевка с голландским бригом
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.EngLineQ4_PutLetter"))
	{
        DeleteAttribute(_location, "box1");
        ChangeItemDescribe("letter_1", "itmdescr_letter_1_EngLineQ4");
        _location.box1.items.letter_1 = 1;
        _location.box1.money = 7460;
        _location.box1.items.map_Curacao = 1;
        ok = false;
	}
	// квест №6, англ., боевка с испанским пинасом
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.EngLineQ6_PutMoney"))
	{
        DeleteAttribute(_location, "box1");
        _location.box1.money = 140000;
        _location.box1.items.map_venesuela = 1;
        _location.box1.items.spyglass3 = 1;
        _location.box1.items.potion4 = 10;
        _location.box1.items.indian7 = 1;
        ok = false;
	}
    // квест №8, англ., боевка с испанским галеоном
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.ELQ8_PutLetter"))
	{
        DeleteAttribute(_location, "box1");
        ChangeItemDescribe("letter_2", "itmdescr_letter_2_EngLineQ8");
        _location.box1.items.letter_2 = 1;
        _location.box1.money = 2203;
        _location.box1.items.map_cuba = 1;
        _location.box1.items.spyglass2 = 1;
        _location.box1.items.potion2 = 7;
        _location.box1.items.potionwine = 2;
        ok = false;
	}
    // квест №9, англ., боевка с Пардалем
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.ELQ9_Diffrent"))
	{
        DeleteAttribute(_location, "box1");
        _location.box1.money = 853000;
        _location.box1.items.blade28 = 1;
        _location.box1.items.pistol6 = 1;
        _location.box1.items.pistol4 = 1;
        _location.box1.items.potion4 = 10;
        _location.box1.items.chest = 1;
        _location.box1.items.icollection = 1;
        _location.box1.items.map_cuba = 1;
        _location.box1.items.jewelry4 = 25;
        _location.box1.items.cirass3 = 1;
        ok = false;
	}
    // квест №7, испанка , выручаем Пардаля, сундук кэпа флибустьеров
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.SLQ7_Diffrent"))
	{
        DeleteAttribute(_location, "box1");
        _location.box1.money = 753400;
        _location.box1.items.blade28 = 1;
        _location.box1.items.pistol6 = 1;
        _location.box1.items.pistol4 = 1;
        _location.box1.items.potion2 = 10;
        _location.box1.items.potionwine = 1;
        _location.box1.items.jewelry4 = 25;
        _location.box1.items.cirass3 = 1;
		_location.box1.items.cirass4 = 1;
        ok = false;
	}
    // квест №8, испанка , защищаем Куману, сундук кэпа флибустьеров
	if (_npchar.id == "CaptainAttack_1" && CheckAttribute(PChar, "questTemp.SLQ8_Diffrent"))
	{
        DeleteAttribute(_location, "box1");
        _location.box1.money = 996400;
        _location.box1.items.spyglass4 = 1;
        _location.box1.items.potionrum = 2;
        _location.box1.items.pistol4 = 5;
        _location.box1.items.potion3 = 11;
        _location.box1.items.chest = 1;
        _location.box1.items.icollection = 1;
        _location.box1.items.jewelry5 = 50;
		_location.box1.items.indian13 = 1;
        ok = false;
	}
	// квест Аскольда.
	if (CheckAttribute(PChar, "questTemp.Ascold.Ship"))
	{
        if (_npchar.id == pchar.questTemp.Ascold.Ship)
        {
            DeleteAttribute(_location, "box1");
            _location.box1.items.ShipsJournal = 1;
            ok = false;
        }
	}
	// Осады  homo 22/10/06	 
	if (findsubstr(_npchar.id, "SiegeCap_" , 0) != -1)
	{
	    aref aData, itm;
	    makearef(aData, NullCharacter.Siege);
	    string sGroup = "Sea_"+NationShortName(sti(aData.nation))+"SiegeCap_1";
	    ref rchar = Group_GetGroupCommander(sGroup);  //поиск текущего командера, он меняется от убиеня
	    Log_TestInfo( _npchar.id +" == "+ rchar.id);
		if (_npchar.id == rchar.id && CheckAttribute(aData, "loot"))
		{
	        DeleteAttribute(_location, "box1");
	        _location.box1.money = sti(aData.loot);
	        _location.box1.items.potionrum = 3;
	        _location.box1.items.potionwine = 1;
	        string idmap = "map_"+aData.island;
            if (Items_FindItem(idmap, &itm) != -1)
            {
                _location.box1.items.(idmap) = 1;
            }

	        ok = false;
		}
	}
	//homo =>
    if (findsubstr(_npchar.id, "_QuestMerchant" , 0) != -1)
    {
        if (trap)
        {
            DeleteAttribute(_location, "box1");
            _location.box1.money = 7000;
            _location.box1.items.blade20 = 1;
            ok = false;
        }
    }
    
    if (_npchar.id == "Head_of_Gold_Squadron" )
    {

        DeleteAttribute(_location, "box2");
        _location.box2.money = 2212;
        _location.box2.items.map_normal = 1;
        ok = false;

    }
    //homo <=
    // пиратка, квест №7. драгоценности в каюту
	if (_npchar.id == "QuestCap_PL7")
	{
        DeleteAttribute(_location, "box1");
        _location.box1.money = 3983;
        _location.box1.items.jewelry1 = 763;
        _location.box1.items.jewelry2 = 562;
        _location.box1.items.jewelry3 = 1771;
        _location.box1.items.jewelry4 = 678;
        _location.box1.items.jewelry6 = 236;
        _location.box1.items.jewelry7 = 321;
        _location.box1.items.jewelry14 = 121;
		_location.box1.items.jewelry15 = 45;
		_location.box1.items.jewelry18 = 311;
        ok = false;
	}
    if (ok) // не квестовый
    {
    	// код для всех
    	iTemp = GetCharacterShipClass(_npchar);
    	if (CheckAttribute(_npchar, "Ship.Mode") && _npchar.Ship.Mode == "Trade") // торговец
    	{
    	    _location.box1.money = (10 - iTemp) * 200 + rand(10 - iTemp) * 2000 + rand(10)*50 + rand(6 - iTemp) * 4000;
    	}
    	else
    	{
    		_location.box1.money = (10 - iTemp) * 90 + rand((10 - iTemp) * 2000);
    	}
    }
}

// ==> две функции Эдди для крутизны невероятной.
void FantomMakeCoolSailor(ref _Character, int _ShipType, string _ShipName, int _CannonsType, int _Sailing, int _Accuracy, int _Cannons)
{
    _Character.Ship.Cannons.Type = _CannonsType;
	_Character.skill.Sailing  = GetCoffDiff(_Sailing, SKILL_MAX);
	_Character.skill.Accuracy = GetCoffDiff(_Accuracy, SKILL_MAX);
	_Character.skill.Cannons  = GetCoffDiff(_Cannons, SKILL_MAX);
    
    _Character.DontRansackCaptain = true; //квестовые не сдаются
    _Character.SinkTenPercent     = false; // не тонуть при 10%, не убегать в бою
    _Character.AboardToFinalDeck  = true; // абордаж всегда
	_Character.DontClearDead      = true;
	_Character.SaveItemsForDead   = true;
	_Character.AlwaysSandbankManeuver = true;

    _Character.Ship.Type = GenerateShip(_ShipType, true);
    if (_ShipName == "none" || _ShipName == "") {SetRandomNameToShip(_Character)}
    else {_Character.Ship.Name = _ShipName}

    SetBaseShipData(_Character);
    SetCrewQuantityFull(_Character);
    Fantom_SetBalls(_Character, "pirate");

	SetCharacterPerk(_Character, "FastReload");
	SetCharacterPerk(_Character, "HullDamageUp");
	SetCharacterPerk(_Character, "SailsDamageUp");
	SetCharacterPerk(_Character, "CrewDamageUp");
	SetCharacterPerk(_Character, "CriticalShoot");
	SetCharacterPerk(_Character, "LongRangeShoot");
	SetCharacterPerk(_Character, "CannonProfessional");
	SetCharacterPerk(_Character, "ShipDefenseProfessional");
	SetCharacterPerk(_Character, "ShipSpeedUp");
	SetCharacterPerk(_Character, "ShipTurnRateUp");

    DeleteAttribute(_Character, "ship.sails");// убрать дыры на парусах
    DeleteAttribute(_Character, "ship.blots");
    DeleteAttribute(_Character, "ship.masts");// вернуть сбытые мачты
    DeleteAttribute(_Character, "Killer.status"); // снять аттрибут "был захвачен на абордаж"
    _Character.questTemp.abordage = 0; //снять аттрибут отказа повторного захвата
   	DeleteAttribute(_Character, "Abordage.Enable"); //снять невозможноть абордажа
}

void FantomMakeCoolFighter(ref _Character, int _Rank, int _Fencing, int _Pistol, string _Blade, string _Gun, float _AddHP)
{
    _Character.rank = GetCoffDiff(_Rank, 1000);
    _Character.skill.Fencing = GetCoffDiff(_Fencing, SKILL_MAX);
    _Character.Skill.FencingLight  = GetCoffDiff(sti(_Character.skill.Fencing), SKILL_MAX);
    _Character.Skill.FencingHeavy  = GetCoffDiff(sti(_Character.skill.Fencing), SKILL_MAX); 
    _Character.skill.Pistol = GetCoffDiff(_Pistol, SKILL_MAX);
	_Character.chr_ai.hp = stf(_Character.chr_ai.hp) + GetCoffDiff(_AddHP, 5000);
	_Character.chr_ai.hp_max = stf(_Character.chr_ai.hp_max) + GetCoffDiff(_AddHP, 5000);
	SetCharacterPerk(_Character, "Energaiser"); // скрытый перк дает 1.5 к приросту энергии, дается ГГ и боссам уровней
	SetCharacterPerk(_Character, "SwordplayProfessional");
	SetCharacterPerk(_Character, "AdvancedDefense");
	SetCharacterPerk(_Character, "CriticalHit");
	SetCharacterPerk(_Character, "Sliding");
	DeleteAttribute(_Character, "Items");
	_Character.SuperShooter  = true;
    GiveItem2Character(_Character, _Blade);
    _Character.equip.blade = _Blade;
    TakeNItems(_Character,"potion1", rand(MOD_SKILL_ENEMY_RATE)+1);
    GiveItem2Character(_Character, _Gun);
    _Character.equip.gun = _Gun;
    TakeNItems(_Character,"bullet", 100);
	GiveItem2Character(_Character,"spyglass3");
	EquipCharacterbyItem(_Character, "spyglass3");
    FaceMaker(_Character);
    SetNewModelToChar(_Character);  // перерисуем модель на лету
}

int GetCoffDiff(float _num, int _maxRange)
{
	switch (MOD_SKILL_ENEMY_RATE)
	{
		case  1: _num *= 0.6;  break;
		case  2: _num *= 0.75; break;
		case  3: _num *= 0.85; break;
		case  4: _num *= 0.9;  break;
		case  5: _num *= 0.95; break;
		case  6: _num *= 1;    break;
		case  7: _num *= 1.1;  break;
		case  8: _num *= 1.2;  break;
		case  9: _num *= 1.3;  break;
		case 10: _num *= 1.5;  break;
	}
	_num += 0.5;
	if (_num > _maxRange) return _maxRange;
	else return sti(_num);
}
// заполнение сундуков и рандитема по квесту
bool SetLocationQuestRandItem(int _index, aref _location, string _locatorName, aref al) // al - ветка локатора из модели, остальное тоже из обратотки локации
{   // метод редактировать не нужно - все задается в квестах
	string  lastSpawnTimeString;
    int     n;
	string  itemId;
	aref checkAref

	/* Пример
 	pchar.GenQuestRandItem.QC_Port = true;
    pchar.GenQuestRandItem.QC_Port.randitem1 = "pistol6";
    // если нужно чтоб было всегда
	pchar.GenQuestRandItem.QC_Port.stay = true; - тереть потом эту ветку самому по квесту
	
    QC_Port - локация где
    randitem1 - локатор
    pistol6 - предмет
    
    если нужно сразу несколько локаторов, то
    pchar.GenQuestRandItem.QC_Port.randitem1 = "pistol6";
    pchar.GenQuestRandItem.QC_Port.randitem2 = "pistol1";
 	*/
	lastSpawnTimeString = _location.id;
    if (CheckAttribute(pchar , "GenQuestRandItem." + lastSpawnTimeString) && CheckAttribute(pchar , "GenQuestRandItem." + lastSpawnTimeString +"."+_locatorName))
    {
        itemId = pchar.GenQuestRandItem.(lastSpawnTimeString).(_locatorName);
        if (!CheckAttribute(pchar , "GenQuestRandItem." + lastSpawnTimeString +".stay"))
        {
			//--> fix eddy. нельзя было положить в несколько локаторов
			DeleteAttribute(pchar , "GenQuestRandItem." + lastSpawnTimeString + "." + _locatorName); // если токо один раз
			makearef(checkAref,  pchar.GenQuestRandItem.(lastSpawnTimeString));
			if (GetAttributesNum(checkAref) == 0) DeleteAttribute(pchar , "GenQuestRandItem." + lastSpawnTimeString);
        }
        // только один предмет!!!!!!!!!!!!
        n = SetRandItemShow(_index, al, itemId); // вывести 3д модель в лакацию, если модель вообще есть
		if (n != -1)
		{
            Log_TestInfo("SetLocationQuestRandItem");
            lastSpawnTimeString = "LastSpawnTime"+_index;
		    _location.(lastSpawnTimeString) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
            lastSpawnTimeString = "RandItemType"+_index;
        	_location.(lastSpawnTimeString) = n;
            return true;
        }
    }
    
    return false;
}
// загрузить модель
int SetRandItemShow(int _index, aref al, string _itemId)
{
    int     n;
    aref    randItem;
    
    n = Items_FindItem(_itemId, &randItem);
	if (n != -1)
	{
        if (!CheckAttribute(randItem, "model") || randItem.model == "")
    	{
    		Trace("SetRandItemShow: no model for item "+randItem.id);
    		return -1;
    	}
	    Items_LoadModel(&randItemModels[_index],  randItem);
    	SendMessage(&randItemModels[_index], "lffffffffffff", MSG_MODEL_SET_POSITION, makeFloat(al.x), makeFloat(al.y), makeFloat(al.z), makeFloat(al.vx.x), makeFloat(al.vx.y), -makeFloat(al.vx.z), makeFloat(al.vy.x), makeFloat(al.vy.y), -makeFloat(al.vy.z), makeFloat(al.vz.x), makeFloat(al.vz.y), -makeFloat(al.vz.z));

        return n;
    }
    return n;
}
// заполнить сундук
bool SetLocationQuestBox(ref _location, string _locatorName)
{   // метод редактировать не нужно - все задается в квестах
    bool    ok = false;
    string  locId;
    aref    arToBox;
    aref    arFromBox;

    /* Пример
 	pchar.GenQuestBox.Havanna_town_04 = true;
    pchar.GenQuestBox.Havanna_town_04.box1.items.jewelry1 = 4;
    pchar.GenQuestBox.Havanna_town_04.box1.items.mineral2 = 10;
    // если нужны деньги, иначе не заполнять
	pchar.GenQuestBox.Havanna_town_04.box1.money          = 100;
    // если нужно чтоб было всегда
	pchar.GenQuestBox.Havanna_town_04.stay = true; - тереть потом эту ветку самому по квесту

    Havanna_town_04 - локация где
    box1 - локатор
    items - список предметов
    
    если нужно сразу несколько локаторов, то
    pchar.GenQuestBox.Havanna_town_04.box2.items.jewelry1 = 34;
 	*/
 	locId  = _location.id;
    if (CheckAttribute(pchar , "GenQuestBox." + locId) && CheckAttribute(pchar , "GenQuestBox." + locId + "." + _locatorName))
    {
        Log_TestInfo("SetLocationQuestBox");

        makearef(arToBox, _location.(_locatorName));
        makearef(arFromBox, pchar.GenQuestBox.(locId).(_locatorName));
        CopyAttributes(arToBox, arFromBox);
        _location.(_locatorName) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear()); // таймер, чтоб не затерлись рандомом при выходе из локации
        if (!CheckAttribute(pchar , "GenQuestBox." + locId +".stay"))
        {
			//--> fix eddy. нельзя было положить в несколько локаторов
			DeleteAttribute(pchar , "GenQuestBox." + locId + "." + _locatorName); // если токо один раз
			makearef(arToBox,  pchar.GenQuestBox.(locId));
			if (GetAttributesNum(arToBox) == 0) DeleteAttribute(pchar , "GenQuestBox." + locId);
        }
        return true;
    }
    return false;
}

// ==> Метод открытия\закрытия локаторов релоад. Если _flag=true - закрыть локатор, если _flag=false - открыть.
void LocatorReloadEnterDisable(string _locationID, string _locator, bool _flag)
{
    aref arDis, arRld;
	makearef(arRld, Locations[FindLocation(_locationID)].reload);
	int	Qty = GetAttributesNum(arRld);
	for (int i=0; i<Qty; i++)
	{
		arDis = GetAttributeN(arRld, i);
        if (arDis.name == _locator)
        {
    		if (_flag) arDis.disable = 1;
    		else DeleteAttribute(arDis, "disable");
            break;
        }
	}
}

//===>>> Функции по работе с диалоговыми файлами. Эдди. ================================================================
// == _strNormal - обычная строка диалога
// == _strBad1, _strBad2, _strBad3 - строки степени возмущения,  "" и "none" - базовый набор.
// == _kind - вид возмущения: "repeat" - снимается на следующий день при условии, что НПС вконец не разозлился (не задействован анги-файл).
//                            "quest"  - квестовые повторялки, не снимаются на следующий день, но может быть задан срок примирения после перевода на ангри.
//                            "block"  - реакцию НПС на повторные клики без включения ангри, для того, чтобы обозначить интеллект, но не ругаться. Ангри вообще не делать.
//                            "cycle"  - крутим цикл из повторялок, ангри не заряжать. На след. день все снимается.
// == _terms для "repeat" и "quest" - сроки в днях примирения само-собой после задействоания ангри-файла, если == 0, то ругань навсегда (примерение только через подарки).
//    _terms для "block" - сроки возврата в норму в днях, если 0 - навсегда.
// == _character и _Node - npchar и Dialog.CurrentNode
string NPCStringReactionRepeat(string _strNormal, string _strBad1, string _strBad2, string _strBad3, string _kind, int _terms, ref _character, string _Node)
{
    _Node = stripblank(_Node); //fix spaces
	string strBack;
    string strTemp = "quest.repeat." + _Node;
    if (!CheckAttribute(_character , strTemp))
    {
        _character.quest.repeat.(_Node) = 0;
        _character.quest.repeat.(_Node).ans = 0;
    }
    if (_kind == "cycle" || _kind == "repeat") //на след.день все снимается.
    {
        if (GetNpcQuestPastDayParam(_character, strTemp) >= 1)
        {
            _character.quest.repeat.(_Node) = 0;
            _character.quest.repeat.(_Node).ans = 0;
        }
    }
    else
    {
        if (GetNpcQuestPastDayParam(_character, strTemp) >= _terms && _terms!=0 && _kind != "quest") //снятие по "block"
        {
            _character.quest.repeat.(_Node) = 0;
            _character.quest.repeat.(_Node).ans = 0;
        }
    }
    switch(_character.quest.repeat.(_Node))
	{
        case "0":
            strBack = _strNormal;
            _character.quest.repeat.(_Node) = 1;
            _character.quest.repeat.(_Node).ans = 0;
            SaveCurrentNpcQuestDateParam(_character, strTemp);
        break;
        case "1":
            if (_strBad1 == "none" || _strBad1 == "")
            {
                strBack = LinkRandPhrase("Я не хочу говорить об одном и том же по нескольку раз.",
                          "Мы уже говорили об этом.",
                          "Эту тему мы уже обсуждали.");
            }
            else
            {
                strBack = _strBad1;
            }
            _character.quest.repeat.(_Node) = 2;
            _character.quest.repeat.(_Node).ans = 1;
        break;
        case "2":
            if (_strBad2 == "none" || _strBad2 == "")
            {
                strBack = LinkRandPhrase("Еще раз повторяю, что я не хочу говорить об одном и том же по десять раз!",
                          "Повторяю, мы уже говорили об этом!",
                          "Я же говорю, мы это уже обсуждали. Проблемы с памятью?");
            }
            else
            {
                strBack = _strBad2;
            }
            _character.quest.repeat.(_Node) = 3;
            _character.quest.repeat.(_Node).ans = 2;
        break;
        case "3":
            if (_strBad3 == "none" || _strBad3 == "")
            {
                strBack = LinkRandPhrase("Я не намерен более с вами общаться.",
                          "Не буду с тобой больше разговаривать!",
                          "Ты меня порядком разозлил, никакого общения более.");
            }
            else
            {
                strBack = _strBad3;
            }
            // ==> Реакция на третье повторение.
            if (_kind == "cycle") //снова на первую строку.
            {
			    _character.quest.repeat.(_Node) = 0;
            }
			else
            {
                if(_kind != "block") //если block, то опять в последнюю строку
                {
                    _character.quest.repeat.(_Node) = 4;
                    CharacterAddAngry(_character, _Node, _kind, _terms);
    			}
            }
            _character.quest.repeat.(_Node).ans = 3;
        break;
        case "4":
            strBack = "NPCStringReactionRepeat. Если вы читаете это, то это явный баг. Сообщите пожалста команде аддона, для Эдди."
        break;
	}
    return strBack;
}

string HeroStringReactionRepeat(string _strNormal, string _strBad1, string _strBad2, string _strBad3, ref _character, string _Node)
{
    _Node = stripblank(_Node); //fix spaces
	string strBack;
    int Temp = sti(_character.quest.repeat.(_Node).ans);
    switch(Temp)
	{
        case "0":
            strBack = _strNormal;
        break;
        case "1":
            if (_strBad1 == "none" || _strBad1 == "")
            {
                strBack = LinkRandPhrase("Хорошо, приятель.",
                          "Как скажешь.",
                          "Не то ляпнул, извини...");
            }
            else
            {
                strBack = _strBad1;
            }
        break;
        case "2":
            if (_strBad2 == "none" || _strBad2 == "")
            {
                strBack = LinkRandPhrase("Хорошо, хорошо. Только не кипятись.",
                          "Как скажешь, только не надо кричать.",
                          "Извини, что-то я сегодня не в форме... Голова с утра раскалывается...");
            }
            else
            {
                strBack = _strBad2;
            }
        break;
        case "3":
            if (_strBad3 == "none" || _strBad3 == "")
            {
                strBack = LinkRandPhrase("Как знаешь.",
                          "Очень жаль...",
                          "Невелика потеря.");
            }
            else
            {
                strBack = _strBad3;
            }
        break;
        case "4":
            strBack = "HeroStringReactionRepeat. Если вы читаете это, то это явный баг. Сообщите пожалста команде аддона, для Эдди."
        break;
	}
    return strBack;
}

// возможны только две переходные ноды: _GoNode1 и _GoNode2, третья не нужна, т.к. идет сразу в ангри-файл.
// неверно, оставил третью ноду для циклов и прочего.
string DialogGoNodeRepeat(string _NormalNode, string _GoNode1, string _GoNode2, string _GoNode3, ref _character, string _Node)
{
    _Node = stripblank(_Node); //fix spaces
	string strBack, Temp;
    Temp = sti(_character.quest.repeat.(_Node).ans);
    switch(Temp)
	{
        case "0":
            strBack = _NormalNode;
        break;
        case "1":
            if (_GoNode1 == "none" || _GoNode1 == "")
            {
                strBack = "exit";
            }
            else
            {
                strBack = _GoNode1;
            }
        break;
        case "2":
            if (_GoNode2 == "none" || _GoNode2 == "")
            {
                strBack = "exit";
            }
            else
            {
                strBack = _GoNode2;
            }
        break;
        case "3":
            if (_GoNode3 == "none" || _GoNode3 == "")
            {
                strBack = "exit";
            }
            else
            {
                strBack = _GoNode3;
            }
        break;
        case "4":
            strBack = "exit"
            Log_SetStringToLog("DialogGoNodeRepeat#4. Если у вас высветилась данная запись, то это явный баг. Прошу сообщить команде аддона ВМЛ, для Эдди.");
        break;
    }
    return strBack;
}

// _Node - имя ноды, идентификатор текущей ангри. Если метод вызывается не в диалоговом файле, то == "none".
void CharacterAddAngry(ref _character, string _Node, string _kind, int _terms)
{
    if (_kind == "repeat" || _kind == "quest")
    {
        DeleteAttribute(_character, "angry");
        _character.angry.kind = _kind;
        _character.angry.name = _Node; //и имя ноды тоже, для расфасовки разных ангри
        if (_terms != 0) // если == 0, то ссора непроходящая с течением времени.
        {
            _character.angry.terms = _terms;
            SaveCurrentNpcQuestDateParam(_character, "angry.terms");
            if (_terms < 10) // размер вознаграждения за примирение, анти с/л
            {
                _character.angry.QtyMoney = (rand(10)+1) * 500;
            }
            else
            {
                _character.angry.QtyMoney = (rand(10)+1) * 5000;
            }
        }
        else
        {
            _character.angry.QtyMoney = (rand(10)+1) * 10000;
        }
    }
    else {Log_Info("CharacterAddAngry. Если вы видите эту строку, то это явный баг. Пожалста, сообщите команде аддона ВМЛ, for Eddy.");}
}

void CharacterDelAngry(ref _character)
{
    DeleteAttribute(_character, "angry");
    DeleteAttribute(_character, "quest.repeat");
}

//В условиях возможного тотального применения ангри по квестам, нужно подстраховаться.
//Если нода дается НПС в момет, когда у него задействован ангри, то нода будет возвращена после примирения.
//Соотв. менять ноды желательно данным методом.
void QuestSetCurrentNode(string _chID, string _Node)
{
    ref chref = characterFromID(_chID);
    if (CheckAttribute(chref, "angry")) chref.dialog.TempNode = _Node;
    else chref.dialog.currentnode = _Node;
}

// фраза от пола НПС
string NPCharSexPhrase(ref _character, string StrMan, string StrWoman)
{
	string strBack;
    if (_character.sex == "woman")
    {
        strBack = StrWoman;
    }
    else
    {
        strBack = StrMan;
    }
    return strBack;
}
// фраза от пола ГГ
string GetSexPhrase(string StrMan, string StrWoman)
{
    return NPCharSexPhrase(PChar, StrMan, StrWoman);
}
//<<<=== Функции по работе с диалоговыми файлами. =======================================================================


// Временно сохранить все данные о нашем корабле в памяти
//--------------------------------------------------------
bool SetTempRemoveParam(ref _refCharacter, string _param)
{
    string sParam = "TmpRemember" + _param;
    
	if( CheckAttribute(_refCharacter, sParam) ) return false;
	if( !CheckAttribute(_refCharacter, _param) ) return false;

	aref dstRef; makearef(dstRef, _refCharacter.(sParam));
	aref srcRef; makearef(srcRef, _refCharacter.(_param));

	CopyAttributes(dstRef,srcRef);
	return true;
}

// Восстановить данные о нашем старом корабле из памяти
//------------------------------------------------------
bool RestoreTempRemoveParam(ref _refCharacter, string _param)
{
    string sParam = "TmpRemember" + _param;
    
	if( !CheckAttribute(_refCharacter, sParam) ) return false;

	aref dstRef; makearef(dstRef, _refCharacter.(_param));
	aref srcRef; makearef(srcRef, _refCharacter.(sParam));

	DeleteAttribute(_refCharacter, _param);
	//--> eddy. структура быть должна, иначе вылет в винду.
	_refCharacter.(_param) = "";
	CopyAttributes(dstRef,srcRef);
	DeleteAttribute(_refCharacter, sParam);
	return true;
}

// установка отмены боевки в резиденции при захвате города
// ВАЖНО: работает только не в пиратском городе - иначе это просто мятеж и губернатора там нет вообще
void SetCaptureResidenceQuest(string _city, string _method, bool _majorOff)
{
    PChar.GenQuestFort.ResidenceQuest.(_city) = true;
    // убирать ли губернатора из локации вообще, он пропадет, но потом обратно пропишется, чтоб не рушить мир игры.
    PChar.GenQuestFort.ResidenceQuest.(_city).MayorOff = _majorOff;
    if (_method != "")
    {
        PChar.GenQuestFort.ResidenceQuest.(_city).method = _method;
	}
}

// вернем диалог после разговора и спрячем
void  SetReturn_Gover_Dialog_Exit(ref NPChar)
{
	LAi_LoginInCaptureTown(NPChar, false);

	PChar.GenQuest.GoverIdx = NPChar.index;
	Pchar.quest.Return_Gover_Dialog_Exit.win_condition.l1          = "ExitFromLocation";
    Pchar.quest.Return_Gover_Dialog_Exit.win_condition.l1.location = Pchar.location;
    Pchar.quest.Return_Gover_Dialog_Exit.win_condition             = "Return_Gover_Dialog_Exit";
}

void SetNewModelToChar(ref chref)
{
    float liveTime = 0.1;
	int colors = argb(64, 64, 64, 64);
	int colore = argb(0, 32, 32, 32);

    if (IsEntity(chref))
    {
    	if(CheckAttribute(chref, "model"))
        {
            SendMessage(chref, "lss",   MSG_CHARACTER_SETMODEL, chref.model, chref.model.animation);
        }
        if(CheckAttribute(chref, "equip.gun"))
        {
            SendMessage(chref, "ls",    MSG_CHARACTER_SETGUN,   chref.equip.gun);
        }
        if(CheckAttribute(chref, "equip.blade"))
        {
            //SendMessage(chref, "lsfll", MSG_CHARACTER_SETBLADE, chref.equip.blade, liveTime, colors, colore);
            SendMessage(chref, "llsfll", MSG_CHARACTER_SETBLADE, 0, chref.equip.blade, liveTime, colors, colore);
        }
    }
}
// функции из квест_реакшн
void SetNationToOfficers(int _nat)
{
    int j, cn;
    ref officer;

    for(j=1; j<COMPANION_MAX; j++)
    {
        cn = GetCompanionIndex(GetMainCharacter(),j);
        if( cn>0 )
        {
	        officer = GetCharacter(cn);
	        if (!GetRemovable(officer)) continue;
	        officer.nation = _nat;
	        Ship_FlagRefresh(officer); //флаг на лету
	    }
	}
}

bool IsOfficerCompanion(ref _refCharacter)
{
	int findIdx = sti(_refCharacter.index);
	ref mc = GetMainCharacter();
	int ci, cn;
	ref npc;
	for(int i=1; i<COMPANION_MAX; i++)
	{
		ci = GetCompanionIndex(mc,i);
		npc = GetCharacter(ci);
		for(int j=1; j<4; j++)
		{
			if(GetOfficersIndex(npc,j)==findIdx) return true;
		}
	}
	return false;
}
///////////////////////////////////////////////////////////////////
void SortItems(ref NPChar)
{// отсортировать предметы в кармане, сундуке
    aref   arToChar;
    aref   arFromChar;
    object objChar;
    int    i;
    aref   curItem;
	string attr;
	ref    itm;
	ref    rObj;
	int    iSortIndex;
	bool   ok;

    objChar.Items = "";
    rObj = &objChar;

    makearef(arToChar,   rObj.Items);
    makearef(arFromChar, NPChar.Items);

    CopyAttributes(arToChar, arFromChar);

    DeleteAttribute(NPChar, "Items");
    NPChar.Items = "";

    makearef(arFromChar, NPChar.equip); // экипировка
    int iMax = GetAttributesNum(arFromChar);
    for(i=0; i<iMax; i++)
    {
        curItem = GetAttributeN(arFromChar, i);
        attr = GetAttributeValue(curItem);
        if (attr != "") //патенты клинит
        {
        	NPChar.Items.(attr) = sti(rObj.Items.(attr));
        }
    }
    // неоптимальная сортировка по индексу itm.SortIndex
	// размерность индекса определяется автоматом - длжен быть непрерывен!!, начинается с 1 - целое число
	ok = true;
	iSortIndex = 1;
	while (iSortIndex < 3)// 2 типа
	{
		ok = false;
		for (i=0; i<ITEMS_QUANTITY; i++)
		{
			makeref(itm, Items[i]);
			attr = itm.id;
			if (CheckAttribute(rObj, "items."+attr) && CheckAttribute(itm, "SortIndex") && sti(itm.SortIndex) == iSortIndex)
			{
                NPChar.Items.(attr) = sti(rObj.Items.(attr));
	   			ok = true;
	      	}
	    }
	    iSortIndex++;
    }
    // все остальные
    for (i=0; i<ITEMS_QUANTITY; i++)
	{
		makeref(itm, Items[i]);
		attr = itm.id;
		if (CheckAttribute(rObj, "items."+attr) && !CheckAttribute(itm, "SortIndex"))
		{
   			NPChar.Items.(attr) = sti(rObj.Items.(attr));
   			ok = true;
      	}
    }
}
// установка квестовых товаров и цен
void SetQuestGoodsToStore(ref refStore)
{
	string    goodName;
	// пример использования по квест_реакшн
	/*
	pchar.GenQuest.StoreGoods.StoreIdx = Bridgetown_STORE;
	pchar.GenQuest.StoreGoods.NowNeedToBe = true;
	*/
	if (CheckAttribute(pchar, "GenQuest.StoreGoods.StoreIdx") && refStore.index == pchar.GenQuest.StoreGoods.StoreIdx)
	{
        // пример обработки события
		/*if (CheckAttribute(pchar, "GenQuest.StoreGoods.NowNeedToBe")) // состояние говорящие, то нужно делать
		{
		    DeleteAttribute(pchar, "GenQuest.StoreGoods"); // одноразовая операция, трем обе ветки
		    goodName = Goods[GOOD_RUM].Name;
		    refStore.Goods.(goodName).Quantity       = 666;
		    // цена из обратного вычисления 20 в модификатор
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_RUM, PRICE_TYPE_BUY, pchar, 20);
		}  */
		if (CheckAttribute(pchar, "GenQuest.StoreGoods.HL2_DiscountPrices")) //Голл.линейка, квест №2, дискаунт в форте Оранж.
		{
		    DeleteAttribute(pchar, "GenQuest.StoreGoods"); // одноразовая операция, трем обе ветки
			goodName = Goods[GOOD_EBONY].Name;
		    pchar.questTemp.HL2Save.EbonyQty = refStore.Goods.(goodName).Quantity;
		    refStore.Goods.(goodName).Quantity       = 400;
		    pchar.questTemp.HL2Save.EbonyPrice = GetStoreGoodsPrice(refStore, GOOD_EBONY, PRICE_TYPE_BUY, pchar, refStore.Goods.(goodName).RndPriceModify);
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_EBONY, PRICE_TYPE_BUY, pchar, 10);

		    goodName = Goods[GOOD_COFFEE].Name;
		    pchar.questTemp.HL2Save.CoffeeQty = refStore.Goods.(goodName).Quantity;
		    refStore.Goods.(goodName).Quantity       = 400;
		    pchar.questTemp.HL2Save.CoffeePrice = GetStoreGoodsPrice(refStore, GOOD_COFFEE, PRICE_TYPE_BUY, pchar, refStore.Goods.(goodName).RndPriceModify);
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_COFFEE, PRICE_TYPE_BUY, pchar, 9);

		    goodName = Goods[GOOD_MAHOGANY].Name;
		    pchar.questTemp.HL2Save.MahoganyQty = refStore.Goods.(goodName).Quantity;
		    refStore.Goods.(goodName).Quantity       = 400;
		    pchar.questTemp.HL2Save.MahoganyPrice =  GetStoreGoodsPrice(refStore, GOOD_MAHOGANY, PRICE_TYPE_BUY, pchar, refStore.Goods.(goodName).RndPriceModify);
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_MAHOGANY, PRICE_TYPE_BUY, pchar, 14);

		    goodName = Goods[GOOD_SANDAL].Name;
		    pchar.questTemp.HL2Save.SandalQty = refStore.Goods.(goodName).Quantity;
		    refStore.Goods.(goodName).Quantity       = 400;
		    pchar.questTemp.HL2Save.SandalPrice = GetStoreGoodsPrice(refStore, GOOD_SANDAL, PRICE_TYPE_BUY, pchar, refStore.Goods.(goodName).RndPriceModify);
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_SANDAL, PRICE_TYPE_BUY, pchar, 11);
        	pchar.GenQuest.StoreGoods.StoreIdx = FortOrange_STORE; //держим цены для след. посещений
        	pchar.GenQuest.StoreGoods.HL2_AgainDiscount = true;
		}
		if (CheckAttribute(pchar, "GenQuest.StoreGoods.HL2_AgainDiscount")) //Голл.линейка, квест №2, держим дисконтные цены
		{
		    goodName = Goods[GOOD_EBONY].Name;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_EBONY, PRICE_TYPE_BUY, pchar, 10);

		    goodName = Goods[GOOD_COFFEE].Name;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_COFFEE, PRICE_TYPE_BUY, pchar, 9);

		    goodName = Goods[GOOD_MAHOGANY].Name;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_MAHOGANY, PRICE_TYPE_BUY, pchar, 14);

		    goodName = Goods[GOOD_SANDAL].Name;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_SANDAL, PRICE_TYPE_BUY, pchar, 11);
		}		
		if (CheckAttribute(pchar, "GenQuest.StoreGoods.HL2_QtyPriceIsBack")) //Голл.линейка, квест №2, возврат цен и кол-ва после квеста.
		{
            DeleteAttribute(pchar, "GenQuest.StoreGoods"); // одноразовая операция, трем обе ветки
		    goodName = Goods[GOOD_EBONY].Name;
		    refStore.Goods.(goodName).Quantity       = pchar.questTemp.HL2Save.EbonyQty;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_EBONY, PRICE_TYPE_BUY, pchar, sti(pchar.questTemp.HL2Save.EbonyPrice));

		    goodName = Goods[GOOD_COFFEE].Name;
		    refStore.Goods.(goodName).Quantity       = pchar.questTemp.HL2Save.CoffeeQty;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_COFFEE, PRICE_TYPE_BUY, pchar, sti(pchar.questTemp.HL2Save.CoffeePrice));

		    goodName = Goods[GOOD_MAHOGANY].Name;
		    refStore.Goods.(goodName).Quantity       = pchar.questTemp.HL2Save.MahoganyQty;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_MAHOGANY, PRICE_TYPE_BUY, pchar, sti(pchar.questTemp.HL2Save.MahoganyPrice));

		    goodName = Goods[GOOD_SANDAL].Name;
		    refStore.Goods.(goodName).Quantity       = pchar.questTemp.HL2Save.SandalQty;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_SANDAL, PRICE_TYPE_BUY, pchar, sti(pchar.questTemp.HL2Save.SandalPrice));
            DeleteAttribute(pchar, "questTemp.HL2Save");
        }
		if (CheckAttribute(pchar, "GenQuest.StoreGoods.HL5_ThreeFleutes")) //Голл.линейка, квест №5, добавляем количества.
		{
            DeleteAttribute(pchar, "GenQuest.StoreGoods"); // одноразовая операция, трем обе ветки
            goodName = Goods[GOOD_FOOD].Name;
		    refStore.Goods.(goodName).Quantity       = 100000;
			refStore.Goods.(goodName).RndPriceModify = GetStoreGoodsRndPriceModify(refStore, GOOD_FOOD, PRICE_TYPE_BUY, pchar, 10);

        }
	}
}

void HolLine2_NullGoods() //для второго квеста голл. линейки.
{
    ref sld;
    int chComp;
    if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed")) pchar.Ship.Cargo.Goods.Ebony = 0;
    if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed")) pchar.Ship.Cargo.Goods.Coffee = 0;
    if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed")) pchar.Ship.Cargo.Goods.Mahogany = 0;
    if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed")) pchar.Ship.Cargo.Goods.Sandal = 0;
    for (int i=1; i<=COMPANION_MAX; i++)
	{
        chComp = GetCompanionIndex(pchar,i);
        if(chComp != -1)
		{
			sld = GetCharacter(chComp);
            if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed")) sld.Ship.Cargo.Goods.Ebony = 0;
            if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed")) sld.Ship.Cargo.Goods.Coffee = 0;
            if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed")) sld.Ship.Cargo.Goods.Mahogany = 0;
            if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed")) sld.Ship.Cargo.Goods.Sandal = 0;
        }
    }
}

// метод обработки капитана в каюте на абордаже. Ставим проверку на минНР + дилог
// refChar - это фантом, а не реальный кэп, он копия, диалог от настоящего, но ИД и индекс не его, а фантомный, поэтому управляем имено этим актером
// CaptanId - реальный ИД кэпа, запомненный в момент копирования.
void SetQuestAboardCabinDialog(ref refChar)
{
	// пример. тут задаю для теста, все это должно быть на изначальном капитане по квестам
	//refChar.DontClearDead      = true;
	//refChar.SaveItemsForDead   =   true;
	//TakeNItems(refChar, "Chest", 1);
	// пример. <--

	if (CheckAttribute(refChar, "CaptanId"))
	{
        pchar.GenQuest.QuestAboardCabinDialogIdx = refChar.index;
        // как использовать, пример
		/*if (true) // true - для теста, должно быть это CheckAttribute(pchar, "квестовая ветка, если нужно") && refChar.CaptanId == "IdNPC")
		{
		    LAi_SetCheckMinHP(refChar, 10, true, "QuestAboardCabinDialog");  // сколько НР мин
            refChar.Dialog.Filename    = "Capitans_dialog.c";   // это нужно задать на изначальном кэпе в др месте
	    	refChar.Dialog.CurrentNode = "QuestAboardCabinDialog";
	    	// все остальное в диалоге, по образцу
		}*/
        //Голл. линейка, квест №7, доставка депеши до Тортуги
        if (refChar.CaptanId == "CaptainAttack_1" && pchar.questTemp.State == "DelivLettTortuga_NotFoundLetter")
		{
		    LAi_SetCheckMinHP(refChar, 10, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		if (refChar.CaptanId == "AntonioDeBarras" && pchar.questTemp.State == "DelivLettTortuga_NotFoundLetter")
		{
		    LAi_SetCheckMinHP(refChar, 10, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
        //Исп. линейка, квест №2, задания Инквизиции
		if (refChar.CaptanId == "JansenDeFonceicao" && CheckAttribute(pchar, "questTemp.State.Usurer"))
		{
		    LAi_SetCheckMinHP(refChar, 10, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//Исп. линейка, квест №3, захват Рока Бразильца
		if (refChar.CaptanId == "RockBrasilian" && pchar.questTemp.State == "TakeRockBras_toNearestMaracaibo")
		{
		    LAi_SetCheckMinHP(refChar, 10, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//Исп. линейка, квест №7, спасение рядового Пардаля, базар с Моисеем Воклейном
		if (refChar.CaptanId == "CaptainAttack_1" && pchar.questTemp.State == "Sp7SavePardal_toAntigua")
		{
		    LAi_SetCheckMinHP(refChar, 50, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		if (refChar.CaptanId == "CaptainAttack_1" && pchar.questTemp.State == "Sp7SavePardal_PardalIsSink")
		{
		    LAi_SetCheckMinHP(refChar, 50, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		if (refChar.CaptanId == "CaptainAttack_1" && pchar.questTemp.State == "Sp8SaveCumana_toCumana")
		{
		    LAi_SetCheckMinHP(refChar, 50, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//фр. линейка, квест №2, атака галеона испанских пиратов
		if (refChar.CaptanId == "CaptainAttack_1" && pchar.questTemp.State == "Fr2Letter_SeekProblems")
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//капитан брига, который везет Изабеллу на убой Сальватору
		if (refChar.CaptanId == "CaptainAttack_1" && pchar.RomanticQuest == "AttackBrig")
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//капитан по квесту мэра на поиск и уничтожение пирата.
		if (refChar.CaptanId == "MQPirate" && CheckAttribute(pchar, "GenQuest.DestroyPirate"))
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
			refChar.dialog.currentnode = "DestroyPirate_Abordage";
		}
		//засада на ГГ по наводке на купца homo
		if (findsubstr(refChar.CaptanId, "_QuestMerchant" , 0) != -1  && trap)
		{
		    LAi_SetCheckMinHP(refChar, 30, true, "QuestAboardCabinDialog");  // сколько НР мин
		    refChar.dialog.filename = "Capitans_dialog.c";  //fix homo 23/03/07
			refChar.dialog.currentnode = "MerchantTrap_Abordage";
		}
		//капитан щебеки Синяя Птица
		if (refChar.CaptanId == "BlueBirdCapitain" && pchar.questTemp.BlueBird == "toSeaBattle")
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//капитан щебеки Синяя Птица
		if (refChar.CaptanId == "BlueBirdTrader" && pchar.questTemp.BlueBird == "attackFleut")
		{
		    LAi_SetCheckMinHP(refChar, 20, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//пиратка, Эдвард Лоу на бриге Морской Волк
		if (refChar.CaptanId == "EdwardLoy" && pchar.questTemp.piratesLine == "KillLoy_SeaWolfBattle")
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//пиратка, квест №7, кэп, продавший Ласточку
		if (refChar.CaptanId == "LeonCapitain" && pchar.questTemp.piratesLine == "Soukins_seekLeon")
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//квест возврата украденного корабля
		if (findsubstr(refChar.CaptanId, "SeekCap_" , 0) != -1)
		{
		    LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
		}
		//квест горожанина по возвращения украденной жены
		if (findsubstr(refChar.CaptanId, "SeekCitizCap_" , 0) != -1)
		{
			if (refChar.quest.SeekCap == "manRapeWife")
			{
				LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
				refChar.Dialog.CurrentNode = "RapeWifeCap_Board"; //даем абордажную ноду
			}
		    if (refChar.quest.SeekCap == "womanRevenge")
			{
				LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
				refChar.Dialog.CurrentNode = "RevengeCap_board"; //даем абордажную ноду
			}
			if (refChar.quest.SeekCap == "womanPirates")
			{
				LAi_SetCheckMinHP(refChar, 5, true, "QuestAboardCabinDialog");  // сколько НР мин
				refChar.Dialog.CurrentNode = "PiratesCap_Board"; //даем абордажную ноду
			}
		}
	}
}

void QuestAboardCabinDialogFree()
{
	ref sld;
	sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
	Lai_SetPlayerType(pchar);
	LAi_RemoveCheckMinHP(sld);
	LAi_SetImmortal(sld, false);
    LAi_SetWarriorType(sld);
    LAi_group_MoveCharacter(sld, LAI_GROUP_BRDENEMY);
}
//выход без боевки, но в плен не надо
void QuestAboardCabinDialogNotBattle()
{
	ref sld;
	sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
	Lai_SetPlayerType(pchar);
	LAi_RemoveCheckMinHP(sld);
	LAi_SetImmortal(sld, false);
    LAi_SetWarriorType(sld);
	DoQuestCheckDelay("LAi_ReloadBoarding", 1.0);
}
// выход с боевкой
void QuestAboardCabinDialogExitWithBattle(string _questName)
{
    QuestAboardCabinDialogFree(); // важный метод
	ref sld;
	sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
	LAi_SetFightMode(pchar, true);
	LAi_SetFightMode(sld, true);
	LAi_group_SetRelation(LAI_GROUP_BRDENEMY, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
    LAi_group_FightGroups(LAI_GROUP_BRDENEMY, LAI_GROUP_PLAYER, true);
	LAi_group_SetCheck(LAI_GROUP_BRDENEMY, _questName);
	// SetCharacterTask_Fight(sld, pchar);
}

void QuestAboardCabinDialogExitWithBattleNoParam()// homo тоже самое, только без параметров
{
    QuestAboardCabinDialogExitWithBattle("");
}

void QuestAboardCabinDialogSurrender()
{
 	ref sld;
	sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
	DeleteAttribute(sld, "DontRansackCaptain"); // если было зачем-то
	pchar.GenQuest.QuestAboardCaptanSurrender = true;
	Lai_SetPlayerType(pchar);
	LAi_RemoveCheckMinHP(sld);
	LAi_SetImmortal(sld, false);
	//на форме убиваем LAi_SetCurHP(characterFromId(sld.CaptanId), 0.0); 
	//sld.LifeDay = 0;    это не фантом многодневка, а фантом локации, трется он сам при закрузке локации, но не при выгрузке
	DoQuestCheckDelay("LAi_ReloadBoarding", 1.0);
}
//==> квестовое пленение с сохранением индекса пленного.
void QuestAboardCabinDialogQuestSurrender()
{
	ref sld;
	sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)];
	sld.DontRansackCaptain = true; // чтоб не сдался второй раз
	Lai_SetPlayerType(pchar);
	LAi_RemoveCheckMinHP(sld);
	LAi_SetImmortal(sld, false);
	//на форме убиваем LAi_SetCurHP(characterFromId(sld.CaptanId), 0.0); 
	//sld.LifeDay = 0;
	pchar.GenQuest.LastQuestPrisonerIdx = SetCharToPrisoner(sld);
	SetCharacterRemovable(sld, false);
	DoQuestCheckDelay("LAi_ReloadBoarding", 1.0);
}
//==> eddy. квестовая обработка 'ноль часов'.
void QuestActions()
{
	ref sld;
	// забираем просроченные лицензии
	for (i=0; i<4; i++)
	{
		if (CheckCharacterItem(pchar, NationShortName(i)+"TradeLicence") && GetDaysContinueNationLicence(i) == -1)
		{
			TakeNationLicence(i);
		}		
	}
	//Если идет линейка ГПК, то шмаляем открытые сундуки. Все, кроме Диффиндура.
	if (sti(pchar.questTemp.LSC.checkBoxes))
	{
		LSC_checkBoxes();
	}
	//************** Фр.линейка, третий квест. открываем двери дома коменданта Гаваны *******************
	if (pchar.questTemp.State == "Fr3TakeAnna_WaitMidnight")
	{
		LocatorReloadEnterDisable("Havana_town", "houseS1", false);
        Pchar.quest.Fr3TakeAnna_FightInHouse.win_condition.l1 = "location";
        Pchar.quest.Fr3TakeAnna_FightInHouse.win_condition.l1.location = "Havana_houseS1";
        Pchar.quest.Fr3TakeAnna_FightInHouse.win_condition = "Fr3TakeAnna_FightInHouse";
		pchar.questTemp.State = "Fr3TakeAnna_DoorIsOpen";
	}
	//************** Изабелла, вечер ужина после спасения братца наступил *******************
	if (CheckAttribute(pchar, "RomanticQuest") && pchar.RomanticQuest == "DelivMigel")
	{		
		pchar.RomanticQuest = "SeeTalkNearHouse"; //детектор на итеме теперь сработает        
        sld = CharacterFromID("MigelDeValdes");
		//Мигель
        sld.dialog.currentnode = "TalkNearHouse_1";
        ChangeCharacterAddressGroup(sld, "SanJuan_town", "quest", "goto24");
        LAi_SetActorType(sld); 
		sld.rank = 1; //чтобы умер сразу
		sld.skill.Fencing = 1;                
		sld.skill.Pistol = 1;
		LAi_SetHP(sld, 1, 1);
		DeleteAttribute(sld, "Items");
		GiveItem2Character(sld, "blade1");
		GiveItem2Character(sld, "pistol1");
		GiveItem2Character(sld, "order");
		TakeNItems(sld,"bullet", 1); 
		//Сальватор с бандюганами
		LAi_group_Delete("EnemyFight");
        sld = CharacterFromID("Husband");
        sld.dialog.currentnode = "TalkNearHouse_1";
        ChangeCharacterAddressGroup(sld, "SanJuan_town", "quest", "goto25");		
        for (int i=8; i<=9; i++)
        {
			sld = GetCharacter(NPC_GenerateCharacter("Bandit_"+i, "pirate_"+i, "man", "man", 30, PIRATE, 1, true));
			FantomMakeCoolFighter(sld, 30, 80, 70, "blade28", "pistol3", 100);			
			LAi_SetStayType(sld);			
			if (i==8) ChangeCharacterAddressGroup(sld, "SanJuan_town", "quest", "goto7");
			else ChangeCharacterAddressGroup(sld, "SanJuan_town", "quest", "goto26");
			LAi_group_MoveCharacter(sld, "EnemyFight");
		}
		SetTimerCondition("Romantic_DeadBrother_Cancel", 0, 0, 1, false);
	}
	//************** Изабелла, свадьба!!!! *******************
	if (CheckAttribute(pchar, "RomanticQuest") && pchar.RomanticQuest == "WaitBeginWidding")
	{	
		pchar.RomanticQuest = "BeginWidding";
		pchar.quest.Isabella_widding.win_condition.l1 = "locator";
		pchar.quest.Isabella_widding.win_condition.l1.location = "SanJuan_church";
		pchar.quest.Isabella_widding.win_condition.l1.locator_group = "quest";
		pchar.quest.Isabella_widding.win_condition.l1.locator = "detector1";
		pchar.quest.Isabella_widding.function = "Isabella_widding";
		sld = CharacterFromID("Isabella");
		LAi_SetActorType(sld);
		ChangeCharacterAddressGroup(sld , "SanJuan_church", "goto",  "goto11");
		sld.dialog.currentnode = "TalkWidding";
		//падре
		LAi_SetActorType(CharacterFromID("SanJuan_Priest"));
		SetTimerCondition("Romantic_Widding_Cancel", 0, 0, 1, false);
	}
	//************** ночной сторож в церкви *******************
	if (CheckAttribute(pchar, "questTemp.different.Church_NightGuard"))
	{
		if (pchar.questTemp.different.Church_NightGuard == pchar.location)
		{
			if (!CheckAttribute(pchar, "pchar.quest.ChurchNightGuard_exitFromLoc") && sti(pchar.questTemp.different.Church_NightGuard.NightQty) < 3)
			{
				pchar.quest.ChurchNightGuard_exitFromLoc.win_condition.l1 = "ExitFromLocation";
				pchar.quest.ChurchNightGuard_exitFromLoc.win_condition.l1.location = pchar.location;
				pchar.quest.ChurchNightGuard_exitFromLoc.function = "ChurchNightGuard_exitFromLoc";
				DoQuestFunctionDelay("ChurchNightGuard_timerInChurch", 10.0 + frand(10.0));
			}
		}
		else
		{	//ГГ не находится в нужной церкви в момент полночи
			string sTitle = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
			AddQuestRecordEx(sTitle, "Church_NightGuard", "2");
			CloseQuestHeader(sTitle);
			DeleteAttribute(pchar, "questTemp.different.Church_NightGuard");
			ChangeCharacterReputation(pchar, -20);
		}
	}
	//************** линейка ГПК, переход в  *******************
	if (pchar.questTemp.LSC == "toWineCellar" )
	{
		if (GetHour() < 1)
		{
			pchar.questTemp.LSC = "toWineCellarGone";
			sld = characterFromId("LSCBarmen");
			LAi_SetStayTypeNoGroup(sld);
			ChangeCharacterAddressGroup(sld, "FleuronTavern", "goto", "goto10");
			PChar.quest.LSC_returnBarmen.win_condition.l1            = "Timer";
			PChar.quest.LSC_returnBarmen.win_condition.l1.date.hour  = 1;
			PChar.quest.LSC_returnBarmen.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 0);
			PChar.quest.LSC_returnBarmen.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
			PChar.quest.LSC_returnBarmen.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 0);
			PChar.quest.LSC_returnBarmen.function = "LSC_returnBarmen";
		}
		else pchar.questTemp.LSC = "toWineCellarLate";
	}
}
// eddy. лицензии торговых компаний. -->
//дать лицензию
void GiveNationLicence(int _nation, int _validity)
{
	string sTemp; 
	ref rItem;
	for(int i=0; i<4; i++)
	{
		if (CheckNationLicence(i) && GetDaysContinueNationLicence(i) == -1) TakeNationLicence(i);
	}
	if (_nation != PIRATE) 
	{
		if (CheckNationLicence(_nation)) TakeNationLicence(_nation);
		sTemp = NationShortName(_nation)+"TradeLicence";
		GiveItem2Character(pchar, sTemp);
		rItem = ItemsFromID(sTemp);
		SaveCurrentNpcQuestDateParam(rItem, "Action_date");
		rItem.Action_date = GetCurrentDate(); 
		rItem.Validity = FindRussianDaysString(_validity); //строка для дескрайба
		rItem.Validity.QtyDays = _validity; //время действия лицензии в днях для расчетов
	}
}
//забрать лицензию 
void TakeNationLicence(int _nation)
{
	string sTemp; 
	if (_nation != PIRATE && CheckNationLicence(_nation)) 
	{
		sTemp = NationShortName(_nation)+"TradeLicence";
		TakeItemFromCharacter(pchar, sTemp);
		DeleteAttribute(ItemsFromID(sTemp), "Action_date");
		DeleteAttribute(ItemsFromID(sTemp), "Validity");
	}
}
//проверить наличие лицензии
bool CheckNationLicence(int _nation)
{
	if (_nation != PIRATE) 
	{
		if (CheckCharacterItem(pchar, NationShortName(_nation)+"TradeLicence")) return true; 
	}
	return false;
}
//проверить сроки лицензии, сколько осталось дней. если -1, то просрочена или отсутствует
int GetDaysContinueNationLicence(int _nation)
{
	int iTerms = -1;
	ref rItem;
	if (_nation != PIRATE) 
	{
		if (CheckNationLicence(_nation))
		{
			rItem = ItemsFromID(NationShortName(_nation)+"TradeLicence");
			int Validity = sti(rItem.Validity.QtyDays);
			iTerms = GetNpcQuestPastDayParam(rItem, "Action_date");
			if (iTerms > Validity) iTerms = -1;
			else iTerms = Validity - iTerms;
		}
	}
	return iTerms;
}
//дать наименование лицензии, например "Лицензия Голландской Вест-Индской компании"
string GetRusNameNationLicence(int _nation)
{
	string sTemp, itmTitle;
	int lngFileID;
	if (_nation != PIRATE) 
	{
		if (CheckNationLicence(_nation))
		{
			sTemp = NationShortName(_nation)+"TradeLicence";
			lngFileID = LanguageOpenFile("ItemsDescribe.txt");
			itmTitle = LanguageConvertString(lngFileID, Items[GetItemIndex(sTemp)].name);
		}
	}
	return itmTitle;
}
// eddy. лицензии торговых компаний. <--

//--> eddy. квест Изабеллы, методы для контроля семейной жизни
//запишем текущие показтели статистики. записать так же не все, а один из показателей.
// если _TypeOfCheck == "all", то обнулить все. _TypeOfCheck может быть "KillFort", "TakeTown", 
void IsabellaSetCurrentState(string _TypeOfCheck)
{
	if (_TypeOfCheck == "all" || _TypeOfCheck == "KillFort")
	{
		DeleteAttribute(pchar, "RomanticQuest.Cheking.KillFort");
		pchar.RomanticQuest.State.KillFort = Statistic_AddValue(PChar, "pir_KillFort", 0) +
											Statistic_AddValue(PChar, "eng_KillFort", 0) +
											Statistic_AddValue(PChar, "fra_KillFort", 0) +
											Statistic_AddValue(PChar, "spa_KillFort", 0) +
											Statistic_AddValue(PChar, "hol_KillFort", 0);
	}
	if (_TypeOfCheck == "all" || _TypeOfCheck == "TakeTown")
	{
		DeleteAttribute(pchar, "RomanticQuest.Cheking.TakeTown");
		pchar.RomanticQuest.State.TakeTown = Statistic_AddValue(PChar, "pir_TakeTown", 0) +
											Statistic_AddValue(PChar, "eng_TakeTown", 0) +
											Statistic_AddValue(PChar, "fra_TakeTown", 0) +
											Statistic_AddValue(PChar, "spa_TakeTown", 0) +
											Statistic_AddValue(PChar, "hol_TakeTown", 0);
	}
	if (_TypeOfCheck == "all" || _TypeOfCheck == "GrabbingTown")
	{
		DeleteAttribute(pchar, "RomanticQuest.Cheking.GrabbingTown");
		pchar.RomanticQuest.State.GrabbingTown = Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
												Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
												Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
												Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
  												Statistic_AddValue(PChar, "hol_GrabbingTown", 0);
	}
	if (_TypeOfCheck == "all" || _TypeOfCheck == "AbordShip")
	{
		DeleteAttribute(pchar, "RomanticQuest.Cheking.AbordShip");
		pchar.RomanticQuest.State.AbordShip = Statistic_AddValue(PChar, "pir_AbordShip", 0) +
											Statistic_AddValue(PChar, "eng_AbordShip", 0) +
											Statistic_AddValue(PChar, "fra_AbordShip", 0) +
											Statistic_AddValue(PChar, "spa_AbordShip", 0) +
											Statistic_AddValue(PChar, "hol_AbordShip", 0);
	}
	if (_TypeOfCheck == "all" || _TypeOfCheck == "KillShip")
	{
		DeleteAttribute(pchar, "RomanticQuest.Cheking.KillShip");	
		pchar.RomanticQuest.State.KillShip = Statistic_AddValue(PChar, "pir_KillShip", 0) +
											Statistic_AddValue(PChar, "eng_KillShip", 0) +
											Statistic_AddValue(PChar, "fra_KillShip", 0) +
											Statistic_AddValue(PChar, "spa_KillShip", 0) +
											Statistic_AddValue(PChar, "hol_KillShip", 0);
	}
}
//проверим текущее состояние в рамках контроля за статистикой
void IsabellaCheckBreachState()
{
	int iTemp;
	iTemp = Statistic_AddValue(PChar, "pir_KillFort", 0) +
			Statistic_AddValue(PChar, "eng_KillFort", 0) +
			Statistic_AddValue(PChar, "fra_KillFort", 0) +
			Statistic_AddValue(PChar, "spa_KillFort", 0) +
			Statistic_AddValue(PChar, "hol_KillFort", 0);
	if (iTemp > sti(pchar.RomanticQuest.State.KillFort)) pchar.RomanticQuest.Cheking.KillFort = iTemp - sti(pchar.RomanticQuest.State.KillFort);
	iTemp = Statistic_AddValue(PChar, "pir_TakeTown", 0) +
			Statistic_AddValue(PChar, "eng_TakeTown", 0) +
			Statistic_AddValue(PChar, "fra_TakeTown", 0) +
			Statistic_AddValue(PChar, "spa_TakeTown", 0) +
			Statistic_AddValue(PChar, "hol_TakeTown", 0);
	if (iTemp > sti(pchar.RomanticQuest.State.TakeTown)) pchar.RomanticQuest.Cheking.TakeTown = iTemp - sti(pchar.RomanticQuest.State.TakeTown);
	iTemp = Statistic_AddValue(PChar, "pir_GrabbingTown", 0) +
			Statistic_AddValue(PChar, "eng_GrabbingTown", 0) +
			Statistic_AddValue(PChar, "fra_GrabbingTown", 0) +
			Statistic_AddValue(PChar, "spa_GrabbingTown", 0) +
  			Statistic_AddValue(PChar, "hol_GrabbingTown", 0);
	if (iTemp > sti(pchar.RomanticQuest.State.GrabbingTown)) pchar.RomanticQuest.Cheking.GrabbingTown = iTemp - sti(pchar.RomanticQuest.State.GrabbingTown);
	iTemp = Statistic_AddValue(PChar, "pir_AbordShip", 0) +
			Statistic_AddValue(PChar, "eng_AbordShip", 0) +
			Statistic_AddValue(PChar, "fra_AbordShip", 0) +
			Statistic_AddValue(PChar, "spa_AbordShip", 0) +
			Statistic_AddValue(PChar, "hol_AbordShip", 0);
	if (iTemp > sti(pchar.RomanticQuest.State.AbordShip)) pchar.RomanticQuest.Cheking.AbordShip = iTemp - sti(pchar.RomanticQuest.State.AbordShip);
	iTemp = Statistic_AddValue(PChar, "pir_KillShip", 0) +
			Statistic_AddValue(PChar, "eng_KillShip", 0) +
			Statistic_AddValue(PChar, "fra_KillShip", 0) +
			Statistic_AddValue(PChar, "spa_KillShip", 0) +
			Statistic_AddValue(PChar, "hol_KillShip", 0);
	if (iTemp > sti(pchar.RomanticQuest.State.KillShip)) pchar.RomanticQuest.Cheking.KillShip = iTemp - sti(pchar.RomanticQuest.State.KillShip);		
	return;
}
//сбросить счетчик по семейному бюджету
void IsabellaNullBudget()
{
	pchar.RomanticQuest.Budget = "0";
	SaveCurrentQuestDateParam("RomanticQuest.Budget");
}
//возвращает текущую задолженность в деньгах. положительное значение - задолж., отрицательное - профицит.
int IsabellaCheckBudgetMoney()
{
	int Debt = GetQuestPastMonthParam("RomanticQuest.Budget")*MOD_SKILL_ENEMY_RATE*10000;
	int CurMoney = sti(pchar.RomanticQuest.Budget);
	return Debt - CurMoney;
}
//добавить денег на текущий баланс бюджета
void IsabellaAddMoneyBudget(int _money)
{
	pchar.RomanticQuest.Budget = sti(pchar.RomanticQuest.Budget) + _money;
	AddMoneyToCharacter(pchar, -_money);
}
// <-- eddy. квест Изабеллы, методы для контроля семейной жизни

void SelectAscoldRendom()
{
	if (CheckAttribute(&colonies[1], "nation"))
	{
		int n, nation;
		int storeArray[MAX_COLONIES];
		int howStore = 0;
		for(n=0; n<MAX_COLONIES; n++)
		{			
			if (colonies[n].nation != "none" && sti(colonies[n].nation) != PIRATE && colonies[n].id != "FortOrange")
			{           
				storeArray[howStore] = n;
				howStore++;
			}
		}
		nation = storeArray[rand(howStore-1)];
		pchar.questTemp.Ascold.TraderId = colonies[nation].id + "_trader";
		nation = storeArray[rand(howStore-1)];
		pchar.questTemp.Ascold.MerchantColony = colonies[nation].id; 
		nation = storeArray[rand(howStore-1)];
		while (colonies[nation].id == "Panama")
		{
			nation = storeArray[rand(howStore-1)];
		}
		pchar.questTemp.Ascold.ShipyarderId = colonies[nation].id + "_shipyarder"; 
		pchar.questTemp.Ascold.ShipyarderColony = colonies[nation].id;
		items[GetItemIndex("Azzy_bottle")].useLocation = colonies[nation].id + "_Shipyard";
	}
}
// квест Изабеллы
void IsabellaInit()
{
	ref ch;
	// ==> состояние квеста Изабеллы, если равен "" - то квест Атилла не даст.
	pchar.RomanticQuest = "canTakeQuest";
	//Атилла
	ch = GetCharacter(NPC_GenerateCharacter("Atilla", "pirate_1", "man", "man", 22, PIRATE, -1, false));
	ch.name 	= "Атилла";
	ch.lastname 	= "";
	GiveItem2Character(ch, GUN_COMMON);
	ch.equip.gun = GUN_COMMON;
	GiveItem2Character(ch, BLADE_LONG);
	ch.equip.blade = BLADE_LONG;
	ch.location	= "Pirates_town";
	ch.location.group = "goto";
	ch.location.locator = "goto1";
	ch.Dialog.Filename = "Quest\Isabella\Atilla.c";
	ch.greeting = "Gr_Smuggler Agent";
	ch.rank 	= 25;
	ch.reputation = "30";
	ch.talker = 5; //начать диалог
    SetRandSPECIAL(ch);
    SetSelfSkill(ch, 80, 70, 90, 60, 50);
    SetShipSkill(ch, 50, 10, 60, 60, 40, 70, 70, 40, 50);
	ch.money = "200";
	LAi_SetCitizenType(ch);
	LAi_group_MoveCharacter(ch, "PIRATE_CITIZENS");
	LAi_SetLoginTime(ch, 0.0, 24.0);
	LAi_SetHP(ch, 200.0, 200.0);
	LAi_NoRebirthEnable(ch);
	ch.DontClearDead = true;  // не убирать труп
	ch.greeting = "Ascold";
	// Злой муж :)
	ch = GetCharacter(NPC_GenerateCharacter("Husband", "Usurer_7", "man", "man", 22, PIRATE, -1, false));	
	ch.name 	= "Сальватор";
	ch.lastname = "Олеварес";
	ch.location	= "SanJuan_houseSp6";
	ch.location.group = "goto";
	ch.location.locator = "goto2";
	ch.Dialog.Filename = "Quest\Isabella\Husband.c";
	GiveItem2Character(ch, "pistol5");
	ch.equip.gun = "pistol5";
	TakeNItems(ch,"bullet", Rand(20)+20);
	GiveItem2Character(ch, "blade34");
	ch.equip.blade = "blade34";
	ch.rank 	= 40;
	ch.reputation = "35";
    SetRandSPECIAL(ch);
    SetSelfSkill(ch, 90, 90, 90, 70, 90);
    SetShipSkill(ch, 20, 90, 10, 10, 10, 20, 10, 10, 50);
	ch.money = "1000";
	ch.SaveItemsForDead = true; // сохранять на трупе вещи
	ch.DontClearDead = true; // не убирать труп через 200с
	LAi_SetLoginTime(ch, 0.0, 24.0);
	LAi_SetHP(ch, 220.0, 220.0);
	LAi_SetHuberStayType(ch);
	LAi_SetStayHuberPointWindow(ch, "goto", "goto1");
	LAi_SetStayHuberPointMap(ch, "goto", "goto2");
	LAi_group_MoveCharacter(ch, "SPAIN_CITIZENS");
	LAi_SetImmortal(ch, true);
	LAi_LoginInCaptureTown(ch, true);
	ch.greeting = "IsabellaHasband";
}
//установить говорилку на горожанина. нужно по Изабелле.
void SetIsabellaTalker(string qName)
{
	ref chr = characterFromID("Isabella");
	chr.talker = 10;
}

void BerglarsInit()
{
	ref sld;
	pchar.questTemp.tugs.berglarState = 1; //счетчик
	//============> грабитель в Сент-Джонсе
	pchar.questTemp.tugs.berglarSentJons = "SentJons_TownCave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarSentJons.hp = 160; //сколько HP
	pchar.questTemp.tugs.berglarSentJons.locator = "basement1"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarSentJons", "citiz_12", "man", "man", 22, ENGLAND, -1, false));
	sld.name 	= "Альберт";
	sld.lastname = "Грин";
	sld.rank = 25;
	sld.city = "SentJons";
	sld.location	= "SentJons_town";
	sld.location.group = "goto";
	sld.location.locator = "goto4";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol2");
	GiveItem2Character(sld, "blade21");
	sld.money = 11460;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 10, 60, 40, 70);
	SetCharacterPerk(sld, "SwordplayProfessional");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//=============> грабитель на Мартинике
	pchar.questTemp.tugs.berglarFortFrance = "FortFrance_Dungeon"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarFortFrance.hp = 180; //сколько HP
	pchar.questTemp.tugs.berglarFortFrance.locator = "basement1"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarFortFrance", "citiz_11", "man", "man", 23, FRANCE, -1, false));
	sld.name 	= "Гаэтано";
	sld.lastname = "Лемье";
	sld.rank = 25;
	sld.city = "FortFrance";
	sld.location	= "FortFrance_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol3");
	GiveItem2Character(sld, "blade24");
	sld.money = 22670;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 70, 10, 50, 60);
	SetCharacterPerk(sld, "BasicDefense");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//============> грабитель в Мариго 
	pchar.questTemp.tugs.berglarMarigo = "Marigo_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarMarigo.hp = 200; //сколько HP
	pchar.questTemp.tugs.berglarMarigo.locator = "gate"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarMarigo", "citiz_1", "man", "man", 21, HOLLAND, -1, false));
	sld.name 	= "Эстебан";
	sld.lastname = "Морэ";
	sld.rank = 25;
	sld.city = "Marigo";
	sld.location	= "Marigo_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol6");
	GiveItem2Character(sld, "blade22");
	sld.money = 27480;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 60, 10, 10, 40, 40);
	SetCharacterPerk(sld, "CriticalHit");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	//============> грабитель в Панаме
	pchar.questTemp.tugs.berglarPanama = "Panama_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarPanama.hp = 200; //сколько HP
	pchar.questTemp.tugs.berglarPanama.locator = "gate"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarPanama", "citiz_4", "man", "man", 27, SPAIN, -1, false));
	sld.name 	= "Хуан";
	sld.lastname = "Карлос";
	sld.rank = 27;
	sld.city = "Panama";
	sld.location	= "Panama_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol3");
	GiveItem2Character(sld, "blade33");
	sld.money = 30180;
	sld.talker = 8; //начать диалог
	SetSelfSkill(sld, 10, 10, 70, 50, 70);
	SetCharacterPerk(sld, "AdvancedDefense");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//============> грабитель в Картахене
	pchar.questTemp.tugs.berglarCartahena = "Cartahena_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarCartahena.hp = 170; //сколько HP
	pchar.questTemp.tugs.berglarCartahena.locator = "gate"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarCartahena", "citiz_7", "man", "man", 22, SPAIN, -1, false));
	sld.name 	= "Мигель";
	sld.lastname = "Карлитос";
	sld.rank = 22;
	sld.city = "Cartahena";
	sld.location	= "Cartahena_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol6");
	GiveItem2Character(sld, "blade23");
	sld.money = 19980;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 60, 10, 10, 50, 50);
	SetCharacterPerk(sld, "Sliding");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//============> грабитель в Санта Каталине
	pchar.questTemp.tugs.berglarSantaCatalina = "SantaCatalina_Cave"; //указание где забивать стрелу
	pchar.questTemp.tugs.berglarSantaCatalina.hp = 230; //сколько HP
	pchar.questTemp.tugs.berglarSantaCatalina.locator = "gate"; //выходной локатор
	sld = GetCharacter(NPC_GenerateCharacter("BerglarSantaCatalina", "citiz_9", "man", "man", 25, SPAIN, -1, false));
	sld.name 	= "Лоренсо";
	sld.lastname = "Сантино";
	sld.rank = 25;
	sld.city = "SantaCatalina";
	sld.location	= "SantaCatalina_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.dialog.filename   = "Quest\Berglars.c";
	sld.greeting = "pirat_quest";
	GiveItem2Character(sld, "pistol4");
	GiveItem2Character(sld, "topor2");
	sld.money = 30450;
	sld.talker = 7; //начать диалог
	SetSelfSkill(sld, 10, 10, 80, 50, 80);
	SetCharacterPerk(sld, "CriticalHit");
	LAi_SetHP(sld, 1.0, 1.0);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
}

void QuestionsInit()
{
	ref sld;
	//============> база вопросов, специально делаю в коде, не в ини. шоб не ковыряли.
	NullCharacter.questions = 21; //кол-во вопросов (нумерация с нуля, соотв. последней записи структуры !!!)
	NullCharacter.questions.q0 = "Как называется группа тросов, закрепленных в просверленном переднем крае марсовой площадки и проходящих через специальный блок на штаге? Подсказка: нужна для того, чтобы избежать перетирания марселей о штаги и зацепления за марсы.";
	NullCharacter.questions.a0 = "анапуть";
	NullCharacter.questions.q1 = "Назовите небольшой деревянный бочонок для запасов пресной воды?";
	NullCharacter.questions.a1 = "анкерок";
	NullCharacter.questions.q2 = "Как называется деревянная цилиндрическая колодка со щеткой, насаженная на древко, служащая для очистки канала орудия от порохового нагара?";
	NullCharacter.questions.a2 = "банник";
	NullCharacter.questions.q3 = "Как называется усиленный ряд наружной обшивки корпуса судна над ватерлинией?";
	NullCharacter.questions.a3 = "бархоут";
	NullCharacter.questions.q4 = "Как называется кусок троса, обшитый кожей, с помощью которого рей или гафель удерживается в обхват у мачты или стеньги?";
	NullCharacter.questions.a4 = "бейфут";
	NullCharacter.questions.q5 = "Как называется перевязка двух тросов тонким тросом или линем?";
	NullCharacter.questions.a5 = "бензель";
	NullCharacter.questions.q6 = "Назовите полное имя пирата и лекаря, прославившегося тем, что написал книгу о флибустьерах и издал ее в Старом Свете. Участник знаменитых походов Генри Моргана.";
	NullCharacter.questions.a6 = "александр оливье эксквемелин";
	NullCharacter.questions.q7 = "Назовите одним словом совокупность судовых снастей.";
	NullCharacter.questions.a7 = "такелаж";
	NullCharacter.questions.q8 = "Назовите одним словом все деревянные принадлежности парусного вооружения судна, к которым прикрепляются снасти и паруса.";
	NullCharacter.questions.a8 = "рангоут";
	NullCharacter.questions.q9 = "Назовите одним словом железный обруч для связывания некоторых частей рангоута.";
	NullCharacter.questions.a9 = "бугель";
	NullCharacter.questions.q10 = "Назовите полное имя античного государственного деятеля, которого пираты захватили в плен в Эгейском море ради выкупа. Выкуп был уплачен, пиратов почти сразу изловили и распяли, как и обещал этот человек, находясь у них на борту. К словам таких людей нужно относиться серьезно.";
	NullCharacter.questions.a10 = "гай юлий цезарь";
	NullCharacter.questions.q11 = "Назовите полное имя выдающегося римского военноначальника, соратника Гая Юлия Цезаря по триумвирату, который в 67 году до н.э. в три месяца очистил Средиземное море от пиратов и уничтожил Тортугу тех времен - Киликию.";
	NullCharacter.questions.a11 = "гней помпей великий";
	NullCharacter.questions.q12 = "Какая великая морская держава спроектировала и первой начала строить тип судов 'фрегат'?";
	NullCharacter.questions.a12 = "франция";
	NullCharacter.questions.q13 = "Назовите первый английский город, основанный на побережье Америки.";
	NullCharacter.questions.a13 = "джеймстаун";
	NullCharacter.questions.q14 = "Как назывался нормативный акт, разделивший в 1494 году Новый свет между Испанией и Португалией по 50-й долготе?";
	NullCharacter.questions.a14 = "тордесильясский договор";
	NullCharacter.questions.q15 = "Назовите полное имя португальца, первым совершившего кругосветное плавание.";
	NullCharacter.questions.a15 = "фернан магеллан";
	NullCharacter.questions.q16 = "Как назывался первый корабль, обошедший землю вокруг света в экспедиции Магеллана и вернувшийся в порт приписки.";
	NullCharacter.questions.a16 = "виктория";
	NullCharacter.questions.q17 = "Назовите полное имя человека, совершившего второе кругосветное плавание.";
	NullCharacter.questions.a17 = "фрэнсис дрейк";
	NullCharacter.questions.q18 = "Назовите полное имя человека, совершившего третье кругосветное плавание.";
	NullCharacter.questions.a18 = "томас кавендиш";
	NullCharacter.questions.q19 = "Назовите первый европейский город, заложенный испанцами в Америке.";
	NullCharacter.questions.a19 = "санто-доминго";
	NullCharacter.questions.q20 = "Назовите полное имя первого пирата Карибского моря.";
	NullCharacter.questions.a20 = "бернардино де талавера";
	NullCharacter.questions.q21 = "Как сейчас называется остров в Карибском море, открытый Христофором Колумбом в 1494 году и названый им островом Сантьяго.";
	NullCharacter.questions.a21 = "ямайка";
	//============> спрашиватель в Тринидаде
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsPortSpein", "citiz_"+(rand(11)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.rank = 5;
	sld.city = "PortSpein";
	sld.location	= "PortSpein_town";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "PortSpein";
	sld.greeting = "cit_common";
	sld.talker = 4; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	SetSelfSkill(sld, 10, 10, 10, 10, 10);
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//============> спрашивательница в Виллемстде
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsVillemstad", "girl_"+(rand(7)+1), "woman", "towngirl", 5, HOLLAND, -1, false));
	sld.rank = 5;
	sld.city = "Villemstad";
	sld.location	= "Villemstad_town";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "Villemstad";
	sld.greeting = "Gr_Woman_Citizen";
	sld.talker = 3; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	//============> спрашивательница в Порт-о-Принсе
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsPortPax", "girl_"+(rand(7)+1), "woman", "towngirl", 5, FRANCE, -1, false));
	sld.rank = 5;
	sld.city = "PortPax";
	sld.location	= "PortPax_town";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "PortPax";
	sld.greeting = "Gr_Woman_Citizen";
	sld.talker = 6; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//============> спрашиватель в Сан-Хуане
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsSanJuan", "citiz_"+(rand(11)+1), "man", "man", 5, SPAIN, -1, false));
	sld.rank = 5;
	sld.city = "SanJuan";
	sld.location	= "SanJuan_town";
	sld.location.group = "goto";
	sld.location.locator = "goto1";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "SanJuan";
	sld.greeting = "cit_common";
	sld.talker = 6; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//============> спрашиватель в Кумане
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsCumana", "citiz_"+(rand(11)+1), "man", "man", 5, SPAIN, -1, false));
	sld.rank = 5;
	sld.city = "Cumana";
	sld.location	= "Cumana_town";
	sld.location.group = "goto";
	sld.location.locator = "goto10";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "Cumana";
	sld.greeting = "cit_common";
	sld.talker = 6; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//============> спрашиватель в Порт Рояле
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsPortRoyal", "girl_"+(rand(7)+1), "woman", "towngirl", 5, ENGLAND, -1, false));
	sld.rank = 5;
	sld.city = "PortRoyal";
	sld.location	= "PortRoyal_town";
	sld.location.group = "goto";
	sld.location.locator = "goto10";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "PortRoyal";
	sld.greeting = "Gr_Woman_Citizen";
	sld.talker = 3; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//============> спрашиватель в Сантьяго
	sld = GetCharacter(NPC_GenerateCharacter("QuetionsSantiago", "citiz_"+(rand(11)+1), "man", "man", 5, SPAIN, -1, false));
	sld.rank = 5;
	sld.city = "Santiago";
	sld.location	= "Santiago_town";
	sld.location.group = "goto";
	sld.location.locator = "goto10";
	sld.dialog.filename   = "Quest\Questions.c";
	sld.dialog.currentnode   = "Santiago";
	sld.greeting = "cit_common";
	sld.talker = 5; //начать диалог
	sld.quest.numQuestion = SelectQuestions(); //номер вопроса
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");

}
// ==> обработка базы вопросов, относится к методу выше
int SelectQuestions()
{
	bool bOk = true;
	int iTemp;
	string sTemp;
	while (bOk)
	{
		iTemp = rand(sti(NullCharacter.questions));
		sTemp = "m" + iTemp;
		if (!CheckAttribute(NullCharacter, "questions" + sTemp))
		{
			NullCharacter.questions.(sTemp) = true;
			break;
		}
	}
	return iTemp;
}
// -- нищие --
void PoormansInit()
{
	ref sld;
	//нищий в Сент-Джонсе
	sld = GetCharacter(NPC_GenerateCharacter("SentJons_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.city = "SentJons";	
	sld.location	= "SentJons_town";
	sld.location.group = "goto";
	sld.location.locator = "goto6";
	sld.forStay.locator = "goto6"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto15";
	sld.forSit.locator1 = "goto17"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto4";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//нищий в Бриджтауне
	sld = GetCharacter(NPC_GenerateCharacter("Bridgetown_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.city = "Bridgetown";
	sld.location	= "Bridgetown_town";
	sld.location.group = "goto";
	sld.location.locator = "goto12";
	sld.forStay.locator = "goto12"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto9";
	sld.forSit.locator1 = "goto19"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto4";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//нищий в Белизе
	sld = GetCharacter(NPC_GenerateCharacter("Beliz_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Beliz";
	sld.location	= "Beliz_town";
	sld.location.group = "goto";
	sld.location.locator = "goto9";
	sld.forStay.locator = "goto9"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto7";
	sld.forSit.locator1 = "goto19"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto3";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Каракасе
	sld = GetCharacter(NPC_GenerateCharacter("Caracas_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Caracas";
	sld.location	= "Caracas_town";
	sld.location.group = "goto";
	sld.location.locator = "goto20";
	sld.forStay.locator = "goto20"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto1";
	sld.forSit.locator1 = "goto21"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto23";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Картахене
	sld = GetCharacter(NPC_GenerateCharacter("Cartahena_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Cartahena";
	sld.location	= "Cartahena_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.forStay.locator = "goto5"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto18";
	sld.forSit.locator1 = "goto14"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto6";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Кумане
	sld = GetCharacter(NPC_GenerateCharacter("Cumana_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Cumana";
	sld.location	= "Cumana_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.forStay.locator = "goto5"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto13";
	sld.forSit.locator1 = "goto15"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto3";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Кюрасао
	sld = GetCharacter(NPC_GenerateCharacter("Villemstad_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, HOLLAND, -1, false));
	sld.city = "Villemstad";
	sld.location	= "Villemstad_town";
	sld.location.group = "goto";
	sld.location.locator = "goto24";
	sld.forStay.locator = "goto24"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto23";
	sld.forSit.locator1 = "goto7"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto8";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	//нищий в Бас-Тере, Гваделупа
	sld = GetCharacter(NPC_GenerateCharacter("BasTer_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, FRANCE, -1, false));
	sld.city = "BasTer";
	sld.location	= "BasTer_town";
	sld.location.group = "goto";
	sld.location.locator = "goto18";
	sld.forStay.locator = "goto18"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto24";
	sld.forSit.locator1 = "goto16"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto17";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//нищий в Гаване
	sld = GetCharacter(NPC_GenerateCharacter("Havana_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Havana";
	sld.location	= "Havana_town";
	sld.location.group = "goto";
	sld.location.locator = "goto12";
	sld.forStay.locator = "goto12"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto16";
	sld.forSit.locator1 = "goto22"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto23";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Порт Рояле
	sld = GetCharacter(NPC_GenerateCharacter("PortRoyal_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.city = "PortRoyal";
	sld.location	= "PortRoyal_town";
	sld.location.group = "goto";
	sld.location.locator = "goto15";
	sld.forStay.locator = "goto15"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto16";
	sld.forSit.locator1 = "goto28"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto7";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//нищий в Форт-де-Франсе, Мартиника
	sld = GetCharacter(NPC_GenerateCharacter("FortFrance_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, FRANCE, -1, false));
	sld.city = "FortFrance";
	sld.location	= "FortFrance_town";
	sld.location.group = "goto";
	sld.location.locator = "goto4";
	sld.forStay.locator = "goto4"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto14";
	sld.forSit.locator1 = "goto10"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto1";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//нищий на Невисе
	sld = GetCharacter(NPC_GenerateCharacter("Charles_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.city = "Charles";
	sld.location	= "Charles_town";
	sld.location.group = "goto";
	sld.location.locator = "goto5";
	sld.forStay.locator = "goto5"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto19";
	sld.forSit.locator1 = "goto13"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto3";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//нищий в Порто Белло
	sld = GetCharacter(NPC_GenerateCharacter("PortoBello_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "PortoBello";
	sld.location	= "PortoBello_town";
	sld.location.group = "goto";
	sld.location.locator = "goto15";
	sld.forStay.locator = "goto15"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto3";
	sld.forSit.locator1 = "goto25"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto12";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Порт-о-Принс, Эспаньола
	sld = GetCharacter(NPC_GenerateCharacter("PortPax_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, FRANCE, -1, false));
	sld.city = "PortPax";
	sld.location	= "PortPax_town";
	sld.location.group = "goto";
	sld.location.locator = "goto14";
	sld.forStay.locator = "goto14"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto25";
	sld.forSit.locator1 = "goto13"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto11";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//нищий в Сантьяго
	sld = GetCharacter(NPC_GenerateCharacter("Santiago_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, SPAIN, -1, false));
	sld.city = "Santiago";
	sld.location	= "Santiago_town";
	sld.location.group = "goto";
	sld.location.locator = "goto6";
	sld.forStay.locator = "goto6"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto3";
	sld.forSit.locator1 = "goto15"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto28";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
	//нищий в Мариго, Сан Мартин
	sld = GetCharacter(NPC_GenerateCharacter("Marigo_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, HOLLAND, -1, false));
	sld.city = "Marigo";
	sld.location	= "Marigo_town";
	sld.location.group = "goto";
	sld.location.locator = "goto16";
	sld.forStay.locator = "goto16"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto1";
	sld.forSit.locator1 = "goto4"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto9";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
	//нищий в Тортуге
	sld = GetCharacter(NPC_GenerateCharacter("Tortuga_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, FRANCE, -1, false));
	sld.city = "Tortuga";
	sld.location	= "Tortuga_town";
	sld.location.group = "goto";
	sld.location.locator = "goto23";
	sld.forStay.locator = "goto23"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto16";
	sld.forSit.locator1 = "goto18"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto1";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
	//нищий в Порт Спейне, Тринидад
	sld = GetCharacter(NPC_GenerateCharacter("PortSpein_Poorman", "panhandler_"+(rand(2)+1), "man", "man", 5, ENGLAND, -1, false));
	sld.city = "PortSpein";
	sld.location	= "PortSpein_town";
	sld.location.group = "goto";
	sld.location.locator = "goto6";
	sld.forStay.locator = "goto6"; //где генеримся в случае стояния
	sld.forSit.locator0 = "goto9";
	sld.forSit.locator1 = "goto14"; //три локатора, где генеримся в случае сидения
	sld.forSit.locator2 = "goto1";
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Common_poorman.c";
	LAi_SetPoorType(sld);
	LAi_SetHP(sld, 50.0, 50.0);
	sld.greeting = "cit_common";
	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
	//заказчик нищих
	sld = GetCharacter(NPC_GenerateCharacter("PoorKillSponsor", "smuggler_boss", "man", "man", 40, PIRATE, -1, false));
	sld.name = "Оливер";
	sld.lastname = "Траст";
	sld.talker = 8; //начать диалог
	LAi_SetLoginTime(sld, 9.0, 22.0);
	sld.Dialog.Filename = "Quest\ForAll_dialog.c";
	sld.dialog.currentnode = "Begin_PoorKill";
	LAi_SetCitizenType(sld);
	LAi_SetHP(sld, 100.0, 100.0);
	sld.greeting = "cit_common";
}

string GetStrSmallRegister(string sBase)
{
	string sResult, Simbol;
	sResult = "";
	int qty = strlen(sBase);
	for (int i=0; i<qty; i++)
	{
		Simbol = GetSymbol(sBase, i);
		switch (Simbol)
		{
			case "А": sResult += "а"; continue; break;
			case "Б": sResult += "б"; continue; break;
			case "В": sResult += "в"; continue; break;
			case "Г": sResult += "г"; continue; break;
			case "Д": sResult += "д"; continue; break;
			case "Е": sResult += "е"; continue; break;
			case "Ё": sResult += "ё"; continue; break;
			case "Ж": sResult += "ж"; continue; break;
			case "З": sResult += "з"; continue; break;
			case "И": sResult += "и"; continue; break;
			case "Й": sResult += "й"; continue; break;
			case "К": sResult += "к"; continue; break;
			case "Л": sResult += "л"; continue; break;
			case "М": sResult += "м"; continue; break;
			case "Н": sResult += "н"; continue; break;
			case "О": sResult += "о"; continue; break;
			case "П": sResult += "п"; continue; break;
			case "Р": sResult += "р"; continue; break;
			case "С": sResult += "с"; continue; break;
			case "Т": sResult += "т"; continue; break;
			case "У": sResult += "у"; continue; break;
			case "Ф": sResult += "ф"; continue; break;
			case "Х": sResult += "х"; continue; break;
			case "Ц": sResult += "ц"; continue; break;
			case "Ч": sResult += "Ч"; continue; break;
			case "Ш": sResult += "ш"; continue; break;
			case "Щ": sResult += "щ"; continue; break;
			case "Э": sResult += "э"; continue; break;
			case "Ю": sResult += "ю"; continue; break;
			case "Я": sResult += "я"; continue; break;
		}
		sResult += Simbol;
	}
	return sResult;
}

//===>>> постановка прерываний на жемчужный генератор
void PearlGen_SetSantaCatalina(string sQuest)
{
	pchar.quest.PearlGen_checkSantaCatalina.win_condition.l1 = "location";
	pchar.quest.PearlGen_checkSantaCatalina.win_condition.l1.location = "Pearl_Jungle_06";
	pchar.quest.PearlGen_checkSantaCatalina.win_condition = "PearlGen_checkSantaCatalina";
}
void PearlGen_SetTeno(string sQuest)
{
	pchar.quest.PearlGen_checkTeno.win_condition.l1 = "location";
	pchar.quest.PearlGen_checkTeno.win_condition.l1.location = "Pearl_Jungle_02";
	pchar.quest.PearlGen_checkTeno.win_condition = "PearlGen_checkTeno";
}
//домашние энкаунтеры. установка тамера "покинуть дом"
void HouseEnc_TimerGoUot(string qName)
{
	if (CheckAttribute(pchar, "questTemp.HouseEncTimer"))
	{
		if (pchar.location == pchar.questTemp.HouseEncTimer && !actLoadFlag && !LAi_grp_alarmactive)
		{
			LAi_group_Attack(characterFromId(pchar.questTemp.HouseEncTimer.Id), Pchar);
		}
		DeleteAttribute(pchar, "questTemp.HouseEncTimer");
	}
}

//удаления группы с задержкой
void Delay_DeleteGroup(string GroupName)
{	
	DoQuestCheckDelay("DeleteGroupOnExitLocation", 2.0);
	pchar.quest.DeleteGroupOnExitLocation.GroupName = GroupName;
}
//установка скелетов по пещерам
void SetSkeletonsToLocation(aref _location)
{
	int iRank, iStep;
	float sTime, eTime;
	ref sld;
	bMonstersGen = true; //флаг генерации монстров
	//--> генерим инкремент к рангу
	if (!CheckAttribute(_location, "Monsters_step"))
	{
		iStep = 0;
		_location.Monsters_step = sti(MOD_SKILL_ENEMY_RATE / 1.5 + 0.5);
		SaveCurrentNpcQuestDateParam(_location, "Monsters_step"); //запись даты 
	}
	else
	{
		if (GetNpcQuestPastDayParam(_location, "Monsters_step") > 3)
		{
			iStep = 0;
			_location.Monsters_step = sti(MOD_SKILL_ENEMY_RATE / 1.5 + 0.5);
			SaveCurrentNpcQuestDateParam(_location, "Monsters_step"); //запись даты 
		}
		else
		{
			iStep = _location.Monsters_step;
			_location.Monsters_step = iStep + sti(MOD_SKILL_ENEMY_RATE / 1.5 + 0.5);
		}
	}
	LAi_group_Delete("EnemyFight");
	//<-- генерим инкремент к рангу
	//--> генерим ранг 
	if (sti(pchar.rank) > 6) 
	{
		if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE;
		else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE/2);
	}
	else
	{	//казуалам зеленый свет на начало игры
		iRank = sti(pchar.rank);
	}
	iRank += iStep;
	//<-- генерим ранг 
	aref grp;
	makearef(grp, _location.locators.monsters);
	int num = GetAttributesNum(grp);
	for(int i = 0; i < num; i++)
	{
		sld = GetCharacter(NPC_GenerateCharacter("Skelet"+_location.index+"_"+i, "Skel"+(rand(3)+1), "skeleton", "man", iRank, PIRATE, 0, true));
		//если квест по зачистке от нечисти - скелетов делаем круче
		if (CheckAttribute(_location, "DestroyGhost"))
		{
			FantomMakeCoolFighter(sld, iRank, 90, 90, LinkRandPhrase("blade33", "blade26", "blade23"), RandPhraseSimple("pistol4", "pistol6"), 100);
			sld.SuperShooter  = true;
		}
		else
		{
			SetFantomParamFromRank(sld, iRank, true);
		}
		LAi_SetWarriorType(sld);
		LAi_warrior_SetStay(sld, true);
		//if (CheckAttribute(_location, "DestroyGhost"))
		//{
		//	LAi_group_MoveCharacter(sld, "EnemyFight");
		//}
		//else
		//{
			LAi_group_MoveCharacter(sld, LAI_GROUP_MONSTERS);
		//}
		ChangeCharacterAddressGroup(sld, _location.id, "monsters", GetAttributeName(GetAttributeN(grp, i)));
	}
	//проверяем, есть ли на локацию квест по очистке от нечисти
	if (CheckAttribute(_location, "DestroyGhost"))
	{	
		chrDisableReloadToLocation = true;
		characters[sti(_location.DestroyGhost)].DestroyGhost = "GoodResult";
		DeleteAttribute(_location, "DestroyGhost");
		LAi_LocationDisableMonGenTimer(_location.id, 3); //монстров не генерить 3 дня
		LAi_group_SetCheck(LAI_GROUP_MONSTERS, "OpenTheDoors");
		AddQuestRecordEx(_location.id + "Church_DestroyGhost", "Church_DestroyGhost", "2");
	}
}

//Перехват всех попыток ГГ залезть в боксы
void QuestCheckTakeBoxes(ref itemsRef) 
{	
    ref locLoad = &locations[FindLocation(pchar.location)];
    ref sld;
	int i, num;
	//--> флаг "в сундуке"
	bMainCharacterInBox = true;
	//<-- флаг "в сундуке"
	//-------------------- КЛАДЫ ------------>>>>>>>>>>>>>
	if (CheckAttribute(itemsRef, "Treasure"))
	{
		Log_Info("Похоже, что это и есть клад.");
		PlaySound("interface\notebook.wav");
		DeleteAttribute(itemsRef, "Treasure");
		//eddy. для безконфликтности квестов

		if (LAi_LocationIsMonstersGen(locLoad) && !bMonstersGen && LAi_grp_playeralarm == 0)
		{
			SetSkeletonsToLocation(locLoad)
		}
	}
	//<<<<<<<<<<---------- КЛАДЫ --------------------------
	//-->> Генератор грабежа жемчуга, у главы поселения 
	if (GetCharacterIndex("HeadMan_2") != -1 && locLoad.id == "PearlTown2_Townhall")
	{
		if (pchar.questTemp.Sharp.SeekSpy != "begin" && pchar.questTemp.Sharp.SeekSpy != "over")
		{	//если взят квест на поиски крысы Шарпа - закрываем его
			pchar.questTemp.Sharp.SeekSpy = "over";
			CloseQuestHeader("SharpPearl_SeekSpy");
    		ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, SPAIN, -10);
		}
		ChangeCharacterReputation(pchar, -20);
		SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
		LAi_group_Attack(characterFromId("HeadMan_2"), Pchar);
		chrDisableReloadToLocation = true;
		pchar.quest.PearlGen_HeadMan_2_death.win_condition.l1 = "NPC_Death";
		pchar.quest.PearlGen_HeadMan_2_death.win_condition.l1.character = "HeadMan_2";
		pchar.quest.PearlGen_HeadMan_2_death.win_condition = "OpenTheDoors";
		return;
	}
	if (GetCharacterIndex("HeadMan_1") != -1 && locLoad.id == "PearlTown1_Townhall")
	{
		if (pchar.questTemp.Sharp.SeekSpy != "begin" && pchar.questTemp.Sharp.SeekSpy != "over")
		{	//если взят квест на поиски крысы Шарпа - закрываем его
			pchar.questTemp.Sharp.SeekSpy = "over";
			CloseQuestHeader("SharpPearl_SeekSpy");
    		ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, SPAIN, -10);
		}		
		ChangeCharacterReputation(pchar, -20);
		SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
		LAi_group_Attack(characterFromId("HeadMan_1"), Pchar);
		chrDisableReloadToLocation = true;
		pchar.quest.PearlGen_HeadMan_1_death.win_condition.l1 = "NPC_Death";
		pchar.quest.PearlGen_HeadMan_1_death.win_condition.l1.character = "HeadMan_1";
		pchar.quest.PearlGen_HeadMan_1_death.win_condition = "OpenTheDoors";
		return;
	}
	//<<-- Генератор грабежа жемчуга, у главы поселения 
	//-->> Квест Изабеллы.
	if (GetCharacterIndex("Husband") != -1  && locLoad.id == "SanJuan_houseSp6" && characters[GetCharacterIndex("Isabella")].location != "SanJuan_houseSp6" && characters[GetCharacterIndex("Husband")].location == "SanJuan_houseSp6")
	{		
		ChangeCharacterReputation(pchar, -3);
		SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
		sld = characterFromId("Husband");
		LAi_SetWarriorType(sld);
		LAi_SetImmortal(sld, false);
		LAi_group_Attack(sld, Pchar);
		chrDisableReloadToLocation = true;
		LAi_LocationFightDisable(locLoad, false); 
		LocatorReloadEnterDisable(locLoad.id, "reload2", true);
		pchar.quest.Husband_death.win_condition.l1 = "NPC_Death";
		pchar.quest.Husband_death.win_condition.l1.character = "Husband";
		pchar.quest.Husband_death.win_condition = "OpenTheDoors";
	}
	//<<-- Квест Изабеллы.
}
//открытие дверей в локации города для коммонов. только на текущие сутки + еще одни, потом снимается само.
void SetOpenDoorCommonLoc(string City, string locationId)
{
	aref arRld, arDis, arRld2, arDis2;
	string LocId;
	makearef(arRld, locations[FindLocation(City + "_town")].reload);
    int Qty2, n, i;
	int Qty = GetAttributesNum(arRld);
	for (i=0; i<Qty; i++)
    {
    	arDis = GetAttributeN(arRld, i);
		LocId = arDis.go;
    	if (LocId == locationId)
    	{
			arDis.disable = false;
			arDis.canEnter = true;
			return;
    	}
		if (arDis.label != "Sea")
		{	
			makearef(arRld2, Locations[FindLocation(LocId)].reload);
			Qty2 = GetAttributesNum(arRld2);
			for (n=0; n<Qty2; n++)
			{
    			arDis2 = GetAttributeN(arRld2, n);
				LocId = arDis2.go;
				if (LocId == locationId)
    			{
					arDis.disable = false;
					arDis.canEnter = true;
					return;					
				}
			}
		}
    }
}

//=============== капитан Шарп ====================
string Sharp_choiceAction()
{
	string sBack;
	ref sld;
	//удалим Шарпа с карты, если он плывет еще где-то
	Map_ReleaseQuestEncounter("Sharp");
	group_DeleteGroup("Sharp_Group");
	if (rand(3) != 2 && sti(pchar.questTemp.Sharp.count) < 5)
	{	//еще динамим, отправляем ГГ в другой город
		string sCity = pchar.questTemp.Sharp.City; //предыдущая цель, запомним для СЖ
		pchar.questTemp.Sharp.City.rumour = true; //флаг дать слух в городе
		pchar.questTemp.Sharp.City = GetSharpCity(); //новый город 
		sBack = GetSharpRumour_toCityTarget();
		pchar.questTemp.Sharp.count = sti(pchar.questTemp.Sharp.count) + 1; //счетчик
		AddQuestRecord("SharpPearl", "2");
		AddQuestUserData("SharpPearl", "sOldTarget", XI_ConvertString("Colony" + sCity + "Dat"));
		AddQuestUserData("SharpPearl", "sTarget", XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc"));
		if (GetIslandByCityName(pchar.questTemp.Sharp.City) != pchar.questTemp.Sharp.City)
		{
			AddQuestUserData("SharpPearl", "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(pchar.questTemp.Sharp.City) + "Dat"));
		}
		//запускаем опять Шарпа на карту
		sld = characterFromId("Sharp");
		string sGroup = "Sharp_Group";
		Group_FindOrCreateGroup(sGroup);
		Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
		Group_LockTask(sGroup);
		Group_AddCharacter(sGroup, sld.id);
		Group_SetGroupCommander(sGroup, sld.id);
		sld.mapEnc.type = "trade";
		sld.mapEnc.worldMapShip = "quest_ship";
		sld.mapEnc.Name = "бриг 'Шарпоносец'";
		string sColony= SelectAnyColony(sCity); //колония, откуда плывет Шарп
		int daysQty = GetMaxDaysFromIsland2Island(GetArealByCityName(sColony), GetArealByCityName(pchar.questTemp.Sharp.City))+3; //дней доехать даем с запасом
		Map_CreateTrader(sColony, pchar.questTemp.Sharp.City, sld.id, daysQty);
		Log_TestInfo("Шарпоносец передресован из " + sColony + "  в " + pchar.questTemp.Sharp.City);
	}
	else
	{
		if (rand(5) < 4)
		{
			sBack = GetSharpRumour_inIsland();
			pchar.questTemp.Sharp.Island = GiveArealByLocation(&locations[FindLocation(pchar.location)]);
			pchar.questTemp.Sharp.count = 0; //счетчик нулим для след. раза
			pchar.questTemp.Sharp.price = 5000+rand(20)*1000; //цена за наводку
			pchar.questTemp.Sharp.price.evil = rand(1); //добрый или злой, для скидки
			Pchar.quest.Sharp_loginNearIsland.win_condition.l1 = "location";
			Pchar.quest.Sharp_loginNearIsland.win_condition.l1.location = pchar.questTemp.Sharp.Island;
			Pchar.quest.Sharp_loginNearIsland.function = "Sharp_loginNearIsland";
			AddQuestRecord("SharpPearl", "3");
			SaveCurrentQuestDateParam("questTemp.Sharp");
			Log_QuestInfo("Сработало на остров.");
		}
		else
		{
			sBack = GetSharpRumour_inCity();
			pchar.questTemp.Sharp.count = 0; //счетчик нулим для след. раза
			pchar.questTemp.Sharp.price = 5000+rand(20)*1000; //цена за наводку
			pchar.questTemp.Sharp.price.evil = rand(1); //добрый или злой, для скидки
			float locx, locy, locz;
			GetCharacterPos(pchar, &locx, &locy, &locz);
			sld = &characters[GetCharacterIndex("Sharp")];
			sld.dialog.currentnode = "CitySharp";
			LAi_SetCitizenType(sld);
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
			ChangeCharacterAddressGroup(sld, pchar.location, "patrol", LAi_FindFarLocator("patrol", locx, locy, locz));
			AddQuestRecord("SharpPearl", "11");
			Log_QuestInfo("Сработало на город.");
		}
	}
	return sBack;
}

//ищем новый город 
string GetSharpCity()
{
	int n, iRes;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].nation != "none" && GetRelation2BaseNation(sti(colonies[n].nation)) != RELATION_ENEMY && GiveArealByLocation(&locations[FindLocation(pchar.location)]) != colonies[n].island) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	iRes = storeArray[rand(howStore-1)];
	return colonies[iRes].id;
}
//фразы по умолчанию
string GetSharpRumour_default()
{
	string sRumour;
	switch (rand(2))
    {
        case 0: sRumour = "Капитан Шарп был здесь, я его видел. Но сейчас его в городе нет, а куда ушел - не знаю."; break;
        case 1: sRumour = "Капитан Шарп заходил в наш город. Где он сейчас я не знаю, у нас его точно нет."; break;
        case 2:	sRumour = "Капитан Барталомью Шарп ушел из города буквально на днях..."; break;
    }
	return sRumour;
}
//фразы в другой город
string GetSharpRumour_toCityTarget()
{
	string sRumour;
	switch (rand(2))
    {
        case 0: sRumour = "Капитан Бартоломью Шарп был у нас в городе и, как всегда, умудрился 'отметиться'... Но сейчас его здесь нет. По слухам он ушел в " + XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc") + "."; break;
        case 1: sRumour = "Знаете, капитан Шарп был в городе недавно. Веселый пират, этот Барталомью... Хорошо, однако, что он покинул наш город. Кажется он ушел в " + XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc") + "."; break;
        case 2:	sRumour = "Вы знаете, недавно нас почтил своим присутствием капитан Шарп. Но, к нашему большому разочарованию, нового карибского анекдота 'от Шарпа' не случилось. Жаль... А сейчас его нет у нас в городе. Говорят, он ушел в " + XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc") + ". Может им повезет..."; break;
    }
	return sRumour;
}
//фразы у осрова
string GetSharpRumour_inIsland()
{
	string sRumour;
	switch (rand(2))
    {
        case 0: sRumour = "Вы знаете, капитан Шарп только что вышел из города на своем бриге. Если бы вы прибыли с пол часа назад, то увидели бы его прямо в порту."; break;
        case 1: sRumour = "Капитан Шарп... Да уже, тот еще капитан. Знаете, он только что покинул наш город. Если вы поторопитесь, то сможете его застать в окрестных водах."; break;
        case 2:	sRumour = "Бартоломью Шарп буквально с час назад вышел из нашего порта в море."; break;
    }
	return sRumour;
}
//фразы в городе
string GetSharpRumour_inCity()
{
	string sRumour;
	switch (rand(2))
    {
        case 0: sRumour = "Пару минут назад капитан Шарп столкнулся со мной лицом к лицу. Ходит здесь по улицам, странно, что не в борделе..."; break;
        case 1: sRumour = "Только что на улице довелось поговорить с капитаном Шарпом. Занятный, знаете ли, тип..."; break;
        case 2:	sRumour = "Капитан Шарп сейчас гуляет по улицам. А мы все ждем, чего же он выкинет в очередной раз..."; break;
    }
	return sRumour;
}

//=============== щебека Синяя Птица.  ====================
string BlueBurd_setTradeShip()
{
	pchar.questTemp.BlueBird.Island = GiveArealByLocation(&locations[FindLocation(pchar.location)]);
	Pchar.quest.BlueBird_loginFleut.win_condition.l1 = "location";
	Pchar.quest.BlueBird_loginFleut.win_condition.l1.location = pchar.questTemp.BlueBird.Island;
	Pchar.quest.BlueBird_loginFleut.function = "BlueBird_loginFleut";
	pchar.questTemp.BlueBird.nation = colonies[FindColony(pchar.questTemp.BlueBird.City)].nation; //нация колонии, откуда кораблик
	aref aName;
	makearef(aName, pchar.questTemp.BlueBird);
	SetRandomNameToShip(aName);
	AddQuestRecord("Xebeca_BlueBird", "10");
	AddQuestUserData("Xebeca_BlueBird", "sCity", XI_ConvertString("Colony" + pchar.questTemp.BlueBird.City + "Dat"));
	AddQuestUserData("Xebeca_BlueBird", "sShipName", "'" + aName.Ship.Name + "'");
	AddQuestUserData("Xebeca_BlueBird", "sCity_2", XI_ConvertString("Colony" + pchar.questTemp.BlueBird.City + "Gen"));
	AddQuestUserData("Xebeca_BlueBird", "sTradeName", GetFullName(characterFromId(pchar.questTemp.BlueBird.City + "_trader")));
	AddQuestUserData("Xebeca_BlueBird", "sIsland", XI_ConvertString(pchar.questTemp.BlueBird.Island + "Gen"));
	SaveCurrentQuestDateParam("questTemp.BlueBird");
	return GetBlueBirdRumour_Ship(); //текст слуха
}
//фразы по слухам, наводки на корабли тоговцев
string GetBlueBirdRumour_Ship()
{
	string sRumour;
	switch (rand(2))
    {
        case 0: sRumour = "Вы знаете, местный торговец, " + GetFullName(characterFromId(pchar.questTemp.BlueBird.City + "_trader")) + ", опять отправляет собственный флейт '" + pchar.questTemp.BlueBird.Ship.Name + "' в Старый Свет за товаром. Недавно флейт вышел из порта."; break;
		case 1: sRumour = "Хм, а дела у нашего негоцианта доморощенного идут неплохо! " + GetFullName(characterFromId(pchar.questTemp.BlueBird.City + "_trader")) + " вновь отправил собственный флейт '" + pchar.questTemp.BlueBird.Ship.Name + "' за товаром, тот только что вышел из нашего порта."; break;
        case 2:	sRumour = "М-да, а " + GetFullName(characterFromId(pchar.questTemp.BlueBird.City + "_trader")) + " все богатеет и богатеет. Только вчера его парусник стоял под разгрузкой в порту, а сегодня порт уже попращался с ним. Флейт '" + pchar.questTemp.BlueBird.Ship.Name + "' ушел опять за товаром, так то!"; break;
    }
	return sRumour;
}
//======================= пиратская линейка ===================================
void PiratesLineInit()
{
	ref sld;
	pchar.questTemp.piratesLine = "begin";
	sld = GetCharacter(NPC_GenerateCharacter("QuestPirate1", "officer_9", "man", "man", 20, PIRATE, -1, true));
	sld.name = "Капитан";
	sld.lastname = "Гудли";
	sld.rank = 20;
	sld.city = "PuertoPrincipe";
	sld.location	= "PuertoPrincipe_town";
	sld.location.group = "goto";
	sld.location.locator = "goto7";
	sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
	sld.dialog.currentnode   = "PiratesMan1";
	sld.greeting = "pirat_quest";
	sld.talker = 8; //начать диалог
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
}

//ищем город определенной нации, проверять наличие ростовщика и тавернщика
string GetQuestNationsCity(int _nation)
{
	int n, iRes;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].nation != "none" && sti(colonies[n].nation) == _nation && GiveArealByLocation(&locations[FindLocation(pchar.location)]) != colonies[n].island) //не на свой остров
		{
			if (GetCharacterIndex(colonies[n].id + "_tavernkeeper") > 0 && GetCharacterIndex(colonies[n].id + "_usurer") > 0)
			{
				storeArray[howStore] = n;
				howStore++;
			}
		}
	}
	if (howStore == 0) return "none";
	iRes = storeArray[rand(howStore-1)];
	return colonies[iRes].id;
}

//ищем не вражескую колонию, куда можно доплыть
string SelectNotEnemyColony(ref NPChar)
{
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		nation = GetNationRelation(sti(npchar.nation), sti(colonies[n].nation));
		if (nation != RELATION_ENEMY && colonies[n].id != "Panama" && colonies[n].nation != "none" && GetIslandByCityName(npchar.city) != colonies[n].islandLable) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}

//ищем любую национальную колонию, куда можно доплыть
string SelectAnyColony(string _City)
{
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id != "Panama" && colonies[n].id != "FortOrange" && colonies[n].nation != "none" && sti(colonies[n].nation) != PIRATE && GetIslandByCityName(_City) != colonies[n].islandLable) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}

string SelectAnyColony2(string _City1, string _City2)
{
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		if (colonies[n].id != "Panama" && colonies[n].id != "FortOrange" && colonies[n].nation != "none" && sti(colonies[n].nation) != PIRATE && GetIslandByCityName(_City1) != colonies[n].islandLable && GetIslandByCityName(_City2) != colonies[n].islandLable) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}

ref CheckLSCCitizen()
{
	ref rCharacter;
	string sTemp, sSeeked;
	sSeeked = GetStrSmallRegister(dialogEditStrings[3]);
	if (sSeeked == "адмирал" || sSeeked == "адмиралa")
	{
		return characterFromId("LSCMayor");
	}
	if (sSeeked == "трактирщик" || sSeeked == "трактирщика" || sSeeked == "тавернщик"|| sSeeked == "тавернщика" || sSeeked == "хозяин таверны" || sSeeked == "хозяина таверны")
	{
		return characterFromId("LSCBarmen");
	}
	if (sSeeked == "официантка" || sSeeked == "официантку")
	{
		return characterFromId("LSCwaitress");
	}
	for(int n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(rCharacter, Characters[n]);
		if (CheckAttribute(rCharacter, "city") && rCharacter.city == "LostShipsCity")
		{
			sTemp = GetStrSmallRegister(rCharacter.name + " " + rCharacter.lastname);
			if (findsubstr(sSeeked, sTemp , 0) != -1)
			{
				return rCharacter;			
			}
		}
	}
	return &NullCharacter;
}

string WhereLSCCitizen(ref rCharacter)
{
	string sShip = "none";
	string sTemp;
	if (rCharacter.location	== "LostShipsCity_town")
	{
		sTemp = rCharacter.location.locator;
		sTemp = strcut(sTemp, 4, 5);
		switch (sTemp)
		{
			case "01": sShip = "на бэттлшипе 'Тартарус', где тюрьма"; break;
			case "02": sShip = "на фрегате 'Каролина'"; break;
			case "03": sShip = "на флейте 'Церес Смити'"; break;
			case "04": sShip = "на галеоне 'Эсмеральда'"; break;
			case "05": sShip = "на каравелле 'Флерон', где таверна"; break;
			case "06": sShip = "на пинасе 'Фурия', в носовой части"; break;
			case "07": sShip = "на пинасе 'Фурия', в кормовой части"; break;
			case "08": sShip = "на галеоне 'Глория', где церковь"; break;
			case "09": sShip = "на флете 'Плуто'"; break;
			case "10": sShip = "на корвете 'Протектор'"; break;
			case "11": sShip = "на варшипе 'Сан Августин', где резиденция адмирала"; break;
			case "12": sShip = "на барке 'Сан Габриель', в носовой части"; break;
			case "13": sShip = "на барке 'Сан Габриель', в кормовой части"; break;
			case "14": sShip = "на бриге 'Санта Флорентина'"; break;
			case "15": sShip = "на галеоне 'Эва'"; break;
			case "16": sShip = "на галеоне 'Веласко'"; break;
		}
	}
	else
	{
		switch (rCharacter.location)
		{
			case "AvaShipInside3": sShip = "в галеоне 'Эва'"; break;
			case "CarolineBank": sShip = "в фрегате 'Каролина'"; break;
			case "CeresSmithy": sShip = "во флейте 'Церес Смити'"; break;
			case "EsmeraldaStoreBig": sShip = "в галеоне 'Эсмеральда'"; break;
			case "FenixPlatform": sShip = "в платформе 'Феникс'"; break;
			case "FernandaDiffIndoor": sShip = "во флейте 'Фернандо Диффиндур'"; break;
			case "FleuronTavern": sShip = "в каравелле 'Флерон', в таверне"; break;
			case "FurieShipInside2": sShip = "в пинасе 'Фурия'"; break;
			case "GloriaChurch": sShip = "в галеоне 'Глория', в церкви"; break;
			case "PlutoStoreSmall": sShip = "во флете 'Плуто'"; break;
			case "ProtectorFisher": sShip = "в корвете 'Протектор'"; break;
			case "SanAugustineResidence": sShip = "в варшипе 'Сан Августин', в резиденции адмирала"; break;
			case "SanGabrielMechanic": sShip = "в барке 'Сан Габриель'"; break;
			case "SantaFlorentinaShipInside4": sShip = "в бриге 'Санта Флорентина'"; break;
			case "TartarusPrison": sShip = "в бэттлшипе 'Тартарус', в тюрьме"; break;
			case "VelascoShipInside1": sShip = "в галеоне 'Веласко'"; break;
		}
	}
	return sShip;
}

//отдать забытый судовой журнал. помещаем в город кэпа, который приплыл к назначенной колонии
void SetCapitainFromSeaToCity(string sChar)
{
	ref sld = &characters[GetCharacterIndex(sChar)];
	sld.quest = "InCity"; //флаг в городе
	sld.City = sld.quest.targetCity; //аттрибут текущего города пропишем
	sld.location	= sld.City + "_PortOffice";
	sld.location.group = "goto";
	sld.location.locator = "goto"+(rand(2)+1);
	LAi_SetCitizenType(sld);

	/*int iColony = FindColony(sld.City);
	string sGroup = "PorpmansShip_" + sld.index
	group_DeleteGroup(sGroup);
	Group_AddCharacter(sGroup, sld.id);			
	Group_SetGroupCommander(sGroup, sld.id);
	Group_SetAddress(sGroup, colonies[iColony].island, "quest_ships", "Quest_ship_"+(rand(2)+1));
	Group_SetTaskNone(sGroup);*/

	//таймер через сколько опять выйти в море
	int Qty = rand(4)+3;
	string name = "Timer_" + sld.id;
	PChar.quest.(name).win_condition.l1            = "Timer";
    PChar.quest.(name).win_condition.l1.date.day   = GetAddingDataDay(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.month = GetAddingDataMonth(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.year  = GetAddingDataYear(0, 0, Qty);
    PChar.quest.(name).function					= "SetCapitainFromCityToSea";
	pchar.quest.(name).CapId = sld.id; //в прерывании запомним Id кэпа
	//даем слух, что кэп в городе
	AddSimpleRumourCity(LinkRandPhrase("Капитан " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' сейчас в городе. Кажется, его зовут " + GetFullName(sld) + ". Он сейчас должнен быть у начальника порта.", 
		"Вы знаете, капитана " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' сейчас можно застать у начальника порта.", 
		"Если вы хотите пообщаться с капитаном " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "', то идите прямоком к начальнику порта. " + GetFullName(sld) + " сейчас там..."), sld.City, Qty, 1, "none");
}
//отдать забытый судовой журнал. записть в квестбук из слухов
void PortmansBook_writeQuestBook(ref rid)
{
	ref sld = characterFromId(rid.addString);
	string sTitle = characters[GetCharacterIndex(sld.quest.firstCity + "_PortMan")].id + "PortmansBook_Delivery";
	AddQuestRecordEx(sTitle, "PortmansBook_Delivery", "4");
	AddQuestUserData(sTitle, "sCapName", GetFullName(sld));
	AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + rid.city + "Gen"));
	AddQuestUserData(sTitle, "sTargetCity", XI_ConvertString("Colony" + sld.quest.targetCity + "Acc"));
	if (GetIslandByCityName(sld.quest.targetCity) != sld.quest.targetCity)
	{
		AddQuestUserData(sTitle, "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(sld.quest.targetCity) + "Dat"));
	}
}

//разыскать кэпа-вора. помещаем в город кэпа, который приплыл к назначенной колонии
void SetRobberFromMapToSea(string sChar)
{
	ref sld = &characters[GetCharacterIndex(sChar)];
	sld.quest = "InPort"; //флаг кэпа стоит в порту
	sld.City = sld.quest.targetCity; //аттрибут текущего города пропишем
	int iColony = FindColony(sld.City);
	sld.nation = colonies[iColony].nation; //нацию дадим, что и у города

	string sGroup = "SeekCapShip_" + sld.index
	group_DeleteGroup(sGroup);
	Group_AddCharacter(sGroup, sld.id);			
	Group_SetGroupCommander(sGroup, sld.id);
	Group_SetAddress(sGroup, colonies[iColony].island, "quest_ships", "Quest_ship_"+(rand(2)+1));
	Group_SetTaskNone(sGroup);

	//таймер через сколько опять выйти на карту
	int Qty = rand(5)+4;
	string name = "SecondTimer_" + sld.id;
	PChar.quest.(name).win_condition.l1            = "Timer";
    PChar.quest.(name).win_condition.l1.date.day   = GetAddingDataDay(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.month = GetAddingDataMonth(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.year  = GetAddingDataYear(0, 0, Qty);
    PChar.quest.(name).function					= "SetRobberFromSeaToMap";
	pchar.quest.(name).CapId = sld.id; //в прерывании запомним Id кэпа
	//даем слух, что кэп на рейде
	AddSimpleRumourCity(LinkRandPhrase("Капитан " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' стоит в порту на рейде. Кажется, его зовут " + GetFullName(sld) + ". Странный тип, скажу я вам, на берег не сходит вообще.", 
		"Вы знаете, капитана " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' сейчас можно встретить, только если выйти на шлюпке в порт. Корабль стоит на рейде, но капитан в город не выходит, предпочитает оставаться на корабле. И и недоело ему?..", 
		"Если вы хотите пообщаться с капитаном " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "', то выходите в порт. " + GetFullName(sld) + " вроде бы его зовут... В общем, он бросил якорь в порту, но на берег не сходит. Настоящий морской волк, даже с палубной качкой расставаться не хочет."), sld.City, Qty, 1, "none");
}
//разыскать кэпа-вора. записть в квестбук из слухов
void PortmansSeekShip_writeQuestBook(ref rid)
{
	ref sld = characterFromId(rid.addString);
	ref npchar = &characters[GetCharacterIndex(sld.quest.firstCity + "_PortMan")];
	string sTitle = npchar.id + "Portmans_SeekShip";
	AddQuestRecordEx(sTitle, "Portmans_SeekShip", "3");
	AddQuestUserData(sTitle, "sShipTypeName", GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName)));
	AddQuestUserData(sTitle, "sShipName", sld.Ship.name);
	AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + rid.city + "Gen"));
	int iTemp = RealShips[sti(sld.Ship.Type)].basetype;
	AddQuestUserData(sTitle, "sSexWord", GetShipSexWord(ShipsTypes[iTemp].name, "ушел", "ушла"));
	AddQuestUserData(sTitle, "sTargetCity", XI_ConvertString("Colony" + sld.quest.targetCity + "Acc"));
	if (GetIslandByCityName(sld.quest.targetCity) != sld.quest.targetCity)
	{
		AddQuestUserData(sTitle, "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(sld.quest.targetCity) + "Dat"));
	}
}

//розыск кэпов горожанами
void CitizCapFromMapToCity(string sChar)
{
	ref sld = &characters[GetCharacterIndex(sChar)];
	sld.quest = "InCity"; //флаг кэпа ходит по городу
	sld.City = sld.quest.targetCity; //аттрибут текущего города пропишем
	int iColony = FindColony(sld.City);
	sld.nation = colonies[iColony].nation; //нацию дадим, что и у города
	if (rand(1))
	{	//ходит в городе
		sld.location	= sld.City + "_town";
		sld.location.group = "goto";
		sld.location.locator = "goto"+(rand(5)+1);
		LAi_SetCitizenType(sld);
		string slai_group = GetNationNameByType(sti(colonies[iColony].nation))  + "_citizens";
		LAi_group_MoveCharacter(sld, slai_group);
		//даем слух, что кэп на рейде
		AddSimpleRumourCity(LinkRandPhrase("Капитан " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' в городе. Кажется, его зовут " + GetFullName(sld) + ".", 
			"Вы знаете, капитана " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' сейчас можно застать в городе.", 
			"Если вы хотите пообщаться с капитаном " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "', то поищите его в на улицах. " + GetFullName(sld) + " вроде бы его зовут..."), sld.City, Qty, 1, "none");
	}
	else
	{	//стоит на рейде в порту
		string sGroup = "SeekCapShip_" + sld.index
		group_DeleteGroup(sGroup);
		Group_AddCharacter(sGroup, sld.id);			
		Group_SetGroupCommander(sGroup, sld.id);
		Group_SetAddress(sGroup, colonies[iColony].island, "quest_ships", "Quest_ship_"+(rand(2)+1));
		Group_SetTaskNone(sGroup);
		//даем слух, что кэп на рейде
		AddSimpleRumourCity(LinkRandPhrase("Капитан " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' находится в порту на рейде. Кажется, его зовут " + GetFullName(sld) + ".", 
			"Вы знаете, капитана " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "' сейчас можно застать в нашем порту. Его корабль стоит на рейде.", 
			"Если вы хотите пообщаться с капитаном " + GetStrSmallRegister(XI_ConvertString(RealShips[sti(sld.Ship.Type)].BaseName + "Acc")) + " '" + sld.Ship.name + "', то поищите его корабль в нашем порту. " + GetFullName(sld) + " вроде бы его зовут..."), sld.City, Qty, 1, "none");
	}
	//таймер через сколько опять выйти на карту
	int Qty = rand(7)+5; //через сколько дней выйдем на карту
	string name = "SecondTimer_" + sld.id;
	PChar.quest.(name).win_condition.l1            = "Timer";
    PChar.quest.(name).win_condition.l1.date.day   = GetAddingDataDay(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.month = GetAddingDataMonth(0, 0, Qty);
    PChar.quest.(name).win_condition.l1.date.year  = GetAddingDataYear(0, 0, Qty);
    PChar.quest.(name).function					= "CitizCapFromSeaToMap";
	pchar.quest.(name).CapId = sld.id; //в прерывании запомним Id кэпа
}

//разыск кэпа от горожан. записть в квестбук из слухов
void CitizSeekCap_writeQuestBook(ref rid)
{
	ref sld = characterFromId(rid.addString);
	ref npchar = &characters[GetCharacterIndex("QuestCitiz_" + sld.quest.cribCity)];
	string sTitle = sld.quest.cribCity + "SCQ_" + npchar.quest.SeekCap;
	AddQuestRecordEx(sTitle, "SCQ_" + npchar.quest.SeekCap, "3");
	AddQuestUserData(sTitle, "sCapName", GetFullName(sld));
	AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + rid.city + "Gen"));
	AddQuestUserData(sTitle, "sTargetCity", XI_ConvertString("Colony" + sld.quest.targetCity + "Acc"));
	if (GetIslandByCityName(sld.quest.targetCity) != sld.quest.targetCity)
	{
		AddQuestUserData(sTitle, "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(sld.quest.targetCity) + "Dat"));
	}
}

//линейка Блада в Барбадосе
//поставить Синко Льягас в город
void SetShipInBridgetown()
{
	int n = FindLocation("Bridgetown_town");
	locations[n].models.always.ship = "Bridgetown_ship";
	locations[n].models.always.shipreflect = "Bridgetown_shipreflect";
	Locations[n].models.always.shipreflect.sea_reflection = 1;
	locations[n].models.always.locators = "Bridgetown_locatorsShip";
	locations[n].models.day.fonarShip = "Bridgetown_fdShip";
	locations[n].models.night.fonarShip = "Bridgetown_fnShip";
	locations[n].models.day.charactersPatch = "Bridgetown_patchship_day";
	locations[n].models.night.charactersPatch = "Bridgetown_patchship_night";

	locations[n].reload.ship1.name = "reloadShip";
	locations[n].reload.ship1.go = "Cabin";
	locations[n].reload.ship1.emerge = "reload1";
	locations[n].reload.ship1.autoreload = "0";
	locations[n].reload.ship1.label = "cabine";

	n = FindLocation("Cabin");
	DeleteAttribute(&locations[n], "boarding");
	DeleteAttribute(&locations[n], "camshuttle");
	DeleteAttribute(&locations[n], "CabinType");

	locations[n].reload.l1.name = "reload1";
	locations[n].reload.l1.go = "Bridgetown_town";
	locations[n].reload.l1.emerge = "reloadShip";
	locations[n].reload.l1.autoreload = "0";
	locations[n].reload.l1.label = "Ship";
}
//убрать Синко Льягас из города
void RemoveShipFromBridgetown()
{
	int n = FindLocation("Bridgetown_town");	
	DeleteAttribute(&locations[n], "models.always.ship");
	DeleteAttribute(&locations[n], "models.always.shipreflect");
	DeleteAttribute(&locations[n], "models.always.shipreflect.sea_reflection");
	locations[n].models.always.locators = "Bridgetown_locators";
	DeleteAttribute(&locations[n], "models.day.fonarShip");
	DeleteAttribute(&locations[n], "models.night.fonarShip");
	locations[n].models.day.charactersPatch = "Bridgetown_patch_day";
	locations[n].models.night.charactersPatch = "Bridgetown_patch_night";
	DeleteAttribute(&locations[n], "reload.ship1");

	n = FindLocation("Cabin");
	DeleteAttribute(&locations[n], "reload");
	Locations[n].boarding = "true";
	Locations[n].boarding.nextdeck = "";
	Locations[n].camshuttle = 1;
	Locations[n].boarding.locatorNum = 1;
	Locations[n].CabinType = true;
	locations[n].environment.weather.rain = false;
	Locations[n].boarding.Loc.Hero = "loc0";
    Locations[n].boarding.Loc.Capt = "aloc2";
}

//линейка ГПК
//пробежаться по сундукам
void LSC_checkBoxes()
{
	ref loc;
	aref arBox;
	string sName;
	for(int i=0; i<MAX_LOCATIONS; i++)
	{				
		loc = &locations[i];
		if (CheckAttribute(loc, "fastreload") && loc.fastreload == "LostShipsCity" && loc.id != "FernandaDiffIndoor")
		{	
			for(int n=1; n<=MAX_HANDLED_BOXES; n++)
			{
				sName = "private" + n;
				if (CheckAttribute(loc, sName))
				{					
					makearef(arBox, loc.(sName));
					if (CheckAttribute(arBox, "equip") && rand(1))
					{
						DeleteAttribute(arBox, "items");
						arBox.items = "";
					}
				}
				else break;
			}
		}
	}
}