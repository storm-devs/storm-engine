void ExternControlsInit(bool bFirst)
{
	//Trace("ExternControlsInit");
	if(bFirst)
	{
		ExternInitKeyCodes();
		CI_CreateAndSetControls( "", "ICancel", CI_GetKeyCode("VK_ESCAPE"), 0, false );
		CI_CreateAndSetControls( "", "IAction", CI_GetKeyCode("VK_SPACE"), 0, false );

		ControlsTreeInit();
		return;
	}

	ExternInitKeyCodes();

	//не успеваем доделать бумажную карту. аддонщики не подведите
	if (MOD_BETTATESTMODE == "On")
	{
		CI_CreateAndSetControls( "", "PaperMap", CI_GetKeyCode("KEY_M"), 0, true );
	}
	//
	
	// boal Не подведем :) -->
	CI_CreateAndSetControls( "", "BOAL_Control", CI_GetKeyCode("VK_F11"), 0, false );
    CI_CreateAndSetControls( "", "BOAL_Control2", CI_GetKeyCode("VK_F12"), 0, false );
    CI_CreateAndSetControls( "", "BOAL_SetCamera", CI_GetKeyCode("VK_F10"), 0, false );
    //CI_CreateAndSetControls( "", "BOAL_Control3", CI_GetKeyCode("VK_F9"), 0, false );
    CI_CreateAndSetControls( "", "BOAL_ControF5", CI_GetKeyCode("VK_F5"), 0, false );
    CI_CreateAndSetControls( "", "BOAL_ControF7", CI_GetKeyCode("VK_F7"), 0, false );
    CI_CreateAndSetControls( "", "BOAL_ControlDebug", CI_GetKeyCode("VK_INSERT"), 0, false );
    
    CI_CreateAndSetControls( "", "TimeScaleFaster", CI_GetKeyCode("VK_ADD"), 0, false );
    CI_CreateAndSetControls( "", "TimeScaleSlower", CI_GetKeyCode("VK_SUBTRACT"), 0, false );
	CI_CreateAndSetControls( "", "VK_PAUSETimePause", CI_GetKeyCode("VK_PAUSE"), 0, false );

	if (MOD_BETTATESTMODE == "On")
	{
		// Teleport
		CI_CreateAndSetControls( "", "TeleportActive", CI_GetKeyCode("VK_F4"), 0, false );
		CI_CreateAndSetControls( "", "TeleportUp", CI_GetKeyCode("VK_UP"), 0, false );
		CI_CreateAndSetControls( "", "TeleportDown", CI_GetKeyCode("VK_DOWN"), 0, false );
		CI_CreateAndSetControls( "", "TeleportSelect", CI_GetKeyCode("VK_RETURN"), 0, false );
		// my scale
		//CI_CreateAndSetControls( "", "TmpScaleOn", CI_GetKeyCode("KEY_R"), 0, false );
		//CI_CreateAndSetControls( "", "TmpScaleOff", CI_GetKeyCode("KEY_R"), INVERSE_CONTROL, false );

		// weather
		CI_CreateAndSetControls( "", "WhrPrevWeather", CI_GetKeyCode("VK_L_BREAKE"), 0, false );
		CI_CreateAndSetControls( "", "WhrNextWeather", CI_GetKeyCode("VK_R_BREAKE"), 0, false );
		CI_CreateAndSetControls( "", "WhrUpdateWeather", CI_GetKeyCode("VK_A_QUOTE"), 0, false );
		//CI_CreateAndSetControls( "", "Tele", CI_GetKeyCode("VK_A_POINT"), 0, false );
		
		CI_CreateAndSetControls( "", "CoastFoamLB", CI_GetKeyCode("VK_LBUTTON"), 0, true );
		CI_CreateAndSetControls( "", "CoastFoamRB", CI_GetKeyCode("VK_RBUTTON"), 0, true );
		CI_CreateAndSetControls( "", "CoastFoamINS", CI_GetKeyCode("VK_INSERT"), 0, true );
		CI_CreateAndSetControls( "", "CoastFoamDEL", CI_GetKeyCode("VK_DELETE"), 0, true );
		CI_CreateAndSetControls( "", "CoastFoamCopy", CI_GetKeyCode("KEY_C"), 0, true );
	}
	// boal <--

	//CI_CreateAndSetControls( "", "Dump", CI_GetKeyCode("KEY_0"), 0, false );

	CI_CreateAndSetControls( "", "TimeScale", CI_GetKeyCode("KEY_R"), 0, false );

	// Cameras =====================================================================
		// character camera
	CI_CreateAndSetControls( "", "ChrCamTurnV", 257, 0, false );
	SetControlForInverting("ChrCamTurnV",false);
	CI_CreateAndSetControls( "", "ChrCamTurnH", 256, 0, false );
	CI_CreateAndSetControls( "", "ChrCamSpecMode", CI_GetKeyCode("VK_CONTROL"), 0, false );
	CI_CreateAndSetControls( "", "Turn V", 257, INVERSE_CONTROL, false );
	SetControlForInverting("Turn V",true);
	CI_CreateAndSetControls( "", "Turn H", 256, 0, false );
	CI_CreateAndSetControls( "PrimaryLand", "ChrCamCameraSwitch", CI_GetKeyCode("VK_TAB"), 0, true );
		// ship follow camera
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Turn_V", 257, INVERSE_CONTROL, false );
	MapControlToGroup("ShipCamera_Turn_V", "BattleInterfaceControls");
	SetControlForInverting("ShipCamera_Turn_V",true);
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Turn_H", 256, 0, false );
	MapControlToGroup("ShipCamera_Turn_H", "BattleInterfaceControls");
	//CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Forward", CI_GetKeyCode("VK_LBUTTON"), 0, true );
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Forward", CI_GetKeyCode("VK_MWHEEL_UP"), 0, true );
	MapControlToGroup("ShipCamera_Forward", "Sailing1Pers");
	//CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Backward", CI_GetKeyCode("VK_RBUTTON"), 0, true );
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Backward", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, true );
	MapControlToGroup("ShipCamera_Backward", "Sailing1Pers");

		// deck camera
	//Trace("DeckCamera_Turn_V");
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Turn_V", 257, 0, false );
	SetControlForInverting("DeckCamera_Turn_V",false);
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Turn_H", 256, 0, false );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Forward", CI_GetKeyCode("VK_RBUTTON"), 0, true );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Backward", CI_GetKeyCode("VK_MBUTTON"), 0, true );
	//CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Left", CI_GetKeyCode("KEY_A"), 0, true );
	//CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Right", CI_GetKeyCode("KEY_D"), 0, true );

		// free camera
	CI_CreateAndSetControls( "", "FreeCamera_Turn_V", 257, INVERSE_CONTROL, false );
	SetControlForInverting("FreeCamera_Turn_V",true);
	CI_CreateAndSetControls( "", "FreeCamera_Turn_H", 256, 0, false );
	CI_CreateAndSetControls( "", "FreeCamera_Forward", CI_GetKeyCode("VK_LBUTTON"), 0, false );  //VK_LBUTTON
	CI_CreateAndSetControls( "", "FreeCamera_Backward", CI_GetKeyCode("VK_RBUTTON"), 0, false );   //VK_RBUTTON


