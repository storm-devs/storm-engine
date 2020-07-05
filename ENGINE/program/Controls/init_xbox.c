void ExternControlsInit(bool bFirst)
{
	if(bFirst)
	{
		ExternInitKeyCodes();
		CI_CreateAndSetControls( "", "ICancel", CI_GetKeyCode("BTN_B"), 0, false );
		CI_CreateAndSetControls( "", "IAction", CI_GetKeyCode("BTN_A"), 0, false );
		CI_CreateAndSetControls( "", "IStartButton", CI_GetKeyCode("BTN_START"), 0, false );
		CI_CreateAndSetControls( "", "IAllCancel", CI_GetKeyCode("IAllCancel"), 0, false );
		return;
	}

	bool bInverse = false;
	if( CheckAttribute(&InterfaceStates,"InvertCameras") ) {
		bInverse = sti(InterfaceStates.InvertCameras)!=0;
	}

	ExternInitKeyCodes();

	//CI_CreateAndSetControls( "", "Dump", CI_GetKeyCode("BTN_WHITE"), 0, false );

	//	CI_CreateAndSetControls( "", "Tele", CI_GetKeyCode("BTN_Y"), 0, false );
	// Cameras =====================================================================
		// character camera
	CI_CreateAndSetControls( "PrimaryLand", "ChrCamTurnV", CI_GetKeyCode("RIGHT_STICK_Y"), GetStickStateY(INVERSE_CONTROL,bInverse), true );
	MapControlToGroup("ChrCamTurnV","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrCamTurnH", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), true );
	MapControlToGroup("ChrCamTurnH","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrCamCameraSwitch", CI_GetKeyCode("LEFT_THUMB"), 0, true );

		// ship follow camera
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Turn_V", CI_GetKeyCode("RIGHT_STICK_Y"), GetStickStateY(INVERSE_CONTROL,bInverse), true );
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Turn_H", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), true );
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Forward", CI_GetKeyCode("RIGHT_TRIGGER"), 0, true );
	CI_CreateAndSetControls( "Sailing3Pers", "ShipCamera_Backward", CI_GetKeyCode("LEFT_TRIGGER"), 0, true );

		// deck camera
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Turn_V", CI_GetKeyCode("RIGHT_STICK_Y"), GetStickStateY(0,bInverse), true );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Turn_H", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), true );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Forward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Backward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Left", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "Sailing1Pers", "DeckCamera_Right", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_BUTTON, false );

		// free camera
	CI_CreateAndSetControls( "", "FreeCamera_Turn_V", CI_GetKeyCode("RIGHT_STICK_Y"), GetStickStateY(INVERSE_CONTROL,bInverse), false );
	CI_CreateAndSetControls( "", "FreeCamera_Turn_H", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), false );
	CI_CreateAndSetControls( "", "FreeCamera_Forward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "", "FreeCamera_Backward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );

	CI_CreateAndSetControls( "", "TimeScale", CI_GetKeyCode("BTN_X"), 0, false );


