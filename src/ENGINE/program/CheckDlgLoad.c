object objChkDlg;

#event_handler("evntCheckDlg","CheckNextDialog");
#event_handler("evntCheckChrDlg","CheckNextChrDialog");

void StartCheckDialog()
{
	objChkDlg.dir = "PROGRAM\DIALOGS\" + LanguageGetLanguage() + "\";
	objChkDlg.mask = "*.h";
	CreateEntity(&objChkDlg,"FINDFILESINTODIRECTORY");
	DeleteClass(&objChkDlg);

	Log_SetStringToLog("=== START DIALOG CHECKING ===");
	trace("*** START DIALOG CHECKING ***");
	PostEvent("evntCheckDlg",0,"l",0);
}

void CheckNextDialog()
{
	int idx = GetEventData();

	aref arList;
	makearef(arList,objChkDlg.filelist);

	if( GetAttributesNum(arList) <= idx )
	{
		Log_SetStringToLog("=== END DIALOG CHECKING ===");
		trace("*** END DIALOG CHECKING ***");
		UnloadSegment("CheckDlgLoad.c");
		return;
	}

	aref arCur = GetAttributeN(arList,idx);
	string dlgName = GetAttributeValue(arCur);
	string dlgExt = "DIALOGS\" + LanguageGetLanguage() + "\" + dlgName;
	string dlgFile = "DIALOGS\" + strcut(dlgName,0,strlen(dlgName)-2) + "c";
	if( LoadSegment(dlgExt) )
	{
		if( LoadSegment(dlgFile) )
		{
			trace(dlgName + " ok");
			UnloadSegment(dlgFile);
			UnloadSegment(dlgExt);
		} else {
			UnloadSegment(dlgExt);
			trace(dlgName + " error!!!!!!!!!");
			Log_SetStringToLog("!!! ERROR dialog : " + dlgName);
		}
	}
	else
	{
		trace(dlgName + " error!!!!!!!!!");
		Log_SetStringToLog("!!! ERROR dialog : " + dlgName);
	}

	PostEvent("evntCheckDlg",0,"l",idx+1);
}

void StartCheckChrDialog()
{
	Log_SetStringToLog("=== START CHARACTERS DIALOG CHECKING ===");
	trace("*** START CHARACTERS DIALOG CHECKING ***");
	PostEvent("evntCheckChrDlg",0,"l",0);
}

void CheckNextChrDialog()
{
	int idx = GetEventData();

	if( idx >= MAX_CHARACTERS )
	{
		Log_SetStringToLog("=== END CHARACTERS DIALOG CHECKING ===");
		trace("*** END CHARACTERS DIALOG CHECKING ***");
		UnloadSegment("CheckDlgLoad.c");
		return;
	}

	string chrID = "?";
	if( CheckAttribute(&Characters[idx],"id") )	chrID = Characters[idx].id;

	string traceMsg = chrID + " (" + idx + ") ";
	bool bError = false;
	if( !CheckAttribute(&Characters[idx],"Dialog.Filename") )
	{
		traceMsg += "hav`t Dialog.FileName Attribute";
		bError = true;
	}
	else
	{
		string dlgName = Characters[idx].Dialog.Filename;
		string dlgFile = "DIALOGS\" + dlgName;
		string dlgExt = "DIALOGS\" + LanguageGetLanguage() + "\" + strcut(dlgName,0,strlen(dlgName)-2) + "h";
		if( LoadSegment(dlgExt) )
		{
			if( LoadSegment(dlgFile) )
			{
				traceMsg += " ok");
				UnloadSegment(dlgFile);
			}
			else
			{
				bError = true;
				if( CheckAttribute(&Characters[idx],"location") ) {
					traceMsg += " load <" + dlgName + "> location: <" + Characters[idx].location +"> error!!!";
				} else {
					traceMsg += " load <" + dlgName + "> error!!!";
				}
			}
			UnloadSegment(dlgExt);
		} else {
			bError = true;
			if( CheckAttribute(&Characters[idx],"location") ) {
				traceMsg += " load <" + dlgName + "> location: <" + Characters[idx].location +"> error!!!";
			} else {
				traceMsg += " load <" + dlgName + "> error!!!";
			}
		}
	}

	if( bError ) {
		trace(traceMsg);
		Log_SetStringToLog(traceMsg);
	}

	PostEvent("evntCheckChrDlg",0,"l",idx+1);
}