// Character ===================================================================
	CI_CreateAndSetControls( "", "ChrTurnH", 256, 0, false );
	CI_CreateAndSetControls( "PrimaryLand", "ChrAction", CI_GetKeyCode("VK_LBUTTON"), 0, true );
	CI_CreateAndSetControls( "PrimaryLand", "ChrForward", CI_GetKeyCode("KEY_W"), 0, true );
	CI_CreateAndSetControls( "PrimaryLand", "ChrForward2", CI_GetKeyCode("VK_RBUTTON"), 0, true ); // вторая команда
	//CI_CreateAndSetControls( "PrimaryLand", "ChrJump", CI_GetKeyCode("KEY_T"), 0, true );
	MapControlToGroup("ChrForward","FightModeControls");
	MapControlToGroup("ChrForward","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrBackward", CI_GetKeyCode("KEY_S"), 0, true );
	MapControlToGroup("ChrBackward","FightModeControls");
	MapControlToGroup("ChrBackward","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrStrafeLeft", CI_GetKeyCode("KEY_A"), 0, true );
	MapControlToGroup("ChrStrafeLeft","FightModeControls");
	MapControlToGroup("ChrStrafeLeft","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrStrafeRight", CI_GetKeyCode("KEY_D"), 0, true );
	MapControlToGroup("ChrStrafeRight","FightModeControls");
	MapControlToGroup("ChrStrafeRight","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrRun", CI_GetKeyCode("VK_SHIFT"), USE_AXIS_AS_BUTTON, true );
	MapControlToGroup("ChrRun","FightModeControls");
	MapControlToGroup("ChrRun","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrFightMode", CI_GetKeyCode("KEY_E"), 0, true );

    // boal -->
	CI_CreateAndSetControls( "PrimaryLand", "BOAL_UsePotion", CI_GetKeyCode("KEY_X"), 0, true );
	MapControlToGroup("BOAL_UsePotion","FightModeControls");

    CI_CreateAndSetControls( "PrimaryLand", "BOAL_DeadSearch", CI_GetKeyCode("KEY_1"), 0, true );
	MapControlToGroup("BOAL_DeadSearch","FightModeControls");
	
	CI_CreateAndSetControls( "FightModeControls", "BOAL_ActivateRush", CI_GetKeyCode("KEY_F"), 0, true ); // boal ярость на суше
	// boal <--
	
	//CI_CreateContainer("", "ChrTurnH1", 15.0);
	//MapControlToGroup("ChrTurnH1","FightModeControls");
	//MapControlToGroup("ChrTurnH1","BattleInterfaceControls");
	//AddToContainer("PrimaryLand", "ChrTurnH1", "TmpChrTurn1", CI_GetKeyCode("KEY_D"), 0, false );
	//MapControlToGroup("TmpChrTurn1","FightModeControls");
	//AddToContainer("PrimaryLand", "ChrTurnH1", "TmpChrTurn2", CI_GetKeyCode("KEY_A"), 0, true );
	//MapControlToGroup("TmpChrTurn2","FightModeControls");

	//CI_CreateAndSetControls( "FightModeControls", "ChrBlockBase", CI_GetKeyCode("VK_SPACE"), 0, true );   //0
	CI_CreateAndSetControls( "FightModeControls", "ChrBlock", CI_GetKeyCode("VK_SPACE"), 0, true );
	CI_CreateAndSetControls( "FightModeControls", "ChrAltAttackBase", CI_GetKeyCode("VK_LBUTTON"), 0, true );   //2
	CI_CreateAndSetControls( "FightModeControls", "ChrAttackBreakBase", CI_GetKeyCode("VK_MBUTTON"), 0, true ); //ChrAttackRoundBase
	CI_CreateAndSetControls( "FightModeControls", "ChrAttackBase", CI_GetKeyCode("VK_RBUTTON"), 0, true );   //1

	//MapControlToGroup("ChrBlockBase","BattleInterfaceControls");
	//CI_CreateAndSetControls( "FightModeControls", "BIFastCommand", CI_GetKeyCode("VK_F3"), 0, true ); //VK_F3

	//CI_CreateAndSetControls( "FightModeControls", "ChrBlock", CI_GetKeyCode("VK_SPACE"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrRecoil", CI_GetKeyCode("KEY_G"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrParry", CI_GetKeyCode("VK_SHIFT"), 0, true );   //3
	CI_CreateAndSetControls( "FightModeControls", "ChrAttackChoseBase", CI_GetKeyCode("VK_SHIFT"), 0, true );

	//CI_CreateAndSetControls( "FightModeControls", "ChrAttackForce", CI_GetKeyCode("KEY_C"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrAttackFast", CI_GetKeyCode("KEY_X"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrAttackRound", CI_GetKeyCode("VK_MBUTTON"), 0, true );   //4

	//CI_CreateAndSetControls( "FightModeControls", "ChrAttackBreak", CI_GetKeyCode("KEY_Z"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrAttackFient", CI_GetKeyCode("KEY_X"), 0, true );
	//CI_CreateAndSetControls( "FightModeControls", "ChrParry", CI_GetKeyCode("KEY_C"), 0, true );


	MapControlToGroup("ChrBlock","BattleInterfaceControls");
	
	CI_CreateAndSetControls( "FightModeControls", "ChrFightMode", CI_GetKeyCode("KEY_E"), 0, true );
	MapControlToGroup("ChrFightMode","BattleInterfaceControls");
	
	CI_CreateAndSetControls( "FightModeControls", "ChrFire", CI_GetKeyCode("KEY_Q"), 0, true );
	
	MapControlToGroup("ChrFire","BattleInterfaceControls");

