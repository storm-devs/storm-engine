#include "battle_interface\msg_control.h"

int NetBI_intNRetValue[8];
int NetBI_ChargeState[5];
int NetBI_retComValue;

object NetBInterface;

//chref.skills.repair.active > 0
//chref.skills.repair.totalcooldowntime = int max (>=0)
//chref.skills.repair.currentcooldowntime = int cur (>=0)
//NetClient_ShipStartRepair()

void NetBI_CreateShipPointer()
{
	objShipPointer.Server = false;
	objShipPointer.textures.friend = "battle_interface\Frendly.tga";
	objShipPointer.textures.enemy = "battle_interface\Enemy.tga";
	
	CreateEntity(&objShipPointer, "shippointer");
	
	LayerAddObject("net_execute", &objShipPointer, 222222);
	LayerAddObject("net_realize", &objShipPointer, -1);
}

void NetBI_SelectShipPointer(int wClientID)
{
	int isEnemy = true;
	float fShSpeed = 0.005;
	float fShAmp = 1.0;
	float fShW = 1.0;
	float fShH = 1.0;
	float fTop = 5.0;

	SendMessage(&objShipPointer, "lllfffff", MSG_SP_CHANGESHIP, wClientID, isEnemy, fShSpeed, fShAmp, fShW, fShH, fTop);
}

void Net_GetTextureUVForShip(int nShipType, ref rLeft, ref rTop, ref rRight, ref rBottom)
{
	float fLeft = 0.0;
	float fTop = 0.0;
	float fRight = 0.125;
	float fBottom = 0.25;

	SetShipPictureDataByShipTypeName( NetShips[nShipType].name );
	int nV = BI_intNRetValue[0] / 8;
	int nH = BI_intNRetValue[0] - nV * 8;
	fLeft = stf(nH) * 0.125;
	fTop = stf(nV) * 0.25;
	fRight = fLeft + 0.125;
	fBottom = fTop + 0.25;

	rLeft = fLeft;
	rTop = fTop;
	rRight = fRight;
	rBottom = fBottom;
}

int Net_GetIntactCannonsNum(int wNetClientID)
{
	ref chref = NetClient_GetClient(wNetClientID);
	int nShipIdx = sti(chref.ship.type);

	int q = GetBortIntactCannonsNum(chref, "cannonf", sti(NetShips[nShipIdx].fcannon)) +
		GetBortIntactCannonsNum(chref, "cannonb", sti(NetShips[nShipIdx].bcannon)) +
		GetBortIntactCannonsNum(chref, "cannonl", sti(NetShips[nShipIdx].lcannon)) +
		GetBortIntactCannonsNum(chref, "cannonr", sti(NetShips[nShipIdx].rcannon));
	return q;
}

int Net_GetHullPercent(ref chref)
{
	if(!CheckAttribute(chref,"Ship.HP")) return 100;
	if(!CheckAttribute(chref,"Ship.MaxHP")) return 100;
	int iret = 100 * sti(chref.Ship.HP) / sti(chref.Ship.MaxHP);
	return iret;
}

int Net_GetSailPercent(ref chref)
{
	if(!CheckAttribute(chref,"Ship.SP")) return 100;
	int nShipIdx = sti(chref.ship.type);
	int iSP = sti(NetShips[nShipIdx].SP);
	if(iSP<=0) return 100;
	int iret = 100 * sti(chref.Ship.SP) / iSP;
	return iret;
}

void NetClient_CreateBattleInterface()
{
	NetBI_SetParameterData();
	NetBI_InitializeCommands();
	NetBI_InitBattleInterface();
	InterfaceSpyGlassInit(true);

	// log interface
	Log_RemoveFromSingleLayers();

	LayerAddObject("Net_Execute",&ILogAndActions,-257);
	LayerAddObject("Net_Realize",&ILogAndActions,-1);
}

void NetClient_DeleteBattleInterface()
{
	InterfaceSpyGlassRelease();
	NetBI_ReleaseBattleInterface();
}

