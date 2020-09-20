string totalInfo = "";

void InitInterface(string iniName)
{
	
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
	GameInterface.title = "titleAskForHelp";
	
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
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
	pchar.quest.Fight_forOtherColony_temp.help = "0";
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
	pchar.quest.Fight_forOtherColony_temp.help = "0";
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
	
	pchar.quest.Fight_forOtherColony_temp.help = "1";
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
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_FOOD_INFO_EXIT);
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
	
	
	int iHelp = sti(pchar.quest.Fight_forOtherColony_temp.help);

	int iEnemy = sti(pchar.quest.attack_other_fort.enemy);
	int iNation = sti(pchar.quest.attack_other_fort.nation);
	string sColony = pchar.quest.attack_other_fort.colony;
	
	if (iHelp == 2)
	{
		totalinfo = "Губернатор " + XI_ConvertString(GetNationNameByType(iEnemy) + "Gen") + " приносит Вам свою благодарность и в знак признательности передает вам " + (10000 + sti(pchar.skill.commerce)*500) + " золотых.";
		pchar.quest.Fight_forOtherColony_temp.help = 0;
	}
	else
	{
		totalinfo = "Губернатор " + XI_ConvertString(GetNationNameByType(iEnemy) + "Gen") + " просит Вас о помощи против сил " + XI_ConvertString(GetNationNameByType(iNation)+"Dat") + " напавших на подчиненую ему колонию.";
	}

	SetCurrentNode("B_OK");
}
