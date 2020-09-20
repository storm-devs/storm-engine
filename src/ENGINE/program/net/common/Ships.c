object NetShips[300];
int iNetShipsNum = 0;

int Net_GetShipsNum() { return iNetShipsNum; }
ref Net_GetShipByName(string sName) { return &NetShips[Net_GetShipIndex(sName)]; }
ref Net_GetShipByIndex(int iIndex) { return &NetShips[iIndex]; }
int Net_GetShipIndex(string sName) 
{ 
	for (int i=0; i<iNetShipsNum; i++) 
	{
		if (NetShips[i].Name == sName) { return i; }
	}
	Trace("ERROR: Can't find NetShip" + sName);
	return -1; 
}

int Net_GetShipCannonsNum(int iShipIndex) 
{ 
	if (iShipIndex < 0) { return 0; }
	return sti(NetShips[iShipIndex].CannonsQuantity); 
}

int Net_GetShipCost(int iShipIndex) 
{
	if (iShipIndex < 0) { return 0; }
	return sti(NetShips[iShipIndex].Cost); 
}

int Net_GetShipUpgradeHullCost(int iShipIndex, int iHullUpgradeIndex)
{
	if (iShipIndex < 0) { return 0; }
	if (iHullUpgradeIndex < 0) { return 0; }

	string sHullUpgrade = "Hull" + (iHullUpgradeIndex + 1);
	return sti(NetShips[iShipIndex].Upgrades.(sHullUpgrade)); 
}

int Net_GetShipUpgradeRiggingCost(int iShipIndex, int iRiggingUpgradeIndex)
{
	if (iShipIndex < 0) { return 0; }
	if (iRiggingUpgradeIndex < 0) { return 0; }

	string sRiggingUpgrade = "Rigging" + (iRiggingUpgradeIndex + 1);
	return sti(NetShips[iShipIndex].Upgrades.(sRiggingUpgrade)); 
}