void NetBI_SetParameterData()
{
	NetBInterface.CommandShowParam.maxShowQuantity = 8;
	NetBInterface.CommandShowParam.iconDistance = 4;
	NetBInterface.CommandShowParam.iconWidth = RecalculateHIcon(64);
	NetBInterface.CommandShowParam.iconHeight = RecalculateVIcon(64);
	NetBInterface.CommandShowParam.leftIconsOffset = sti(showWindow.left)+RecalculateHIcon(16);
	NetBInterface.CommandShowParam.downIconsOffset = sti(showWindow.bottom)-RecalculateVIcon(80);
	NetBInterface.CommandShowParam.buttonWidth = RecalculateHIcon(8);
	NetBInterface.CommandShowParam.buttonHeight = RecalculateVIcon(64);
	NetBInterface.CommandShowParam.buttonOffset = RecalculateHIcon(4);
	NetBInterface.CommandShowParam.buttonTexture = "battle_interface\lr_buttons.tga";
	NetBInterface.CommandShowParam.shipStateWidth = RecalculateHIcon(64);
	NetBInterface.CommandShowParam.shipStateHeight = RecalculateVIcon(16);
	NetBInterface.CommandShowParam.shipStateTexture = "battle_interface\indicators.tga";
	NetBInterface.CommandShowParam.shipStateOffset = RecalculateVIcon(0);
	NetBInterface.CommandShowParam.GeraldWidth = RecalculateHIcon(32);
	NetBInterface.CommandShowParam.GeraldHeight = RecalculateVIcon(32);
	NetBInterface.CommandShowParam.commandFont = "bold_numbers";
	NetBInterface.CommandShowParam.printXOffset = RecalculateHIcon(32);
	NetBInterface.CommandShowParam.printYOffset = RecalculateVIcon(-26);
	NetBInterface.CommandShowParam.commandNoteFont = "interface_normal";
	NetBInterface.CommandShowParam.noteXOffset = RecalculateHIcon(0);
	NetBInterface.CommandShowParam.noteYOffset = RecalculateVIcon(-30);
	NetBInterface.CommandShowParam.argbTFactorColor = argb(256,64,64,64);

	NetBInterface.navigation.aspectRatio			= showWindow.aspectRatio;
	NetBInterface.navigation.navigatorWidth			= RecalculateHIcon(256);
	NetBInterface.navigation.navigatorHeight		= RecalculateVIcon(256);
	NetBInterface.navigation.rightPos				= sti(showWindow.right) + RecalculateHIcon(24);
	NetBInterface.navigation.topPos					= sti(showWindow.top) - RecalculateVIcon(24);
	NetBInterface.navigation.speedShowFont			= "bold_numbers";
	NetBInterface.navigation.speedOutYOffset		= RecalculateVIcon(196);
	NetBInterface.navigation.shipSpeedXOffset		= RecalculateHIcon(30);
	NetBInterface.navigation.windSpeedXOffset		= RecalculateHIcon(-30);
	NetBInterface.navigation.fontScale				= 0.6;
	NetBInterface.navigation.windWidth				= 32;
	NetBInterface.navigation.windHeight				= 128;
	NetBInterface.navigation.compasTexture			= "battle_interface\compass.tga";
	NetBInterface.navigation.cannonsTexture			= "battle_interface\indicators_cannons_reload.tga";
	NetBInterface.navigation.emptyTexture			= "battle_interface\indicators_dark_and_center_ship.tga";
	NetBInterface.navigation.windTexture			= "battle_interface\wind_pointer.tga";
	NetBInterface.navigation.leftChargeBegAngle		= 214;
	NetBInterface.navigation.leftChargeEndAngle		= 326;
	NetBInterface.navigation.rightChargeBegAngle	= 146;
	NetBInterface.navigation.rightChargeEndAngle	= 34;
	NetBInterface.navigation.forwardChargeBegAngle	= 343;
	NetBInterface.navigation.forwardChargeEndAngle	= 378;
	NetBInterface.navigation.backwardChargeBegAngle	= 198;
	NetBInterface.navigation.backwardChargeEndAngle	= 162;
	NetBInterface.navigation.mapRadius				= 55;
	NetBInterface.navigation.horizontRadius			= 400;
	NetBInterface.navigation.minScale				= 0.5;
	NetBInterface.navigation.maxScale				= 4;
	NetBInterface.navigation.curScale				= 2;
	NetBInterface.navigation.scaleStep				= 0.1;
	NetBInterface.navigation.argbReadyCannonColor	= argb(255,0,155,0);
	NetBInterface.navigation.argbChargeCannonColor	= argb(255,0,255,0);
	NetBInterface.navigation.argbDamageCannonColor	= argb(255,255,0,0);
	NetBInterface.navigation.argbSeaColor			= argb(255,10,30,80);
	NetBInterface.navigation.argbFireZoneColor		= argb(60,250,250,250);
	NetBInterface.navigation.argbEnemyShipColor		= argb(255,255,0,0);
	NetBInterface.navigation.argbFrendShipColor		= argb(255,0,255,0);
	NetBInterface.navigation.argbNeutralShipColor	= argb(255,128,128,128);
	NetBInterface.navigation.argbDeadShipColor		= argb(255,0,0,255);
	NetBInterface.navigation.argbBackMaxColor		= argb(255,0,0,128);
	NetBInterface.navigation.argbBackMinColor		= argb(0,0,0,128);
	NetBInterface.navigation.shipShowRadius			= 8.0;

	NetBInterface.navigation.chargeTexture			= "battle_interface\list_icon2.tga";
	NetBInterface.navigation.chargeTextureGreed		= "8,8";
	NetBInterface.navigation.chargePos				= "0,"+RecalculateVIcon(270);
	NetBInterface.navigation.chargePictureSize		= RecalculateHIcon(48)+","+RecalculateVIcon(48);
	//
	NetBInterface.navigation.sailstateTexture		= "battle_interface\list_icon2.tga";
	NetBInterface.navigation.sailstateTextureGreed	= "8,8";
	NetBInterface.navigation.sailstatePos			= RecalculateHIcon(30)+","+RecalculateVIcon(170);
	NetBInterface.navigation.sailstatePictureSize	= RecalculateHIcon(48)+","+RecalculateVIcon(48);
	//
	NetBInterface.navigation.windStateTexture		= "battle_interface\list_icon2.tga";
	NetBInterface.navigation.windTextureGreed		= "8,8";
	NetBInterface.navigation.windPos				= RecalculateHIcon(-30)+","+RecalculateVIcon(170);
	NetBInterface.navigation.windPictureSize		= RecalculateHIcon(48)+","+RecalculateVIcon(48);

	NetBInterface.textinfo.Date.font = "interface_normal";
	NetBInterface.textinfo.Date.scale = 1.1;
	NetBInterface.textinfo.Date.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
	NetBInterface.textinfo.Date.pos.y = RecalculateVIcon(200);
	NetBInterface.textinfo.Date.text = XI_convertString("Date:")+GetDataDay()+" "+XI_ConvertString("target_month_" + GetDataMonth())+" "+GetDataYear();
	//
	NetBInterface.textinfo.Location.font = "interface_normal";
	NetBInterface.textinfo.Location.scale = 1.1;
	NetBInterface.textinfo.Location.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
	NetBInterface.textinfo.Location.pos.y = RecalculateVIcon(218);
	NetBInterface.textinfo.Location.text = XI_convertString("Open Sea");
	//
	NetBInterface.textinfo.Speed.font = "interface_normal";
	NetBInterface.textinfo.Speed.scale = 0.9;
	NetBInterface.textinfo.Speed.color = argb(255,255,255,255);
	NetBInterface.textinfo.Speed.shadow = false;
	NetBInterface.textinfo.Speed.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
	NetBInterface.textinfo.Speed.pos.y = RecalculateVIcon(315);
	NetBInterface.textinfo.Speed.text = XI_convertString("Speed");
	//
	NetBInterface.textinfo.Ammo.font = "interface_normal";
	NetBInterface.textinfo.Ammo.scale = 0.9;
	NetBInterface.textinfo.Ammo.color = argb(255,255,255,255);
	NetBInterface.textinfo.Ammo.shadow = false;
	NetBInterface.textinfo.Ammo.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
	NetBInterface.textinfo.Ammo.pos.y = RecalculateVIcon(399);
	NetBInterface.textinfo.Ammo.text = XI_convertString("Ammunition");
	NetBInterface.textinfo.Ammo.refreshable = true;

	NetBInterface.LineInfo.speed.color = argb(255,0,0,0);
	NetBInterface.LineInfo.speed.begin.x = sti(showWindow.right) - RecalculateHIcon(168);
	NetBInterface.LineInfo.speed.begin.y = RecalculateVIcon(334);
	NetBInterface.LineInfo.speed.end.x = RecalculateHIcon(128);
	NetBInterface.LineInfo.speed.end.y = 0;
	//
	NetBInterface.LineInfo.Ammo.color = argb(255,0,0,0);
	NetBInterface.LineInfo.Ammo.begin.x = sti(showWindow.right) - RecalculateHIcon(168);
	NetBInterface.LineInfo.Ammo.begin.y = RecalculateVIcon(418);
	NetBInterface.LineInfo.Ammo.end.x = RecalculateHIcon(128);
	NetBInterface.LineInfo.Ammo.end.y = 0;

	NetBInterface.battleborder.used = false;

	NetBInterface.ShipIcon.sailorfontid			= "interface_normal";
	NetBInterface.ShipIcon.sailorfontcolor		= argb(255,255,255,255);
	NetBInterface.ShipIcon.sailorfontscale		= 1.1;

	NetBInterface.ShipIcon.backtexturename		= "battle_interface\ShipBackIcon.tga";
	NetBInterface.ShipIcon.backcolor			= argb(255,128,128,128);
	NetBInterface.ShipIcon.backuv				= "0.0,0.0,1.0,1.0";
	NetBInterface.ShipIcon.backoffset			= "0.0,0.0";
	NetBInterface.ShipIcon.backiconsize			= "128,128";

	NetBInterface.ShipIcon.shipstatetexturename	= "battle_interface\ShipState.tga";
	NetBInterface.ShipIcon.shipstatecolor		= argb(255,128,128,128);
	NetBInterface.ShipIcon.shiphpuv				= "0.0,0.109,0.5,0.6875";
	NetBInterface.ShipIcon.shiphpoffset			= "-32,-13";
	NetBInterface.ShipIcon.shiphpiconsize		= "64,74";
	NetBInterface.ShipIcon.shipspuv				= "0.5,0.109,1.0,0.6875";
	NetBInterface.ShipIcon.shipspoffset			= "32,-13";
	NetBInterface.ShipIcon.shipspiconsize		= "64,74";

	NetBInterface.ShipIcon.shipclasstexturename	= "battle_interface\ShipClass.tga";
	NetBInterface.ShipIcon.shipclasscolor		= argb(255,128,128,128);
	NetBInterface.ShipIcon.shipclassuv			= "0.0,0.0,1.0,1.0";
	NetBInterface.ShipIcon.shipclassoffset		= "-14,-50";
	NetBInterface.ShipIcon.shipclassiconsize	= "64,16";
	NetBInterface.ShipIcon.gunchargeprogress	= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

	NetBInterface.ShipIcon.shiptexturename		= "battle_interface\ship_icons1.tga";
	NetBInterface.ShipIcon.shipcolor			= argb(255,128,128,128);
	NetBInterface.ShipIcon.shipoffset			= "-14,-12";
	NetBInterface.ShipIcon.shipiconsize			= "64,64";

	NetBInterface.ShipIcon.iconoffset1 = "70,70";
	NetBInterface.ShipIcon.iconoffset2 = "70,198";
	NetBInterface.ShipIcon.iconoffset3 = "70,326";
	NetBInterface.ShipIcon.iconoffset4 = "70,454";

	NetBInterface.ShipIcon.commandlistverticaloffset = -40;

	NetBInterface.CommandList.CommandMaxIconQuantity = 8;
	NetBInterface.CommandList.CommandIconSpace = 1;
	NetBInterface.CommandList.CommandIconLeft = 157;
	NetBInterface.CommandList.CommandIconWidth = RecalculateHIcon(48);
	NetBInterface.CommandList.CommandIconHeight = RecalculateVIcon(48);

	NetBInterface.CommandList.CommandNoteFont = "interface_normal";
	NetBInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
	NetBInterface.CommandList.CommandNoteScale = 1.0;
	NetBInterface.CommandList.CommandNoteOffset = RecalculateHIcon(0) + "," + RecalculateVIcon(-42);

	NetBInterface.CommandList.ActiveIcon_Texture = "battle_interface\enter_list.tga";
	NetBInterface.CommandList.ActiveIcon_Offset = RecalculateHIcon(-49) + ",0";
	NetBInterface.CommandList.ActiveIcon_Size = RecalculateHIcon(48) + "," + RecalculateVIcon(48);
	NetBInterface.CommandList.ActiveIcon_UV1 = "0.5,0.0,1.0,1.0";
	NetBInterface.CommandList.ActiveIcon_UV2 = "0.0,0.0,0.5,1.0";
	NetBInterface.CommandList.ActiveIcon_Note = XI_ConvertString("MenuNote");
}

