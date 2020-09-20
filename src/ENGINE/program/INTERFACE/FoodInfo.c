string totalInfo = "";

void InitInterface(string iniName)
{
	SetTimeScale(0.0);
	EngineLayersOffOn(true);
	GameInterface.title = "titleFoodInfo";
	
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
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_FOOD_INFO_EXIT );
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
	int chrIndex = -1;
	int iCrewQuantity = 0;
	int foodNeeded = 0;
	int iFoodQuantity = 0;
	
	int iSlavesQuantity = 0;
	
	for (int i=0; i<COMPANION_MAX; i++)
	{
		chrIndex = GetCompanionIndex(pchar, i);
		if (chrIndex != -1)
		{
			iCrewQuantity = iCrewQuantity + characters[chrIndex].ship.crew.quantity;
			iFoodQuantity = iFoodQuantity + GetCargoGoods(&characters[chrIndex], GOOD_FOOD);
		}
	}
	foodNeeded = iCrewQuantity/FOOD_BY_CREW;
	int days = 0;
	if(foodNeeded > 0)
	{
		days = iFoodQuantity / foodNeeded;
	}
	if (days > 0)
	{
		totalinfo = XI_ConvertString("HungerAlarm") + days + ".";
	}
	else
	{
		totalinfo = XI_ConvertString("FoodAlarm");
		for (i=0; i<COMPANION_MAX; i++)
		{
			chrIndex = GetCompanionIndex(pchar, i);
			if (chrIndex != -1)
			{
				iCrewQuantity = characters[chrIndex].ship.crew.quantity;
				characters[chrIndex].ship.crew.quantity = iCrewQuantity - (iCrewQuantity/50);	
				
				iSlavesQuantity = GetCargoGoods(&characters[chrIndex], GOOD_SLAVES);
				iSlavesQuantity = iSlavesQuantity - (iSlavesQuantity)/10;
				SetCharacterGoods(&characters[chrIndex], GOOD_SLAVES, iSlavesQuantity);
			}
		}
	}
	SetCurrentNode("B_OK");
}
