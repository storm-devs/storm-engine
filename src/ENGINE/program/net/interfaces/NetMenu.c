void InitInterface(string iniName)
{
	GameInterface.title = "";

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	
	SetEventHandler("OnNetMenuDisconnect", "ICancelProcedure", 0);
	SetEventHandler("OnNetMenuReturn", "IReturnToGame", 0);
	SetEventHandler("exitCancel", "IReturnToGame", 0);

	FreezeGroupControls("BattleInterfaceControls", true);
}

void IDoExit(int exitCode)
{
	DelEventHandler("OnNetMenuDisconnect", "ICancelProcedure");
	DelEventHandler("OnNetMenuReturn", "IReturnToGame");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ICancelProcedure()
{
	Net_Stop();

	IDoExit( RC_INTERFACE_NET_FINDGAME );
}

void IReturnToGame()
{
	IDoExit( INTERFACE_NET_NETMENU );
}