void NetBI_InitializeCommands()
{
	int idLngFile = LanguageOpenFile("commands_name.txt");

	NetBInterface.CommandTextures.list.t0.name = "battle_interface\list_icon2.tga";
	NetBInterface.CommandTextures.list.t0.xsize = 8;
	NetBInterface.CommandTextures.list.t0.ysize = 8;

	NetBInterface.CommandTextures.ChargeTexNum = 0;
	NetBInterface.CommandTextures.CommandTexNum = 0;

	NetBInterface.Commands.Cancel.enable	= false;
	NetBInterface.Commands.Cancel.picNum	= 50;
	NetBInterface.Commands.Cancel.selPicNum	= 58;
	NetBInterface.Commands.Cancel.texNum	= 0;
	NetBInterface.Commands.Cancel.event		= "Cancel";
	NetBInterface.Commands.Cancel.note		= LanguageConvertString(idLngFile, "sea_Cancel");
	//
	NetBInterface.Commands.Charge.enable	= false;
	NetBInterface.Commands.Charge.picNum	= 21;
	NetBInterface.Commands.Charge.selPicNum	= 29;
	NetBInterface.Commands.Charge.texNum	= 0;
	NetBInterface.Commands.Charge.event		= "BI_Charge";
	NetBInterface.Commands.Charge.note		= LanguageConvertString(idLngFile, "sea_Charge");
	//
	NetBInterface.Commands.Speed.enable		= false;
	NetBInterface.Commands.Speed.picNum		= 17;
	NetBInterface.Commands.Speed.selPicNum	= 25;
	NetBInterface.Commands.Speed.texNum		= 0;
	NetBInterface.Commands.Speed.event		= "BI_Speed";
	NetBInterface.Commands.Speed.note		= LanguageConvertString(idLngFile, "sea_Speed");
	//
	NetBInterface.Commands.Repair.enable	= false;
	NetBInterface.Commands.Repair.picNum	= 55;
	NetBInterface.Commands.Repair.selPicNum	= 63;
	NetBInterface.Commands.Repair.cooldownPicNum = 62;
	NetBInterface.Commands.Repair.texNum	= 0;
	NetBInterface.Commands.Repair.event		= "BI_Repair";
	NetBInterface.Commands.Repair.note		= LanguageConvertString(idLngFile, "sea_Repair");

	NetBInterface.charge.charge1.picNum		= 32; // balls
	NetBInterface.charge.charge1.selPicNum	= 40;
	NetBInterface.charge.charge2.picNum		= 35; // grapes
	NetBInterface.charge.charge2.selPicNum	= 43;
	NetBInterface.charge.charge3.picNum		= 34; // "Knippels"
	NetBInterface.charge.charge3.selPicNum	= 42;
	NetBInterface.charge.charge4.picNum		= 33; // bombs
	NetBInterface.charge.charge4.selPicNum	= 41;

	NetBInterface.UserIcons.ui1.enable	= true;
	NetBInterface.UserIcons.ui1.pic		= 20;
	NetBInterface.UserIcons.ui1.selpic	= 28;
	NetBInterface.UserIcons.ui1.tex		= 0;
	NetBInterface.UserIcons.ui1.name	= "sail_none";
	//----------------------------------------------
	NetBInterface.UserIcons.ui2.enable	= true;
	NetBInterface.UserIcons.ui2.pic		= 19;
	NetBInterface.UserIcons.ui2.selpic	= 27;
	NetBInterface.UserIcons.ui2.tex		= 0;
	NetBInterface.UserIcons.ui2.name	= "sail_midi";
	//----------------------------------------------
	NetBInterface.UserIcons.ui3.enable	= true;
	NetBInterface.UserIcons.ui3.pic		= 18;
	NetBInterface.UserIcons.ui3.selpic	= 26;
	NetBInterface.UserIcons.ui3.tex		= 0;
	NetBInterface.UserIcons.ui3.name	= "sail_fast";

	LanguageCloseFile(idLngFile);
}