void Net_InitShips()
{
	ref rMassive;
    ref refShip;

	makeref(rMassive, NetShips);
	int iShipMax = GetArraySize(rMassive);

	int n = 0;

	for (int idx=0; idx<iShipMax; idx++)
	{
	    makeref(refShip, NetShips[idx]);

		refship.index = idx;

		refShip.TradeOff = false;

		refship.Soundtype = "lugger";

		refship.InertiaAccelerationX = 0.2;		refship.InertiaBrakingX = 0.7;
		refship.InertiaAccelerationY = 0.4;		refship.InertiaBrakingY = 0.4;
		refship.InertiaAccelerationZ = 1.0;		refship.InertiaBrakingZ = 1.0;

		refship.Rocking.y = 0.5;
		refship.Rocking.az = 0.05;
		refShip.Rocking.factor = 0.025;

		refship.WaterLine = 0.0;
		refship.SpeedDependWeight = 0.0;
		refship.SubSeaDependWeight = 0.0;
		refship.TurnDependWeight = 0.0;
		refship.TurnDependSpeed = 0.0;

		refShip.Height.Bombs.Y		= 3.0;		refShip.Height.Bombs.DY		= 1.0;
		refShip.Height.Grapes.Y		= 4.0;		refShip.Height.Grapes.DY	= 1.0;
		refShip.Height.Knippels.Y	= 25.0;		refShip.Height.Knippels.DY	= 15.0;
		refShip.Height.Balls.Y		= 3.0;		refShip.Height.Balls.DY		= 1.0;

		refship.Upgrades.Hull1		= 0;
		refship.Upgrades.Hull2		= 1000;
		refship.Upgrades.Hull3		= 20000;
		refship.Upgrades.Rigging1	= 0;
		refship.Upgrades.Rigging2	= 1300;
		refship.Upgrades.Rigging3	= 13000;

		refShip.MaxCaliber	= 12;
		refShip.Cannon = "";

		aref cannonl, cannonr, cannonb, cannonf;

		refShip.FireHeight = 3.0;
		
		refShip.rcannon = 0;
		refShip.lcannon = 0;
		refShip.fcannon = 0;
		refShip.bcannon = 0;

		makearef(cannonl, NetShips[idx].Cannons.Borts.cannonl);
		makearef(cannonr, NetShips[idx].Cannons.Borts.cannonr);
		makearef(cannonf, NetShips[idx].Cannons.Borts.cannonf);
		makearef(cannonb, NetShips[idx].Cannons.Borts.cannonb);

		cannonf.FireZone	= Degree2Radian(30.0);
		cannonf.FireDir		= Degree2Radian(0.0);
		cannonf.FireAngMin	= -0.35;
		cannonf.FireAngMax	= 0.60;

		cannonr.FireZone	= Degree2Radian(80.0);
		cannonr.FireDir		= Degree2Radian(90.0);
		cannonr.FireAngMin	= -0.35;
		cannonr.FireAngMax	= 0.60;

		cannonb.FireZone	= Degree2Radian(30.0);
		cannonb.FireDir		= Degree2Radian(180.0);
		cannonb.FireAngMin	= -0.35;
		cannonb.FireAngMax	= 0.60;

		cannonl.FireZone	= Degree2Radian(80.0);
		cannonl.FireDir		= Degree2Radian(270.0);
		cannonl.FireAngMin	= -0.35;
		cannonl.FireAngMax	= 0.60;
	}

	///////////////////////////////////////////////////////////////////////////
	//// War Tartane
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name							= "WarTartane";
	refShip.Class							= 7;
	refShip.Cannon							= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber						= 8;
	refShip.Weight							= Tonnes2CWT(10);
	refShip.Capacity						= 50;
	refShip.CannonsQuantity					= 8;
	refShip.rcannon 						= 4;
	refShip.lcannon 						= 4;
	refShip.fcannon 						= 0;
	refShip.bcannon 						= 0;
	refShip.MaxCrew							= 16;
	refShip.MinCrew							= 5;
	refShip.SpeedRate						= 15.0;
	refShip.TurnRate						= 95;
	refShip.Cost							= 550;
	refShip.HP								= 300;
	refShip.SP								= 100;
	refship.CanEncounter					= false;
	refship.Type.Merchant					= false;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 10.0;
	refShip.sea_enchantment					= 4.0;
	refShip.lowpolycrew						= 3;
	
	refShip.Massa = 50000.0;
	refShip.Volume = 100.0;

	//refShip.Rocking.factor = 0.008;
	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.05;

	refship.InertiaAccelerationX	= 3.0;	refship.InertiaBrakingX		= 2.0;
	refship.InertiaAccelerationY	= 14;	refship.InertiaBrakingY		= 5;
	refship.InertiaAccelerationZ	= 5.0;	refship.InertiaBrakingZ		= 3.0;
	
	refShip.GeraldSails.rey_a2		= 1;

	refShip.Height.Bombs.Y				= 1.0;	refShip.Height.Bombs.DY		= 0.5;
	refShip.Height.Grapes.Y				= 2.0;	refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 10.0;	refShip.Height.Knippels.DY	= 8.0;
	refShip.Height.Balls.Y				= 1.0;	refShip.Height.Balls.DY		= 0.5;
	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 9;
	refship.Track1.Width	= "1.0, 2.0";
	refship.Track1.Speed	= "5.2, 6.2";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7;
	refship.Track2.Width	= "1.2, 2.0";
	refship.Track2.Speed	= "0.15, 0.25";


	///////////////////////////////////////////////////////////////////////////
	//// Lugger
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Lugger";
	refShip.Class					= 6;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 8;
	refShip.Weight								= Tonnes2CWT(150);
	refShip.Capacity        			= 600;
	refShip.CannonsQuantity				= 12;
	refShip.rcannon 							= 4;
	refShip.lcannon 							= 4;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 2;
	refShip.MaxCrew         			= 50;
	refShip.MinCrew         			= 12;
	refShip.SpeedRate       			= 14.6;
	refShip.TurnRate        			= 40.0;
	refShip.Cost           			= 10000;
	refShip.HP              			= 800;
	refShip.SP              			= 100;
	refShip.AbordageLocation			= "BOARDING_BARQUE";
	refship.Type.Merchant					= true;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 6.0;
	refShip.sea_enchantment					= 2.0;
	refShip.lowpolycrew = 8;

	//refShip.Rocking.factor = 0.012;
	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 0.2;
	refship.SpeedDependWeight			= 0.1;
	refship.SubSeaDependWeight			= 0.4;
	refship.TurnDependWeight			= 0.8;
	
	refship.InertiaAccelerationX	= 10.0;	refship.InertiaBrakingX		= 10.0;
	refship.InertiaAccelerationY	= 10;	refship.InertiaBrakingY		= 10;
	refship.InertiaAccelerationZ	= 10.0;	refship.InertiaBrakingZ		= 10.0;
	
	refShip.Height.Bombs.Y				= 1.0;	refShip.Height.Bombs.DY		= 0.5;
	refShip.Height.Grapes.Y				= 2.0;	refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 10.0;	refShip.Height.Knippels.DY	= 8.0;
	refShip.Height.Balls.Y				= 1.0;	refShip.Height.Balls.DY		= 0.5;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 9;
	refship.Track1.Width	= "2.0, 3.0";
	refship.Track1.Speed	= "8.0, 10.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7;
	refship.Track2.Width	= "3.0, 4.5";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_a3		= 1;

	///////////////////////////////////////////////////////////////////////////
	//// Sloop 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Sloop";
	refShip.Class					= 6;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 8;
	refShip.Weight						= Tonnes2CWT(170);
	refShip.Capacity        			= 500;
	refShip.CannonsQuantity				= 16;
	refShip.rcannon 					= 6;
	refShip.lcannon 					= 6;
	refShip.fcannon 					= 2;
	refShip.bcannon 					= 2;
	refShip.MaxCrew         			= 80;
	refShip.MinCrew         			= 10;
	refShip.SpeedRate					= 14.0;
	refShip.TurnRate        			= 45.0;
	refShip.Cost           			= 13000;
	refShip.HP              			= 1200;
	refShip.SP              			= 160;
	refShip.AbordageLocation			= "BOARDING_SLOOP";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 5;
	refShip.sea_enchantment					= 2.0;
	refShip.lowpolycrew = 8;
	
	//refShip.Rocking.factor = 0.015;
	refship.Rocking.y = 0.25;
	refship.Rocking.az = 0.05;
	
	refship.WaterLine							= -0.0;

	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 0.3;
	refship.TurnDependWeight			= 0.6;
	
	refship.InertiaAccelerationX	= 8.0;	refship.InertiaBrakingX		= 8.0;
	refship.InertiaAccelerationY	= 8;	refship.InertiaBrakingY		= 8;
	refship.InertiaAccelerationZ	= 8.0;	refship.InertiaBrakingZ		= 8.0;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 9;
	refship.Track1.Width	= "2.0, 2.5";
	refship.Track1.Speed	= "5.2, 6.2";

	refship.Track2.ZStart	= -0.05;
	refship.Track2.LifeTime = 7;
	refship.Track2.Width	= "3.0, 4.0";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_a2		= 1;

	///////////////////////////////////////////////////////////////////////////
	//// Schooner
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Schooner";
	refShip.Class					= 5;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 16;
	refShip.Weight						= Tonnes2CWT(200);
	refShip.Capacity        			= 1400;
	refShip.CannonsQuantity				= 20;
	refShip.rcannon 					= 8;
	refShip.lcannon 					= 8;
	refShip.fcannon 					= 2;
	refShip.bcannon 					= 2;
	refShip.MaxCrew         			= 120;
	refShip.MinCrew         			= 24;
	refShip.SpeedRate					= 13.0;
	refShip.TurnRate        			= 35.0;
	refShip.Cost           			= 24000;
	refShip.HP              			= 1800;
	refShip.SP              			= 540;
	refShip.AbordageLocation			= "BOARDING_SCHOONER";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 8.0;
	refShip.sea_enchantment					= 2.0;
	refShip.lowpolycrew = 12;

	refship.Rocking.y = 0.4;
	refship.Rocking.az = 0.03;

	refship.WaterLine					= -0.2;

	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 0.2;
	refship.TurnDependWeight			= 0.4;
	
	refship.InertiaAccelerationX	= 8.0;	refship.InertiaBrakingX		= 8.0;
	refship.InertiaAccelerationY	= 6;		refship.InertiaBrakingY		= 6;
	refship.InertiaAccelerationZ	= 6.0;	refship.InertiaBrakingZ		= 6.0;
	
	refShip.Height.Bombs.Y				= 1.0;		refShip.Height.Bombs.DY		= 1.0;
	refShip.Height.Grapes.Y				= 2.0;		refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 15.0;		refShip.Height.Knippels.DY	= 10.0;
	refShip.Height.Balls.Y				= 1.0;		refShip.Height.Balls.DY		= 1.0;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 8;
	refship.Track1.Width	= "1.2, 2.2";
	refship.Track1.Speed	= "7.2, 8.2";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 6;
	refship.Track2.Width	= "2.2, 3.2";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_a2		= 1;

	///////////////////////////////////////////////////////////////////////////
	/// Barque
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Barque";
	refShip.Class					= 5;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 12;
	refShip.Weight								= Tonnes2CWT(350);
	refShip.Capacity       				= 1500;
	refShip.CannonsQuantity				= 16;
	refShip.rcannon 							= 6;
	refShip.lcannon 							= 6;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 2;
	refShip.MaxCrew         			= 80;
	refShip.MinCrew         			= 16;
	refShip.SpeedRate					= 12.0;
	refShip.TurnRate        			= 30.0;
	refShip.Cost           			= 17400;
	refShip.HP              			= 1900;
	refShip.SP              			= 400;
	refShip.AbordageLocation			= "BOARDING_BARQUE";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 3.0;
	refShip.sea_enchantment					= 1.8;
	refShip.lowpolycrew = 10;

	refship.Rocking.y = 0.4;
	refship.Rocking.az = 0.025;
	
	refship.WaterLine							= 1.15;
	refship.SpeedDependWeight			= 0.4;
	refship.SubSeaDependWeight			= 0.8;
	refship.TurnDependWeight			= 1.6;
	
	refship.InertiaAccelerationX	= 4.0;	refship.InertiaBrakingX		= 4.0;
	refship.InertiaAccelerationY	= 4;	refship.InertiaBrakingY		= 4.0;
	refship.InertiaAccelerationZ	= 6.0;	refship.InertiaBrakingZ		= 6.0;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 7.5;
	refship.Track1.Width	= "6.0, 7.0";
	refship.Track1.Speed	= "7.2, 9.2";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 5.0;
	refship.Track2.Width	= "6.0, 8.0";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	///////////////////////////////////////////////////////////////////////////
	/// Caravel
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Caravel";
	refShip.Class					= 5;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 8;
	refShip.Weight								= Tonnes2CWT(500);
	refShip.Capacity        			= 2800;
	refShip.CannonsQuantity				= 32;
	refShip.rcannon 							= 13;
	refShip.lcannon 							= 13;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 180;
	refShip.MinCrew         			= 16;
	refShip.SpeedRate					= 10.0;
	refShip.TurnRate        			= 45;
	refShip.Cost           			= 27400;
	refShip.HP              			= 2500;
	refShip.SP              			= 300;
	refShip.AbordageLocation			= "BOARDING_SCHOONER";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 2;
	refShip.sea_enchantment					= 1.5;
	refShip.lowpolycrew = 16;

	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
		
	
	refship.WaterLine							= 1.0;
	refship.SpeedDependWeight			= 0.5;
	refship.SubSeaDependWeight			= 0.6;
	refship.TurnDependWeight			= 1.2;
	
	refship.InertiaAccelerationX	= 3.0;	refship.InertiaBrakingX		= 3.0;
	refship.InertiaAccelerationY	= 3;	refship.InertiaBrakingY		= 3;
	refship.InertiaAccelerationZ	= 3.0;	refship.InertiaBrakingZ		= 3.0;
	
	refShip.Height.Bombs.Y				= 1.0;	refShip.Height.Bombs.DY		= 0.5;
	refShip.Height.Grapes.Y				= 2.0;	refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 10.0;	refShip.Height.Knippels.DY	= 8.0;
	refShip.Height.Balls.Y				= 1.0;	refShip.Height.Balls.DY		= 0.5;
	
	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 10;
	refship.Track1.Width	= "4.0, 5.0";
	refship.Track1.Speed	= "7.2, 9.2";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7.5;
	refship.Track2.Width	= "5.0, 7.0";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_a3		= 1;
	

	///////////////////////////////////////////////////////////////////////////
	/// Fleut 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Fleut";
	refShip.Class					= 4;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 12;
	refShip.Weight								= Tonnes2CWT(450);
	refShip.Capacity        			= 2800;
	refShip.CannonsQuantity				= 20;
	refShip.rcannon 							= 7;
	refShip.lcannon 							= 7;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 140;
	refShip.MinCrew         			= 16;
	refShip.SpeedRate					= 12.0;
	refShip.TurnRate        			= 35.0;
	refShip.Cost           			= 32400;
	refShip.HP              			= 2500;
	refShip.SP              			= 450;
	refShip.AbordageLocation			= "BOARDING_SCHOONER";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 2;
	refShip.sea_enchantment					= 1.5;
	refShip.lowpolycrew = 16;
	
	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 1.2;
	refship.SpeedDependWeight			= 0.35;
	refship.SubSeaDependWeight			= 0.8;
	refship.TurnDependWeight			= 1.6;
	
	refship.InertiaAccelerationX	= 4.5;	refship.InertiaBrakingX		= 4.5;
	refship.InertiaAccelerationY	= 6;	refship.InertiaBrakingY		= 6;
	refship.InertiaAccelerationZ	= 4.0;	refship.InertiaBrakingZ		= 4.0;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 12;
	refship.Track1.Width	= "2.0, 3.0";
	refship.Track1.Speed	= "8.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7.5;
	refship.Track2.Width	= "6.0, 8.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	refShip.GeraldSails.rey_a3		= 1;
	refShip.GeraldSails.rey_a2		= 1;

	///////////////////////////////////////////////////////////////////////////
	/// Brig
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Brig";
	refShip.Class					= 4;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(400);
	refShip.Capacity        			= 1800;
	refShip.CannonsQuantity				= 16;
	refShip.rcannon 							= 6;
	refShip.lcannon 							= 6;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 2;
	refShip.MaxCrew         			= 180;
	refShip.MinCrew         			= 32;
	refShip.SpeedRate       			= 13.5;
	refShip.TurnRate        			= 40.0;
	refShip.Cost           			= 45000;
	refShip.HP              			= 2500;
	refShip.SP              			= 700;
	refShip.AbordageLocation			= "BOARDING_PINNACE";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 2.0;
	refShip.sea_enchantment					= 1.8;
	refShip.lowpolycrew = 12;

	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.04;
	
	refship.WaterLine							= 0.1;
	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 0.6;
	refship.TurnDependWeight			= 1.2;
	
	refship.InertiaAccelerationX	= 5.5;	refship.InertiaBrakingX		= 5.5;
	refship.InertiaAccelerationY	= 8;	refship.InertiaBrakingY		= 8;
	refship.InertiaAccelerationZ	= 6.0;	refship.InertiaBrakingZ		= 6.0;
	
	refShip.Height.Bombs.Y				= 1.0;		refShip.Height.Bombs.DY		= 1.0;
	refShip.Height.Grapes.Y				= 2.0;		refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 15.0;		refShip.Height.Knippels.DY	= 10.0;
	refShip.Height.Balls.Y				= 1.0;		refShip.Height.Balls.DY		= 1.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;
	
	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 12;
	refship.Track1.Width	= "2.0, 3.0";
	refship.Track1.Speed	= "6.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7.5;
	refship.Track2.Width	= "4.0, 6.0";
	refship.Track2.Speed	= "0.15, 0.25";

	///////////////////////////////////////////////////////////////////////////
	/// Galeon LIGHT
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Galeon_l";
	refShip.Class					= 4;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 16;
	refShip.Weight								= Tonnes2CWT(800);
	refShip.Capacity        			= 3200;
	refShip.CannonsQuantity				= 20;
	refShip.rcannon 							= 8;
	refShip.lcannon 							= 8;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 240;
	refShip.MinCrew         			= 16;
	refShip.SpeedRate					= 11.0;
	refShip.TurnRate        			= 30.0;
	refShip.Cost           			= 40000;
	refShip.HP              			= 3600;
	refShip.SP              			= 500;
	refShip.AbordageLocation			= "BOARDING_PINNACE";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 1.0;
	refShip.sea_enchantment					= 1.2;
	refShip.lowpolycrew = 16;

	refship.Rocking.y = 0.45;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 0.85;
	refship.SpeedDependWeight			= 0.4;
	refship.SubSeaDependWeight			= 0.95;
	refship.TurnDependWeight			= 0.2;
	
	refship.InertiaAccelerationX	= 4.0;	refship.InertiaBrakingX		= 4.0;
	refship.InertiaAccelerationY	= 6;	refship.InertiaBrakingY		= 6;
	refship.InertiaAccelerationZ	= 4.0;	refship.InertiaBrakingZ		= 4.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 16.5;
	refship.Track1.Width	= "5.2, 7.3";
	refship.Track1.Speed	= "6.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 10.5;
	refship.Track2.Width	= "6.0, 8.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	refShip.GeraldSails.rey_a2		= 1;

	///////////////////////////////////////////////////////////////////////////
	/// Corvette
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Corvette";
	refShip.Class					= 3;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(600);
	refShip.Capacity        			= 2100;
	refShip.CannonsQuantity				= 32;
	refShip.rcannon 							= 13;
	refShip.lcannon 							= 13;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 220;
	refShip.MinCrew         			= 30;
	refShip.SpeedRate					= 14.5;
	refShip.TurnRate        			= 40.0;
	refShip.Cost           			= 60000;
	refShip.HP              			= 3000;
	refShip.SP              			= 900;
	refShip.AbordageLocation			= "BOARDING_CORVETTE";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 2.2;
	refShip.sea_enchantment					= 1.5;
	refShip.lowpolycrew = 16;

	refship.Rocking.y = 0.8;
	refship.Rocking.az = 0.025;
	
	refship.WaterLine							= -0.4;
	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 0.7;
	refship.TurnDependWeight			= 1.4;
	
	refship.InertiaAccelerationX	= 8.0;	refship.InertiaBrakingX		= 8.0;
	refship.InertiaAccelerationY	= 6;	refship.InertiaBrakingY		= 6;
	refship.InertiaAccelerationZ	= 6.0;	refship.InertiaBrakingZ		= 6.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.17;
	refship.Track1.LifeTime = 12.5;
	refship.Track1.Width	= "4.0, 5.0";
	refship.Track1.Speed	= "6.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 6.5;
	refship.Track2.Width	= "5.0, 7.0";
	refship.Track2.Speed	= "0.15, 0.25";

	refShip.GeraldSails.rey_a2		= 1;

	///////////////////////////////////////////////////////////////////////////
	/// Galeon HEAVY
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Galeon_h";
	refShip.Class					= 3;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(1100);
	refShip.Capacity        			= 3000;
	refShip.CannonsQuantity				= 36;
	refShip.rcannon 							= 15;
	refShip.lcannon 							= 15;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 400;
	refShip.MinCrew         			= 64;
	refShip.SpeedRate					= 12.0;
	refShip.TurnRate        			= 25.0;
	refShip.Cost           			= 70000;
	refShip.HP              			= 5000;
	refShip.SP              			= 1100;
	refShip.AbordageLocation			= "BOARDING_PINNACE";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 2.0;
	refShip.sea_enchantment					= 1.2;
	refShip.lowpolycrew = 20;

	
	refship.Rocking.y = 0.4;
	refship.Rocking.az = 0.02;
	
	refship.WaterLine							= -0.2;

	refship.SpeedDependWeight			= 0.5;
	refship.SubSeaDependWeight			= 0.3;
	refship.TurnDependWeight			= 1.0;
	
	refship.InertiaAccelerationX	= 4.5;	refship.InertiaBrakingX		= 4.5;
	refship.InertiaAccelerationY	= 3;	refship.InertiaBrakingY		= 3;
	refship.InertiaAccelerationZ	= 4.5;	refship.InertiaBrakingZ		= 4.5;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.0;
	refship.Track1.LifeTime = 14.5;
	refship.Track1.Width	= "5.2, 7.3";
	refship.Track1.Speed	= "5.5, 6.5";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 8.5;
	refship.Track2.Width	= "6.0, 8.0";
	refship.Track2.Speed	= "0.15, 0.25";

	///////////////////////////////////////////////////////////////////////////
	/// Pinnace 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Pinnace";
	refShip.Class					= 3;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS12;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(800);
	refShip.Capacity        			= 4000;
	refShip.CannonsQuantity				= 18;
	refShip.rcannon 							= 6;
	refShip.lcannon 							= 6;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew								= 220;
	refShip.MinCrew         			= 24;
	refShip.SpeedRate					= 13.0;
	refShip.TurnRate        			= 30.0;
	refShip.Cost           			= 50000;
	refShip.HP              			= 4000;
	refShip.SP              			= 1000;
	refShip.AbordageLocation			= "BOARDING_PINNACE";
	refship.Type.Merchant					= true;
	refship.Type.War						= false;
	refShip.fWindAgainstSpeed				= 2.3;
	refShip.sea_enchantment					= 1.1;
	refShip.lowpolycrew = 20;

	refship.Rocking.y = 0.4;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 0.8;
	refship.SpeedDependWeight			= 0.4;
	refship.SubSeaDependWeight			= 1.3;
	refship.TurnDependWeight			= 1.0;
	
	refship.InertiaAccelerationX	= 3.0;	refship.InertiaBrakingX		= 3.0;
	refship.InertiaAccelerationY	= 4;	refship.InertiaBrakingY		= 4;
	refship.InertiaAccelerationZ	= 3.0;	refship.InertiaBrakingZ		= 3.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.30;
	refship.Track1.LifeTime = 14.5;
	refship.Track1.Width	= "5.7, 7.9";
	refship.Track1.Speed	= "6.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 8.5;
	refship.Track2.Width	= "10.0, 12.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	///////////////////////////////////////////////////////////////////////////
	/// Frigate 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Frigate";
	refShip.Class					= 2;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(1000);
	refShip.Capacity        			= 2500;
	refShip.CannonsQuantity				= 46;
	refShip.rcannon 							= 20;
	refShip.lcannon 							= 20;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 300;
	refShip.MinCrew         			= 30;
	refShip.SpeedRate       			= 12.5;
	refShip.TurnRate        			= 38.0;
	refShip.Cost           			= 100000;
	refShip.HP              			= 2500;
	refShip.SP              			= 1000;
	refShip.AbordageLocation			= "BOARDING_CORVETTE";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 2.0;
	refShip.sea_enchantment					= 1.2;
	refShip.lowpolycrew = 20;

	
	refship.Rocking.y = 0.4;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 0.5;

	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 1.0;
	refship.TurnDependWeight			= 1.0;
	
	refship.InertiaAccelerationX	= 5.0;	refship.InertiaBrakingX		= 5.0;
	refship.InertiaAccelerationY	= 4;	refship.InertiaBrakingY		= 4;
	refship.InertiaAccelerationZ	= 5.0;	refship.InertiaBrakingZ		= 5.0;
	
	refShip.Height.Bombs.Y				= 1.0;	refShip.Height.Bombs.DY		= 0.5;
	refShip.Height.Grapes.Y				= 2.0;	refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y			= 10.0;	refShip.Height.Knippels.DY	= 8.0;
	refShip.Height.Balls.Y				= 1.0;	refShip.Height.Balls.DY		= 0.5;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 14.5;
	refship.Track1.Width	= "3.7, 3.9";
	refship.Track1.Speed	= "7.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 8.5;
	refship.Track2.Width	= "8.0, 9.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	///////////////////////////////////////////////////////////////////////////
	/// Lineship 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Lineship";
	refShip.Class					= 1;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(2000);
	refShip.Capacity        			= 4000;
	refShip.CannonsQuantity				= 56;
	refShip.rcannon 							= 25;
	refShip.lcannon 							= 25;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 4;
	refShip.MaxCrew         			= 420;
	refShip.MinCrew         			= 40;
	refShip.SpeedRate					= 12.0;
	refShip.TurnRate        			= 36.0;
	refShip.Cost           			= 200000;
	refShip.HP              			= 8500;
	refShip.SP              			= 1400;
	refShip.AbordageLocation			= "BOARDING_BATTLESHIP";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 1.0;
	refShip.sea_enchantment					= 1.1;
	refShip.lowpolycrew = 24;

	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
		
	refship.WaterLine					= 2.2;

	refship.SpeedDependWeight			= 0.4;
	refship.SubSeaDependWeight			= 1.15;
	refship.TurnDependWeight			= 0.6;
	
	refship.InertiaAccelerationX	= 3.0;	refship.InertiaBrakingX		= 3.0;
	refship.InertiaAccelerationY	= 2;	refship.InertiaBrakingY		= 2;
	refship.InertiaAccelerationZ	= 3.0;	refship.InertiaBrakingZ		= 3.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.15;
	refship.Track1.LifeTime = 16.5;
	refship.Track1.Width	= "3.0, 4.0";
	refship.Track1.Speed	= "8.0, 8.0";

	refship.Track2.ZStart	= -0.25;
	refship.Track2.LifeTime = 10.5;
	refship.Track2.Width	= "6.0, 7.0";
	refship.Track2.Speed	= "0.15, 0.25";

	///////////////////////////////////////////////////////////////////////////
	/// Warship 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Warship";
	refShip.Class					= 1;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(2500);
	refShip.Capacity        			= 4500;
	refShip.CannonsQuantity				= 68;
	refShip.rcannon 							= 30;
	refShip.lcannon 							= 30;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 6;
	refShip.MaxCrew         			= 500;
	refShip.MinCrew         			= 50;
	refShip.SpeedRate					= 12.8;
	refShip.TurnRate        			= 34;
	refShip.Cost           			= 300000;
	refShip.HP              			= 7500;
	refShip.SP              			= 1500;
	refShip.AbordageLocation			= "BOARDING_BATTLESHIP";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 1.0;
	refShip.sea_enchantment					= 1.1;
	refShip.lowpolycrew = 24;

	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
	
	refship.WaterLine							= 0.5;

	refship.SpeedDependWeight			= 0.45;
	refship.SubSeaDependWeight			= 1.0;
	refship.TurnDependWeight			= 1.0;
	
	refship.InertiaAccelerationX	= 2.5;	refship.InertiaBrakingX		= 2.5;
	refship.InertiaAccelerationY	= 1.5;		refship.InertiaBrakingY		= 1.5;
	refship.InertiaAccelerationZ	= 2.5;	refship.InertiaBrakingZ		= 2.5;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.25;
	refship.Track1.LifeTime = 9.5;
	refship.Track1.Width	= "7.0, 7.5";
	refship.Track1.Speed	= "8.0, 9.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 7.5;
	refship.Track2.Width	= "8.0, 10.0";
	refship.Track2.Speed	= "1.0, 2.0";
	///////////////////////////////////////////////////////////////////////////
	/// Battleship 
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Battleship";
	refShip.Class					= 1;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS16;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(3000);
	refShip.Capacity        			= 5500;
	refShip.CannonsQuantity				= 80;
	refShip.rcannon 							= 36;
	refShip.lcannon 							= 36;
	refShip.fcannon 							= 2;
	refShip.bcannon 							= 6;
	refShip.MaxCrew         			= 600;
	refShip.MinCrew         			= 80;
	refShip.SpeedRate					= 13.4;
	refShip.TurnRate        			= 32.0;
	refShip.Cost           			= 400000;
	refShip.HP              			= 9000;
	refShip.SP              			= 1600;
	refShip.AbordageLocation			= "BOARDING_BATTLESHIP";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 1.0;
	refShip.sea_enchantment					= 1.1;
	refShip.lowpolycrew = 24;

	refship.Rocking.y = 0.5;
	refship.Rocking.az = 0.035;
	
	
	refship.WaterLine							= -0.2;
	refship.SpeedDependWeight			= 0.5;
	refship.SubSeaDependWeight			= 0.75;
	refship.TurnDependWeight			= 1.9;
	
	refship.InertiaAccelerationX	= 2.0;	refship.InertiaBrakingX		= 2.0;
	refship.InertiaAccelerationY	= 1;	refship.InertiaBrakingY		= 1;
	refship.InertiaAccelerationZ	= 2.0;	refship.InertiaBrakingZ		= 2.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.25;
	refship.Track1.LifeTime = 16.5;
	refship.Track1.Width	= "6.0, 8.2";
	refship.Track1.Speed	= "9.0, 9.0";

	refship.Track2.ZStart	= 0.0;
	refship.Track2.LifeTime = 13.5;
	refship.Track2.Width	= "8.0, 9.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	///////////////////////////////////////////////////////////////////////////
	/// Manowar
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            			= "Manowar";
	refShip.Class					= 1;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS24;
	refShip.MaxCaliber      			= 24;
	refShip.Weight								= Tonnes2CWT(4000);
	refShip.Capacity        			= 7000;
	refShip.CannonsQuantity				= 102;
	refShip.rcannon 							= 45;
	refShip.lcannon 							= 45;
	refShip.fcannon 							= 4;
	refShip.bcannon 							= 8;
	refShip.MaxCrew         			= 800;
	refShip.MinCrew         			= 160;
	refShip.SpeedRate					= 14.2;
	refShip.TurnRate        			= 30.0;
	refShip.Cost           			= 500000;
	refShip.HP              			= 15000;
	refShip.SP              			= 2500;
	refShip.AbordageLocation			= "BOARDING_BATTLESHIP";
	refship.Type.Merchant					= false;
	refship.Type.War						= true;
	refShip.fWindAgainstSpeed				= 1.0;
	refShip.sea_enchantment					= 1.0;
	refShip.lowpolycrew = 28;

	refship.Rocking.y = 0.3;
	refship.Rocking.az = 0.02	;

	refship.WaterLine							= 0.5;
	refship.SpeedDependWeight			= 0.6;
	refship.SubSeaDependWeight			= 1.8;
	refship.TurnDependWeight			= 2.6;
	
	refship.InertiaAccelerationX	= 1.0;	refship.InertiaBrakingX		= 1.0;
	refship.InertiaAccelerationY	= 1;	refship.InertiaBrakingY		= 1;
	refship.InertiaAccelerationZ	= 1.0;	refship.InertiaBrakingZ		= 1.0;
	
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.30;
	refship.Track1.LifeTime = 18.5;
	refship.Track1.Width	= "6.5, 7.5";
	refship.Track1.Speed	= "8.0, 9.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 10.5;
	refship.Track2.Width	= "16.0, 18.0";
	refship.Track2.Speed	= "0.15, 0.25";

	///////////////////////////////////////////////////////////////////////////
	/// FORT
	///////////////////////////////////////////////////////////////////////////
	makeref(refShip, NetShips[n]); n++;
	refShip.Name            		= "FORT";
	refShip.Class           		= 1;
	refShip.Cannon          		= "cannon16";
	refShip.MaxCaliber      		= 24;
	refShip.Weight					= Tonnes2CWT(3500);
	refShip.Capacity        		= 6000;
	refShip.CannonsQuantity			= 101;
	refShip.rcannon 				= 46;
	refShip.lcannon 				= 45;
	refShip.fcannon 				= 2;
	refShip.bcannon 				= 8;
	refShip.MaxCrew         		= 5000;
	refShip.MinCrew         		= 100;
	refShip.SpeedRate				= 12;
	refShip.TurnRate				= 20;
	refShip.Cost           			= 300000;
	refShip.HP              		= 15000;
	refShip.SP              		= 100;
	refship.WaterLine				= 0.0;
	refship.SpeedDependWeight		= 0.3;
	refship.SubSeaDependWeight		= 0.9;
	refship.TurnDependWeight		= 0.8;
	refShip.TradeOff				= true;

	// Check
	for (int i=0; i<n; i++)
	{
	    makeref(refShip, NetShips[i]);
		if (!CheckAttribute(refShip, "Name")) { continue; }
		if (!CheckAttribute(refShip, "TurnRate"))
		{
			Trace("ERROR: Ship " + i + " doesn't have TurnRate attribute");
		}
		if (!CheckAttribute(refShip,"SpeedRate"))
		{
			Trace("ERROR: Ship " + i + " doesn't have SpeedRate attribute");
		}
		if (!CheckAttribute(refShip,"Class"))
		{
			Trace("ERROR: Ship " + i + " doesn't have class attribute");
		}
		else
		{
			int iClass = sti(refShip.Class);
			if (iClass > 7) { Trace("ERROR: Ship " + i + " class > 7"); }
			if (iClass < 1) { Trace("ERROR: Ship " + i + " class < 1"); }
		}
		refship.InertiaAccelerationY = stf(refship.InertiaAccelerationY) / 6.0;
	}

	Trace("NetShips init = " + n);
	iNetShipsNum = n;
}