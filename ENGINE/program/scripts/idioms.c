
void FillRussianNationString(int iNation, int iStep, ref sParam)
{
	string sNationName =  GetNationNameByType(iNation);

	string sTempString = sParam;

	if(iStep == 1)
	{
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation", XI_ConvertString("people"+sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNationAcc", XI_ConvertString("people"+sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNationGen", XI_ConvertString("people"+sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNationDat", XI_ConvertString("people"+sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNationAbl", XI_ConvertString("people"+sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNationAdj", XI_ConvertString("people"+sNationName + "Adj"));

		sTempString = DialogAddParamToStr(sTempString, "sNation", XI_ConvertString(sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sNationAcc", XI_ConvertString(sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sNationGen", XI_ConvertString(sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sNationDat", XI_ConvertString(sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sNationAbl", XI_ConvertString(sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sNationAdj", XI_ConvertString(sNationName + "Adj"));
	}
	if(iStep == 2)
	{
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2", XI_ConvertString("people"+sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2Acc", XI_ConvertString("people"+sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2Gen", XI_ConvertString("people"+sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2Dat", XI_ConvertString("people"+sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2Abl", XI_ConvertString("people"+sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation2Adj", XI_ConvertString("people"+sNationName + "Adj"));

		sTempString = DialogAddParamToStr(sTempString, "sNation2", XI_ConvertString(sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sNation2Acc", XI_ConvertString(sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sNation2Gen", XI_ConvertString(sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sNation2Dat", XI_ConvertString(sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sNation2Abl", XI_ConvertString(sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sNation2Adj", XI_ConvertString(sNationName + "Adj"));
	}
	if(iStep == 3)
	{
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3", XI_ConvertString("people"+sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3Acc", XI_ConvertString("people"+sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3Gen", XI_ConvertString("people"+sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3Dat", XI_ConvertString("people"+sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3Abl", XI_ConvertString("people"+sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sPeopleNation3Adj", XI_ConvertString("people"+sNationName + "Adj"));

		sTempString = DialogAddParamToStr(sTempString, "sNation3", XI_ConvertString(sNationName));
		sTempString = DialogAddParamToStr(sTempString, "sNation3Acc", XI_ConvertString(sNationName + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sNation3Gen", XI_ConvertString(sNationName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sNation3Dat", XI_ConvertString(sNationName + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sNation3Abl", XI_ConvertString(sNationName + "Abl"));
		sTempString = DialogAddParamToStr(sTempString, "sNation3Adj", XI_ConvertString(sNationName + "Adj"));
	}

	sParam = sTempString;
}

void FillRussianColonyString(int iColony, int iStep, ref sParam)
{
	string sColony = colonies[iColony].id;

	string sTempString = sParam;
	if(iStep == 1)
	{
		sTempString = DialogAddParamToStr(sTempString, "sTargetColony", XI_ConvertString("colony" + sColony));
		sTempString = DialogAddParamToStr(sTempString, "sTargetColonyAcc", XI_ConvertString("colony" + sColony + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sTargetColonyDat", XI_ConvertString("colony" + sColony + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sTargetColonyGen", XI_ConvertString("colony" + sColony + "Gen"));

		sTempString = DialogAddParamToStr(sTempString, "sWhereToTargetColony", XI_ConvertString("colony" + sColony + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sWhereFromTargetColony", XI_ConvertString("colony" + sColony + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sWhereTargetColony", XI_ConvertString("colony" + sColony + "Dat"));
	}
	if(iStep == 2)
	{
		sTempString = DialogAddParamToStr(sTempString, "sNextColony", XI_ConvertString("colony" + sColony));
		sTempString = DialogAddParamToStr(sTempString, "sNextColonyAcc", XI_ConvertString("colony" + sColony + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sNextColonyDat", XI_ConvertString("colony" + sColony + "Dat"));
		sTempString = DialogAddParamToStr(sTempString, "sNextColonyGen", XI_ConvertString("colony" + sColony + "Gen"));

		sTempString = DialogAddParamToStr(sTempString, "sWhereToNextColony", XI_ConvertString("colony" + sColony + "Acc"));
		sTempString = DialogAddParamToStr(sTempString, "sWhereFromNextColony", XI_ConvertString("colony" + sColony + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sWhereNextColony", XI_ConvertString("colony" + sColony + "Dat"));
	}
	sParam = sTempString;
}

void FindRussianGoodString(int iGood, ref sParam, int iStep)
{
	string sGoodName = Goods[iGood].name;
	string sTempString = sParam;

	if(iStep == 1)
	{
		sTempString = DialogAddParamToStr(sTempString, "sGood", XI_ConvertString(sGoodName));
		sTempString = DialogAddParamToStr(sTempString, "sGoodGen", XI_ConvertString(sGoodName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sGoodAcc", XI_ConvertString(sGoodName + "Acc"));
	}
	if(iStep == 2)
	{
		sTempString = DialogAddParamToStr(sTempString, "sGood2", XI_ConvertString(sGoodName));
		sTempString = DialogAddParamToStr(sTempString, "sGood2Gen", XI_ConvertString(sGoodName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sGood2Acc", XI_ConvertString(sGoodName + "Acc"));
	}
	if(iStep == 3)
	{
		sTempString = DialogAddParamToStr(sTempString, "sGood3", XI_ConvertString(sGoodName));
		sTempString = DialogAddParamToStr(sTempString, "sGood3Gen", XI_ConvertString(sGoodName + "Gen"));
		sTempString = DialogAddParamToStr(sTempString, "sGood3Acc", XI_ConvertString(sGoodName + "Acc"));
	}

	sParam = sTempString;
}

string FindRussianDaysString(int idays)
{
	string sDays = idays;
	int iLen1 = strlen(&sDays) - 1;
	int iLen2 = strlen(&sDays) - 2;
	
	string sChar1 = GetSymbol(&sDays, iLen1);
	string sChar2 = GetSymbol(&sDays, iLen2);

	if(LanguageGetLanguage() != "Russian")
	{
		if(idays == 1 || idays == 0)
		{
			sDays = XI_ConvertString("day2");
		}
		else
		{
			sDays = XI_ConvertString("day1");
		}
	}
	else
	{
		if(idays < 10)
		{
			switch (sChar1)
			{
				case "0":
					sDays = XI_ConvertString("day1");
				break;
				case "1":
					sDays = XI_ConvertString("day2");
				break;
				case "2":
					sDays = XI_ConvertString("day3");
				break;
				case "3":
					sDays = XI_ConvertString("day3");
				break;
				case "4":
					sDays = XI_ConvertString("day3");
				break;
				case "5":
					sDays = XI_ConvertString("day1");
				break;
				case "6":
					sDays = XI_ConvertString("day1");
				break;
				case "7":
					sDays = XI_ConvertString("day1");
				break;
				case "8":
					sDays = XI_ConvertString("day1");
				break;
				case "9":
					sDays = XI_ConvertString("day1");
				break;
			}
		}
		else
		{
			if(sChar2 == "1")
			{
				switch (sChar1)
				{
					case "0":
						sDays = XI_ConvertString("day1");
					break;
					case "1":
						sDays = XI_ConvertString("day1");
					break;
					case "2":
						sDays = XI_ConvertString("day1");
					break;
					case "3":
						sDays = XI_ConvertString("day1");
					break;
					case "4":
						sDays = XI_ConvertString("day1");
					break;
					case "5":
						sDays = XI_ConvertString("day1");
					break;
					case "6":
						sDays = XI_ConvertString("day1");
					break;
					case "7":
						sDays = XI_ConvertString("day1");
					break;
					case "8":
						sDays = XI_ConvertString("day1");
					break;
					case "9":
						sDays = XI_ConvertString("day1");
					break;
				}
			}
			else
			{
				switch (sChar1)
				{
					case "0":
						sDays = XI_ConvertString("day1");
					break;
					case "1":
						sDays = XI_ConvertString("day2");
					break;
					case "2":
						sDays = XI_ConvertString("day3");
					break;
					case "3":
						sDays = XI_ConvertString("day3");
					break;
					case "4":
						sDays = XI_ConvertString("day3");
					break;
					case "5":
						sDays = XI_ConvertString("day1");
					break;
					case "6":
						sDays = XI_ConvertString("day1");
					break;
					case "7":
						sDays = XI_ConvertString("day1");
					break;
					case "8":
						sDays = XI_ConvertString("day1");
					break;
					case "9":
						sDays = XI_ConvertString("day1");
					break;
				}
			}
		}
	}
	sDays = idays + " " + sDays;
	return sDays;
}

string FindRussianMoneyString(int imoney)
{
	string sMoney = imoney;
	int iLen1 = strlen(&sMoney) - 1;
	int iLen2 = strlen(&sMoney) - 2;
	
	string sChar1 = GetSymbol(&sMoney, iLen1);
	string sChar2 = GetSymbol(&sMoney, iLen2);
	
	if(LanguageGetLanguage() != "Russian")
	{
		if(imoney == 1 || imoney == 0)
		{
			sMoney = XI_ConvertString("gold2");
		}
		else
		{
			sMoney = XI_ConvertString("gold1");
		}
	}
	else
	{
		if(imoney < 10)
		{
			switch (sChar1)
			{
				case "0":
					sMoney = XI_ConvertString("gold1");
				break;
				case "1":
					sMoney = XI_ConvertString("gold2");
				break;
				case "2":
					sMoney = XI_ConvertString("gold3");
				break;
				case "3":
					sMoney = XI_ConvertString("gold3");
				break;
				case "4":
					sMoney = XI_ConvertString("gold3");
				break;
				case "5":
					sMoney = XI_ConvertString("gold1");
				break;
				case "6":
					sMoney = XI_ConvertString("gold1");
				break;
				case "7":
					sMoney = XI_ConvertString("gold1");
				break;
				case "8":
					sMoney = XI_ConvertString("gold1");
				break;
				case "9":
					sMoney = XI_ConvertString("gold1");
				break;
			}
		}
		else
		{
			if(sChar2 == "1")
			{
				switch (sChar1)
				{
					case "0":
						sMoney = XI_ConvertString("gold1");
					break;
					case "1":
						sMoney = XI_ConvertString("gold1");
					break;
					case "2":
						sMoney = XI_ConvertString("gold1");
					break;
					case "3":
						sMoney = XI_ConvertString("gold1");
					break;
					case "4":
						sMoney = XI_ConvertString("gold1");
					break;
					case "5":
						sMoney = XI_ConvertString("gold1");
					break;
					case "6":
						sMoney = XI_ConvertString("gold1");
					break;
					case "7":
						sMoney = XI_ConvertString("gold1");
					break;
					case "8":
						sMoney = XI_ConvertString("gold1");
					break;
					case "9":
						sMoney = XI_ConvertString("gold1");
					break;
				}
			}
			else
			{
				switch (sChar1)
				{
					case "0":
						sMoney = XI_ConvertString("gold1");
					break;
					case "1":
						sMoney = XI_ConvertString("gold2");
					break;
					case "2":
						sMoney = XI_ConvertString("gold3");
					break;
					case "3":
						sMoney = XI_ConvertString("gold3");
					break;
					case "4":
						sMoney = XI_ConvertString("gold3");
					break;
					case "5":
						sMoney = XI_ConvertString("gold1");
					break;
					case "6":
						sMoney = XI_ConvertString("gold1");
					break;
					case "7":
						sMoney = XI_ConvertString("gold1");
					break;
					case "8":
						sMoney = XI_ConvertString("gold1");
					break;
					case "9":
						sMoney = XI_ConvertString("gold1");
					break;
				}
			}
		}
	}
	sMoney = MakeMoneyShow(imoney, MONEY_SIGN,MONEY_DELIVER) + " " + sMoney;
	return sMoney;
}
// eddy. такой код наверное лучше, чем выше на три экрана :)
string FindRussianQtyString(int _iQty)
{
    string sTemp;
    if (_iQty <= 20)
    {
        sTemp = XI_ConvertString("qty3");
        if (_iQty == 1) sTemp = XI_ConvertString("qty1");
        if (_iQty > 1 && _iQty < 5) sTemp = XI_ConvertString("qty2");
    }
    else
    {
        sTemp = _iQty;
        sTemp = GetSymbol(sTemp, strlen(sTemp)-1);
        int iTemp = sti(sTemp);
        sTemp = XI_ConvertString("qty3");
        if (iTemp == 1) sTemp = XI_ConvertString("qty1");
        if (iTemp > 1 && _iQty < 5) sTemp = XI_ConvertString("qty2");
    }
    sTemp = _iQty + " " + sTemp;
    return sTemp;
}
//eddy. мес€цы
string FindRussianMonthString(int _Month)
{
    string sTemp;
    if (_Month <= 20)
    {
        sTemp = XI_ConvertString("month3");
        if (_Month == 1) sTemp = XI_ConvertString("month1");
        if (_Month > 1 && _Month < 5) sTemp = XI_ConvertString("month2");
    }
    else
    {
        sTemp = _Month;
        sTemp = GetSymbol(sTemp, strlen(sTemp)-1);
        int iTemp = sti(sTemp);
        sTemp = XI_ConvertString("month3");
        if (iTemp == 1) sTemp = XI_ConvertString("month1");
        if (iTemp > 1 && _Month < 5) sTemp = XI_ConvertString("month2");
    }
    sTemp = _Month + " " + sTemp;
    return sTemp;
}

string GetTextOnShipsQuantity(int iShips)
{
	string sShips = iShips;
	int iLen1 = strlen(&sShips) - 1;
	int iLen2 = strlen(&sShips) - 2;
	
	string sChar1 = GetSymbol(&sShips, iLen1);
	string sChar2 = GetSymbol(&sShips, iLen2);

	if(LanguageGetLanguage() != "Russian")
	{
		if(iShips == 1 || iShips == 0)
		{
			sShips = XI_ConvertString("ship2");
		}
		else
		{
			sShips = XI_ConvertString("ships");
		}
	}
	else
	{
		if(iShips < 10)
		{
			switch (sChar1)
			{
				case "0":
					sShips = XI_ConvertString("ship1");
				break;
				case "1":
					sShips = XI_ConvertString("ship2");
				break;
				case "2":
					sShips = XI_ConvertString("ship3");
				break;
				case "3":
					sShips = XI_ConvertString("ship3");
				break;
				case "4":
					sShips = XI_ConvertString("ship3");
				break;
				case "5":
					sShips = XI_ConvertString("ship1");
				break;
				case "6":
					sShips = XI_ConvertString("ship1");
				break;
				case "7":
					sShips = XI_ConvertString("ship1");
				break;
				case "8":
					sShips = XI_ConvertString("ship1");
				break;
				case "9":
					sShips = XI_ConvertString("ship1");
				break;
			}
		}
		else
		{
			if(sChar2 == "1")
			{
				switch (sChar1)
				{
					case "0":
						sShips = XI_ConvertString("ship1");
					break;
					case "1":
						sShips = XI_ConvertString("ship1");
					break;
					case "2":
						sShips = XI_ConvertString("ship1");
					break;
					case "3":
						sShips = XI_ConvertString("ship1");
					break;
					case "4":
						sShips = XI_ConvertString("ship1");
					break;
					case "5":
						sShips = XI_ConvertString("ship1");
					break;
					case "6":
						sShips = XI_ConvertString("ship1");
					break;
					case "7":
						sShips = XI_ConvertString("ship1");
					break;
					case "8":
						sShips = XI_ConvertString("ship1");
					break;
					case "9":
						sShips = XI_ConvertString("ship1");
					break;
				}
			}
			else
			{
				switch (sChar1)
				{
					case "0":
						sShips = XI_ConvertString("ship1");
					break;
					case "1":
						sShips = XI_ConvertString("ship2");
					break;
					case "2":
						sShips = XI_ConvertString("ship3");
					break;
					case "3":
						sShips = XI_ConvertString("ship3");
					break;
					case "4":
						sShips = XI_ConvertString("ship3");
					break;
					case "5":
						sShips = XI_ConvertString("ship1");
					break;
					case "6":
						sShips = XI_ConvertString("ship1");
					break;
					case "7":
						sShips = XI_ConvertString("ship1");
					break;
					case "8":
						sShips = XI_ConvertString("ship1");
					break;
					case "9":
						sShips = XI_ConvertString("ship1");
					break;
				}
			}
		}
	}
	sShips = iShips + " " + sShips;
	return sShips;
}


string GetTextOnSecondShipsQuantity(int iShips)
{
 string sShips = iShips;
	int iLen1 = strlen(&sShips) - 1;
	int iLen2 = strlen(&sShips) - 2;
	
	string sChar1 = GetSymbol(&sShips, iLen1);
	string sChar2 = GetSymbol(&sShips, iLen2);

	if(LanguageGetLanguage() != "Russian")
	{
		if(iShips == 1 || iShips == 0)
		{
			sShips = XI_ConvertString("ship2");
		}
		else
		{
			sShips = XI_ConvertString("ships");
		}
	}
	else
	{
		if(iShips < 10)
		{
			switch (sChar1)
			{
				case "0":
					sShips = XI_ConvertString("ship1");
				break;
				case "1":
					sShips = XI_ConvertString("ship3");
				break;
				case "2":
					sShips = XI_ConvertString("ship1");
				break;
				case "3":
					sShips = XI_ConvertString("ship1");
				break;
				case "4":
					sShips = XI_ConvertString("ship1");
				break;
				case "5":
					sShips = XI_ConvertString("ship1");
				break;
				case "6":
					sShips = XI_ConvertString("ship1");
				break;
				case "7":
					sShips = XI_ConvertString("ship1");
				break;
				case "8":
					sShips = XI_ConvertString("ship1");
				break;
				case "9":
					sShips = XI_ConvertString("ship1");
				break;
			}
		}
		else
		{
			if(sChar2 == "1")
			{
				switch (sChar1)
				{
					case "0":
						sShips = XI_ConvertString("ship1");
					break;
					case "1":
						sShips = XI_ConvertString("ship1");
					break;
					case "2":
						sShips = XI_ConvertString("ship1");
					break;
					case "3":
						sShips = XI_ConvertString("ship1");
					break;
					case "4":
						sShips = XI_ConvertString("ship1");
					break;
					case "5":
						sShips = XI_ConvertString("ship1");
					break;
					case "6":
						sShips = XI_ConvertString("ship1");
					break;
					case "7":
						sShips = XI_ConvertString("ship1");
					break;
					case "8":
						sShips = XI_ConvertString("ship1");
					break;
					case "9":
						sShips = XI_ConvertString("ship1");
					break;
				}
			}
			else
			{
				switch (sChar1)
				{
					case "0":
						sShips = XI_ConvertString("ship1");
					break;
					case "1":
						sShips = XI_ConvertString("ship3");
					break;
					case "2":
						sShips = XI_ConvertString("ship1");
					break;
					case "3":
						sShips = XI_ConvertString("ship1");
					break;
					case "4":
						sShips = XI_ConvertString("ship1");
					break;
					case "5":
						sShips = XI_ConvertString("ship1");
					break;
					case "6":
						sShips = XI_ConvertString("ship1");
					break;
					case "7":
						sShips = XI_ConvertString("ship1");
					break;
					case "8":
						sShips = XI_ConvertString("ship1");
					break;
					case "9":
						sShips = XI_ConvertString("ship1");
					break;
				}
			}
		}
	}
	sShips = iShips + " " + sShips;
	return sShips;
}


string FindRussianPeoplesString(int iPeoples, string sCase)
{
	string sPeoples = iPeoples;
	string sPeoplesAcc;
	int iLen1 = strlen(&sPeoples) - 1;
	int iLen2 = strlen(&sPeoples) - 2;
	
	string sChar1 = GetSymbol(&sPeoples, iLen1);
	string sChar2 = GetSymbol(&sPeoples, iLen2);
	if(LanguageGetLanguage() != "Russian")
	{
		if(iPeoples == 1 || iPeoples == 0)
		{
			sPeoples = XI_ConvertString("people1");
			sPeoplesAcc = XI_ConvertString("people1");
		}
		else
		{
			sPeoples = XI_ConvertString("people2");
			sPeoplesAcc = XI_ConvertString("people2");
		}
	}
	else
	{
		if(iPeoples < 10)
		{
			switch (sChar1)
			{
				case "0":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "1":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people2");
				break;
				case "2":
					sPeoples = XI_ConvertString("people2");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "3":
					sPeoples = XI_ConvertString("people2");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "4":
					sPeoples = XI_ConvertString("people2");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "5":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "6":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "7":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "8":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
				case "9":
					sPeoples = XI_ConvertString("people1");
					sPeoplesAcc = XI_ConvertString("people1");
				break;
			}
		}
		else
		{
			if(sChar2 == "1")
			{
				switch (sChar1)
				{
					case "0":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "1":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "2":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "3":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "4":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "5":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "6":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "7":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "8":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "9":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
				}
			}
			else
			{
				switch (sChar1)
				{
					case "0":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "1":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people2");
					break;
					case "2":
						sPeoples = XI_ConvertString("people2");
						sPeoplesAcc = XI_ConvertString("people2");
					break;
					case "3":
						sPeoples = XI_ConvertString("people2");
						sPeoplesAcc = XI_ConvertString("people2");
					break;
					case "4":
						sPeoples = XI_ConvertString("people2");
						sPeoplesAcc = XI_ConvertString("people2");
					break;
					case "5":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "6":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "7":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "8":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
					case "9":
						sPeoples = XI_ConvertString("people1");
						sPeoplesAcc = XI_ConvertString("people1");
					break;
				}
			}
		}
	}

	switch(sCase)
	{
		case "No":
			sPeoples = iPeoples + " " + sPeoples;
		break;

		case "Acc":
			sPeoples = iPeoples + " " + sPeoplesAcc;
		break;
	}
	return sPeoples;
}


string FindRussianSlavesString(int iPeoples, string sCase)
{
	string sPeoples = iPeoples;
	string sPeoplesAcc;
	int iLen1 = strlen(&sPeoples) - 1;
	int iLen2 = strlen(&sPeoples) - 2;
	
	string sChar1 = GetSymbol(&sPeoples, iLen1);
	string sChar2 = GetSymbol(&sPeoples, iLen2);
	if(sChar2 == "1")
	{
		switch (sChar1)
		{
			case "0":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "1":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "2":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "3":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "4":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "5":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "6":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "7":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "8":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "9":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
		}
	}
	else
	{
		switch (sChar1)
		{
			case "0":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "1":
				sPeoples = XI_ConvertString("slaves3");
				sPeoplesAcc = XI_ConvertString("slaves2");
			break;
			case "2":
				sPeoples = XI_ConvertString("slaves2");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "3":
				sPeoples = XI_ConvertString("slaves2");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "4":
				sPeoples = XI_ConvertString("slaves2");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "5":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "6":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "7":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "8":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "9":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
		}
	}

	switch(sCase)
	{
		case "No":
			sPeoples = iPeoples + " " + sPeoples;
		break;

		case "Acc":
			sPeoples = iPeoples + " " + sPeoplesAcc;
		break;
	}
	return sPeoples;
}


string FindRussianSailorString(int iPeoples, string sCase)
{
	string sPeoples = iPeoples;
	string sPeoplesAcc;
	int iLen1 = strlen(&sPeoples) - 1;
	int iLen2 = strlen(&sPeoples) - 2;

	string sChar1 = GetSymbol(&sPeoples, iLen1);
	string sChar2 = GetSymbol(&sPeoples, iLen2);
	if(sChar2 == "1")
	{
        sPeoples = XI_ConvertString("sailor1");
		sPeoplesAcc = XI_ConvertString("sailor1");
	}
	else
	{
		switch (sChar1)
		{
			case "0":
				sPeoples = XI_ConvertString("sailor1");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "1":
				sPeoples = XI_ConvertString("sailor3");
				sPeoplesAcc = XI_ConvertString("sailor2");
			break;
			case "2":
				sPeoples = XI_ConvertString("sailor2");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "3":
				sPeoples = XI_ConvertString("sailor2");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "4":
				sPeoples = XI_ConvertString("sailor2");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "5":
				sPeoples = XI_ConvertString("sailor1");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "6":
				sPeoples = XI_ConvertString("sailor1");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "7":
				sPeoples = XI_ConvertString("sailor1");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
			case "8":
				sPeoples = XI_ConvertString("slaves1");
				sPeoplesAcc = XI_ConvertString("slaves1");
			break;
			case "9":
				sPeoples = XI_ConvertString("sailor1");
				sPeoplesAcc = XI_ConvertString("sailor1");
			break;
		}
	}

	switch(sCase)
	{
		case "No":
			sPeoples = iPeoples + " " + sPeoples;
		break;

		case "Acc":
			sPeoples = iPeoples + " " + sPeoplesAcc;
		break;
	}
	return sPeoples;
}