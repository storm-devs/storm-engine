#define I_MIN_MORALE	10

// boal -->
#define FOOD_BY_CREW       10.0
#define FOOD_BY_SLAVES     20.0
#define FOOD_BY_PASSENGERS 10.0
// boal <--

//bool bInterfaceFood = false;
//int iFoodQuantity = 0;

// вернет число дней на сколько есть еда на всех кораблях
int CalculateFood()
{
	int chrIndex;
	int iCrewQuantity = 0;
	float foodNeeded = 0;
	int iSlavesQuantity = 0;
	int iFoodQuantity = 0;

	for (int i=0; i<COMPANION_MAX; i++)
	{
		chrIndex = GetCompanionIndex(pchar, i);
		if (chrIndex != -1)
		{
            if (!GetRemovable(&characters[chrIndex])) continue;
			if (characters[chrIndex].ship.type != SHIP_NOTUSED)
			{
				iCrewQuantity   = iCrewQuantity   + sti(characters[chrIndex].ship.crew.quantity);
				iFoodQuantity   = iFoodQuantity   + GetCargoGoods(&characters[chrIndex], GOOD_FOOD);
				iSlavesQuantity = iSlavesQuantity + GetCargoGoods(&characters[chrIndex], GOOD_SLAVES);
			}
		}
	}
	int iPassQuantity = GetPassengersQuantity(pchar);

	foodNeeded = makefloat(iCrewQuantity/FOOD_BY_CREW + iPassQuantity/FOOD_BY_PASSENGERS + iSlavesQuantity/FOOD_BY_SLAVES);

	if (foodNeeded < 1)
	{
		foodNeeded = 1;
	}

	iFoodQuantity = makeint(iFoodQuantity/foodNeeded + 0.2);

	return iFoodQuantity;
}

// еды на одном корабле
int CalculateShipFood(ref _chr)
{
	int iCrewQuantity = 0;
	float foodNeeded = 0;
	int iSlavesQuantity = 0;
	int iFoodQuantity = 0;
	int iPassQuantity = 0;

	iCrewQuantity   =  sti(_chr.ship.crew.quantity);
	iFoodQuantity   =  GetCargoGoods(_chr, GOOD_FOOD);
	iSlavesQuantity =  GetCargoGoods(_chr, GOOD_SLAVES);
	if (_chr.id == pchar.id) 
	{
		iPassQuantity = GetPassengersQuantity(pchar);
	}

	foodNeeded = makefloat(iCrewQuantity/FOOD_BY_CREW + iPassQuantity/FOOD_BY_PASSENGERS + iSlavesQuantity/FOOD_BY_SLAVES);

	if (foodNeeded < 1)
	{
		foodNeeded = 1;
	}

	iFoodQuantity = makeint(iFoodQuantity/foodNeeded + 0.2);

	return iFoodQuantity;
}

// boal 21.04.04 крысы на корабле -->
void DailyRatsEatGoodsUpdate(ref chref)
{
    int iGoods = GOOD_POWDER + rand(GOOD_OIL - GOOD_POWDER);
    int iQuantity = GetCargoGoods(chref, iGoods);
    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt");
    if (iQuantity > 10 && rand(5) != 2) // шанс не жрать, если весь спектр
    {
        if (GetCharacterItem(chref, "indian11") > 0) return; // проверка крысиного бога

        iQuantity = 1+ rand(makeint(iQuantity / (3+GetSummonSkillFromNameToOld(chref, SKILL_REPAIR) + GetSummonSkillFromNameToOld(chref,SKILL_FORTUNE))));
        RemoveCharacterGoodsSelf(chref, iGoods, iQuantity);
        //PlaySound("interface\notebook.wav");
        Log_SetStringToLog(RandSwear() + " Крысы на корабле " +
                           chref.Ship.Name + LinkRandPhrase(" испортили ", " сожрали ", " уничтожили ") +
                           iQuantity + " шт. " + LanguageConvertString(iSeaGoods, "seg_" + Goods[iGoods].Name));

        AddCharacterExpToSkill(chref, SKILL_REPAIR, 7);
        AddCharacterExpToSkill(chref, SKILL_FORTUNE, 2);
    }
    LanguageCloseFile(iSeaGoods);
}