// Character ===================================================================
	CI_CreateAndSetControls( "PrimaryLand", "ChrTurnH", CI_GetKeyCode("LEFT_STICK_X"), GetStickStateX(0,bInverse), true );
	MapControlToGroup("ChrTurnH","FightModeControls");
	MapControlToGroup("ChrTurnH","BattleInterfaceControls");

	CI_CreateAndSetControls( "PrimaryLand", "ChrForward1", CI_GetKeyCode("DPAD_UP"), 0, true );
	MapControlToGroup("ChrForward1","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrBackward1", CI_GetKeyCode("DPAD_DOWN"), 0, true );
	MapControlToGroup("ChrBackward1","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrTurnHR", CI_GetKeyCode("DPAD_RIGHT"), 0, true );
	MapControlToGroup("ChrTurnHR","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrTurnHL", CI_GetKeyCode("DPAD_LEFT"), 0, true );
	MapControlToGroup("ChrTurnHL","FightModeControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrCamNormalize", CI_GetKeyCode("RIGHT_THUMB"), 0, true );
	MapControlToGroup("ChrCamNormalize","FightModeControls");
	MapControlToGroup("ChrCamNormalize","BattleInterfaceControls");

	CI_CreateAndSetControls( "PrimaryLand", "ChrTurnV", CI_GetKeyCode("LEFT_STICK_Y"), GetStickStateY(0,bInverse), true );
	MapControlToGroup("ChrTurnV","FightModeControls");
	MapControlToGroup("ChrTurnV","BattleInterfaceControls");

	CI_CreateAndSetControls( "PrimaryLand", "ChrAction", CI_GetKeyCode("BTN_A"), 0, true );
	CI_CreateAndSetControls( "PrimaryLand", "ChrForward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	MapControlToGroup("ChrForward","FightModeControls");
	MapControlToGroup("ChrForward","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrBackward", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );
	MapControlToGroup("ChrBackward","FightModeControls");
	MapControlToGroup("ChrBackward","BattleInterfaceControls");
	CI_CreateAndSetControls( "FightModeControls", "ChrBlock", CI_GetKeyCode("LEFT_TRIGGER"), 0, true );
	MapControlToGroup("ChrBlock","BattleInterfaceControls");
	CI_CreateAndSetControls( "FightModeControls", "ChrAttack", CI_GetKeyCode("BTN_A"), 0, true );
	CI_CreateAndSetControls( "FightModeControls", "ChrRecoil", CI_GetKeyCode("BTN_B"), 0, true );
	CI_CreateAndSetControls( "PrimaryLand", "ChrFightMode", CI_GetKeyCode("BTN_X"), 0, true );
	MapControlToGroup("ChrFightMode","FightModeControls");
	MapControlToGroup("ChrFightMode","BattleInterfaceControls");
	CI_CreateAndSetControls( "FightModeControls", "ChrFire", CI_GetKeyCode("RIGHT_TRIGGER"), 0, true );
	MapControlToGroup("ChrFire","BattleInterfaceControls");
	CI_CreateAndSetControls( "PrimaryLand", "ChrRun", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	MapControlToGroup("ChrRun","FightModeControls");
	MapControlToGroup("ChrRun","BattleInterfaceControls");
	SetControlTreshold(sti(objControlsState.map.controls.ChrRun),12.0);


// Ship ========================================================================
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_TurnLeft1", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_TurnRight1", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_SailUp1", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "Sailing3Pers", "Ship_SailDown1", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );
	//
	CI_CreateAndSetControls( "Sailing1Pers", "Ship_TurnLeft", CI_GetKeyCode("DPAD_LEFT"), 0, false );
	MapControlToGroup("Ship_TurnLeft","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "Ship_TurnRight", CI_GetKeyCode("DPAD_RIGHT"), 0, false );
	MapControlToGroup("Ship_TurnRight","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "Ship_SailUp", CI_GetKeyCode("DPAD_UP"), 0, false );
	MapControlToGroup("Ship_SailUp","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "Ship_SailDown", CI_GetKeyCode("DPAD_DOWN"), 0, false );
	MapControlToGroup("Ship_SailDown","Sailing3Pers");
	//
	CI_CreateAndSetControls( "Sailing1Pers", "Ship_Fire", CI_GetKeyCode("BTN_A"), 0, true );
	MapControlToGroup("Ship_Fire","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "Sea_CameraSwitch", CI_GetKeyCode("RIGHT_THUMB"), 0, true );
	MapControlToGroup("Sea_CameraSwitch","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "TelescopeIn", CI_GetKeyCode("RIGHT_TRIGGER"), 0, true );
	CI_CreateAndSetControls( "Sailing1Pers", "TelescopeOut", CI_GetKeyCode("LEFT_TRIGGER"), 0, true );

// World map ===================================================================
	CI_CreateAndSetControls( "WorldMapControls", "WMapTurnH", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapTurnV", CI_GetKeyCode("RIGHT_STICK_Y"), GetStickStateY(0,bInverse), true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapCameraRotate", CI_GetKeyCode("RIGHT_STICK_X"), GetStickStateX(0,bInverse), true );
	CI_CreateAndSetControls( "WorldMapControls", "WMapForward", CI_GetKeyCode("RIGHT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapBackward", CI_GetKeyCode("LEFT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapCameraSwitch", CI_GetKeyCode("RIGHT_THUMB"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailUp", CI_GetKeyCode("DPAD_UP"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailDown", CI_GetKeyCode("DPAD_DOWN"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnLeft", CI_GetKeyCode("DPAD_LEFT"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnRight", CI_GetKeyCode("DPAD_RIGHT"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailUp1", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipSailDown1", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnLeft1", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapShipTurnRight1", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapCancel", CI_GetKeyCode("BTN_A"), 0, false );
	CI_CreateAndSetControls( "WorldMapControls", "WMapSkipEncounter", CI_GetKeyCode("BTN_B"), 0, false );


// Interface ===================================================================
	//				Main interface
	CI_CreateAndSetControls( "PrimaryLand", "Interface", CI_GetKeyCode("BTN_B"), 0, true );
	MapControlToGroup("Interface","Sailing3Pers");
	MapControlToGroup("Interface","Sailing1Pers");
	CI_CreateAndSetControls( "", "MainMenu", CI_GetKeyCode("BTN_START"), 0, false );
	CI_CreateAndSetControls( "", "IStartButton", CI_GetKeyCode("BTN_START"), 0, false );
	CI_CreateAndSetControls( "", "IAllCancel", CI_GetKeyCode("BTN_BACK"), 0, false );
	CI_CreateAndSetControls( "", "ICancel", CI_GetKeyCode("BTN_B"), 0, false );
	CI_CreateAndSetControls( "", "IAction", CI_GetKeyCode("BTN_A"), 0, false );
	CI_CreateAndSetControls( "", "ILeftShift", CI_GetKeyCode("LEFT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "", "IRightShift", CI_GetKeyCode("RIGHT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "", "IUp", CI_GetKeyCode("DPAD_UP"), 0, false );
	CI_CreateAndSetControls( "", "IDown", CI_GetKeyCode("DPAD_DOWN"), 0, false );
	CI_CreateAndSetControls( "", "IRight", CI_GetKeyCode("DPAD_RIGHT"), 0, false );
	CI_CreateAndSetControls( "", "ILeft", CI_GetKeyCode("DPAD_LEFT"), 0, false );
	CI_CreateAndSetControls( "", "IUp2", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "", "IDown2", CI_GetKeyCode("LEFT_STICK_Y"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "", "IRight2", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_BUTTON, false );
	CI_CreateAndSetControls( "", "ILeft2", CI_GetKeyCode("LEFT_STICK_X"), USE_AXIS_AS_INVERSEBUTTON, false );
	CI_CreateAndSetControls( "", "IContextHelp", CI_GetKeyCode("BTN_X"), 0, false );

	//				Battle interface
	CI_CreateAndSetControls( "Sailing1Pers", "BICommandsActivate", CI_GetKeyCode("BTN_Y"), 0, true );
	MapControlToGroup("BICommandsActivate","Sailing3Pers");
	CI_CreateAndSetControls( "PrimaryLand", "LICommandsActivate", CI_GetKeyCode("BTN_Y"), 0, true );
	MapControlToGroup("LICommandsActivate","FightModeControls");
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsConfirm", CI_GetKeyCode("BTN_A"), 0, false );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsCancel", CI_GetKeyCode("BTN_B"), 0, false );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsLeft", CI_GetKeyCode("DPAD_LEFT"), 0, false );
	CI_CreateAndSetControls( "BattleInterfaceControls", "BICommandsRight", CI_GetKeyCode("DPAD_RIGHT"), 0, false );
	CI_CreateAndSetControls( "Sailing1Pers", "MiniMapZoomIn", CI_GetKeyCode("BTN_WHITE"), 0, true );
	MapControlToGroup("MiniMapZoomIn","Sailing3Pers");
	CI_CreateAndSetControls( "Sailing1Pers", "MiniMapZoomOut", CI_GetKeyCode("BTN_BLACK"), 0, true );
	MapControlToGroup("MiniMapZoomOut","Sailing3Pers");

// Dialog =====================================================================
	CI_CreateAndSetControls( "DialogControls", "DlgAction", CI_GetKeyCode("BTN_A"), 0, false );
	CI_CreateAndSetControls( "", "DlgCancel", CI_GetKeyCode("BTN_BACK"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgUp", CI_GetKeyCode("DPAD_UP"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgDown", CI_GetKeyCode("DPAD_DOWN"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgScrollUp", CI_GetKeyCode("DPAD_LEFT"), 0, false );
	CI_CreateAndSetControls( "DialogControls", "DlgScrollDown", CI_GetKeyCode("DPAD_RIGHT"), 0, false );


	CI_CreateAndSetControls( "", "CheatA", CI_GetKeyCode("RIGHT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "", "CheatB", CI_GetKeyCode("LEFT_TRIGGER"), 0, false );
	CI_CreateAndSetControls( "", "CheatC", CI_GetKeyCode("BTN_B"), 0, false );
}

void ExternInitKeyCodes()
{
	objControlsState.KeyFontName = "BUTTON_SYMBOL";
	objControlsState.key_codes.DPAD_UP = 0;
	objControlsState.key_codes.DPAD_UP.img = "u";
	objControlsState.key_codes.DPAD_RIGHT = 1;
	objControlsState.key_codes.DPAD_RIGHT.img = "r";
	objControlsState.key_codes.DPAD_DOWN = 2;
	objControlsState.key_codes.DPAD_DOWN.img = "d";
	objControlsState.key_codes.DPAD_LEFT = 4;
	objControlsState.key_codes.DPAD_LEFT.img = "l";
	objControlsState.key_codes.LEFT_STICK = 5;
	objControlsState.key_codes.LEFT_STICK.img = "p";
	objControlsState.key_codes.LEFT_THUMB = 6;
	objControlsState.key_codes.LEFT_THUMB.img = "p";
	objControlsState.key_codes.RIGHT_STICK = 7;
	objControlsState.key_codes.RIGHT_STICK.img = "P";
	objControlsState.key_codes.RIGHT_THUMB = 8;
	objControlsState.key_codes.RIGHT_THUMB.img = "P";
	objControlsState.key_codes.LEFT_TRIGGER = 9;
	objControlsState.key_codes.LEFT_TRIGGER.img = "L";
	objControlsState.key_codes.RIGHT_TRIGGER = 10;
	objControlsState.key_codes.RIGHT_TRIGGER.img = "R";
	objControlsState.key_codes.BTN_BACK = 11;
	objControlsState.key_codes.BTN_BACK.img = "k";
	objControlsState.key_codes.BTN_START = 12;
	objControlsState.key_codes.BTN_START.img = "s";
	objControlsState.key_codes.BTN_WHITE = 13;
	objControlsState.key_codes.BTN_WHITE.img = "w";
	objControlsState.key_codes.BTN_BLACK = 14;
	objControlsState.key_codes.BTN_BLACK.img = "b";
	objControlsState.key_codes.BTN_A = 15;
	objControlsState.key_codes.BTN_A.img = "A";
	objControlsState.key_codes.BTN_B = 16;
	objControlsState.key_codes.BTN_B.img = "B";
	objControlsState.key_codes.BTN_X = 17;
	objControlsState.key_codes.BTN_X.img = "X";
	objControlsState.key_codes.BTN_Y = 18;
	objControlsState.key_codes.BTN_Y.img = "Y";
	objControlsState.key_codes.LEFT_STICK_X = 19;
	objControlsState.key_codes.LEFT_STICK_X.img = "h";
	objControlsState.key_codes.LEFT_STICK_Y = 20;
	objControlsState.key_codes.LEFT_STICK_Y.img = "v";
	objControlsState.key_codes.RIGHT_STICK_X = 21;
	objControlsState.key_codes.RIGHT_STICK_X.img = "H";
	objControlsState.key_codes.RIGHT_STICK_Y = 22;
	objControlsState.key_codes.RIGHT_STICK_Y.img = "V";

	objControlsState.key_codes.LEFT_STICK_X.stick = true;
	objControlsState.key_codes.LEFT_STICK_Y.stick = true;
	objControlsState.key_codes.RIGHT_STICK_X.stick = true;
	objControlsState.key_codes.RIGHT_STICK_Y.stick = true;

	// Key groups for controls remapping
	objControlsState.grouplist.Sailing1Pers = true;
	objControlsState.grouplist.Sailing3Pers = true;
	objControlsState.grouplist.FightModeControls = true;
	objControlsState.grouplist.PrimaryLand = true;
}

int GetStickStateX(int state,bool inverse)
{
	return state;
}

int GetStickStateY(int state,bool inverse)
{
	if(!inverse) return state;
	if(state==0) return INVERSE_CONTROL;
	if(state==INVERSE_CONTROL) return 0;
	return 0;
}
