string totalInfo = "";

void InitInterface(string iniName)
{
	
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
	GameInterface.title = "titleCareerMoney";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	CalculateInfoData();
	
	SetFormatedText("INFO_TEXT",totalInfo);
	
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);
	
	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); 
	SetEventHandler("exitCancel","ProcessCancelExit",0); 
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0); 
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_CAREER_MONEY_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_CAREER_MONEY_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
	SetTimeScale(1.0);
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
			// напасть
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_CAREER_MONEY_EXIT);
			SetTimeScale(1.0);
		}
	break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	string sCareer = GetNationNameByType(sti(pchar.nation));
	
	sCareer = sCareer + "_career_counter";
	
	string srank = pchar.(sCareer);
	int iMoney = sti(Career[sti(srank)].money);
	totalinfo = XI_ConvertString("You've got salary") + FindRussianMoneyString(iMoney) + ".";
	srank = srank + rand(1000000);
	pchar.quest.(srank).win_condition.l1 = "timer";
	pchar.quest.(srank).win_condition.l1.date.day = GetAddingDataDay(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.year = GetAddingDataYear(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.hour = rand(23);
	pchar.quest.(srank).win_condition = "career_money";
	
	AddMoneyToCharacter(pchar, iMoney);
	
	SetCurrentNode("B_OK");
}