// boal food for crew 20.01.2004 -->
void DailyEatCrewUpdate()   // сюда пихаю все что в 1 день
{
    ref mainCh = GetMainCharacter();
    int i, cn, crew, morale;
    ref chref;
    int nMoraleDecreaseQ;
    
    // to_do
    // boal 030804 Начисление денег верфям -->
    //DailyShipyardMoneyUpdate();
    // boal 030804 Начисление денег верфям <--
    mainCh.questTemp.abordage = 0; // fix квест потопить пирата второй абордаж

    //таможня
    //if(IsCharacterPerkOn(mainCh, "CustomsHouse"))
    //{
    //    AddGoverGoods();
    //}

    SetNewDayHealth(); // здоровье за день
    // >>>>>======== квест Аззи, подсчет контрольныйх сумм по неуязвимости =================
    AzzyCheckSumControl();
    // <<<<<======== квест Аззи, подсчет контрольныйх сумм по неуязвимости =================
	// ОЗГи -->
	//SetPortShoreEnter(mainCh);
    DeleteAttribute(mainCh, "GenQuest.Hunter2Pause");  // boal бойня в форте кончилась - ОЗГи вернулись
    // ОЗГи <--

    //  уже не нужно SetAllHabitueToNew(); // сменить всех пьяниц в тавернах

	////////////////      ЕДА     /////////////////
	if (bNoEatNoRats) return; // betatest
    if (sti(mainCh.Ship.Type) == SHIP_NOTUSED ) return;

    for(i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mainCh,i);
		if( cn>=0 )
		{
			chref = GetCharacter(cn);

			if (!GetRemovable(chref)) continue;

			// RATS -->
			DailyRatsEatGoodsUpdate(chref);
			// RATS <--

			crew = GetCrewQuantity(chref);
			if (crew < 1 && GetCargoGoods(chref, GOOD_SLAVES) < 1)   continue;
			if (!CheckAttribute(chref,"Ship.Crew.Morale"))
			{
			    chref.Ship.Crew.Morale = 50;
			}
			// расчет медицины -->
			if (rand(4) == 2)
			{
				// матросы
				cn = crew / 10;
				if (cn > 30) cn = 30;
				cn = rand(cn)+1;
                if (crew < cn) {cn = crew}
                if (cn > 0)
                {
					if (GetCargoGoods(chref, GOOD_MEDICAMENT) < 1)
				    {
				        Log_Info("На корабле " + chref.Ship.Name + " от болезней умерло " + FindRussianSailorString(cn, "No"));
				        crew = crew - cn;
				        chref.Ship.Crew.Quantity = crew;
				        // мораль в минус
	                    morale = sti(chref.Ship.Crew.Morale);

	                    if (CheckOfficersPerk(chref, "IronWill")) cn /= 1.5;

                        AddCrewMorale(chref, -makeint(cn / 2)); // до 15 пунктов за раз
				    }
					else
					{
					    RemoveCharacterGoodsSelf(chref, GOOD_MEDICAMENT, cn);
					    if (GetCargoGoods(chref, GOOD_MEDICAMENT) < 16)
					    {
					        Log_Info("На корабле " + chref.Ship.Name + " осталось мало медикаментов");
					    }
					}
			    }
			    // рабы
       			cn = GetCargoGoods(chref, GOOD_SLAVES) / 10;
				if (cn > 30) cn = 30;
				cn = rand(cn)+1;
                if (GetCargoGoods(chref, GOOD_SLAVES) < cn) {cn = GetCargoGoods(chref, GOOD_SLAVES)}
                if (cn > 0)
                {
					if (GetCargoGoods(chref, GOOD_MEDICAMENT) < 1)
				    {
				        Log_SetStringToLog("На корабле " + chref.Ship.Name + " от болезней умерло " + FindRussianSlavesString(cn, "No"));
            			RemoveCharacterGoodsSelf(chref, GOOD_SLAVES, cn);
				    }
					else
					{
                        cn /= 3;
						RemoveCharacterGoodsSelf(chref, GOOD_MEDICAMENT, cn);
					}
			    }
			    // повторный контроль
			    if (crew < 1 && GetCargoGoods(chref, GOOD_SLAVES) < 1)   continue;
			}
			// расчет медицины <--

			crew = makeint((crew+5.1) / FOOD_BY_CREW); // eat ratio
			//if (crew == 0) crew = 1;
			if (crew > 0)
			{
				if (GetCargoGoods(chref, GOOD_RUM) >= crew)
				{
				    RemoveCharacterGoodsSelf(chref, GOOD_RUM, crew);
				    // проверка на остатки
	                cn = makeint(GetCargoGoods(chref, GOOD_RUM) / crew);
	                if (cn < 1)
	                {
	                   Log_SetStringToLog("На корабле " + chref.Ship.Name + " весь ром выпит");
	                }
	                // поднимем мораль
	                AddCrewMorale(chref, 2);
				}
				else
				{
				    crew = GetCargoGoods(chref, GOOD_RUM);
				    RemoveCharacterGoodsSelf(chref, GOOD_RUM, crew);
				}
			}
			crew = GetCrewQuantity(chref);
			// рассчет перегруза команды на мораль -->
			if (crew > GetOptCrewQuantity(chref))
			{
				AddCrewMorale(chref, -(1+rand(3)));
			} 
			// рассчет перегруза команды на мораль <--
			// расчет еды после Рома
			crew = makeint((crew+5.1) / FOOD_BY_CREW + GetPassengersQuantity(chref) / FOOD_BY_PASSENGERS); // eat ratio
			crew = crew + makeint( (GetCargoGoods(chref, GOOD_SLAVES)+6)/ FOOD_BY_SLAVES);  // учет рабов
			if (crew == 0) crew = 1;
			if (GetCargoGoods(chref, GOOD_FOOD) >= crew)
			{
			   RemoveCharacterGoodsSelf(chref, GOOD_FOOD, crew);
			   // проверка на остатки
               cn = makeint(GetCargoGoods(chref, GOOD_FOOD) / crew);
               if (cn < 4)
               {
                  Log_SetStringToLog("На корабле " + chref.Ship.Name + " продовольствия осталось на " + FindRussianDaysString(cn));
                  Log_SetStringToLog("Нужно срочно пополнить запасы!");
                  PlaySound("interface\notebook.wav");
               }
               // возможный бунт рабов
               if (sti(chref.index) == GetMainCharacterIndex() && GetCargoGoods(chref, GOOD_SLAVES) > (GetCrewQuantity(chref)*1.5 + sti(chref.Ship.Crew.Morale)))
               {
                    nMoraleDecreaseQ = 12 - GetSummonSkillFromNameToOld(chref, SKILL_LEADERSHIP);
                    if (CheckOfficersPerk(chref, "IronWill")) nMoraleDecreaseQ /= 2;
                    if (rand(2) == 1 && nMoraleDecreaseQ > rand(10))
                    {
                        if (IsEntity(worldMap))
                    	{
                            chref.GenQuest.SlavesMunity = true;

                            Log_Info("Рабы подняли восстание!");
                            MunityOnShip("SlavesMunity");
                        }
                    }
               }
			}
			else
			{
			    crew = GetCargoGoods(chref, GOOD_FOOD);
			    RemoveCharacterGoodsSelf(chref, GOOD_FOOD, crew);
			    PlaySound("interface\notebook.wav");

			    Log_SetStringToLog("На корабле " + chref.Ship.Name + " матросы голодают. Мораль команды падает!");

			    if (sti(chref.index) == GetMainCharacterIndex())
                {
			    	AddCharacterHealth(mainCh, -1);
			    }

			    cn = GetCrewQuantity(chref);
			    if (cn > 1)
			    {
			       chref.Ship.Crew.Quantity = cn - makeint(cn/10 +0.5);
			       Log_SetStringToLog("Матросы умирают от голода");
			    }
			    cn = GetCargoGoods(chref, GOOD_SLAVES);
			    if (cn > 0)
			    {
			       RemoveCharacterGoodsSelf(chref, GOOD_SLAVES, makeint(cn/5 + 0.5));
			       Log_SetStringToLog("Рабы умирают от голода");
			    }
                morale = sti(chref.Ship.Crew.Morale);

                nMoraleDecreaseQ = 12 - GetSummonSkillFromNameToOld(chref, SKILL_LEADERSHIP);
                if (CheckOfficersPerk(chref, "IronWill")) nMoraleDecreaseQ /= 2;
                chref.Ship.Crew.Morale = morale - nMoraleDecreaseQ;
                if (sti(chref.Ship.Crew.Morale) < MORALE_MIN)
                {
                   chref.Ship.Crew.Morale = MORALE_MIN;  
                }   
			}
			if (sti(chref.index) == GetMainCharacterIndex())
			{
			    if (sti(chref.Ship.Crew.Morale) <= MORALE_MIN)
			    {
					int locidx = FindLocation(chref.location);
				    if(IsEntity(worldMap) && GetCrewQuantity(chref) > 0)
					{
				        Log_SetStringToLog("Бунт на корабле " + chref.Ship.Name + "!!!! ");
				        MunityOnShip("ShipMunity");
				    }
			    }
			}
			else
			{
				if (GetShipRemovable(chref))  // ПГГ не бунтует
				{
					if (sti(chref.Ship.Crew.Morale) <= MORALE_MIN || sti(chref.loyality) <= 0) // допуск, что лояльность есть у всех офов
				    {
				        if (GetCrewQuantity(chref) > 0)
				        {
							Log_SetStringToLog("Бунт на корабле " + chref.Ship.Name + "!!!! ");
					        Log_SetStringToLog("Корабль выходит из эскадры");
					        Statistic_AddValue(mainCh, "ShipMunity", 1);
					        RemoveCharacterCompanion(mainCh, chref);
					        //fix  ПГГ
							if (!CheckAttribute(chref, "PGGAi"))
							{
								chref.LifeDay = 0; // стереть при выходе
							}
							else
							{
							    chref.PGGAi.IsPGG = true;
							    chref.RebirthPhantom = true;
							    chref.PGGAi.location.town = PGG_FindRandomTownByNation(sti(chref.nation));
								chref.Dialog.FileName = "PGG_Dialog.c";
								chref.Dialog.CurrentNode = "Second Time";

								PGG_ChangeRelation2MainCharacter(chref, -20);
							}
					        chref.location = ""; // нафиг, нафиг..а то в таверне появлялся...
					        chref.location.group = "";
					        chref.location.locator = "";
				        }
				    }
			    }
			}
		}
	}
}

// boal 20.01.2004 <--