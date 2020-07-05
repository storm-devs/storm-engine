void InitInterface(string iniName)
{
	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	SetEventHandler("OnCancel", "ICancelProcedure", 0);
}

void IDoExit(int exitCode)
{
	DelEventHandler("OnCancel", "ICancelProcedure");
	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ICancelProcedure()
{
	IDoExit( RC_INTERFACE_NET_CHAT );
}