// Ship ========================================================================
	
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_TurnLeft", CI_GetKeyCode("KEY_A"), 0, true );
	MapControlToGroup("Ship_TurnLeft","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_TurnRight", CI_GetKeyCode("KEY_D"), 0, true );
	MapControlToGroup("Ship_TurnRight","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_SailUp", CI_GetKeyCode("KEY_W"), 0, true );
	MapControlToGroup("Ship_SailUp","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_SailDown", CI_GetKeyCode("KEY_S"), 0, true );
	MapControlToGroup("Ship_SailDown","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_Fire", CI_GetKeyCode("VK_LBUTTON"), 0, true );
	MapControlToGroup("Ship_Fire","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "Sea_CameraSwitch", CI_GetKeyCode("VK_TAB"), 0, true );
	MapControlToGroup("Sea_CameraSwitch","Sailing1Pers");

	CI_CreateAndSetControls( "Sailing1Pers", "TelescopeIn", CI_GetKeyCode("VK_CONTROL"), 0, true );
	CI_CreateAndSetControls( "Sailing1Pers", "TelescopeOut", CI_GetKeyCode("VK_CONTROL"), INVERSE_CONTROL, true );


// World map ===================================================================
	CI_CreateAndSetControls( "", "WMapTurnH", 256, 0, false );
	CI_CreateAndSetControls( "", "WMapTurnV", 257, INVERSE_CONTROL, false );
	//SetControlForInverting("WMapTurnV",true);
	CI_CreateAndSetControls( "WorldMapControls", "WMapCameraRotate", CI_GetKeyCode("VK_CONTROL"), 0, true );
	//CI_CreateAndSetControls( "WorldMapControls", "WMapForward", CI_GetKeyCode("VK_LBUTTON"), 0, true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapForward", CI_GetKeyCode("VK_MWHEEL_UP"), 0, true );
	//CI_CreateAndSetControls( "WorldMapControls", "WMapBackward", CI_GetKeyCode("VK_RBUTTON"), 0, true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapBackward", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapCameraSwitch", CI_GetKeyCode("VK_TAB"), 0, true );
	//CI_CreateAndSetControls( "WorldMapControls", "WMapPauseSwitch", CI_GetKeyCode("VK_PAUSE"), 0, true );
	
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailUp", CI_GetKeyCode("KEY_W"), 0, true );
	
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailDown", CI_GetKeyCode("KEY_S"), 0, true );
	
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnLeft", CI_GetKeyCode("KEY_A"), 0, true );
	
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnRight", CI_GetKeyCode("KEY_D"), 0, true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapCancel", CI_GetKeyCode("VK_SPACE"), 0, true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapSkipEncounter", CI_GetKeyCode("VK_RETURN"), 0, true );


// Interface ===================================================================
	//				Main interface
	CI_CreateAndSetControls( "", "ITurnH", 256, 0, false );
	CI_CreateAndSetControls( "", "ITurnV", 257, INVERSE_CONTROL, false );
	CI_CreateAndSetControls( "", "ILClick", CI_GetKeyCode("VK_LBUTTON"), 0, false );
	CI_CreateAndSetControls( "", "IRClick", CI_GetKeyCode("VK_RBUTTON"), 0, false );
	CI_CreateAndSetControls( "Sailing1Pers", "Interface", CI_GetKeyCode("VK_F2"), 0, false );
	MapControlToGroup("Interface","Sailing3Pers");
	MapControlToGroup("Interface","PrimaryLand");
	MapControlToGroup("Interface","WorldMapControls");
	MapControlToGroup("Interface","FightModeControls");
	CI_CreateAndSetControls( "", "MainMenu", CI_GetKeyCode("VK_F1"), 0, false );
	CI_CreateAndSetControls( "Sailing1Pers", "MainMenuDouble", CI_GetKeyCode("VK_ESCAPE"), 0, false );
	MapControlToGroup("MainMenuDouble","Sailing3Pers");
	MapControlToGroup("MainMenuDouble","PrimaryLand");
	MapControlToGroup("MainMenuDouble","WorldMapControls");
	MapControlToGroup("MainMenuDouble","FightModeControls");
	CI_CreateAndSetControls( "", "QuickSave", CI_GetKeyCode("VK_F6"), 0, false );
	CI_CreateAndSetControls( "", "QuickLoad", CI_GetKeyCode("VK_F9"), 0, false );
	CI_CreateAndSetControls( "", "IStartButton", CI_GetKeyCode("VK_RETURN"), 0, false );
	//CI_CreateAndSetControls( "", "IAllCancel", CI_GetKeyCode("VK_BACK"), 0, false );
	CI_CreateAndSetControls( "", "ICancel", CI_GetKeyCode("VK_ESCAPE"), 0, false );
	CI_CreateAndSetControls( "", "IAction", CI_GetKeyCode("VK_SPACE"), 0, false );
	CI_CreateAndSetControls( "", "ILeftShift", CI_GetKeyCode("VK_SHIFT"), 0, false );
	CI_CreateAndSetControls( "", "IRightShift", CI_GetKeyCode("VK_SHIFT"), 0, false );
	CI_CreateAndSetControls( "", "IUp", CI_GetKeyCode("VK_UP"), 0, false );
	CI_CreateAndSetControls( "", "IDown", CI_GetKeyCode("VK_DOWN"), 0, false );
	CI_CreateAndSetControls( "", "IRight", CI_GetKeyCode("VK_RIGHT"), 0, false );
	CI_CreateAndSetControls( "", "ILeft", CI_GetKeyCode("VK_LEFT"), 0, false );
	//CI_CreateAndSetControls( "", "IContextHelp", CI_GetKeyCode("VK_CONTROL"), 0, false );

	// controls for instant exit from interface (used by function XI_RegistryExitKey() )
	CI_CreateAndSetControls( "", "IExit_F2", CI_GetKeyCode("VK_F2"), 0, false );
	CI_CreateAndSetControls( "", "IExit_Escape", CI_GetKeyCode("VK_ESCAPE"), 0, false );
	CI_CreateAndSetControls( "", "IExit_F1", CI_GetKeyCode("VK_F1"), 0, false );

	//				Battle interface
	CI_CreateAndSetControls( "Sailing3Pers", "BICommandsActivate", CI_GetKeyCode("VK_RETURN"), 0, true );
	MapControlToGroup("BICommandsActivate","Sailing1Pers");
	MapControlToGroup("BICommandsActivate","WorldMapControls");
	CI_CreateAndSetControls( "PrimaryLand", "LICommandsActivate", CI_GetKeyCode("VK_RETURN"), 0, true );
	MapControlToGroup("LICommandsActivate","FightModeControls");
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirm", CI_GetKeyCode("VK_RETURN"), 0, true );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancel", CI_GetKeyCode("VK_ESCAPE"), 0, false );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeft", CI_GetKeyCode("VK_LEFT"), 0, true );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRight", CI_GetKeyCode("VK_RIGHT"), 0, true );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsUp", CI_GetKeyCode("VK_UP"), 0, true );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsDown", CI_GetKeyCode("VK_DOWN"), 0, true );
	CI_CreateAndSetControls( "Sailing3Pers", "MiniMapZoomIn", CI_GetKeyCode("KEY_E"), 0, true );
	MapControlToGroup("MiniMapZoomIn","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "MiniMapZoomOut", CI_GetKeyCode("KEY_F"), 0, true );
	MapControlToGroup("MiniMapZoomOut","Sailing1Pers");
	CI_CreateAndSetControls( "Sailing3Pers", "BIFastCommand", CI_GetKeyCode("VK_SPACE"), 0, true ); //VK_F3
	MapControlToGroup("BIFastCommand","Sailing1Pers");
	MapControlToGroup("BIFastCommand","PrimaryLand");
	MapControlToGroup("BIFastCommand","FightModeControls");
	MapControlToGroup("BIFastCommand","WorldMapControls");
	MapControlToGroup("BIFastCommand","BattleInterfaceControls");

	// hot keys for cannon charge reload
		// Balls
	CI_CreateAndSetControls("Sailing3Pers","hk_charge1", CI_GetKeyCode("KEY_1"), 0, true );
	MapControlToGroup("hk_charge1","Sailing1Pers");
	MapControlToGroup("hk_charge1","BattleInterfaceControls");
		// Grapes
	CI_CreateAndSetControls("Sailing3Pers","hk_charge2", CI_GetKeyCode("KEY_2"), 0, true );
	MapControlToGroup("hk_charge2","Sailing1Pers");
	MapControlToGroup("hk_charge2","BattleInterfaceControls");
		// Knippels
	CI_CreateAndSetControls("Sailing3Pers","hk_charge3", CI_GetKeyCode("KEY_3"), 0, true );
	MapControlToGroup("hk_charge3","Sailing1Pers");
	MapControlToGroup("hk_charge3","BattleInterfaceControls");
		// Bombs
	CI_CreateAndSetControls("Sailing3Pers","hk_charge4", CI_GetKeyCode("KEY_4"), 0, true );
	MapControlToGroup("hk_charge4","Sailing1Pers");
	MapControlToGroup("hk_charge4","BattleInterfaceControls");

// Dialog =====================================================================
	CI_CreateAndSetControls( "DialogControls", "DlgAction", CI_GetKeyCode("VK_SPACE"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgAction1", CI_GetKeyCode("VK_LBUTTON"), 0, false );   // boal new engine
	CI_CreateAndSetControls( "DialogControls", "DlgAction2", CI_GetKeyCode("VK_RETURN"), 0, false );
	CI_CreateAndSetControls( "", "DlgCancel", CI_GetKeyCode("VK_ESCAPE"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgUp", CI_GetKeyCode("VK_UP"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgDown", CI_GetKeyCode("VK_DOWN"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgUp2", CI_GetKeyCode("VK_MWHEEL_UP"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgDown2", CI_GetKeyCode("VK_MWHEEL_DOWN"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgUp3", CI_GetKeyCode("KEY_W"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgDown3", CI_GetKeyCode("KEY_S"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgScrollUp", CI_GetKeyCode("VK_PRIOR"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgScrollDown", CI_GetKeyCode("VK_NEXT"), 0, false );

	// Net Controls =====================================================================
	CI_CreateAndSetControls( "NetShipControls", "Net_Menu", CI_GetKeyCode("VK_ESCAPE"), 0, true );
	CI_CreateAndSetControls( "NetShipControls", "Say", CI_GetKeyCode("KEY_Y"), 0, true );
	CI_CreateAndSetControls( "NetShipControls", "Team_Say", CI_GetKeyCode("KEY_O"), 0, true );
	CI_CreateAndSetControls( "NetShipControls", "Person_Say", CI_GetKeyCode("KEY_T"), 0, true );
	CI_CreateAndSetControls( "", "gamestat", CI_GetKeyCode("VK_F1"), 0, true );
	CI_CreateAndSetControls( "", "toplist", CI_GetKeyCode("VK_F2"), 0, true );
	//CI_CreateAndSetControls( "", "gamechat", CI_GetKeyCode("VK_F3"), 0, true );

	MapControlToGroup("ShipCamera_Turn_H", "NetShipControls");
	MapControlToGroup("ShipCamera_Turn_V", "NetShipControls");

	MapControlToGroup("DeckCamera_Turn_H", "NetShipControls");
	MapControlToGroup("DeckCamera_Turn_V", "NetShipControls");

	MapControlToGroup("BICommandsActivate", "NetShipControls");

	// temporary control
	CI_CreateAndSetControls( "NetShipControls", "Ship_StartRepair", CI_GetKeyCode("KEY_L"), 0, true );
	
	MapControlToGroup("Ship_TurnLeft", "NetShipControls");
	MapControlToGroup("Ship_TurnRight", "NetShipControls");
	MapControlToGroup("Ship_SailUp", "NetShipControls");
	MapControlToGroup("Ship_SailDown", "NetShipControls");
	MapControlToGroup("Ship_Fire", "NetShipControls");

	MapControlToGroup("Sea_CameraSwitch", "NetShipControls");
	MapControlToGroup("TelescopeIn", "NetShipControls");

	MapControlToGroup("hk_charge1", "NetShipControls");
	MapControlToGroup("hk_charge2", "NetShipControls");
	MapControlToGroup("hk_charge3", "NetShipControls");
	MapControlToGroup("hk_charge4", "NetShipControls");

	MapControlToGroup("ShipCamera_Forward", "NetShipControls");
	MapControlToGroup("ShipCamera_Backward", "NetShipControls");
	MapControlToGroup("DeckCamera_Forward", "NetShipControls");
	MapControlToGroup("DeckCamera_Backward", "NetShipControls");
}

void ExternInitKeyCodes()
{
	objControlsState.key_codes.VK_LBUTTON	= 1;
	objControlsState.key_codes.VK_LBUTTON.img = "q";
	objControlsState.key_codes.VK_RBUTTON	= 2;
	objControlsState.key_codes.VK_RBUTTON.img = "w";
	objControlsState.key_codes.VK_BACK		= 8;
	objControlsState.key_codes.VK_BACK.img = "b";
	objControlsState.key_codes.VK_TAB		= 9;
	objControlsState.key_codes.VK_TAB.img = "t";
	objControlsState.key_codes.VK_RETURN	= 13;
	objControlsState.key_codes.VK_RETURN.img = "n";
	objControlsState.key_codes.VK_SHIFT		= 16;
	objControlsState.key_codes.VK_SHIFT.img = "i";
	objControlsState.key_codes.VK_CONTROL	= 17;
	objControlsState.key_codes.VK_CONTROL.img = "c";
	objControlsState.key_codes.VK_ESCAPE	= 27;
	objControlsState.key_codes.VK_ESCAPE.img = "e";
	objControlsState.key_codes.VK_SPACE		= 32;
	objControlsState.key_codes.VK_SPACE.img = "s";
	objControlsState.key_codes.VK_PRIOR		= 33;
	objControlsState.key_codes.VK_PRIOR.img = "p";
	objControlsState.key_codes.VK_NEXT		= 34;
	objControlsState.key_codes.VK_NEXT.img = "j";
	objControlsState.key_codes.VK_LEFT		= 37;
	objControlsState.key_codes.VK_LEFT.img = "l";
	objControlsState.key_codes.VK_UP		= 38;
	objControlsState.key_codes.VK_UP.img = "u";
	objControlsState.key_codes.VK_RIGHT		= 39;
	objControlsState.key_codes.VK_RIGHT.img = "r";
	objControlsState.key_codes.VK_DOWN		= 40;
	objControlsState.key_codes.VK_DOWN.img = "d";
	objControlsState.key_codes.VK_INSERT	= 45;
	objControlsState.key_codes.VK_INSERT.img = "r";
	objControlsState.key_codes.VK_DELETE	= 46;
	objControlsState.key_codes.VK_DELETE.img = "d";

	objControlsState.key_codes.KEY_0 = 48;
	objControlsState.key_codes.KEY_0.img = "0";
	objControlsState.key_codes.KEY_1 = 49;
	objControlsState.key_codes.KEY_1.img = "1";
	objControlsState.key_codes.KEY_2 = 50;
	objControlsState.key_codes.KEY_2.img = "2";
	objControlsState.key_codes.KEY_3 = 51;
	objControlsState.key_codes.KEY_3.img = "3";
	objControlsState.key_codes.KEY_4 = 52;
	objControlsState.key_codes.KEY_4.img = "4";
	objControlsState.key_codes.KEY_5 = 53;
	objControlsState.key_codes.KEY_5.img = "5";
	objControlsState.key_codes.KEY_6 = 54;
	objControlsState.key_codes.KEY_6.img = "6";
	objControlsState.key_codes.KEY_7 = 55;
	objControlsState.key_codes.KEY_7.img = "7";
	objControlsState.key_codes.KEY_8 = 56;
	objControlsState.key_codes.KEY_8.img = "8";
	objControlsState.key_codes.KEY_9 = 57;
	objControlsState.key_codes.KEY_9.img = "9";

	objControlsState.key_codes.KEY_A = 65;
	objControlsState.key_codes.KEY_A.img = "A";
	objControlsState.key_codes.KEY_B = 66;
	objControlsState.key_codes.KEY_B.img = "B";
	objControlsState.key_codes.KEY_C = 67;
	objControlsState.key_codes.KEY_C.img = "C";
	objControlsState.key_codes.KEY_D = 68;
	objControlsState.key_codes.KEY_D.img = "D";
	objControlsState.key_codes.KEY_E = 69;
	objControlsState.key_codes.KEY_E.img = "E";
	objControlsState.key_codes.KEY_F = 70;
	objControlsState.key_codes.KEY_F.img = "F";
	objControlsState.key_codes.KEY_G = 71;
	objControlsState.key_codes.KEY_G.img = "G";
	objControlsState.key_codes.KEY_H = 72;
	objControlsState.key_codes.KEY_H.img = "H";
	objControlsState.key_codes.KEY_I = 73;
	objControlsState.key_codes.KEY_I.img = "I";
	objControlsState.key_codes.KEY_J = 74;
	objControlsState.key_codes.KEY_J.img = "J";
	objControlsState.key_codes.KEY_K = 75;
	objControlsState.key_codes.KEY_K.img = "K";
	objControlsState.key_codes.KEY_L = 76;
	objControlsState.key_codes.KEY_L.img = "L";
	objControlsState.key_codes.KEY_M = 77;
	objControlsState.key_codes.KEY_M.img = "M";
	objControlsState.key_codes.KEY_N = 78;
	objControlsState.key_codes.KEY_N.img = "N";
	objControlsState.key_codes.KEY_O = 79;
	objControlsState.key_codes.KEY_O.img = "O";
	objControlsState.key_codes.KEY_P = 80;
	objControlsState.key_codes.KEY_P.img = "P";
	objControlsState.key_codes.KEY_Q = 81;
	objControlsState.key_codes.KEY_Q.img = "Q";
	objControlsState.key_codes.KEY_R = 82;
	objControlsState.key_codes.KEY_R.img = "R";
	objControlsState.key_codes.KEY_S = 83;
	objControlsState.key_codes.KEY_S.img = "S";
	objControlsState.key_codes.KEY_T = 84;
	objControlsState.key_codes.KEY_T.img = "T";
	objControlsState.key_codes.KEY_U = 85;
	objControlsState.key_codes.KEY_U.img = "U";
	objControlsState.key_codes.KEY_V = 86;
	objControlsState.key_codes.KEY_V.img = "V";
	objControlsState.key_codes.KEY_W = 87;
	objControlsState.key_codes.KEY_W.img = "W";
	objControlsState.key_codes.KEY_X = 88;
	objControlsState.key_codes.KEY_X.img = "X";
	objControlsState.key_codes.KEY_Y = 89;
	objControlsState.key_codes.KEY_Y.img = "Y";
	objControlsState.key_codes.KEY_Z = 90;
	objControlsState.key_codes.KEY_Z.img = "Z";

	objControlsState.key_codes.VK_NUMPAD0	= 96;
	objControlsState.key_codes.VK_NUMPAD0.img = ")";
	objControlsState.key_codes.VK_NUMPAD1	= 97;
	objControlsState.key_codes.VK_NUMPAD1.img = "!";
	objControlsState.key_codes.VK_NUMPAD2	= 98;
	objControlsState.key_codes.VK_NUMPAD2.img = "@";
	objControlsState.key_codes.VK_NUMPAD3	= 99;
	objControlsState.key_codes.VK_NUMPAD3.img = "#";
	objControlsState.key_codes.VK_NUMPAD4	= 100;
	objControlsState.key_codes.VK_NUMPAD4.img = "$";
	objControlsState.key_codes.VK_NUMPAD5	= 101;
	objControlsState.key_codes.VK_NUMPAD5.img = "%";
	objControlsState.key_codes.VK_NUMPAD6	= 102;
	objControlsState.key_codes.VK_NUMPAD6.img = "^";
	objControlsState.key_codes.VK_NUMPAD7	= 103;
	objControlsState.key_codes.VK_NUMPAD7.img = "&";
	objControlsState.key_codes.VK_NUMPAD8	= 104;
	objControlsState.key_codes.VK_NUMPAD8.img = "*";
	objControlsState.key_codes.VK_NUMPAD9	= 105;
	objControlsState.key_codes.VK_NUMPAD9.img = "(";
	objControlsState.key_codes.VK_MULTIPLY	= 106;
	objControlsState.key_codes.VK_MULTIPLY.img = "m";
	objControlsState.key_codes.VK_ADD		= 107;
	objControlsState.key_codes.VK_ADD.img = "+";
	objControlsState.key_codes.VK_SEPARATOR	= 108;
	objControlsState.key_codes.VK_SEPARATOR.img = "_";
	objControlsState.key_codes.VK_SUBTRACT	= 109;
	objControlsState.key_codes.VK_SUBTRACT.img = "-";
	objControlsState.key_codes.VK_DECIMAL	= 110;
	objControlsState.key_codes.VK_DECIMAL.img = ".";
	objControlsState.key_codes.VK_DIVIDE	= 111;
	objControlsState.key_codes.VK_DIVIDE.img = "/";
	objControlsState.key_codes.VK_F1		= 112;
	objControlsState.key_codes.VK_F1.img = "z";
	objControlsState.key_codes.VK_F2		= 113;
	objControlsState.key_codes.VK_F2.img = "x";
	objControlsState.key_codes.VK_F3		= 114;
	objControlsState.key_codes.VK_F3.img = "v";
	objControlsState.key_codes.VK_F4		= 115;
	objControlsState.key_codes.VK_F4.img = "<";
	objControlsState.key_codes.VK_F5		= 116;
	objControlsState.key_codes.VK_F5.img = "";
	objControlsState.key_codes.VK_F6		= 117;
	objControlsState.key_codes.VK_F6.img = "";
	objControlsState.key_codes.VK_F7		= 118;
	objControlsState.key_codes.VK_F7.img = "";
	objControlsState.key_codes.VK_F8		= 119;
	objControlsState.key_codes.VK_F8.img = "";
	objControlsState.key_codes.VK_F9		= 120;
	objControlsState.key_codes.VK_F9.img = "";

	objControlsState.key_codes.VK_MWHEEL_UP		= 258;
	objControlsState.key_codes.VK_MWHEEL_UP.img = ">";
	objControlsState.key_codes.VK_MWHEEL_DOWN	= 259;
	objControlsState.key_codes.VK_MWHEEL_DOWN.img = ">";
	
	// boal -->
	objControlsState.key_codes.VK_MBUTTON	= 4;
	objControlsState.key_codes.VK_MBUTTON.img = ">";
	objControlsState.key_codes.VK_F10		= 121;
	objControlsState.key_codes.VK_F10.img = "z";
    objControlsState.key_codes.VK_F11		= 122;
	objControlsState.key_codes.VK_F11.img = "z";
    objControlsState.key_codes.VK_F12		= 123;
	objControlsState.key_codes.VK_F12.img = "z";
	objControlsState.key_codes.VK_PAUSE	= 19;
	objControlsState.key_codes.VK_PAUSE.img = "z";
	
	objControlsState.key_codes.VK_L_BREAKE		= 219;   //[ {
	objControlsState.key_codes.VK_L_BREAKE.img = "s";
	objControlsState.key_codes.VK_R_BREAKE		= 221;   //[ {
	objControlsState.key_codes.VK_R_BREAKE.img = "s";
	objControlsState.key_codes.VK_A_POINT		= 186;  //; :
	objControlsState.key_codes.VK_A_POINT.img = "s";
	objControlsState.key_codes.VK_A_QUOTE		= 222;  //' "
	objControlsState.key_codes.VK_A_QUOTE.img = "s";
	objControlsState.key_codes.VK_A_SMALL		= 188;  //, <
	objControlsState.key_codes.VK_A_SMALL.img = "s";
	objControlsState.key_codes.VK_A_BIG		= 190;  //. >
	objControlsState.key_codes.VK_A_BIG.img = "s";
	objControlsState.key_codes.VK_A_QWS	    = 191;  //?/
	objControlsState.key_codes.VK_A_QWS.img = "s";
	objControlsState.key_codes.VK_A_PLUS	    = 187;  //= +
	objControlsState.key_codes.VK_A_PLUS.img = "s";
	objControlsState.key_codes.VK_A_MINUS	    = 189;  //- _
	objControlsState.key_codes.VK_A_MINUS.img = "s";
	objControlsState.key_codes.VK_A_SLUSH	    = 220;  // \ |
	objControlsState.key_codes.VK_A_SLUSH.img = "s";

	objControlsState.key_codes.VK_A_TILDA	    = 192;  // ` ~
	objControlsState.key_codes.VK_A_TILDA.img = ">";
	
	objControlsState.key_codes.VK_CAPSLOCK	    = 20;
	objControlsState.key_codes.VK_CAPSLOCK.img = "s";

	// boal <--

	// Key groups for controls remapping
	objControlsState.grouplist.Sailing1Pers = true;
	objControlsState.grouplist.Sailing3Pers = true;
	objControlsState.grouplist.FightModeControls = true;
	objControlsState.grouplist.PrimaryLand = true;
	objControlsState.grouplist.WorldMapControls = true;   // boal
}