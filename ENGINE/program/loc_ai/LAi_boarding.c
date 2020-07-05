//  логика абордажа, переработка boal 28.04.06
#define BRDLT_SHIP	0
#define BRDLT_FORT	1

#define BRDLT_MAXCREW 44

#define LOCINITBRD_DEFAULTLOCATION "BOARDING_U_H_CS_T12"
#define LOCINITBRD_DEFAULTLOCATION_FORT "Any_fort_1"

#define MAX_GROUP_SIZE   13

bool   IsFort = false; 
int    boarding_location = -1;
int    boarding_location_type = -1;
object boarding_fader;


int   boarding_player_crew = 0;
int   boarding_officers    = 0;
float boarding_player_base_crew    = 0.0; //boal
float boarding_player_crew_per_chr = 1.0;
int   boarding_player_crew_start   = 0; //sd

int   boarding_enemy_crew = 0;
float boarding_enemy_base_crew    = 0.0;
float boarding_enemy_crew_per_chr = 1.0;
int   boarding_enemy_crew_start   = 0; //sd

ref    boarding_enemy;
object boarding_adr[4];
float  boarding_exp = 0;
float  boarding_player_hp = 40;
float  boarding_enemy_hp = 40;
int    boarding_echr_index = -1;
int    boarding_erank = 10;
bool   LAi_boarding_process = false;
bool   Surrendered = false; // сдача в плен

//Процес абордажа
bool LAi_IsBoardingProcess()
{
	return LAi_boarding_process;
}

//Получить картинку для перехода в абордаж
string LAi_GetBoardingImage(ref echr, bool isMCAttack)
{
	ref mchr = GetMainCharacter();
	string deckID = "";
	isMCAttack   = true;// boal 110804 fix всегда герой
	if(isMCAttack)
	{
		deckID = GetShipLocationID(echr);
	}else{
		deckID = GetShipLocationID(mchr);
	}
	int locID = -1;
	if(deckID != "")
	{
		locID = FindLocation(deckID);
		if(locID < 0)
		{
			Trace("Boarding: ship location not found <" + deckID + ">, set default");
			deckID = "";
		}
	}
	if(locID >= 0)
	{
		if(CheckAttribute(&Locations[locID], "image"))
		{
			if(Locations[locID].image != "")
			{
				return Locations[locID].image;
			}
		}
	}
	return "loading\battle.tga";
}

