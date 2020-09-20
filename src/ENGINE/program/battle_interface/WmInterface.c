void InitWmInterface()
{
	DeleteAttribute(&BattleInterface,"");
	WM_InitializeCommands();
	WM_SetParameterData();

	CreateEntity(&BattleInterface,"WM_INTERFACE");
	LayerAddObject("execute",&BattleInterface,-1);
	LayerAddObject("realize",&BattleInterface,-1);

	SetEventHandler("BI_CommandEndChecking","WM_CommandEndChecking",0);
	SetEventHandler("BI_LaunchCommand","WM_LaunchCommand",0);
	SetEventHandler("WM_SetPossibleCommands","WM_SetPossibleCommands",0);
	SetEventHandler("WM_UpdateCurrentAction","WM_UpdateCurrentAction",0);

	CreateILogAndActions(LOG_FOR_WORLDMAP);
	Log_SetActiveAction("EnterToSea");

	SetEventHandler("BI_UpdateWmInterface","BI_UpdateWmInterface",0);
	PostEvent("BI_UpdateWmInterface",1000);
	SetEventHandler("Control Activation","WM_ProcessControlPress",0); // boal
}
// boal -->
void WM_ProcessControlPress()
{
	string ControlName = GetEventData();
	if( sti(InterfaceStates.Launched) != 0 ) {return;}

	switch(ControlName)
	{
		case "BICommandsActivate": 
			PlaySound("interface\ok.wav"); // boal даешь звуки!
		break;
	}
}
// boal <--

void BI_UpdateWmInterface()
{
	SendMessage(&BattleInterface,"l",BI_MSG_REFRESH);
	PostEvent("BI_UpdateWmInterface",1000);
}

void DeleteWmInterface()
{
	DeleteClass(&BattleInterface);
	DelEventHandler("BI_CommandEndChecking", "WM_CommandEndChecking");
	DelEventHandler("BI_LaunchCommand", "WM_LaunchCommand");
	DelEventHandler("WM_SetPossibleCommands","WM_SetPossibleCommands");
	DelEventHandler("WM_UpdateCurrentAction","WM_UpdateCurrentAction");

	DelEventHandler("BI_UpdateWmInterface","BI_UpdateWmInterface");
	DelEventHandler("Control Activation","WM_ProcessControlPress"); // boal
}

ref WM_CommandEndChecking()
{
	BI_retComValue = 0;
	string comName = GetEventData();

	switch(comName)
	{
	case "cancel":
		BI_retComValue = -1;
		break;
	}

	return &BI_retComValue;
}