void NetBI_InitBattleInterface()
{

	NetBInterface.SelCharacter = -1;
	NetBInterface.boardRadius = 50;
	NetBInterface.freeDistance = MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER;
	NetBInterface.blindSpeed = 0.003;
	NetBInterface.MaxWind = 30.0;
	NetBInterface.MaxShipSpeed = 20.0;
	NetBInterface.ShowNavigator = true;
	NetBInterface.ShowCommands = true;
	if(iArcadeSails == 1) {
		NetBInterface.ShipSpeedScaler = 1.0 / 8.0;
	} else {
		NetBInterface.ShipSpeedScaler = 1.0 / 3.0;
	}

	NetBInterface.MainChrIndex = NetClient_GetThisClientID();
	CreateEntity(&NetBInterface,"battle_interface");
	NetBInterface.Server = false;
	DeleteAttribute(&NetBInterface,"navigation");
	DeleteAttribute(&NetBInterface,"ShipIcon");
	DeleteAttribute(&NetBInterface,"CommandList");
	LayerAddObject("Net_Execute",&NetBInterface,-1);
	LayerAddObject("Net_Realize",&NetBInterface,-1);

	SetEventHandler(BI_EVENT_GET_DATA,"NetBI_GetData",0);

	SetEventHandler("NCEvent_ShipCreate","NetBI_CreateShip",0);
	SetEventHandler("NCEvent_ShipDelete","NetBI_DeleteShip",0);
	SetEventHandler("NCEvent_ShipDead","NetBI_DeadShip",0);

	SetEventHandler("BI_CallUpdateShip","NetBI_UpdateShip",0);

	SetEventHandler("BI_SetPossibleCommands","NetBI_SetPossibleCommands",0);
	SetEventHandler("BI_CommandEndChecking","NetBI_CommandEndChecking",0);
	SetEventHandler("BI_LaunchCommand","NetBI_LaunchCommand",0);

	SetEventHandler("BI_GetChargeQuantity","NetBI_GetChargeQuantity",0);
	SetEventHandler("GetCurrentCharge","Net_GetCurrentCharge",0);

	SetEventHandler("neGetCooldownFactor","Net_GetCooldownFactor",0);

	SendMessage( &NetBInterface, "l", BI_MSG_REFRESH );

	SetEventHandler("DoSailHole","Net_ProcessSailDamage",0);
	//SetEventHandler("BI_CallUpdateShip","BI_UpdateShip",0);
	//SetEventHandler("evntPerkAgainUsable","BI_PerkAgainUsable",0);
	//SetEventHandler("evntSetUsingAbility","procSetUsingAbility",0);
}