//Начать абордаж с главным персонажем
void LAi_StartBoarding(int locType, ref echr, bool isMCAttack)
{
    SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
	//ResetSoundScheme();
	ResetSound(); // new
	PauseAllSounds();
	
	bQuestCheckProcessFreeze = true;//fix
	
	// NK -->
	if(locType == BRDLT_FORT)
    {
        isMCAttack = true;
        IsFort = true;
    }
	else
    {
        IsFort = false;
    }
    // NK <--
	//Установим обработчик на убийство группы
	SetEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill", 1);
	//Настроим интерфейс
	DeleteBattleInterface();
	InitBattleLandInterface();
	//StartBattleLandInterface();
	//Сохраним индекс врага
	boarding_echr_index = sti(echr.index);
	//Параметры сражающихся сторон
	ref mchr = GetMainCharacter();
	int mclass = GetCharacterShipClass(mchr);
	int mcrew = GetCrewQuantity(mchr);
	
	// Saving enemy captain rank for future use in CalculateAppropriateSkills (Gray 12.02.2005)
	mchr.EnemyRank = echr.rank
	
	DeleteAttribute(pchar, "abordage_active");
	// boal учет оружия 21.01.2004 -->
	Log_TestInfo("Наших до оружия " + mcrew);
	mcrew = GetWeaponCrew(mchr, mcrew);
	
	// boal 21.01.2004 <--
	int eclass = GetCharacterShipClass(echr);
	int ecrew = GetCrewQuantity(echr);
	int ecrewBak;

    Log_TestInfo("Наших с оружием " + mcrew + " Врагов " + ecrew);
    
    boarding_location_type = locType;
	// Сдача в плен -->
	bool ok = (TestRansackCaptain) && (boarding_location_type != BRDLT_FORT);
    if (!CheckAttribute(echr, "DontRansackCaptain")) //квестовые не сдаются
    {
    	if (CheckForSurrender(mchr, echr, 1) || ok) // 1 - это учет первый раз, до битвы на палубе
    	{
    		echr.ship.crew.morale = 5;// после захвата у них мораль такая

    		if (mclass < eclass)
			{
				AddCrewMorale(mchr, ( 3 + 2*rand(eclass-mclass)));
			} 
			else
			{
				AddCrewMorale(mchr, 3);
			}
			SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
            //Следующей локации нет
			DelEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill");
			boarding_location = -1;
			
			// Saved value is not needed anymore... (Gray 14.02.2005)
			DeleteAttribute(GetMainCharacter(), "EnemyRank");
		    bQuestCheckProcessFreeze = false;//fix
		    
			//Настроим интерфейс
			Log_SetActiveAction("Nothing");
			EndBattleLandInterface();
            //Выгружаемся в интерфейс
            
			ChangeCrewExp(pchar, "Soldiers", 1);
			LaunchRansackMain(pchar, echr, "crew"); 
			// на деле параметры LaunchRansackMain не важны совсем - все определеятеся от реалий - жив кэп и сколько у него матросов - их и обрабатываем
			// но они используются в сообщениях  crew - это сдался сразу
    		LAi_boarding_process = false;  
			Event(SHIP_CAPTURED, "l", sti(echr.index));
    		return;
    	}
	}
	// Сдача в плен <--
	
	// MusketsShoot --> 
	float tmpDefence;
    if (CheckOfficersPerk(mchr, "MusketsShoot") && IsFort == false)
    {
        tmpDefence   = MakeFloat(GetSummonSkillFromName(echr, SKILL_DEFENCE)) / SKILL_MAX;
        ecrewBak = makeint(ecrew * 0.25);
		if (ecrewBak > mcrew) ecrewBak = mcrew;
		ecrewBak = makeint(ecrewBak * (2.1 - tmpDefence) / 2.0);
        ecrew = ecrew - ecrewBak;
        Log_SetStringToLog("Мушкетным залпом убито " + ecrewBak + " человек команды противника.");
        PlaySound("INTERFACE\_musketshot.wav");
    }
    if (CheckOfficersPerk(echr, "MusketsShoot") && IsFort == false)
    {
        tmpDefence   = MakeFloat(GetSummonSkillFromName(mchr, SKILL_DEFENCE)) / SKILL_MAX;
        ecrewBak = makeint(mcrew * 0.25);
		if (ecrewBak > ecrew) ecrewBak = ecrew;
		ecrewBak = makeint(ecrewBak * (2.1 - tmpDefence) / 2.0);
        mcrew = mcrew - ecrewBak;
        Log_SetStringToLog("Мушкетным залпом убито " + ecrewBak + " человек нашей команды.");
        Statistic_AddValue(mchr, "DeadCrewBoard", ecrewBak);
        PlaySound("INTERFACE\_musketshot.wav");
    }
    // MusketsShoot <--
    
	if(ecrew < 0) ecrew = 0;
	if(mcrew < 0) mcrew = 0;
	boarding_erank = sti(echr.rank);

	// база для расчета
	boarding_enemy_base_crew = ecrew;
	boarding_player_base_crew = mcrew;
	
	//Количество хитпойнтов для бонуса - учет опыта солдат
	GetBoardingHP(mchr, echr, &boarding_player_hp, &boarding_enemy_hp);
	
	//Определяем цепь локаций для абордажа
	boarding_location = -1;

	string deckID = "";
	isMCAttack   = true;// boal 110804 fix всегда герой
	if(isMCAttack)
	{
		deckID = GetShipLocationID(echr);
	}else{
		deckID = GetShipLocationID(mchr);
	}
	int locID = -1;
	if(deckID != "")
	{
		locID = FindLocation(deckID);
		if(locID < 0)
		{
			Trace("Boarding: ship location not found <" + deckID + ">, set default");
			deckID = "";
		}
	}
	if(deckID == "")
	{
		if(locType == BRDLT_FORT)
		{
			deckID = LOCINITBRD_DEFAULTLOCATION_FORT;
		}else{
			deckID = LOCINITBRD_DEFAULTLOCATION;
		}
		locID = FindLocation(deckID);
	}
	if (locID < 0)
	{   // исключительная ситуация
		Trace("Boarding: ship location not found <" + deckID + ">, no start boarding");
		// Saved value is not needed anymore... (Gray 14.02.2005)
		DeleteAttribute(GetMainCharacter(), "EnemyRank");
	    bQuestCheckProcessFreeze = false;//fix
		    
		if(boarding_location_type == BRDLT_SHIP)
		{
            ChangeCrewExp(pchar, "Soldiers", 1);
            SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
			LaunchRansackMain(GetMainCharacter(), echr, "captain");	  // на деле параметры LaunchRansackMain не важны совсем - все определеятеся от реалий
            LAi_boarding_process = false;
			Event(SHIP_CAPTURED, "l", sti(echr.index)); // to_do can be harmfull
		}
		else
		{
			if (boarding_location_type == BRDLT_FORT)
			{
                ChangeCrewExp(pchar, "Soldiers", 1);
                SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
				Event(FORT_CAPTURED, "l", sti(echr.index));
				echr.Ship.Crew.Quantity = 10 + rand(350); // рабы (остатки выживших)
                LaunchFortCapture(echr);
                LAi_boarding_process = false;
			}else{
				Trace("Boarding: unknow boarding location type");
			}
		}
		return;
	}

	pchar.abordage = 0;
	//Определяем размеры команд
	boarding_enemy = echr;
	//Максимальное количество человек на корабле
	int maxcrew = MAX_GROUP_SIZE;
	
    if(CheckAttribute(&Locations[locID], "boarding.locatorNum"))
	{
		maxcrew = sti(Locations[locID].boarding.locatorNum);
	}
    //  модифицируем от класса корабля тут
    int iMaxcrew  = func_min(mclass, eclass); // класс 1 - сама круть 7 - хрень
    switch (iMaxcrew)
    {
        case 7:
			iMaxcrew = 4;
		break;
		case 6:
			iMaxcrew = 5;
		break;
		case 5:
			iMaxcrew = 6;
		break;
		case 4:
			iMaxcrew = 7;
		break;
		case 3:
			iMaxcrew = 8;
		break;
		case 2:
			iMaxcrew = 11;
		break;
		case 1:
			iMaxcrew = 13;
		break;
    }
    if (iMaxcrew < maxcrew) maxcrew = iMaxcrew;
    
    if (boarding_location_type == BRDLT_SHIP && eclass != 7)  // на тартане каюты нет
	{
    	boarding_enemy.ShipCabinLocationId = GetShipCabinID(echr);
	}

	// boal check -->
	if(maxcrew > BRDLT_MAXCREW)
	{
	    maxcrew = BRDLT_MAXCREW;
	}
	// boal check <--

	//Отношение сторон

	// fort -->
	// boal 21.01.2004 -->
	int cn, j, compCrew;
    ref officer;
    if(IsFort)
    {
        mcrew = mcrew + GetTroopersCrewQuantity(GetMainCharacter()); // если нет перка, то нолик
        boarding_player_base_crew = mcrew;// учет всех кораблей
        //Максимальное количество человек
        Log_TestInfo("Итого: До расчета mcrew = "+mcrew+ " ecrew = "+ ecrew + " boarding_enemy_hp = "+ boarding_enemy_hp + " boarding_player_hp = "+boarding_player_hp);
    }
    // boal 21.01.2004 <--
    // fort <--
	
	float rel;
	if(mcrew > ecrew)
	{
        // нам бонуc boal
        boarding_player_hp = boarding_player_hp + GetBoarding_player_hp_Bonus(mcrew, ecrew);
		if (boarding_player_hp > 1000) boarding_player_hp = 1000;
		
        if(mcrew > maxcrew)
		{
			rel = makefloat(mcrew) / makefloat(maxcrew);
			mcrew = maxcrew;
			ecrew = MakeInt(ecrew / rel + 0.5);
		}
	}
	else
	{
		// boal 30.01.2004 -->
		boarding_enemy_hp = boarding_enemy_hp + GetBoarding_enemy_hp_Bonus(mcrew, ecrew);
		if (boarding_enemy_hp > 1500) boarding_enemy_hp = 1500;
		// boal 30.01.2004 <--
        if(ecrew > maxcrew)
		{       
			rel = makefloat(ecrew) / makefloat(maxcrew);
			ecrew = maxcrew;
			mcrew = MakeInt(mcrew/rel + 0.5);
		}
	}
	if(mcrew < 1) mcrew = 1;
	if(ecrew < 1) ecrew = 1;
	
    Log_TestInfo("После приведения mcrew = "+mcrew+ " ecrew = "+ ecrew + " boarding_enemy_hp = "+ boarding_enemy_hp + " boarding_player_hp = "+boarding_player_hp);

	boarding_enemy_crew        = ecrew;
	boarding_enemy_crew_start  = ecrew;
	boarding_player_crew       = mcrew;
	boarding_player_crew_start = mcrew;
	//Количество офицеров
	boarding_officers = 0;
	int passq;
	for(int i = 1; i < 4; i++)
	{
		passq = GetOfficersIndex(pchar, i);
		if (passq >= 0)
		{
            // boal 05.09.03 offecer need to go to abordage -->
		    // to_do if(makeint(Characters[GetOfficersIndex(GetMainCharacter(), i)].AbordageMode) == 0) continue;	// не берем
			// boal 05.09.03 offecer need to go to abordage <--
			boarding_officers = boarding_officers + 1;   
			LAi_SetOfficerType(&characters[passq]);
		}
	}
  
    //boarding_player_crew_per_chr = (curplayercrew + boarding_officers)/(mcrew + boarding_officers);
    boarding_player_crew_per_chr = makefloat(boarding_player_base_crew / makefloat(mcrew)); //приведение типа
	// START MOD Code by Stone-D : 30/07/2003
	boarding_enemy_crew_per_chr = makefloat(boarding_enemy_base_crew / makefloat(ecrew)); // Stone-D : For calculating final crew numbers
	// END MOD Code by Stone-D : 30/07/2003

	//Выставим игроку и офицерам максимальную жизнь и запомним адреса
	LAi_SetCurHPMax(mchr);
	boarding_adr[0].location = mchr.location;
	boarding_adr[0].group    = mchr.location.group;
	boarding_adr[0].locator  = mchr.location.locator;
	for(i = 1; i < 4; i++)
	{
		int idx = GetOfficersIndex(GetMainCharacter(), i);
		if(idx < 0)
		{
			boarding_adr[i].location = "";
			boarding_adr[i].group    = "";
			boarding_adr[i].locator  = "";
			continue;
		}
		LAi_SetCurHPMax(&Characters[idx]);
		boarding_adr[i].location = Characters[idx].location;
		boarding_adr[i].group    = Characters[idx].location.group;
		boarding_adr[i].locator  = Characters[idx].location.locator;
	}
	//Стартуем
	LAi_boarding_process = true;
	LAi_LoadLocation(deckID, locType);
	CreateEntity(&boarding_fader, "fader");
	SendMessage(&boarding_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
}

//Загрузить локацию абордажа
void LAi_LoadLocation(string locationID, int locType)
{
//	Log_TestInfo("LoadLocation()");
	ReloadProgressStart();
	//Ищем локацию
	int locIndex = FindLocation(locationID);
	Log_SetActiveAction("Nothing");
    Dead_Char_num = 0; // boal dead body
	if(locIndex >= 0)
	{
		//Устанавливаем главного персонажа
		ref mchr = GetMainCharacter();
		mchr.location = locationID;
		mchr.location.group = "rld";
		// boal random place to star battle 28/11/03 -->
		int locNum[20];
		int locI;
		int maxLocators = 2;
		
		if (CheckAttribute(&Locations[locIndex], "boarding.locatorNum"))
		{
			maxLocators = sti(Locations[locIndex].boarding.locatorNum);
		}
		locI = 0;
		locNum[locI] = rand(maxLocators-1);
		string sLocType = "loc";
		// определение стороны на палубе
		if (CheckAttribute(&Locations[locIndex], "UpDeckType"))
		{
		    sLocType = ChooseShipUpDeck(mchr, boarding_enemy);
		}

		mchr.location.locator = sLocType + locNum[locI];
		// для каюты перекрыть
		if (CheckAttribute(&Locations[locIndex], "CabinType") && CheckAttribute(&Locations[locIndex], "boarding.Loc.Hero"))
		{
		    mchr.location.locator = Locations[locIndex].boarding.Loc.Hero;
		}
		bool   okLoc;
		int iLoc, j;
		// boal random place to star battle 28/11/03 <--
		if (!CheckAttribute(&Locations[locIndex], "CabinType"))
		{ // не грузим офов, если каюту
	        //Устанавливаем офицеров, если такие есть
	        int logined = 1;
			for(int i = 1; i < 4; i++)
			{
				int idx = GetOfficersIndex(mchr, i);
				if(idx < 0) continue;
				//Log_TestInfo("Load Off " + Characters[idx].id);
				if (logined > maxLocators) break;

	            // boal 05.09.03 offecer need to go to abordage -->
//			    if(makeint(Characters[idx].AbordageMode) == 0) continue;	// не берем
				// boal 05.09.03 offecer need to go to abordage <--
				DeleteAttribute(&Characters[idx], "location");
				//Characters[idx].location = locationID;
				// boal чтоб грузились в фортах -->
				Characters[idx].location.loadcapture = true;
			    // boal чтоб грузились в фортах <--
				//Characters[idx].location.group = "rld";
				// boal random place to star battle 28/11/03 -->
				iLoc = rand(3);

				okLoc = false;
				while (!okLoc)
				{
				    for(j = 0; j <=locI; j++)
				    {
				        if (locNum[j] == iLoc)
				        {
				            iLoc = rand(3);
				            okLoc = false;
				            break;
				        }
				        okLoc = true;
				    }
				}
				locI++;
			    locNum[locI] = iLoc;
				//Characters[idx].location.locator = sLocType + locNum[locI];
				// boal random place to star battle 28/11/03 <--
				ChangeCharacterAddressGroup(&Characters[idx], locationID, "rld", sLocType + locNum[locI]);
				logined = logined + 1;
			}
		}
		//Перегружаемся в локацию
		boarding_location = locIndex;
		if(LoadLocation(&Locations[boarding_location]))
		{
			//работа с сундуком в каюте boal -->
			if (CheckAttribute(&Locations[boarding_location], "CabinType"))
			{
				FillAboardCabinBox(&Locations[boarding_location], boarding_enemy);
			}  
			//Расставляем персонажей
			LAi_SetBoardingActors(locationID);
			// boal <--
			//Вытащим саблю
			SendMessage(&mchr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightMode", 1);
			//Запретим диалог
			dialogDisable = true;
		}else{
			Trace("Boarding: Boarding location not loaded, current loc <" + locationID + ">");
		}
	}else{
		Trace("Boarding: Boarding location not found, current loc <" + locationID + ">");
	}
	ReloadProgressEnd();
	PostEvent("LoadSceneSound", 500);
}

//Перегрузиться в следующую локации
void LAi_ReloadBoarding()
{
//	Log_TestInfo("ReloadBoarding()");
	//Разрешим диалоги
	dialogDisable = false;
	//Проверим предыдущую локацию
	if(boarding_location < 0)
	{
		Trace("Boarding: No loaded current boarding location");
		LAi_boarding_process = false;
		return;
	}
	//Установить хендлеры для обработки
	SetEventHandler("FaderEvent_StartFade", "LAi_ReloadStartFade", 0);
	SetEventHandler("FaderEvent_EndFade", "LAi_ReloadEndFade", 0);
	//Создаём фейдер и запускаем
	CreateEntity(&boarding_fader, "fader");
	
	string nextDeck = Locations[boarding_location].boarding.nextdeck;
	if (nextDeck != "")
	{
		if(CheckAttribute(&Locations[FindLocation(nextDeck)], "image"))
		{
			SendMessage(&boarding_fader, "ls", FADER_PICTURE, Locations[FindLocation(nextDeck)].image);
		}
	}
	float fadeOutTime = RELOAD_TIME_FADE_OUT;
	SendMessage(&boarding_fader, "lfl", FADER_OUT, fadeOutTime, false);
	SendMessage(&boarding_fader, "l", FADER_STARTFRAME);
}

void LAi_ReloadStartFade()
{
	//Выгружаем локацию
 //ResetSoundScheme();
	ResetSound(); // new
	PauseAllSounds();
	SendMessage(&Particles,"l", PS_CLEAR_CAPTURED);
	DelEventHandler("FaderEvent_StartFade", "LAi_ReloadStartFade");
	if(boarding_location >= 0) UnloadLocation(&Locations[boarding_location]);
}

void LAi_ReloadEndFade()
{        
	bool bCaptanSurrender = false;
	//Загружаем следующую локацию
	DelEventHandler("FaderEvent_EndFade", "LAi_ReloadEndFade");
	SendMessage(&boarding_fader, "lfl", FADER_IN, RELOAD_TIME_FADE_IN, true);
	//Определим возможность продолжения перегрузок
	bool canReload = true;
	
	if(boarding_enemy_crew <= 0) canReload = false;
	if(!CheckAttribute(&Locations[boarding_location], "boarding.nextdeck")) canReload = false;
	if(Locations[boarding_location].boarding.nextdeck == "") canReload = false;
	// boal -->
	if (!canReload)// && CheckAttribute(boarding_enemy, "AboardToFinalDeck"))
    {
		if (CheckAttribute(boarding_enemy, "ShipCabinLocationId"))
		{
            if (!CheckAttribute(boarding_enemy, "DontRansackCaptain")) //квестовые не сдаются
	        {
	            Surrendered = (Surrendered) || (TestRansackCaptain);
	    		if (Surrendered && (boarding_location_type == BRDLT_SHIP))
	    		{
	    		    bCaptanSurrender = true;    // это уже не первая палуба точно, значит потери были
	   			}
	    	}
    		if (!bCaptanSurrender)
    		{
				boarding_enemy_crew = 1;
				if(CheckAttribute(&Locations[FindLocation(boarding_enemy.ShipCabinLocationId)], "image"))
				{
					SendMessage(&boarding_fader, "ls", FADER_PICTURE, Locations[FindLocation(boarding_enemy.ShipCabinLocationId)].image);
				}
				LAi_LoadLocation(boarding_enemy.ShipCabinLocationId, -1);
				DeleteAttribute(boarding_enemy, "ShipCabinLocationId"); // чтоб не зациклилось
				return;
			}
		}
    }
    // boal <--
 	if(canReload && !Surrendered) // так как  nextdeck  везда пусто, никогда не рабоатет
	{
		//Продолжаем абордаж
		LAi_LoadLocation(Locations[boarding_location].boarding.nextdeck, -1);
	}
    else
    {
  		if (CheckAttribute(pchar, "GenQuest.QuestAboardCaptanSurrender")) // квестовая сдача в плен кэпа
		{
		    DeleteAttribute(pchar, "GenQuest.QuestAboardCaptanSurrender"); // разово
		    bCaptanSurrender = true;
		}
		SendMessage(&Particles,"l", PS_CLEAR_CAPTURED); // повторное удаление партиклов, фикс огней в море.
		//Следующей локации нет
		DelEventHandler("LAi_event_GroupKill", "LAi_BoardingGroupKill");
		boarding_location = -1;
		//Пересчитываем команду игрока

		float crew = boarding_player_crew * boarding_player_crew_per_chr; // ВЫЖИВШИЕ офицеры - это не мартросы не должны влиять
		// boal 22.01.2004 -->
		ref mchar       = GetMainCharacter();
		float fDefenceSkill = 0.9 + MakeFloat(GetSummonSkillFromName(mchar, SKILL_DEFENCE)) / SKILL_MAX;
		int deadCrew    = makeint((boarding_player_base_crew - crew) / fDefenceSkill + 0.3); // бонус выжившим
		int deadCrewWOMedic = makeint(boarding_player_base_crew - crew); // без бонуса
		float leaderSkill = GetSummonSkillFromNameToOld(mchar, SKILL_LEADERSHIP);
		int iTemp;
		if (leaderSkill < 1) leaderSkill = 1; //fix
		
		// расчет медицины -->
		iTemp = deadCrewWOMedic - deadCrew;
		if (iTemp > 0)
		{
		    if (GetCargoGoods(mchar, GOOD_MEDICAMENT) < iTemp)
		    {
                deadCrewWOMedic = iTemp - GetCargoGoods(mchar, GOOD_MEDICAMENT); // умерли от ран
				RemoveCharacterGoodsSelf(mchar, GOOD_MEDICAMENT, GetCargoGoods(mchar, GOOD_MEDICAMENT)); // все нулим
				deadCrew += deadCrewWOMedic; // трупов больше
				Log_Info("Из-за нехватки медикаментов от ран умерли " + deadCrewWOMedic + " матросов");
		    }
		    else
		    {
		        RemoveCharacterGoodsSelf(mchar, GOOD_MEDICAMENT, iTemp);
		        if (GetCargoGoods(mchar, GOOD_MEDICAMENT) < 16)
			    {
			        Log_Info("На корабле " + mchar.Ship.Name + " осталось мало медикаментов");
			    }
		    }
		}
		// расчет медицины <--
		// Saved value is not needed anymore... (Gray 14.02.2005)
		DeleteAttribute(mchar, "EnemyRank");
        bQuestCheckProcessFreeze = false;//fix
        
		RemoveCharacterGoodsSelf(mchar, GOOD_WEAPON, deadCrew);
		
		crew = boarding_player_base_crew - deadCrew; // выжившие с бонусом
		
		Statistic_AddValue(mchar, "DeadCrewBoard", deadCrew);
		AddCharacterExpToSkill(mchar, "Defence", makeint(deadCrew / 3 + 0.5)); //качаем защиту
        AddCharacterExpToSkill(mchar, "Grappling", makeint(deadCrew / 3 + 0.5));
                	
		// после боя падает мораль
		if (deadCrew > makeint(crew+0.3)) // погибло больше, чем выжило
		{
      		AddCrewMorale(mchar, sti(-20 / leaderSkill));
		}
		else
		{  //растет, если потерь мало
			AddCrewMorale(mchar, sti(leaderSkill));
		}

		// boal 22.01.2004 <--
		SetCrewQuantityOverMax(GetMainCharacter(), MakeInt(crew + 0.3)); // десант весь ГГ как перегруз команды
		Log_TestInfo("----- в конце стало " + crew +" матросов ---");
		//Пересчитываем команду соперника
		crew = 0;// какие еще люди? все трупы! boarding_enemy_base_crew*(0.1 + rand(20)*0.01);
		if (boarding_echr_index >= 0)
		{
			SetCrewQuantity(&Characters[boarding_echr_index], MakeInt(crew + 0.3));
			boarding_echr_index = -1;
		}

		//Начислим опыт
		//AddCharacterExp(GetMainCharacter(), MakeInt(boarding_exp));
                
		//Настроим интерфейс
		Log_SetActiveAction("Nothing");
		EndBattleLandInterface();
		//Востановим адреса
		ref mchr = GetMainCharacter();
		mchr.location         = boarding_adr[0].location;
		mchr.location.group   = boarding_adr[0].group;
		mchr.location.locator = boarding_adr[0].locator;
		
		for(int i = 1; i < 4; i++)
		{
			int idx = GetOfficersIndex(GetMainCharacter(), i);
			if(idx < 0) continue;
			Characters[idx].location         = boarding_adr[i].location;
			Characters[idx].location.group   = boarding_adr[i].group;
			Characters[idx].location.locator = boarding_adr[i].locator;
		}
		//Выгружаемся в интерфейс
		LAi_boarding_process = false;
		// START MOD Code by Stone-D : 27/07/2003
        if (bCaptanSurrender)
        {
			ChangeCrewExp(pchar, "Soldiers", 4);
			LAi_SetCurHPMax(boarding_enemy);  // нужно, чтоб был живой!!!
			SetCrewQuantity(boarding_enemy, MakeInt(boarding_enemy_base_crew*(rand(20)*0.01))); // раз кэп живой, то можно раненых воскресить
			LaunchRansackMain(pchar, boarding_enemy, "captain"); //капитан в плен
			LAi_boarding_process = false;
			Event(SHIP_CAPTURED, "l", sti(boarding_enemy.index));
			return;
		}
		// END MOD Code by Stone-D : 27/07/2003
		if(boarding_location_type == BRDLT_SHIP)
		{       
		    ChangeCrewExp(pchar, "Soldiers", 5);
		    // нельзя это делать до формы, критерий будет "" LAi_SetCurHP(boarding_enemy, 0.0); // умер, чтоб на форме обмена не жил
			LaunchRansackMain(pchar, boarding_enemy, ""); //не сдался
			LAi_boarding_process = false;	 
			Event(SHIP_CAPTURED, "l", sti(boarding_enemy.index));
			return;
		}
		if(boarding_location_type == BRDLT_FORT)
		{
            ChangeCrewExp(pchar, "Soldiers", 7);
			Event(FORT_CAPTURED, "l", sti(boarding_enemy.index));
			boarding_enemy.Ship.Crew.Quantity = 10 + rand(350); // рабы (остатки выживших)
			LaunchFortCapture(boarding_enemy);
			LAi_boarding_process = false;
			return;
		}
		Trace("Boarding: unknow boarding location type");
		Log_TestInfo("Boarding - unknown boarding location type : " + boarding_location_type);
	}
}

//Разрешить перегрузку на следующую палубу
#event_handler("LAi_event_boarding_EnableReload", "LAi_EnableReload");
void LAi_EnableReload()
{
    //Log_Testinfo("LAi_BoardingGroupKill boardM = " + boardM);
	if (boardM == 1)
	{
	//	Trace("Выключить шума абордажа");
		//ResetSound();
		ResetSoundScheme(); //надо гасить только Scheme, иначе партиклы звука на кострах, факелах, и прочем - не пашут
		//StopSound(abordageSoundID, 0);
		SetSoundScheme("deck"); // обычная схема
		SetMusic("music_bitva");
		boardM = -1;
		//Log_Testinfo("Выключить шума абордажа " + abordageSoundID);
	}
	Surrendered = CheckForSurrender(GetMainCharacter(), boarding_enemy, 2); // проверка сдачи в плен, перед каютой 2 - второй учет метода

	SetEventHandler("Control Activation","LAi_ActivateReload",1);
	Log_SetActiveAction("Reload");
	//Уберём саблю
	ref mchr = GetMainCharacter();
	SendMessage(&mchr, "lsl", MSG_CHARACTER_EX_MSG, "ChangeFightMode", 0);
	
	// sd -->
	Log_TestInfo("Start boarding_enemy_crew_start: " + boarding_enemy_crew_start + " boarding_enemy_crew: " + boarding_enemy_crew);
	Log_TestInfo("Start boarding_player_crew_start: " + boarding_player_crew_start + " boarding_player_crew: " + boarding_player_crew);
	// sd <--
	
	//Вернём выживших обратно в кучу
	for(int i = 0; i < LAi_numloginedcharacters; i++)
	{
		int index = LAi_loginedcharacters[i];
		if(index >= 0)
		{
			if(index != GetMainCharacterIndex())
			{
				ref chr = &Characters[index];
				if(LAi_IsDead(chr) == false && !IsOfficer(chr)) // boal && sd не нужны офицеры для матросов
				{
					if(chr.chr_ai.group == LAI_GROUP_PLAYER)
					{
						boarding_player_crew = boarding_player_crew + 1;
					}
				}
			}
		}
	}
	// END MOD Code by Stone-D : 01/08/2003 -->
	Log_TestInfo("New boarding_player_crew: " + boarding_player_crew);
	Log_TestInfo("Next Location: " + Locations[boarding_location].boarding.nextdeck);
	// END MOD Code by Stone-D : 01/08/2003 <--
}

//Активация перегрузки на следующую палубу
void LAi_ActivateReload()
{
	string controlName = GetEventData();
	if(controlName != "ChrAction") return;
	DelEventHandler("Control Activation", "LAi_ActivateReload");
	Log_SetActiveAction("Nothing");
	LAi_ReloadBoarding();
}

//Расставить персонажей для боя
void LAi_SetBoardingActors(string locID)
{
    int    limit;
	int    i;
	ref    chr;
	string model;
	string ani;
	int    xhp;
	int    locIndex = FindLocation(locID);
	int    mclass = GetCharacterShipClass(GetMainCharacter());
	int    eclass = GetCharacterShipClass(boarding_enemy);
	ref    mchr = GetMainCharacter(); // boal star with new loc always
    int    locMChar;
    
	limit = MAX_GROUP_SIZE;
	// локаторов разное число на моделях :( Переделал все на инфу из кода boal 01.01.05  +1 всегда для ГГ
	chr = &Locations[locIndex];
	
    if(CheckAttribute(chr, "boarding.locatorNum")) limit = sti(chr.boarding.locatorNum);

	Log_TestInfo("Location: " + locID + " Limit: " + limit);
	Log_TestInfo("Player: " + boarding_player_crew + " х " + boarding_player_crew_per_chr + " Enemy: " + boarding_enemy_crew + " х " + boarding_enemy_crew_per_chr);
	

	//Установим союзников из команды
	// boal прежде всего ставим офицеров, коротых в фотре почему-то нет -->
	/*if (IsFort) // boal 21.01.05 не прошло и года... починил в другом месте - правильно
	{
        for(i = 1; i < 4; i++)
		{
			int idx = GetOfficersIndex(mchr, i);
			if(idx < 0) continue;
            // boal 05.09.03 offecer need to go to abordage -->
		    if(makeint(Characters[idx].AbordageMode) == 0) continue;	// не берем
		    // грузим сами насильно уже в сущ локацию
		    chr = &Characters[idx];
		    PlaceCharacter(chr, mchr.location.group, mchr.location);
		}
	} */
	// boal прежде всего ставим офицеров, коротых в фотре почему-то нет <--
    string sLocType = "loc";
	// определение стороны на палубе
	if (CheckAttribute(&Locations[locIndex], "UpDeckType"))
	{
	    sLocType = ChooseShipUpDeck(GetMainCharacter(), boarding_enemy);
	}
	if (!CheckAttribute(&Locations[locIndex], "CabinType"))
	{ // не грузим матросов в каюту
		for(i = LAi_numloginedcharacters; i < limit; i++)
		{
			if(boarding_player_crew <= 0) break;
			model = LAi_GetBoardingModel(GetMainCharacter(), &ani);
			// boal star with new loc always  -->
			if (mchr.location.locator == (sLocType + i))
			{ // искодим из того, что наша локация всегда < 4 офицеры пусть накладываются а матросик идет к противнику.
	           locMChar = rand(3);
	           while (mchr.location.locator == (sLocType + locMChar))
	           {
	               locMChar = rand(3);
	           }
		       chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLocType+locMChar);// 0-всегда свободен, если офицеров нет, а i != 0, тк мы точно есть
			}
			else
			{
			   chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLocType + i);
			}
			// boal star with new loc always  <--

			LAi_group_MoveCharacter(chr, LAI_GROUP_PLAYER);

			boarding_player_crew = boarding_player_crew - 1;
			// boal расчет как у нормальных фантомов -->
			//LAi_SetAdjustFencingSkill(chr, 3.0, 6.0);
			//LAi_AdjustFencingSkill(chr);

			// boal <--
			if (!IsFort)
			{
	            SetFantomParamAbordOur(chr);
			}
			else
			{
			    SetFantomParamFortOur(chr);
			}
			SetNewModelToChar(chr); //иначе сабли не те, что реально
			AddCharHP(chr, boarding_player_hp); // влияение опыта и морали в НР
			/*if (!bNewFantomGenerator)
	        {
				xhp = GetBoarding_player_hp(boarding_player_hp);
				LAi_SetHP(chr, xhp, xhp);
			}*/
		}
	}
	//Установим врагов
	if (sLocType == "loc")
	{
	    sLocType = "aloc";
	}
	else
	{
	    sLocType = "loc";
	}
	for(i = 0; i < limit; i++) // <= тк loc0 .. loc4 = 5
	{
		if(boarding_enemy_crew <= 0) break;
		model = LAi_GetBoardingModel(boarding_enemy, &ani);
		if (i == 0 && CheckAttribute(&Locations[locIndex], "boarding.Loc.Capt")) //локатор каюты
		{
		    chr = LAi_CreateFantomCharacterEx(model, ani, "rld", Locations[locIndex].boarding.Loc.Capt);
		}
		else
		{
			chr = LAi_CreateFantomCharacterEx(model, ani, "rld", sLocType + i);
		}

		LAi_group_MoveCharacter(chr, LAI_GROUP_BRDENEMY);
		
		boarding_enemy_crew = boarding_enemy_crew - 1;
		// boal расчет как у нормальных фантомов -->
		//LAi_SetAdjustFencingSkill(chr, 2.0, 6.0);
		//LAi_AdjustFencingSkill(chr);

		if (!IsFort)
		{
            SetFantomParamAbordEnemy(chr);
		}
		else
		{
            SetFantomParamFortEnemy(chr);
			//xhp = GetBoarding_enemy_hp(LAi_GetCharacterMaxHP(chr));
		}
		// капитана в рубку!!! boal
		if (i == 0 && CheckAttribute(&Locations[locIndex], "CabinType"))
		{
			ChangeAttributesFromCharacter(chr, boarding_enemy, true);
			chr.CaptanId = boarding_enemy.id; // иначе у фантома свой ИД   // to_do поправить опечатку
			chr.SuperShooter = true; // супер стрелок (вероятность пальбы выше, от сложности, если еще и пистоль есть)
			chr.greeting = "CapSinkShip";
			SetCharacterPerk(chr, "Energaiser"); // скрытый перк дает 1.5 к приросту энергии, дается ГГ и боссам уровней
			SetQuestAboardCabinDialog(chr); /// проверка на минНр
			// если это место отработало, то кэп либо убит, либо по квестам сдается в плен
		}
		SetNewModelToChar(chr); //иначе сабли не те, что реально
		AddCharHP(chr, boarding_enemy_hp); // влияение опыта и морали в НР
		/*if (!bNewFantomGenerator)
        {
            xhp = GetBoarding_enemy_hp(boarding_enemy_hp);
			if (xhp < 20)
			{
			   xhp = 20;
			}
			LAi_SetHP(chr, xhp, xhp);
		} */
	}
	//Заставим драться эти 2 группы
	LAi_group_FightGroupsEx(LAI_GROUP_PLAYER, LAI_GROUP_BRDENEMY, true, GetMainCharacterIndex(), -1, false, false);
	LAi_group_SetCheckEvent(LAI_GROUP_BRDENEMY);
}

