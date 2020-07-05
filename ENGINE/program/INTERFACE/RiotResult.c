string totalInfo = "";

void InitInterface_RI(string iniName, aref chr, int isVictory)
{
	SetTimeScale(0.0);
	EngineLayersOffOn(true);
	GameInterface.title = "titleFoodInfo";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	CalculateInfoData(chr, isVictory);
	
	SetFormatedText("INFO_TEXT",totalInfo);
	
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);
	
	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); 
	SetEventHandler("exitCancel","ProcessCancelExit",0); 
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0); 
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_RIOT_RESULT_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);;
	DeleteAttribute(pchar, "riot");
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_RIOT_RESULT_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
	DeleteAttribute(pchar, "riot");
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
	DeleteAttribute(pchar, "riot");
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
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_RIOT_RESULT_EXIT);
			SetTimeScale(1.0);;
			LaunchRiotBattle();
		}
	break;
	case "B_OK_DECK":
		if(comName=="activate" || comName=="click")
		{
			// напасть
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_RIOT_RESULT_EXIT);
			SetTimeScale(1.0);
			LaunchRiotOnShip();
		}
	break;
	case "B_CANCEL":
		if(comName=="activate" || comName=="click")
		{
			// напасть
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_RIOT_RESULT_EXIT);
			SetTimeScale(1.0);
			//LaunchRiotBattle();
		}
	break;
	}
	DeleteAttribute(pchar, "riot");
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData(aref chr, int isVictory)
{
	if(CheckAttribute(pchar, "RiotOnCompanionShip"))
	{		
		if(isVictory == VICTORY)
		{
			totalinfo = XI_ConvertString("ShipWithCaptain") + GetCharacterFullName(chr.id) + XI_ConvertString("ButMutinyWasDrownedInBlood");
		}
		else
		{
			totalinfo = XI_ConvertString("ShipWithCaptain") + GetCharacterFullName(chr.id) + XI_ConvertString("AndMutinyHasSuccesed");
		}
		
		SetNodeUsing("B_OK", true);
		SetNodeUsing("B_CANCEL", true);
		SetNodeUsing("B_OK_DECK", false);
		SetCurrentNode("B_OK");
		DeleteAttribute(pchar, "RiotOnCompanionShip");
	}
	else
	{
		totalinfo = XI_ConvertString("CrewRiot");
		SetNodeUsing("B_OK_DECK", true);
		SetNodeUsing("B_OK", false);
		SetNodeUsing("B_CANCEL", false);
		SetCurrentNode("B_OK_DECK");
	}
}