void WM_LaunchCommand()
{
	int charIdx = GetEventData();
	string commandName = GetEventData();
	int targetNum = GetEventData();
	string locName = GetEventData();

	ref chRef = GetCharacter(charIdx);
	if( LAi_IsDead(chRef) ) return;

	aref arFader;
	if( FindClass(arFader,"fader") ) {return;}

	if(targetNum==-1 && locName=="cancel") {
		SendMessage(&BattleInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}
	if(commandName=="cancel") {
		SendMessage(&BattleInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}

	switch(commandName)
	{
	case "EnterToSea":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	case "EnterToIsland":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	case "EnterToShip":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	case "EnterToStorm":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	case "EnterToAttack":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	case "EnterToEnemy":
		SendMessage(&worldMap,"l",MSG_WORLDMAP_LAUNCH_EXIT_TO_SEA);
		break;
	}

	BI_SetSpecCommandMode(BI_COMMODE_MY_SHIP_SELECT,-1,-1,-1, 0);
}

void WM_SetPossibleCommands()
{
	int chIdx = GetEventData();
	int mainIdx = sti(pchar.index);

	if( chIdx<0 || CharacterIsDead(GetCharacter(chIdx)) )
	{
		// отключим все команды
		aref aroot,arcur;
		makearef(aroot,BattleInterface.Commands);
		int q = GetAttributesNum(aroot);
		for(int i=0; i<q; i++)
		{
			arcur = GetAttributeN(aroot,i);
			arcur.enable = false;
		}
		return;
	}

	BattleInterface.Commands.Cancel.enable = true;
	BattleInterface.Commands.EnterToAttack.enable = false;
	BattleInterface.Commands.EnterToShip.enable	= false;
	BattleInterface.Commands.EnterToSea.enable = false;
	BattleInterface.Commands.EnterToIsland.enable = false;
	BattleInterface.Commands.EnterToStorm.enable = false;
	BattleInterface.Commands.EnterToEnemy.enable = false;

	bool bDefault = true;
	switch( sti(worldMap.encounter_type) )
	{
	case 0:
		BattleInterface.Commands.EnterToIsland.enable = true;
		//Log_SetActiveAction("EnterToIsland");
		Log_SetActiveAction("EnterToSea");  //boal
		bDefault = false;
		break;
	case 1:
		BattleInterface.Commands.EnterToShip.enable	= true;
		Log_SetActiveAction("EnterToShip");
		bDefault = false;
		break;
	case 2:
		BattleInterface.Commands.EnterToAttack.enable = true;
		Log_SetActiveAction("EnterToAttack");
		bDefault = false;
		break;
	case 3:
		BattleInterface.Commands.EnterToEnemy.enable = true;
		Log_SetActiveAction("EnterToEnemy");
		bDefault = false;
		break;
	case 4:
		BattleInterface.Commands.EnterToStorm.enable = true;
		Log_SetActiveAction("EnterToStorm");
		bDefault = false;
		break;
	}

	if( bDefault )
	{
		if( sti(worldMap.encounter_island) ) {
			BattleInterface.Commands.EnterToIsland.enable = true;
			//Log_SetActiveAction("EnterToIsland");
			Log_SetActiveAction("EnterToSea");  //boal
		} else {
			BattleInterface.Commands.EnterToSea.enable = true;
			Log_SetActiveAction("EnterToSea");
		}
	}
}

void WM_UpdateCurrentAction()
{
	bool bDefault = true;
	switch( sti(worldMap.encounter_type) )
	{
	case 0:
		//Log_SetActiveAction("EnterToIsland");
		Log_SetActiveAction("EnterToSea");  //boal
		bDefault = false;
		break;
	case 1:
		Log_SetActiveAction("EnterToShip");
		bDefault = false;
		break;
	case 2:
		Log_SetActiveAction("EnterToAttack");
		bDefault = false;
		break;
	case 3:
		Log_SetActiveAction("EnterToEnemy");
		bDefault = false;
		break;
	case 4:
		Log_SetActiveAction("EnterToStorm");
		bDefault = false;
		break;
	}
	if( bDefault ) {
		if( sti(worldMap.encounter_island) ) {
			//Log_SetActiveAction("EnterToIsland");
			Log_SetActiveAction("EnterToSea");  //boal
		} else {
			Log_SetActiveAction("EnterToSea");
		}
	}
	SendMessage(&BattleInterface,"l",BI_MSG_REFRESH);
}

void WM_InitializeCommands()
{
	int idLngFile = LanguageOpenFile("commands_name.txt");

	BattleInterface.Commands.Cancel.enable			= false;
	BattleInterface.Commands.Cancel.picNum			= 1;
	BattleInterface.Commands.Cancel.selPicNum		= 0;
	BattleInterface.Commands.Cancel.texNum			= 0;
	BattleInterface.Commands.Cancel.event			= "Cancel";
	BattleInterface.Commands.Cancel.note			= LanguageConvertString(idLngFile, "sea_Cancel");

	BattleInterface.Commands.EnterToSea.enable		= false;
 	BattleInterface.Commands.EnterToShip.picNum		= 1;
	BattleInterface.Commands.EnterToShip.selPicNum	= 9;
	BattleInterface.Commands.EnterToSea.texNum		= 1;
	BattleInterface.Commands.EnterToSea.event		= "EnterToSea";
	BattleInterface.Commands.EnterToSea.note		= LanguageConvertString(idLngFile, "worldmap_sea");

	BattleInterface.Commands.EnterToIsland.enable		= false;
	BattleInterface.Commands.EnterToIsland.picNum		= 1;//0;
	BattleInterface.Commands.EnterToIsland.selPicNum	= 9;//8;
	BattleInterface.Commands.EnterToIsland.texNum		= 1;
	BattleInterface.Commands.EnterToIsland.event		= "EnterToIsland";
	BattleInterface.Commands.EnterToIsland.note			= LanguageConvertString(idLngFile, "worldmap_sea");

	BattleInterface.Commands.EnterToShip.enable		= false;
	BattleInterface.Commands.EnterToSea.picNum		= 4;
	BattleInterface.Commands.EnterToSea.selPicNum	= 12;
	BattleInterface.Commands.EnterToShip.texNum		= 1;
	BattleInterface.Commands.EnterToShip.event		= "EnterToShip";
	BattleInterface.Commands.EnterToShip.note		= LanguageConvertString(idLngFile, "worldmap_sea");

	BattleInterface.Commands.EnterToStorm.enable		= false;
	BattleInterface.Commands.EnterToStorm.picNum		= 5;
	BattleInterface.Commands.EnterToStorm.selPicNum		= 13;
	BattleInterface.Commands.EnterToStorm.texNum		= 1;
	BattleInterface.Commands.EnterToStorm.event			= "EnterToStorm";
	BattleInterface.Commands.EnterToStorm.note			= LanguageConvertString(idLngFile, "worldmap_sea");

	BattleInterface.Commands.EnterToAttack.enable		= false;
	BattleInterface.Commands.EnterToAttack.picNum		= 2;
	BattleInterface.Commands.EnterToAttack.selPicNum	= 10;
	BattleInterface.Commands.EnterToAttack.texNum		= 1;
	BattleInterface.Commands.EnterToAttack.event		= "EnterToAttack";
	BattleInterface.Commands.EnterToAttack.note			= LanguageConvertString(idLngFile, "worldmap_sea");

	BattleInterface.Commands.EnterToEnemy.enable	= false;
	BattleInterface.Commands.EnterToEnemy.picNum	= 3;
	BattleInterface.Commands.EnterToEnemy.selPicNum	= 11;
	BattleInterface.Commands.EnterToEnemy.texNum	= 1;
	BattleInterface.Commands.EnterToEnemy.event		= "EnterToEnemy";
	BattleInterface.Commands.EnterToEnemy.note		= LanguageConvertString(idLngFile, "worldmap_sea");

	LanguageCloseFile(idLngFile);

	worldMap.encounter_type = -1;
}

void WM_SetParameterData()
{
	BattleInterface.CommandTextures.list.t0.name = "battle_interface\cancel.tga";
	BattleInterface.CommandTextures.list.t0.xsize = 2;
	BattleInterface.CommandTextures.list.t0.ysize = 1;

	BattleInterface.CommandTextures.list.t1.name = "battle_interface\worldmapcommands.tga";
	BattleInterface.CommandTextures.list.t1.xsize = 8;
	BattleInterface.CommandTextures.list.t1.ysize = 2;

	BattleInterface.wm_sign.fontid			= "interface_normal";
	BattleInterface.wm_sign.fontcolor		= argb(255,255,255,255);
	BattleInterface.wm_sign.fontscale		= 1.1;

	BattleInterface.wm_sign.shipnamefontid			= "interface_normal";
	BattleInterface.wm_sign.shipnamefontcolor		= argb(255,255,255,255);
	BattleInterface.wm_sign.shipnamefontscale		= 1.1;

	BattleInterface.wm_sign.backtexturename		= "battle_interface\ShipBackIcon.tga";
	BattleInterface.wm_sign.backcolor				= argb(255,128,128,128);
	BattleInterface.wm_sign.backuv					= "0.0,0.0,1.0,1.0";
	BattleInterface.wm_sign.backoffset				= "0.0,0.0";
	BattleInterface.wm_sign.backiconsize			= "128,128";

	BattleInterface.wm_sign.shipstatetexturename	= "battle_interface\ShipState.tga";
	BattleInterface.wm_sign.shipstatecolor			= argb(255,128,128,128);
	BattleInterface.wm_sign.shiphpuv				= "0.0,0.109,0.5,0.6875";
	BattleInterface.wm_sign.shiphpoffset			= "-32,-13";
	BattleInterface.wm_sign.shiphpiconsize			= "64,74";
	BattleInterface.wm_sign.shipspuv				= "0.5,0.109,1.0,0.6875";
	BattleInterface.wm_sign.shipspoffset			= "32,-13";
	BattleInterface.wm_sign.shipspiconsize			= "64,74";

	BattleInterface.wm_sign.shipclasstexturename	= "battle_interface\ShipClass.tga";
	BattleInterface.wm_sign.shipclasscolor			= argb(255,128,128,128);
	BattleInterface.wm_sign.shipclassuv			= "0.0,0.0,1.0,1.0";
	BattleInterface.wm_sign.shipclassoffset		= "-14,-50";
	BattleInterface.wm_sign.shipclassiconsize		= "64,16";
	BattleInterface.wm_sign.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

	BattleInterface.wm_sign.shiptexturename		= "battle_interface\ship_icons2.tga";
	BattleInterface.wm_sign.shipcolor				= argb(255,128,128,128);
	BattleInterface.wm_sign.shipoffset				= "-14,-12";
	BattleInterface.wm_sign.shipiconsize			= "64,64";

	BattleInterface.wm_sign.commandlistverticaloffset = -40;

	BattleInterface.wm_sign.iconoffset1 = "70,70";
	BattleInterface.wm_sign.iconoffset2 = "70,198";
	BattleInterface.wm_sign.iconoffset3 = "70,326";
	BattleInterface.wm_sign.iconoffset4 = "70,454";
	BattleInterface.wm_sign.iconoffset5 = "70,582";

	BattleInterface.CommandList.CommandMaxIconQuantity = 8;
	BattleInterface.CommandList.CommandIconSpace = 1;
	BattleInterface.CommandList.CommandIconLeft = 108;//157; boal
	BattleInterface.CommandList.CommandIconWidth = RecalculateHIcon(48);
	BattleInterface.CommandList.CommandIconHeight = RecalculateVIcon(48);

	BattleInterface.CommandList.CommandNoteFont = "interface_normal";
	BattleInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
	BattleInterface.CommandList.CommandNoteScale = 1.0;
	BattleInterface.CommandList.CommandNoteOffset = RecalculateHIcon(0) + "," + RecalculateVIcon(-42);

	BattleInterface.CommandList.CommandNoteFont = "interface_normal";
	BattleInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
	BattleInterface.CommandList.CommandNoteScale = 1.0;
	BattleInterface.CommandList.CommandNoteOffset = RecalculateHIcon(0) + "," + RecalculateVIcon(-42);

	BattleInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.tga";
	BattleInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
	BattleInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
	BattleInterface.CommandList.UDArrow_Size = RecalculateHIcon(32) + "," + RecalculateVIcon(32);
	BattleInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(-41) + "," + RecalculateVIcon(-30);
	BattleInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(-41) + "," + RecalculateVIcon(46);

	/*BattleInterface.CommandList.ActiveIcon_Texture = "battle_interface\enter_list.tga";
	BattleInterface.CommandList.ActiveIcon_Offset = RecalculateHIcon(-49) + ",0";
	BattleInterface.CommandList.ActiveIcon_Size = RecalculateHIcon(48) + "," + RecalculateVIcon(48);
	BattleInterface.CommandList.ActiveIcon_UV1 = "0.5,0.0,1.0,1.0";
	BattleInterface.CommandList.ActiveIcon_UV2 = "0.0,0.0,0.5,1.0";
	BattleInterface.CommandList.ActiveIcon_Note = XI_ConvertString("MenuNote"); */

	BattleInterface.maincharindex = pchar.index;

	WM_SetShipData();
}

void WM_SetShipData()
{
	int i,n,cn;
	string signattr;
	float uvleft,uvtop, uvright,uvbottom;

	i = 1;
	for(n=0; n<COMPANION_MAX; n++)
	{
		cn = GetCompanionIndex(pchar,n);
		if( cn!=-1 )
		{
			signattr = "sign" + i;
			BattleInterface.wm_sign.(signattr).leftprogress = GetHullPercent(&Characters[cn]) * 0.01;
			BattleInterface.wm_sign.(signattr).rightprogress = GetSailPercent(&Characters[cn]) * 0.01;
			BattleInterface.wm_sign.(signattr).starprogress = GetStarProgressByValue( GetCharacterShipClass(&Characters[cn]) );
			GetTextureUVForShip( sti(RealShips[sti(characters[cn].Ship.Type)].basetype), &uvleft,&uvtop, &uvright,&uvbottom);
			BattleInterface.wm_sign.(signattr).faceuv = uvleft+","+uvtop + "," + uvright+","+uvbottom;
			BattleInterface.wm_sign.(signattr).text = GetCrewQuantity(&Characters[cn]);
			i++;
		}
	}
}

float GetStarProgressByValue(int n)
{
	float f = 0.0;
	switch( n )
	{
	case 1: f = 0.219; break;
	case 2: f = 0.359; break;
	case 3: f = 0.5; break;
	case 4: f = 0.641; break;
	case 5: f = 0.781; break;
	case 6: f = 0.983; break;
	}
	return f;
}