int nPaymentQ;
int nMoraleDecreaseQ;

void InitInterface(string iniName)
{
    // лочим квест и карту
    bQuestCheckProcessFreeze = true;
    if(IsEntity(worldMap))
    {
    	wdmLockReload            = true;
    }
    
 	StartAboveForm(true);

	GameInterface.title = "titleSalary";

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);


	int tmpLangFileID = LanguageOpenFile("interface_strings.txt");
	SetFormatedText("INFO_TEXT1",LanguageConvertString(tmpLangFileID,"Salary info"));
	//SetFormatedText("INFO_TEXT2",LanguageConvertString(tmpLangFileID,"Officer info"));
	SetFormatedText("INFO_TEXT3",LanguageConvertString(tmpLangFileID,"All info"));
	LanguageCloseFile(tmpLangFileID);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

    CalculateInfoData();
    
	SetEventHandler("InterfaceBreak","ProcessBreakExit",0)
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	
	EI_CreateFrame("INFO_BORDERS", 250,152,550,342);
}

void ProcessBreakExit()
{
	SkipSailorPayment();
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void ProcessCancelExit()
{
	SkipSailorPayment();
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void IDoExit(int exitCode)
{
 	EndAboveForm(true);
 	
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
	PostEvent("StopQuestCheckProcessFreeze", 1000);//boal 230804 заморозка проверки квестов
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	case "B_OK":
		if(comName=="activate" || comName=="click")
		{
			ExecuteSailorPayment();
   			IDoExit(RC_INTERFACE_SALARY_EXIT);
		}
	break;

	case "B_CANCEL":
		if(comName=="activate" || comName=="click")
		{
			SkipSailorPayment();
			IDoExit(RC_INTERFACE_SALARY_EXIT);
		}
		if(comName=="upstep")
		{
			if(GetSelectable("B_OK"))	{SetCurrentNode("B_OK");}
		}
	break;
	}
}

void ExecuteSailorPayment()
{
	AddMoneyToCharacter(GetMainCharacter(),-nPaymentQ);
	DeleteAttribute(GetMainCharacter(),"CrewPayment");
}

void SkipSailorPayment()
{
	ref mchref = GetMainCharacter();

	mchref.CrewPayment = nPaymentQ;

	int cn;
	for(int i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mchref,i);
		if(cn>=0)
		{
			AddCrewMorale(&Characters[cn], -nMoraleDecreaseQ);
		}
	}

	if(AddCrewMorale(mchref, 0)<=I_MIN_MORALE)
    {
		if (IsEntity(worldMap) && GetCrewQuantity(mchref) > 0)
    	{
            MunityOnShip("ShipMunity");
        }
		else
        {
            mchref.quest.Munity_on_Ship_Map.win_condition.l1 = "MapEnter";
            mchref.quest.Munity_on_Ship_Map.win_condition    = "Munity_on_Ship_Map";
        }
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	int i,cn;
	ref mchref,chref;

	mchref = GetMainCharacter();
	// boal новый учет зп -->
	float nLeaderShip = GetSummonSkillFromNameToOld(mchref,SKILL_LEADERSHIP);
	// перенес расчет в CharacterUtilite int nCommerce   = GetSummonSkillFromName(mchref,SKILL_COMMERCE);

	nPaymentQ = 0;
	for(i=0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mchref,i);
		if( cn>=0 )//&& GetRemovable(&Characters[cn]) )
		{
			chref = GetCharacter(cn);
			if (GetRemovable(chref)) // считаем только своих, а то вских сопровождаемых кормить!!!
			{
			    // для каждого корабля учитываем класс и считаем отдельно
			    nPaymentQ += GetSalaryForShip(chref);
			}
		}
	}

	if( CheckAttribute(mchref,"CrewPayment") )
    {
		nPaymentQ += makeint(mchref.CrewPayment); // а тут помним все до копейки!
	}
    // boal новый учет зп <--
	nMoraleDecreaseQ = 40-nLeaderShip;
	if( CheckOfficersPerk(mchref,"IronWill") ) nMoraleDecreaseQ /= 2; // у офицеров тоже нужно смотреть!!!

	//CreateString(true,"payment",""+nPaymentQ,FONT_NORMAL,COLOR_NORMAL,320,390,SCRIPT_ALIGN_CENTER,1.0);
    SetFormatedText("INFO_TEXT3",MakeMoneyShow(nPaymentQ, MONEY_SIGN,MONEY_DELIVER));
	if( sti(mchref.Money) < nPaymentQ )
	{
		SetSelectable("B_OK",false);
		SetCurrentNode("B_CANCEL");
	}
}