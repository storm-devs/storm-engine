
void InitInterface_I(string iniName, int iMode)
{
	string totalInfo = "";
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
    GameInterface.title = "title_map";

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

    SetFormatedText("MAP_CAPTION", XI_ConvertString("PortAlert"));

	SetFormatedText("INFO_TEXT_QUESTION", XI_ConvertString("MapWhatYouWantToDo"));

	SetCurrentNode("B_OK");

	switch (iMode)
	{
		case DISEASE_ON_SHIP:
			totalinfo = XI_ConvertString("DiseaseShipAlert");
		break;
		case DISEASE_ON_COLONY:
			totalinfo = XI_ConvertString("DiseaseColonyAlert");
		break;
	}
	SetFormatedText("INFO_TEXT",totalInfo);

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); // Выход на море
	SetEventHandler("exitCancel","ProcessCancelExit",0); // Выход на море по крестику или Esc
	SetEventHandler("ievnt_command","ProcCommand",0); // выход на карту только тут (по НЕТ)
	SetEventHandler("evntDoPostExit","DoPostExit",0); // выход из интерфейса
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_MAP_EXIT );
	//wdmReloadToSea();
	SetTimeScale(1.0);
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_MAP_EXIT );
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
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_MAP_EXIT);
			//wdmReloadToSea();
			SetTimeScale(1.0);
		}
		if(comName=="downstep")
		{
			if(GetSelectable("B_CANCEL"))	{SetCurrentNode("B_CANCEL");}

		}
	break;
	case "B_CANCEL":
		if(comName=="activate" || comName=="click")
		{
			ProcessBreakExit()
			Sea_LandLoad();
		}
	break;
	}
}


void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}