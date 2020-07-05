void InitInterface(string iniName)
{
	GameInterface.title = "";

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	SetEventHandler("OnEndGameOk", "ICancelProcedure", 0);
	SetEventHandler("exitCancel", "ICancelProcedure", 0);

	FreezeGroupControls("BattleInterfaceControls", true);

	SetFormatedText("TEXT_ENDGAME", oGameOver.Str);
}

void IDoExit(int exitCode)
{
	DelEventHandler("OnEndGameOk", "ICancelProcedure");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ICancelProcedure()
{
	Net_Stop();

	IDoExit( RC_INTERFACE_NET_FINDGAME );
}