void NetBI_ReleaseBattleInterface()
{
	DeleteClass( &NetBInterface );// BattleInterface
	DeleteClass( &objShipPointer );

	DelEventHandler(BI_EVENT_GET_DATA,"NetBI_GetData");

	DelEventHandler(SHIP_CREATE,"NetBI_CreateShip");
	DelEventHandler(SHIP_DELETE,"NetBI_DeleteShip");
	DelEventHandler(SHIP_DEAD,"NetBI_DeadShip");

	DelEventHandler("BI_CallUpdateShip","NetBI_UpdateShip");

	DelEventHandler("BI_SetPossibleCommands","NetBI_SetPossibleCommands");
	DelEventHandler("BI_CommandEndChecking","NetBI_CommandEndChecking");
	DelEventHandler("BI_LaunchCommand","NetBI_LaunchCommand");

	DelEventHandler("neGetCooldownFactor","Net_GetCooldownFactor");
	DelEventHandler("GetCurrentCharge","Net_GetCurrentCharge");
	DelEventHandler("BI_GetChargeQuantity","NetBI_GetChargeQuantity");
	DelEventHandler("DoSailHole","Net_ProcessSailDamage");

	Log_MoveToSingleLayers();
}

ref Net_GetCooldownFactor()
{
	string sComName = GetEventData();
	BI_g_fRetVal = 1.0;
	ref chref = NetClient_GetClient( NetClient_GetThisClientID() );

	if( sComName == "BI_Repair" ) {
		float f1 = stf(chref.skills.repair.totalcooldowntime);
		float f2 = stf(chref.skills.repair.currentcooldowntime);
		if( f1>0.0 && f2<f1 ) {
			BI_g_fRetVal = f2 / f1;
		}
		//trace("Repair Cooldown = "+ f2 + " / " + f1);
	}
	return &BI_g_fRetVal;
}

