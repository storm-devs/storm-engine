bool bMessageBoxActive = false;
string sMBOkFunc, sMBCancelFunc, sMBRetry;

void CreateMessageBox(string sCaptionID, string sMessageID, string sIconID, string sOkFunc, string sCancelFunc, string sRetryFunc)
{
	string sIni = "resource\ini\interfaces\Utils\MessageBox.ini";
	
	XI_MakeNode(sIni, "RECTANGLE", "MB_MAIN_BACK", 60000);
	XI_MakeNode(sIni, "FORMATEDTEXT", "MB_MESSAGE", 60003);

	XI_MakeNode(sIni, "TEXTBUTTON2", "MB_BTN_OK", 60002);
	XI_MakeNode(sIni, "TEXTBUTTON2", "MB_BTN_RETRY", 60002);
	XI_MakeNode(sIni, "TEXTBUTTON2", "MB_BTN_CANCEL", 60002);

	XI_MakeNode(sIni, "IMAGECOLLECTION", "MB_IMAGECOLLECTION", 60001);
	EI_CreateFrame("MB_IMAGECOLLECTION", 200, 250, 600, 550);

	SetEventHandler("MB_BTN_Ok", "MB_BTN_Ok", 0);
	SetEventHandler("MB_BTN_Retry", "MB_BTN_Retry", 0);
	SetEventHandler("MB_BTN_Cancel", "MB_BTN_Cancel", 0);

	bMessageBoxActive = true;
}

void KillMessageBox()
{
	if (!bMessageBoxActive) { return; }
	bMessageBoxActive = false;

	DelEventHandler("MB_BTN_Ok", "MB_BTN_Ok");
	DelEventHandler("MB_BTN_Retry", "MB_BTN_Retry");
	DelEventHandler("MB_BTN_Cancel", "MB_BTN_Cancel");
}

void MB_BTN_Ok()
{
	KillMessageBox();
	if (strlen(sMBOkFunc)) { call sMBOkFunc(); }
}

void MB_BTN_Cancel()
{
	KillMessageBox();
	if (strlen(sMBCancelFunc)) { call sMBCancelFunc(); }
}

void MB_BTN_Retry()
{
	KillMessageBox();
	if (strlen(sMBRetry)) { call sMBRetry(); }
}