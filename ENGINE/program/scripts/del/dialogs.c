
string SelectTakeItemDialog(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	int isfight = sti(Quests[iQuest].l.isfightpossible);
	//ג עוסעמגûץ צוכÿץ ןנמגונÿול ןנמסעמ מעהאקף ןנוהלועא.
	isFight = 0;

	string sStringID;
	
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isfight == 0)
		{
			//בונוע ןנוהלוע
			sStringID = "SelectTakeItemDialog" + rand(3);
		}
		if(isfight == 1)
		{
			//בונוע ןנוהלוע
		}
		if(isfight == 2)
		{
			//ןנוהכאדאוע גסענועטעüסÿ חא דמנמהמל ג ודמ המלו
		}
		if(isfight == 3)
		{
			//ןנוהכאדאוע גסענועטעüסÿ חא דמנמהמל
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sParam = "";
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectGiveItemDialog(aref chr)
{
	int iQuest = sti(chr.CurrentQuest);
	int isfight = sti(Quests[iQuest].l.isfightpossible);
	//ג עוסעמגûץ צוכÿץ ןנמגונÿול ןנמסעמ מעהאקף ןנוהלועא.
	isFight = 0;

	string sStringID;
	
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isfight == 0)
		{
			//בונוע ןנוהלוע
			sStringID = "SelectGiveItemDialog" + rand(3);
		}
		if(isfight == 1)
		{
			//בונוע ןנוהלוע
		}
		if(isfight == 2)
		{
			//ןנוהכאדאוע גסענועטעüסÿ חא דמנמהמל ג ודמ המלו
		}
		if(isfight == 3)
		{
			//ןנוהכאדאוע גסענועטעüסÿ חא דמנמהמל
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sParam = "";
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectSinkShipReminderDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sShipName = characters[GetCharacterIndex(Quests[sti(chr.CurrentQuest)].l.shipchar)].ship.name;
	sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectSinkShipDialogReminder";
		if(Quests[sti(chr.CurrentQuest)].l.item != "-1")
		{
			sStringID = "SelectCaptureShipAndItemDialogReminder";
		}
		if(Quests[sti(chr.CurrentQuest)].l.capturechar != "-1")
		{
			sStringID = "SelectCaptureShipAndPassengerDialogReminder";
		}
		
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID;
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectSinkShipDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sShipName = characters[GetCharacterIndex(Quests[sti(chr.CurrentQuest)].l.shipchar)].ship.name;
	sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectSinkShipDialog" + rand(9);
		if(Quests[sti(chr.CurrentQuest)].l.item != "-1")
		{
			sStringID = "SelectCaptureShipAndItemDialog" + rand(9);
		}
		if(Quests[sti(chr.CurrentQuest)].l.capturechar != "-1")
		{
			sStringID = "SelectCaptureShipAndPassengerDialog" + rand(9);
		}
		
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectHuntForSlavesDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);

	string sCharName = GetCharacterFullName(chr.id);
	string sSlaves = FindRussianSlavesString(sti(Quests[sti(chr.CurrentQuest)].l.slaves), "No"));

	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	sParam = DialogAddParamToStr(sParam, "sSlaves", sSlaves);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectHuntForSlavesDialog" + rand(5);
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryTradeDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	
	int iGood = sti(Quests[sti(chr.CurrentQuest)].l.goods);
	FindRussianGoodString(iGood, &sParam, 1);

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(sti(Quests[sti(chr.CurrentQuest)].l.isdelayed) == 0)
		{
			sStringID = "SelectDeliveryTradeDialog" + rand(9);
		}
		else
		{
			sStringID = "SelectDelayedDeliveryTradeDialog" + rand(9);
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryTradeReminderDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	
	int iGood = sti(Quests[sti(chr.CurrentQuest)].l.goods);
	FindRussianGoodString(iGood, &sParam, 1);
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(sti(Quests[sti(chr.CurrentQuest)].l.isdelayed) == 0)
		{
			sStringID = "SelectDeliveryTradeDialogReminder";
		}
		else
		{
			sStringID = "SelectDelayedDeliveryTradeDialogReminder";
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID;
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectEscortShipDialog(aref chr)
{
	string sShipName = characters[GetCharacterIndex(Quests[sti(chr.CurrentQuest)].l.shipchar)].ship.name;
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(sti(Quests[sti(chr.CurrentQuest)].l.isdelayed) == 0)
		{
			sStringID = "SelectEscortShipDialog" + rand(9);
		}
		else
		{
			sStringID = "SelectDelayEscortShipDialog" + rand(9);
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID;
	}

	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectEscortShipReminderDialog(aref chr)
{
	string sShipName = characters[GetCharacterIndex(Quests[sti(chr.CurrentQuest)].l.shipchar)].ship.name;
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(sti(Quests[sti(chr.CurrentQuest)].l.isdelayed) == 0)
		{
			sStringID = "SelectEscortShipDialogReminder";
		}
		else
		{
			sStringID = "SelectDelayEscortShipDialogReminder";
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}


string SelectDeliveryCitizenDialog(aref chr)
{
	string sCharName = GetCharacterFullName(Quests[sti(chr.CurrentQuest)].l.passenger);
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectDeliveryCitizenDialog" + rand(5);
		chr.CurrentQuest.lastdialog = sStringID;
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryPassengerDialog(aref chr)
{
	int isDelayed = sti(Quests[sti(chr.CurrentQuest)].l.isdelayed);
	string sCharName = GetCharacterFullName(Quests[sti(chr.CurrentQuest)].l.passenger);
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isDelayed == 1)
		{
			sStringID = "SelectDelayDeliveryPassengerDialog" + rand(9);
		}
		else
		{
			sStringID = "SelectDeliveryPassengerDialog" + rand(9);
		}
		chr.CurrentQuest.lastdialog = sStringID;
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryPassengerReminderDialog(aref chr)
{
	int isDelayed = sti(Quests[sti(chr.CurrentQuest)].l.isdelayed);
	string sCharName = GetCharacterFullName(Quests[sti(chr.CurrentQuest)].l.passenger);
	
	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	
	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isDelayed == 1)
		{
			sStringID = "SelectDelayDeliveryPassengerDialogReminder";
		}
		else
		{
			sStringID = "SelectDeliveryPassengerDialogReminder";
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryItemDialog(aref chr)
{
	int isDelayed = sti(Quests[sti(chr.CurrentQuest)].l.isdelayed);
	string sItem = Quests[sti(chr.CurrentQuest)].l.item;
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sItem", sItem);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isDelayed == 1)
		{
			sStringID = "SelectDelayDeliveryItemDialog" + rand(9);
		}
		else
		{
			sStringID = "SelectDeliveryItemDialog" + rand(9);
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryItemReminderDialog(aref chr)
{
	int isDelayed = sti(Quests[sti(chr.CurrentQuest)].l.isdelayed);
	string sItem = Quests[sti(chr.CurrentQuest)].l.item;
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sItem", sItem);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		if(isDelayed == 1)
		{
			sStringID = "SelectDelayDeliveryItemDialogReminder";
		}
		else
		{
			sStringID = "SelectDeliveryItemDialogReminder";
		}
		if(sti(Quests[sti(chr.CurrentQuest)].l.ishovernor) == 1)
		{
			sStringID = "Hovernor" + sStringID;
		}
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectCaptureFortDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectCaptureFortDialog" + rand(9);
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

void SelectCaptureFortReminderDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectCaptureFortDialogReminder";
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDefendFortDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sTime = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;

	string sParam = "";

	int iNation = sti(Quests[sti(chr.CurrentQuest)].l.agressor);
	FillRussianNationString(iNation, 1, &sParam);

	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);
	
	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectCaptureFortDialog" + rand(9);
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectKillCharacterDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	string sName = GetCharacterFullName(characters[sti(Quests[sti(chr.CurrentQuest)].l.target_character)].id);
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sTime = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;
		
	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);
	sParam = DialogAddParamToStr(sParam, "sName", sName);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectKillCharacterDialog" + rand(9);
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectStealItemDialog(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));
	string sName = GetCharacterFullName(characters[sti(Quests[sti(chr.CurrentQuest)].l.target_character)].id);
	string sItem = XI_ConvertString(Quests[sti(chr.CurrentQuest)].l.item);
	string sItemAcc = XI_ConvertString("Acc_"+Quests[sti(chr.CurrentQuest)].l.item);
	string sItemPP3 = XI_ConvertString(Items[FindItem(Quests[sti(chr.CurrentQuest)].l.item)].pp3);
	
	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	int iYear = GetAddingDataYear(0,0, idays);
	int iMonth = GetAddingDataMonth(0,0, idays);
	int iDay = GetAddingDataDay(0,0, idays);
	string sTime = iDay + " " + XI_ConvertString("target_month_" + iMonth) + " " + iYear;
		
	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);
	sParam = DialogAddParamToStr(sParam, "sName", sName);
	sParam = DialogAddParamToStr(sParam, "sItem", sItem);
	sParam = DialogAddParamToStr(sParam, "sItemAcc", sItemAcc);
	sParam = DialogAddParamToStr(sParam, "sItemPP3", sItemPP3);

	string sStringID;
	if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
	{
		sStringID = chr.CurrentQuest.lastdialog;
	}
	else
	{
		sStringID = "SelectStealItemDialog" + rand(9);
		chr.CurrentQuest.lastdialog = sStringID
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectPassengerQuestInfo(aref chr)
{
	string sParam = "";
	string sMoney = FindRussianMoneyString(sti(chr.quest.deliverymoney));
	int iColony = FindColony(chr.quest.colony);
	FillRussianColonyString(iColony, 1, &sParam);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);

	string sStringID = "SelectPassengerQuestInfo";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}

string SelectPrisonerInfo(aref chr)
{
	string sMoney = FindRussianMoneyString(CalculateRansomPrice(chr));

	string sParam = "";
	FillRussianNationString(sti(chr.nation), 1, &sParam);

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	string sStringID = "SelectPrisonerInfo";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}	

string SelectPrisonerQuestInfo(aref chr)
{
	string sParam = "";
	int iColony = FindColony(chr.colony);
	FillRussianColonyString(iColony, 1, &sParam);

	string sCharName = GetCharacterFullName(characters[sti(chr.employer)].id)
	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);

	string sStringID = "SelectPrisonerQuestInfo";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}	

string SelectEscortQuestInfo(aref chr)
{
	string sParam = "";
	int iColony = FindColony(chr.quest.convoyquest.colony);
	FillRussianColonyString(iColony, 1, &sParam);
	string sMoney = FindRussianMoneyString(sti(chr.quest.convoyquest.money));

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	string sStringID = "SelectEscortQuestInfo";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}


string SelectBountyQuestInfo(aref chr, int iNation)
{
	string sParam = "";
	FillRussianNationString(iNation, 1, &sParam);
	string sNation = GetNationNameByType(iNation);
	string sMoney = FindRussianMoneyString(sti(chr.bounty.(sNation).money));

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	string sStringID = "SelectBountyText";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}


string SelectOfficerSalaryInfo(ref chr)
{
	string sParam = "";
	string sMoney = FindRussianMoneyString(sti(chr.quest.OfficerPrice));

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	string sStringID = "SelectOfficerSalaryInfo";
	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}

string SelectHovernorGreetingsDialog(ref chr)
{
	int iNation = sti(chr.nation);
	int iRelation = GetReputationFromNation2Character(pchar, iNation);

	string sParam = "";
	sParam = DialogAddParamToStr(sParam, "sCharName", pchar.name);

	string sStringID;

	if(iRelation <= 99)
	{
		sStringID = "SelectHovernorGreetingsDialog0";
	}

	if(iRelation <= 60)
	{
		sStringID = "SelectHovernorGreetingsDialog1";
	}

	if(iRelation <= 40)
	{
		sStringID = "SelectHovernorGreetingsDialog2";
	}

	string sResult = DialogAssembleStr(sStringID, sParam);
	return sResult;
}

string SelectDeliveryTradeDialogQuestion(aref chr)
{
	int iQuest = sti(chr.currentquest);
	string sTargetChar = Quests[iQuest].l.target_npc;
	//string sParentChar = Quests[iQuest].l.parent_npc;
	if(chr.id == sTargetChar)
	{
		string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

		string sParam = "";
		
		int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.parent_colony);
		FillRussianColonyString(iColony, 1, &sParam);

		
		int iGood = sti(Quests[sti(chr.CurrentQuest)].l.goods);
		FindRussianGoodString(iGood, &sParam, 1);

		int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
		string sTime = FindRussianDaysString(idays);

		sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
		sParam = DialogAddParamToStr(sParam, "sTime", sTime);

		string sStringID;
		if(CheckAttribute(chr, "CurrentQuest.lastdialog"))
		{
			sStringID = chr.CurrentQuest.lastdialog;
		}
		else
		{
			if(sti(Quests[sti(chr.CurrentQuest)].l.isdelayed) == 0)
			{
				sStringID = "SelectDeliveryTradeDialogQuestion";
			}
			else
			{
				sStringID = "SelectDelayedDeliveryTradeDialogQuestion";
			}
			chr.CurrentQuest.lastdialog = sStringID
		}
	}

	
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

string SelectDeliveryTradeDialogEx(aref chr)
{
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	
	int iGood = sti(Quests[sti(chr.CurrentQuest)].l.goods);
	FindRussianGoodString(iGood, &sParam, 1);

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sShipName = generateRandomNameToShip(sti(chr.nation));

	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);
	sParam = DialogAddParamToStr(sParam, "sShipName", sShipName);

	string sStringID;

	switch(sti(chr.FranceQuestProgress))
	{
		case 1:
			sStringID = "SelectFirstFranceLineDialog";
		break;
		case 3:
			sStringID = "SelectThirdFranceLineDialog";
		break;
	}

	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}


string SelectDeliveryPassengerDialogEx(aref chr)
{
	string sCharName = GetCharacterFullName(Quests[sti(chr.CurrentQuest)].l.passenger);
	string sMoney = FindRussianMoneyString(sti(Quests[sti(chr.CurrentQuest)].l.money));

	int idays = sti(Quests[sti(chr.CurrentQuest)].l.time);
	string sTime = FindRussianDaysString(idays);

	string sParam = "";
	
	int iColony = FindColony(Quests[sti(chr.CurrentQuest)].l.target_colony);
	FillRussianColonyString(iColony, 1, &sParam);
	sParam = DialogAddParamToStr(sParam, "sCharName", sCharName);
	sParam = DialogAddParamToStr(sParam, "sMoney", sMoney);
	sParam = DialogAddParamToStr(sParam, "sTime", sTime);

	string sStringID;
	
	switch(sti(chr.FranceQuestProgress))
	{
		case 2:
			sStringID = "SelectSecondFranceLineDialog";
		break;
	}
	string sResult = DialogAssembleStr(sStringID, sParam);

	return sResult;
}

