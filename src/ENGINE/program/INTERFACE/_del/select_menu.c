bool exitBool = true;

void InitInterface(string iniName)
{
	GameInterface.title = "titleSelect";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	
	string sLocation = pchar.location;
	int iColonyQuantity = pchar.colony_quantity;
	
	if(IsLocationCaptured(sLocation) || iColonyQuantity > 0)
	{
		SetSelectable("I_COLONY",true);
	}
	else
	{
		SetSelectable("I_COLONY",false);
	}
	if (sti(pchar.nation) != PIRATE)
	{
		SetSelectable("I_COLONY",false);
	}
	
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_SELECTMENU_EXIT );
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("evntDoPostExit","DoPostExit");

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		exitBool = true;
	}
	if(exitBool)	DeleteAttribute(&InterfaceStack,"SelectMenu_node");
	EndCancelInterface(exitBool);
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	if(comName=="activate" || comName=="click")
	{
		PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_SELECTMENU_EXIT);
		PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
		InterfaceStack.SelectMenu_node = nodName;
	}
	
	switch (nodName)
	{
		case "I_ITEMS":
			if(comName == "downstep" && GetSelectable("I_COLONY"))
			{
				SetCurrentNode("I_COLONY");
			}
			else
			{
				SetCurrentNode("I_CHARACTER");
			}
		break;
			
		case "I_CHARACTER":
			if(comName == "upstep" && GetSelectable("I_COLONY"))
			{
				SetCurrentNode("I_COLONY");
			}
			else
			{
				SetCurrentNode("I_ITEMS");
			}
		break;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	exitBool = false;
	IDoExit(exitCode);
}