//Убийство группы
void LAi_BoardingGroupKill()
{
	string group = GetEventData();
	if(group != LAI_GROUP_BRDENEMY) return;
	// отключим шум абордажа (который включается при аларме в sound.c)

	PostEvent("LAi_event_boarding_EnableReload", 5000);
}

//Моделька для абордажного персонажа
string LAi_GetBoardingModel(ref rCharacter, ref ani)
{
	ani = "man";
	int iNation = sti(rCharacter.nation);
	string atr;
	string model;
	
	if(sti(rCharacter.index) == GetMainCharacterIndex())
	{
        // boal 290904 форма солдат -->
        if (CheckAttribute(rCharacter, "GenQuest.CrewSkelMode"))
        {
            model = GetRandSkelModel();
			ani = "man";
			return model;
        }
        if (isMainCharacterPatented() && sti(Items[sti(rCharacter.EquipedPatentId)].TitulCur) > 1) //форма только со звания капитан
        {
            atr = "boardingModel.enemy";
            iNation = sti(Items[sti(rCharacter.EquipedPatentId)].Nation);
        }
        else
        {
            atr = "boardingModel.player";
        }
        // boal 290904 форма солдат <--
	}else
    {   //boal -->
		if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Trade")
		{
            atr = "boardingModel.merchant";
        }
		else
        {//boal <--
            atr = "boardingModel.enemy";
        }
	}
	
	if (iNation < 0) iNation = PIRATE;
	//eddy. замаскировавшися пиратов тоже надо учитывать
	if (CheckAttribute(rCharacter, "Ship.Mode") && rCharacter.Ship.Mode == "Pirate") iNation = PIRATE;
	Nations[iNation].boardingModel.player = "";
	Nations[iNation].boardingModel.enemy = "";
	
	aref models;
	makearef(models, Nations[iNation].(atr));
	int num = GetAttributesNum(models);
	if(num <= 0) return "";
	int i = rand(num - 1);
	atr = GetAttributeName(GetAttributeN(models, i));
	model = models.(atr);
	atr = atr + ".ani";
	if(CheckAttribute(models, atr))
	{
		if(models.(atr) != "")
		{
			ani = models.(atr);
		}
	}
	return model;
}

