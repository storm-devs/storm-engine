
#define MAX_RUMOURS	10000;
int RumoursQuantity = 0;

object Rumours[MAX_RUMOURS];

void GenerateRumour(string sType, int iArgument1, int iArgument2, int iArgument3)
{
	//hovernor_change_location - губернатор меняет столицу. iArgument1 - индекс перса, iArgument2 - старая столица, iArgument3 - новая столица
	//country_lost_all_colonies - страна потеряла все колонии. iArgument1 - нация
	//hovernor_killed - губернатор убит. iArgument1 - нация
	//hovernor_captured - губернатор захвачен. iArgument1 - нация
	//new_capital - столица перенесена. iArgument1 - новая столица, iArgument2 - нация

	//gold_convoy_begin - образование золотого конвоя, iArgument1 - iNation, iArgument2 - beginColonyIndex, iArgument2 - endColonyIndex
	//gold_convoy_end - завершение золотого конвоя, iArgument1 - iNation, iArgument2 - beginColonyIndex
	//gold_convoy_dead - убийство золотого конвоя, iArgument1 - iNation, iArgument2 - character index, who killed convoy

	//trade_up - товар подорожал, iArgument1 = GOOD_, iArgument2 = price
	//trade_down - товар подешевел, iArgument1 = Good, iArgument2 = price
	//war_begin - начало войны, iArgument1 = Nation, iArgument2 = Nation
	//war_end - конец войны, iArgument1 = Nation, iArgument2 = Nation
	//friend_end - конец дружбы. iArgument1 = Nation, iArgument2 = Nation
	//friend_begin - начало дружбы. iArgument1 = Nation, iArgument2 = Nation
	//make_alliance - альянс, iArgument1 = Nation, iArgument2 = Nation, iArgument3 = Nation
	//alliance_end - конец альянса. iArgument1 = Nation, iArgument2 = Nation
	//colony_captured - колония захвачена, iArgument1 = Colony, iArgument2 = кому принадлежала, iArgument3 - кто захватил
	//colony_occupation - окупация колонии, iArgument1 = Nation, iArgument2 - Enemy, iArgument3 - Colony
	//colony_robbing - разграбление колонии, iArgument1 = Nation, iArgument2 - Nation, iArgument3 - Colony
	//colony_destroy - уничтожение колонии, iArgument1 = Nation, iArgument2 - Nation, iArgument3 - Colony
	//colony_defend - снятие окупации колонии, iArgument1 = Nation, iArgument2 - Nation, iArgument3 - Colony
	//ranger_dead - смерть рейнджера, iArgument1 = characterIndex, iArgument2 - type
	//ranger_captured - рейнджер захвачен, iArgument1 = characterIndex, iArgument2 - island, iArgument3 - type
	//ranger_uncaptured - рейнджер освобожден, iArgument1 = characterIndex, iArgument2 - island, iArgument3 - type
	//disease_begin - началась болезнь, iArgument1 = island.index, iArgument2 - нация
	//disease_end - закончилась болезнь, iArgument1 = island.index
	//colony_colonists - прибытие колонистов, iArgument1 = colony.index
	//history_event - историческое событие, iArgument1 = event.index
	//ship_info - информация по судам, iArgument1 - индекс персонажа, iArgument2 - индекс колонии, с которой отправился, iArgument3 индекс колонии куда отправился
	//colony_upgrade - апргейд колонии, iArgument1 - индекс апргейда, iArgument2 - индекс колонии, iArgument3 - шаг апгрейда
	//ranger_career - рейнджер поступил на службу, iArgument1 - characterIndex, iArgument2 - нация, iArgument3 - тип рейнджера
	
	int iNumber = CheckRumourPosition();
	
	int iInfoColony = -1;
	string sTypeInfo = "-1";

	string sTargetColony, sTargetColonyAcc, sTargetColonyDat, sTargetColonyGen;
	string sNextColony, sNextColonyAcc, sNextColonyDat, sNextColonyGen;
	string sWho, sName, sMoney;
	string sGood, sGoodgen, sGoodAcc;
	string sShipName, sUpgrade;

	string sParam = "";
	string sStringID;
	string sResult;
	switch (sType)
	{
		case "hovernor_change_location":
			FillRussianColonyString(iArgument2, 1, &sParam);
			FillRussianColonyString(iArgument3, 2, &sParam);
			FillRussianNationString(sti(Characters[iArgument1].nation), 1, &sParam);
			sParam = DialogAddParamToStr(sParam, "sCharName", Characters[iArgument1].id);
			sStringID = "HovernorChangeLocationRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "country_lost_all_colonies":
			FillRussianNationString(iArgument1, 1, &sParam);
			sStringID = "DefeatedNationRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "hovernor_killed":
			FillRussianNationString(iArgument1, 1, &sParam);
			sStringID = "HovernorKilledRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "hovernor_captured":
			FillRussianNationString(iArgument1, 1, &sParam);
			sStringID = "HovernorCapturedRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "new_capital":
			FillRussianColonyString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 1, &sParam);
			
			sStringID = "NewCapitalFounded" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "gold_convoy_dead":
			FillRussianNationString(iArgument1, 1, &sParam);
			sParam = DialogAddParamToStr(sParam, "sCharName", Characters[iArgument2].id);
			sStringID = "DefeatGoldConvoyEndRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		case "gold_convoy_end":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianColonyString(iArgument2, 1, &sParam);
			sStringID = "SelectGoldConvoyEndRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "gold_convoy_begin":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianColonyString(iArgument2, 1, &sParam);
			FillRussianColonyString(iArgument3, 2, &sParam);
			
			sStringID = "SelectGoldConvoyBeginRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "ranger_career":
			if (iArgument2 == 0)
			{
				sWho = XI_ConvertString("whotrader");
			}
			else
			{
				sWho = XI_ConvertString("whocorsair");
			}
			sName = GetCharacterFullName(characters[iArgument1].id);
			FillRussianNationString(iArgument2, 1, &sParam);
			
			sParam = DialogAddParamToStr(sParam, "sWho", sWho);
			sParam = DialogAddParamToStr(sParam, "sCharName", sName);

			sStringID = "SelectRangeCareerRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "colony_occupation":
			FillRussianNationString(iArgument1, 2, &sParam);
			FillRussianNationString(iArgument2, 1, &sParam);
			
			FillRussianColonyString(iArgument3, 1, &sParam);

			sStringID = "SelectColonyOccupationRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "colony_captured":
			iInfoColony = iArgument3;
			sTypeInfo = "nation";
			FillRussianNationString(iArgument2, 1, &sParam);
			FillRussianNationString(iArgument3, 2, &sParam);

			FillRussianColonyString(iArgument1, 1, &sParam);

			sStringID = "SelectColonyCapturedRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "colony_robbing":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			FillRussianColonyString(iArgument3, 1, &sParam);

			sStringID = "SelectColonyRobbingRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "colony_destroy":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			FillRussianColonyString(iArgument3, 1, &sParam);

			sStringID = "SelectColonyDestroyRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "colony_defend":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);
			
			FillRussianColonyString(iArgument3, 1, &sParam);

			sStringID = "SelectColonyDefendRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "trade_up":
			sGood = XI_ConvertString(Goods[iArgument1].name);
			sGoodGen = XI_ConvertString(Goods[iArgument1].name + "Gen");
			sGoodAcc = XI_ConvertString(Goods[iArgument1].name + "Acc");
			sMoney = FindRussianMoneyString(iArgument2);

			sParam = DialogAddParamToStr(sParam, "sGood", sGood);
			sParam = DialogAddParamToStr(sParam, "sGoodGen", sGoodGen);
			sParam = DialogAddParamToStr(sParam, "sGoodAcc", sGoodAcc);
			sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

			sStringID = "SelectPriceUpRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "trade_down":
			sGood = XI_ConvertString(Goods[iArgument1].name);
			sGoodGen = XI_ConvertString(Goods[iArgument1].name + "Gen");
			sGoodAcc = XI_ConvertString(Goods[iArgument1].name + "Acc");
			sMoney = FindRussianMoneyString(iArgument2);

			sParam = DialogAddParamToStr(sParam, "sGood", sGood);
			sParam = DialogAddParamToStr(sParam, "sGoodGen", sGoodGen);
			sParam = DialogAddParamToStr(sParam, "sGoodAcc", sGoodAcc);
			sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

			sStringID = "SelectPriceDownRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "war_begin":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			sStringID = "SelectWarBeginRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "war_end":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			sStringID = "SelectWarEndRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "friend_end":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);
	
			sStringID = "SelectFriendshipEndRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "friend_begin":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			sStringID = "SelectFriendshipBeginRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "make_alliance":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);
			FillRussianNationString(iArgument3, 3, &sParam);
	
			sStringID = "SelectMakeAllianceRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;

		case "alliance_end":
			FillRussianNationString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 2, &sParam);

			sStringID = "SelectAllianceBrokenRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "ranger_dead":
			if (iArgument2 == 0)
			{
				sWho = XI_ConvertString("whotrader");
			}
			else
			{
				sWho = XI_ConvertString("whocorsair");
			}
			sName = GetCharacterFullName(characters[iArgument1].id);
			
			sParam = DialogAddParamToStr(sParam, "sWho", sWho);
			sParam = DialogAddParamToStr(sParam, "sCharName", sName);
	
			sStringID = "SelectDeathOfRangerRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "ranger_captured":
			if (iArgument3 == 0)
			{
				sWho = XI_ConvertString("whotrader");
			}
			else
			{
				sWho = XI_ConvertString("whocorsair");
			}
			sName = GetCharacterFullName(characters[iArgument1].id);
			
			FillRussianColonyString(iArgument2, 1, &sParam);

			sStringID = "SelectRangerCapturedRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "ranger_uncaptured":
			if (iArgument3 == 0)
			{
				sWho = XI_ConvertString("whotrader");
			}
			else
			{
				sWho = XI_ConvertString("whotrader");
			}
			sName = GetCharacterFullName(characters[iArgument1].id);
			
			FillRussianColonyString(iArgument2, 1, &sParam);

			sParam = DialogAddParamToStr(sParam, "sWho", sWho);
			sParam = DialogAddParamToStr(sParam, "sCharName", sName);
			sStringID = "SelectRangerUnCapturedRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "disease_begin":
			FillRussianColonyString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 1, &sParam);


			sStringID = "SelectDiseaseBeginRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "disease_end":
			FillRussianColonyString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 1, &sParam);


			sStringID = "SelectDiseaseEndRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		case "colony_colonists":
			FillRussianColonyString(iArgument1, 1, &sParam);
			FillRussianNationString(iArgument2, 1, &sParam);

			sStringID = "SelectColonistReachedRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
		
		/*case "history_event":
			sResult = HistoryEvents[iArgument1].text;
		break;*/

		case "ship_info":
			FillRussianColonyString(iArgument2, 1, &sParam);

			sShipName = characters[iArgument1].ship.name;

			FillRussianColonyString(iArgument3, 2, &sParam);

			sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);
			
			sStringID = "SelectShipInfoRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);

			Rumours[iNumber].area = "tavern";
			Rumours[iNumber].isquest = 1;
		break;

		case "colony_upgrade":
			iInfoColony = iArgument2;
			string sStep = "l"+iArgument3;
			sTypeInfo = ColonyUpgrades[iArgument1].(sStep).name;

			FillRussianColonyString(iArgument2, 1, &sParam);

			FillRussianNationString(iArgument3, 1, &sParam);

			sUpgrade = XI_ConvertString(ColonyUpgrades[iArgument1].(sStep).name);
			string sUpgradeAcc = XI_ConvertString(ColonyUpgrades[iArgument1].(sStep).name + "Acc");

			sParam = DialogAddParamToStr(sParam, "sUpgrade", sUpgrade);
			sParam = DialogAddParamToStr(sParam, "sUpgradeAcc", sUpgradeAcc);

			sStringID = "SelectColonyUpgradeRumour" + rand(2);
			sResult = DialogAssembleStr(sStringID, sParam);
		break;
	}	
	
	Rumours[iNumber].text = sResult;
	Rumours[iNumber].date = environment.date.day + " " + XI_ConvertString("target_month_" + environment.date.month) + " " + environment.date.year + ".";
	Rumours[iNumber].day = environment.date.day;
	Rumours[iNumber].month = environment.date.month;
	Rumours[iNumber].year = environment.date.year;
	if(iInfoColony != -1)
	{
		Rumours[iNumber].colony = iInfoColony;
	}
	else
	{
		DeleteAttribute(&Rumours[iNumber], "colony");
	}
	if(sType != "-1")
	{
		Rumours[iNumber].colony.fieldupdate = sTypeInfo;
	}

	RumoursQuantity ++;
}


