// Заставка А в это время... BOAL 15/01/2006
bool bNeedResetSound;
void InitInterface(string iniName)
{
	bQuestCheckProcessFreeze = true;
    GameInterface.title = "";

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

    bNeedResetSound = false;
	if (CheckAttribute(pchar, "AutoQuest.FramePic")) // признак картинки
	{
	    SetNewPicture("SCROLLPICT", pchar.AutoQuest.FramePic);
	    DeleteAttribute(pchar, "AutoQuest.FramePic");
	    bNeedResetSound = true;
	}

	SetFormatedText("INFO_TEXT", pchar.AutoQuest.FrameText);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0); // Выход на море по крестику или Esc
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	
	SetEventHandler("My_EndView","EndView",0);
	PostEvent("My_EndView", makeint(stf(pchar.AutoQuest.FrameAutoEnd)*1000));
}
void EndView()
{
    IDoExit(RC_INTERFACE_SALARY_EXIT);
}
void ProcessBreakExit()
{
	IDoExit(RC_INTERFACE_SALARY_EXIT);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_SALARY_EXIT);
}

void IDoExit(int exitCode)
{
	if (bNeedResetSound)
	{
		ResetSound();
	    LoadSceneSound();
    }
	bQuestCheckProcessFreeze = false;
	if (pchar.AutoQuest.FrameQuest != "")
	{
		if (pchar.AutoQuest.FrameQuest == "Reload_To_Location")
		{
		    DoQuestReloadToLocation(PChar.AutoQuest.FrameLocation, PChar.AutoQuest.FrameGroup, PChar.AutoQuest.FrameLocator, PChar.AutoQuest.FrameLocationQuest);
		}
		else
		{
		    if (pchar.AutoQuest.FrameQuest == "Run_Function")
		    {
		        PChar.GenQuest.CallFunctionParam = pchar.AutoQuest.FrameFunction;
				DoQuestCheckDelay("CallFunctionParam", stf(pchar.AutoQuest.FrameFunctionDelay));
		    }
		    else
		    {
		        DoQuestCheckDelay(pchar.AutoQuest.FrameQuest, 0.6);
		    }
		}
	}
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("My_EndView","EndView");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
	//PostEvent("StopQuestCheckProcessFreeze", 50);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();
    
	switch(nodName)
	{
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}