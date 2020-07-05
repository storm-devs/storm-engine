void InitInterface(string iniName)
{
	GameInterface.title = "";

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	SetEventHandler("OnCancel", "ICancelProcedure", 0);
	SetEventHandler("EditExit", "ChatS_Enter", 0);
	SetEventHandler("OnSend", "ChatS_OnSend", 0);

	// set string say mode
	SetFormatedText("CAPTION_STRING", XI_ConvertString("NetSay_Mode_" + NCSay.Mode));

	FreezeGroupControls("BattleInterfaceControls", true);
}

void IDoExit(int exitCode)
{
	DelEventHandler("OnCancel", "ICancelProcedure");
	DelEventHandler("EditExit", "ChatS_Enter");
	DelEventHandler("OnSend", "ChatS_OnSend");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ICancelProcedure()
{
	IDoExit( RC_INTERFACE_NET_CHATSTRING );
}

void ChatS_OnSend()
{
	string sMessage = GameInterface.EDITBOX_STRING.str;
	if (sMessage == "") 
	{
		ICancelProcedure();
		return;
	}

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_SAY);
	switch (NCSay.Mode)
	{
		case "Say":			NMAddByte(iSMsg, NSC_SAY_CHAT); break;
		case "Team_Say":	NMAddByte(iSMsg, NSC_SAY_TEAM); break;
		case "Person_Say":	NMAddByte(iSMsg, NSC_SAY_PERSON); break;
	}
	NMAddString(iSMsg, sMessage, 128);
	NetClient_SendMessage(iSMsg, false);
	NMDelete(iSMsg);

	ICancelProcedure();
}

void ChatS_Enter()
{
	string sEditControl = GetEventData();
	if (sEditControl != "EDITBOX_STRING") return;

	ChatS_OnSend();
}