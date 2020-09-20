
void InitInterface_gm(string iniName)
{
	GameInterface.title = "titleGameMenu";

	SetTimeScale(0.0);
	locCameraSleep(true);

	EngineLayersOffOn(true);

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("frame","IProcessFrame",0);

	SetEventHandler("NewClick","NewClick",0);
	SetEventHandler("LoadClick","LoadClick",0);
	SetEventHandler("SaveClick","SaveClick",0);
	SetEventHandler("ResumeClick","ResumeClick",0);
	SetEventHandler("OptionsClick","OptionsClick",0);
	SetEventHandler("QuitClick","QuitClick",0);
	SetEventHandler("QuitProcess","QuitProcess",0);

	SetEventHandler("ConfirmExitClick","ConfirmExitClick",0);
	SetEventHandler("ConfirmExitCancel","ConfirmExitCancel",0);

	InterfaceStates.showGameMenuOnExit = true;

	//EI_CreateFrame("MENU_BORDERS",257,117,542,438);

	if(CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") && sti(InterfaceStates.Buttons.Resume.enable) == true)
	{
		SetSelectable("MB_RESUME", true);
	} else {

		SetSelectable("MB_RESUME", false);
	}

	if (QuickSaveGameEnabledHardcore()) // boal 09.07.06
	{
		SetSelectable("MB_SAVE", true);
	} 
	else 
	{
		SetSelectable("MB_SAVE", false);
	}

	if (!CheckSaveGameEnabled())
	{
		SetSelectable("MB_SAVE", false);
	}

	SetFormatedText("VERSION_TEXT", VERSION_NUMBER1 + GetVerNum());

	string sText = "";
	//if(LanguageGetLanguage() == "Russian")
	//{
		sText = "Seaward.Ru, Akella ";
		//sText = sText + "";//"\n";
		sText = sText + "- 2007";
		SetFormatedText("COPYRIGHTS_TEXT", sText);
	/*}
	else
	{
		sText = "Playlogic 2007";
		sText = sText + "\n";
		sText = sText + "Akella 2007";
		SetFormatedText("COPYRIGHTS_TEXT", sText);
	}  */

	SetNewPicture("LOGO", "Interfaces\SL_logo.tga");

	if(LAi_IsBoardingProcess())	SetSelectable("MB_RESUME",false);

	//XI_RegistryExitKey("IExit_Escape");
	//XI_RegistryExitKey("IExit_F1");
}


void IProcessFrame()
{
}

void ProcessCancelExit()
{
	if(CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") && sti(InterfaceStates.Buttons.Resume.enable) == true)
	IDoExit(RC_INTERFACE_DO_RESUME_GAME, true);
}

void IDoExit(int exitCode, bool bClear)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("frame","IProcessFrame");

	DelEventHandler("NewClick","NewClick");
	DelEventHandler("LoadClick","LoadClick");
	DelEventHandler("SaveClick","SaveClick");
	DelEventHandler("ResumeClick","ResumeClick");
	DelEventHandler("OptionsClick","OptionsClick");
	DelEventHandler("QuitClick","QuitClick");
    DelEventHandler("QuitProcess","QuitProcess");
    
	DelEventHandler("ConfirmExitClick","ConfirmExitClick");
	DelEventHandler("ConfirmExitCancel","ConfirmExitCancel");

	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	DelPerkFromActiveList("TimeSpeed");
	locCameraSleep(false);
	DeleteAttribute(pchar, "pause");

	interfaceResultCommand = exitCode;

	if( interfaceResultCommand == RC_INTERFACE_DO_RESUME_GAME ) {
		DeleteEntitiesByType("scrshoter");
	}
	EndCancelInterface(bClear);
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "MB_NEW":

			if (comName == "click" || comName == "activate"){

				if(CheckAttribute(&InterfaceStates,"Buttons.Resume.enable") && sti(InterfaceStates.Buttons.Resume.enable) == true)
				{
					ShowConfirmWindow(true);
				} else {

					NewClick();
				}
			}

			break;

		case "CONFIRM_WINDOW_MB_YES":

			if (comName == "click" || comName == "activate"){

				//ShowConfirmWindow(false);
				NewClick();
			}

			if (comName == "deactivate"){

				ShowConfirmWindow(false);
			}

			break;

		case "CONFIRM_WINDOW_MB_NO":

			if (comName == "click" || comName == "activate"){
				ShowConfirmWindow(false);
			}

			if (comName == "deactivate"){

				ShowConfirmWindow(false);
			}

			break;
	}

}

void NewClick()
{
	InterfaceStates.showGameMenuOnExit = false;
	IDoExit( RC_INTERFACE_DO_NEW_GAME, true);
}

void LoadClick()
{
	IDoExit( RC_INTERFACE_DO_LOAD_GAME, true);
}

void SaveClick()
{
	IDoExit( RC_INTERFACE_DO_SAVE_GAME, true);
}

void ResumeClick()
{
	IDoExit(RC_INTERFACE_DO_RESUME_GAME, true);
}


void OptionsClick()
{
	IDoExit(RC_INTERFACE_DO_OPTIONS, true);
}

void QuitClick()
{
	XI_WindowDisable("MAIN_WINDOW",true);
	XI_WindowDisable("CONFIRM_EXIT_WINDOW",false);
	XI_WindowShow("CONFIRM_EXIT_WINDOW", true);
	SetCurrentNode("CONFIRM_EXIT_NO");
}

void ConfirmExitClick()
{
    PauseParticles(true); //fix вылета у форта
	EngineLayersOffOn(false);
	QuitProcess();
	//IDoExit(-1, false); // fix
	//ExitProgram();
}

void ConfirmExitCancel()
{
    XI_WindowDisable("CONFIRM_EXIT_WINDOW",true);
	XI_WindowShow("CONFIRM_EXIT_WINDOW",false);
	XI_WindowDisable("MAIN_WINDOW",false);
	SetCurrentNode("MB_EXITGAME");
}

void ShowConfirmWindow(bool show)
{
	if (show){

		SetCurrentNode("CONFIRM_WINDOW_MB_NO");

		XI_WindowDisable("MAIN_WINDOW", true);
		XI_WindowDisable("CONFIRM_WINDOW", false);
		XI_WindowShow("CONFIRM_WINDOW", true);
		EI_CreateFrame("CONFIRM_WINDOW_BORDER",190,190,610,360);

	} else {

		XI_WindowDisable("CONFIRM_WINDOW", true);
		XI_WindowShow("CONFIRM_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);

		if(GetSelectable("MB_RESUME")) 
			SetCurrentNode("MB_RESUME"); 
		else
			SetCurrentNode("MB_NEW");
	}
}

void QuitProcess()
{
    // вылетам у форта НЕТ -->
    if (bSeaActive && !bAbordageStarted)
    {
		SendMessage(&AIBalls, "l", MSG_MODEL_RELEASE);
	}
	// вылетам у форта НЕТ <--
	IDoExit(-1, false);
	ExitProgram();
}