ref Net_GetCurrentCharge()
{
	int wChrID = NetClient_GetThisClientID();
	ref chref = NetClient_GetClient( wChrID );
	NetBI_intNRetValue[0] = -1;

	if( CheckAttribute(chref,"Ship.Cannons.Charge.Type") )
	{
		switch(sti(chref.Ship.Cannons.Charge.Type))
		{
		case GOOD_BALLS:
			NetBI_intNRetValue[0] = 32;
			NetBInterface.textinfo.Ammo.text = XI_convertString("Balls")+": "+sti(chref.ship.goods.balls);
			break;
		case GOOD_GRAPES:
			NetBI_intNRetValue[0] = 35;
			NetBInterface.textinfo.Ammo.text = XI_convertString("Grapes")+": "+sti(chref.ship.goods.grapes);
			break;
		case GOOD_KNIPPELS:
			NetBI_intNRetValue[0] = 34;
			NetBInterface.textinfo.Ammo.text = XI_convertString("Knippels")+": "+sti(chref.ship.goods.knippels);
			break;
		case GOOD_BOMBS:
			NetBI_intNRetValue[0] = 33;
			NetBInterface.textinfo.Ammo.text = XI_convertString("Bombs")+": "+sti(chref.ship.goods.bombs);
			break;
		}
	}

	float fState = NetClient_ShipGetSailState(wChrID);
	if( fState < 0.33 ) {
		NetBI_intNRetValue[1] = 20;
	} else {
		if( fState < 0.66 ) {
			NetBI_intNRetValue[1] = 19;
		} else {
			NetBI_intNRetValue[1] = 18;
		}
	}

	NetBI_intNRetValue[2] = 61;

	return &NetBI_intNRetValue;
}