// boal 03/08/06 полностью переписал метод Stone-D от 27/07/2003
bool CheckForSurrender(ref mchr, ref echr, int _deck)
{
    if(boarding_location_type == BRDLT_FORT) return false; // Forts don't surrender.
    
    if (GetPrisonerQty() > PRISONER_MAX) return false; // очень много пленых
    
    if (sti(mchr.rank) < (sti(echr.rank) - MOD_SKILL_ENEMY_RATE / 2))  return false; // 26/06/07 Проверка на ранг
    
	int eclass = GetCharacterShipClass(echr);
	int mclass = GetCharacterShipClass(mchr);
	if (eclass == 1) return false; // 1 класс не сдается в принципе
	
	float fCrewRate = 0.5;  
	if (sti(echr.Nation) == PIRATE)
	{
		fCrewRate = 0.2;
	}
	else
	{
		if (CheckAttribute(echr, "Ship.Mode") && echr.Ship.Mode == "Trade") // торговцы склонны сдаться
		{
			fCrewRate = 0.9; 
		}
	}
	
	float mcrew = stf(GetWeaponCrew(mchr, GetCrewQuantity(mchr))); // честный учет с оружием
	float ecrew = stf(GetCrewQuantity(echr));
	
	float fRep    = abs(REPUTATION_NEUTRAL - sti(mchr.reputation)) / 50.0; // приведение к 0..1
	float emorale = stf(echr.ship.crew.morale) / MORALE_MAX; // 0..1  это рандом
	float mmorale = stf(mchr.ship.crew.morale) / MORALE_MAX; // 0..1
	float mskill  = (GetSummonSkillFromNameToOld(mchr, "Leadership") + GetSummonSkillFromNameToOld(mchr, "Grappling")) / 20.0;  // 0..10
	float eskill  = (GetCharacterSkillToOld(echr, "Leadership") + GetCharacterSkillToOld(echr, "Defence")) / 20.0;    // 0..10

    mcrew = mcrew * (mcrew * GetCrewExp(mchr, "Soldiers")) / (GetOptCrewQuantity(mchr) * GetCrewExpRate()); 
    ecrew = ecrew * (ecrew * GetCrewExp(echr, "Soldiers")) / (GetOptCrewQuantity(echr) * GetCrewExpRate());  // это рандом, а значит случайность
    mcrew = mcrew *(0.5 + mmorale);
    ecrew = ecrew *(0.5 + emorale); // рандом в  emorale - она ранд
    
    mcrew = mcrew * (0.2 + mskill)*(0.05 + fRep)*fCrewRate;
    ecrew = ecrew * (0.2 + eskill);
	// подсчет компаньенов у сторон  -->
    int mShip = GetCompanionQuantity(mchr);
    int eShip;
    string sGroupID = Ship_GetGroupID(echr);
    if (sGroupID != "")
    {
    	eShip = Group_GetLiveCharactersNum(sGroupID);
    }
    else
    {
    	eShip = 1;
    }
	// <--
	float fStep = 1;
	if (_deck == 2)
	{
		fStep = 1.4;
	}
    mcrew = mcrew * (1.0 + mclass / 20.0) * fStep; // влияние класса минимально, тк есть в экипаже
    ecrew = ecrew * (1.0 + eclass / 20.0);  // класс также влияет наоборот, дает бонус мелким
    
    mcrew = mcrew * (1.0 + (mShip-1) / 5.0);
    ecrew = ecrew * (1.0 + (eShip-1) / 5.0);
    if (bBettaTestMode) // иначе плодил компил.лог в подзорку
    {
    	Log_Info("Surrender Hero = "+ mcrew + "    Enemy = " + ecrew + " eShipQty = " + eShip);
    }
    if (mcrew > ecrew)
	{
		return true; // Yay! Surrender!
	}
	return false; // не сдался
}

// boal 03/12/05 выбор локатора от корабля лок - маленькие, алок - большие-->
string ChooseShipUpDeck(ref _mchar, ref _enemy)
{
    string sLoc = "loc";
    
	if (GetCargoMaxSpace(_mchar) > GetCargoMaxSpace(_enemy))
	{
	    sLoc = "aloc";
	}
	
    return sLoc;
}
// boal 03/12/05 <--