int CheckRumourPosition()
{
	for(int i = 0; i < MAX_RUMOURS; i++)
	{
		if (!CheckAttribute(&Rumours[i], "text"))
		{
			return i;
		}
	}
	return -1;
}

void EmptyAllRumours()
{
	for (int i=0; i<MAX_RUMOURS; i++)
	{
		DeleteAttribute(&Rumours[i], "text");
		DeleteAttribute(&Rumours[i], "questbook");
		DeleteAttribute(&Rumours[i], "date");
		DeleteAttribute(&Rumours[i], "isquest");
	}
}

int SelectRumour()
{
	int k = 0;
	int Rum[MAX_RUMOURS];

	for (int i = 0; i < RumoursQuantity; i++)
	{
		if (CheckAttribute(&Rumours[i], "text") && !CheckAttribute(&Rumours[i], "questbook"))
		{
			Rum[k] = i;
			k++;
		}
	}
	if (k > 0)
	{
		k = rand(k-1);
		return Rum[k];
	}
	return -1;
}

void CheckColonyInfoUpdate(int iRumour)
{
	int iColony;
	string sFieldForUpdate;
	if(CheckAttribute(&Rumours[iRumour], "colony"))
	{
		iColony = sti(Rumours[iRumour].colony);

		if(CheckAttribute(&Rumours[iRumour], "colony.fieldupdate") && iColony > 0);
		{
			sFieldForUpdate = Rumours[iRumour].colony.fieldupdate;
			if(CheckAttribute(&coloniesInfo[iColony], sFieldForUpdate))
			{
				coloniesInfo[iColony].(sFieldForUpdate) = colonies[iColony].(sFieldForUpdate);
			}
		}
	}
}

void AddRumourToQuestBook(int step)
{
	rumours[step].questbook = "marked";
}

string SelectNews(int step)
{
	string sText;
	
	
	if (step >= MAX_RUMOURS)
	{
		step = MAX_RUMOURS - 1;
	}
	
	if (!CheckAttribute(&Rumours[step], "text"))
	{
		if(rumours[step].questbook == "marked")
		{
			sText = XI_ConvertString("Speak_No_News");
			//"Мы давно в плавании и ничего не слышали о последних событиях в архипелаге.";
		}
		else
		{
			sText = sText + Rumours[step].text;
		
			AddRumourToQuestBook(sti(step));
		}
	}
	else
	{
		sText = sText + Rumours[step].text;
		
		AddRumourToQuestBook(sti(step));
	}
	
	return sText;
}