void NetBI_CreateShipPrivate(int wNetClientID)
{
	int wNetThisClient = NetClient_GetThisClientID();

	ref rc = NetClient_GetClient(wNetClientID);

	bool myShip = false;
	int shipRelation = BI_RELATION_ENEMY;
	if( wNetClientID==wNetThisClient )
	{
		myShip = true;
		shipRelation = BI_RELATION_FRIEND;
	}

	ref shipRef = Net_GetShipByIndex( sti(rc.ship.type) );

	int iColor = argb(255,255,0,0); // enemy ship
	if( CheckAttribute(&NetBInterface,"navigation.argbEnemyShipColor") ) {
		iColor = sti(NetBInterface.navigation.argbEnemyShipColor);
	}
	if (sti(NCServer.GameType) == NETGAME_DEATHMATCH)
	{
		if (wNetClientID == wNetThisClient) { // our ship
			iColor = argb(255,0,255,0);
			if( CheckAttribute(&NetBInterface,"navigation.argbFrendShipColor") ) {
				iColor = sti(NetBInterface.navigation.argbFrendShipColor);
			}
		}
	}
	else
	{
		iColor = NetTeamColor[sti(rc.Team)];
	}

	SendMessage(&NetBInterface, "llaalll", BI_IN_CREATE_SHIP, wNetClientID, rc, shipRef, myShip, shipRelation, iColor);
}

void NetBI_UpdateShip()
{
	int wNetClientID = GetEventData();
	NetBI_CreateShipPrivate(wNetClientID);
}

void NetBI_CreateShip()
{
	int wNetClientID = GetEventData();
	NetBI_CreateShipPrivate(wNetClientID);
}

void NetBI_DeleteShip()
{
	RefreshBattleInterface();
}

void NetBI_DeadShip()
{
	RefreshBattleInterface();
}

void NetBI_SetPossibleCommands()
{
	int chIdx = GetEventData();

	// для главного персонажа
	NetBInterface.Commands.Charge.enable	= true;//CheckAttribute(mainCh,"Ship.Cannons.Charge.Type");
	NetBInterface.Commands.Speed.enable		= true;
	NetBInterface.Commands.Repair.enable	= true;
}

ref NetBI_GetCooldownFactor()
{
	ref chref = NetClient_GetClient( NetClient_GetThisClientID() );
}

ref NetBI_CommandEndChecking()
{
	NetBI_retComValue = 0;
	string comName = GetEventData();

	switch(comName)
	{
	case "BI_Charge":
		NetBI_retComValue = BI_COMMODE_CANNON_CHARGE;
		break;
	case "BI_Speed":
		NetBI_retComValue = BI_COMMODE_USER_ICONS;
		break;
	case "BI_Repair":
		NetBI_retComValue = 0;
		break;
	}

	return &NetBI_retComValue;
}

void NetBI_LaunchCommand()
{
	int charIdx = GetEventData();
	string commandName = GetEventData();
	int targetNum = GetEventData();
	string locName = GetEventData();

	if(targetNum==-1 && locName=="cancel") commandName = "";

	switch(commandName)
	{
	case "BI_Charge":
		string sChargeType="Balls";
		switch(targetNum)
		{
		case 1:
			//Log_SetStringToLog("Cannonballs");
			PlaySound("interface\_balls.wav");
			sChargeType="Balls";
			break;
		case 2:
			//Log_SetStringToLog("Grapes");
			PlaySound("interface\_grapes.wav");
			sChargeType="Grapes";
			break;
		case 3:
			//Log_SetStringToLog("Knippels");
			PlaySound("interface\_chain.wav");
			sChargeType="Knippels";
			break;
		case 4:
			//Log_SetStringToLog("Bombs");
			PlaySound("interface\_bombs.wav");
			sChargeType="Bombs";
			break;
		}
		NetClient_ShipChangeCharge(sChargeType);
		break;
	case "BI_Speed":
		switch(locName)
		{
		case "sail_none":
			NetClient_ShipSetSailState(0.0);
			break;
		case "sail_midi":
			NetClient_ShipSetSailState(0.5);
			break;
		case "sail_fast":
			NetClient_ShipSetSailState(1.0);
			break;
		}
		break;
	case "BI_Repair":
		NetClient_ShipStartRepair();
		break;
	}
}

ref NetBI_GetChargeQuantity()
{
	int chIdx = GetEventData();
	NetBI_ChargeState[0] = 4;
	NetBI_ChargeState[1] = 100;//GetCargoGoods(refChar,GOOD_BALLS);
	NetBI_ChargeState[2] = 100;//GetCargoGoods(refChar,GOOD_GRAPES);
	NetBI_ChargeState[3] = 100;//GetCargoGoods(refChar,GOOD_KNIPPELS);
	NetBI_ChargeState[4] = 100;//GetCargoGoods(refChar,GOOD_BOMBS);

	return &NetBI_ChargeState;
}

ref NetBI_GetData()
{
	int dataType = GetEventData();
	int wNetClientID;
	ref chRef;

	switch(dataType)
	{
		// Получаем номер картинки корабля
	case BIDT_SHIPPICTURE:
		wNetClientID = GetEventData();
		chRef = NetClient_GetClient(wNetClientID);
		int st = sti(chRef.ship.type);
		//st = sti(RealShips[st].basetype);
		SetShipPictureDataByShipType( st );
		break;
	}
	return &BI_intNRetValue;
}

ref Net_ProcessSailDamage()
{
	// от кого удар
	int shootIdx = GetEventData();
	// перс
	int wNetClientID = GetEventData();

	string sMastName = GetEventData();
	// координаты паруса
	string reyName = GetEventData();
	int groupNum = GetEventData();
	// данные о дырках
	int holeCount = GetEventData();
	int holeData = GetEventData();
	int maxHoleCount = GetEventData();
	// мощность паруса
	float sailPower = GetEventData();

	ref chref = NetServer_GetClient(wNetClientID);//NetClient_GetClient(wNetClientID);

	if (LAi_IsImmortal(chref)) 
	{ 
		BI_g_fRetVal = 0;
		return &BI_g_fRetVal;
	}

	string groupName = ""+groupNum;
	aref arSail;
	makearef(arSail,chref.ship.sails.(reyName).(groupName));

	float sailDmg = 0.0;
	float sailDmgMax = 100.0 * sailPower;
	if( !CheckAttribute(arSail,"dmg") )	{ sailDmg = 0.0; }

	if(sMastName=="*")
	{
		sailDmg = sailDmg + Net_GetRigDamage(shootIdx,sti(NSBalls.CurrentBallType),chref);
		if(sailDmg>sailDmgMax)	{ sailDmg = sailDmgMax; }
		int needHole = GetNeedHoleFromDmg(sailDmg,sailDmgMax,maxHoleCount);
		if(holeCount!=needHole)
		{
			if(holeCount<needHole)
			{
				holeData = RandomHole2Sail(wNetClientID,reyName,groupNum, maxHoleCount, holeData, needHole-holeCount);
				holeCount = needHole;
			}
			else
			{
				sailDmg = GetNeedDmgFromHole(holeCount,sailDmgMax,maxHoleCount);
			}
		}
	}
	else
	{
		if(sMastName!="#")	{ arSail.mastFall = sMastName; }
		sailDmg = sailDmgMax;
	}

/*	if (sailDmg < 0.5)
	{
		Log_SetStringtoLog("MUST DIE!!! " + sailDmg);
	}*/

	arSail.hc = holeCount;
	arSail.hd = holeData;
	arSail.mhc = maxHoleCount;
	arSail.sp = sailPower;
	arSail.dmg = sailDmg;

	chref.ship.SP = Net_CalculateShipSP(chref);
	BI_g_fRetVal = sailDmg;
	return &BI_g_fRetVal;
}

float Net_GetRigDamage(int shootIdx, int iBallType, ref damage_chr)
{
	ref rBall = Net_GetGoodByIndex(iBallType);
	float fDmgRig = stf(rBall.DamageRig);
	fDmgRig = fDmgRig * stf(damage_chr.ship.SailsDamageModifier);
	return fDmgRig;
}

//================================================================
// Посчитать состояние парусов
//================================================================
float Net_CalculateShipSP(ref chref)
{
	float fSP = 100;//GetCharacterShipSP(chref);
	aref arRoot,arGroup,arSail;
	int q,n,i,j;

	makearef(arRoot, chref.ship.sails);
	q = GetAttributesNum(arRoot);
	for(i=0; i<q; i++)
	{
		arGroup = GetAttributeN(arRoot,i);
		n = GetAttributesNum(arGroup);
		for(j=0; j<n; j++)
		{
			arSail = GetAttributeN(arGroup,j);
			fSP -= stf(arSail.dmg);
		}
	}

	if(fSP<0.0) fSP = 0.0;
	return fSP;
}