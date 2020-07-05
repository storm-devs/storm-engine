object NetCannons[32];
int iNetCannonsNum;

int NETCANNON_CANNON, NETCANNON_CULVERINE, NETCANNON_MORTAR;

ref Net_GetCannonByIndex(int iIndex) { return &NetCannons[iIndex]; }
int Net_GetCannonsNum() { return iNetCannonsNum; }

void Net_InitCannonsCalcSpeedV0(ref rCannon, float fFireRange)
{
	rCannon.FireRange = fFireRange;
	rCannon.SpeedV0 = sqrt(fFireRange * 9.81 / sin( 2.0 * MakeFloat(rCannon.FireAngMax)) );
}

int Net_GetCannonCost(int iCannonIndex)
{
	if (iCannonIndex < 0) { return 0; }
	return sti(NetCannons[iCannonIndex].Cost);
}

int Net_GetCannonUpgradeCost(int iCannonIndex, int iUpgradeType)
{
	if (iCannonIndex < 0) { return 0; }
	if (iUpgradeType < 0) { return 0; }

	string sUpgrade = "Upgrade" + (iUpgradeType + 1);
	return sti(NetCannons[iCannonIndex].(sUpgrade));
}

void Net_InitCannons()
{
	ref rCannon;
	int n = 0;
	
	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "culverine";
	rCannon.picture = "cannons2";
	rCannon.caliber = 12;
	rCannon.ReloadTime = 20;
	rCannon.Cost = 120;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 27;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 1.0;
	rCannon.TradeOff = false;
	rCannon.hp = 40.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 500.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "culverine";
	rCannon.picture = "cannons3";
	rCannon.caliber = 16;
	rCannon.ReloadTime = 24;
	rCannon.Cost = 160;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 42;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 2;
	rCannon.TradeOff = false;
	rCannon.hp = 55.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 550.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "culverine";
	rCannon.picture = "cannons4";
	rCannon.caliber = 24;
	rCannon.ReloadTime = 30;
	rCannon.Cost = 200;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 51;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 4;
	rCannon.TradeOff = false;
	rCannon.hp = 70.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 600.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "cannon";
	rCannon.picture = "cannons6";
	rCannon.caliber = 12;
	rCannon.ReloadTime = 12;
	rCannon.Cost = 100;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 20;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 1.0;
	rCannon.TradeOff = false;
	rCannon.hp = 40.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 350.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "cannon";
	rCannon.picture = "cannons7";
	rCannon.caliber = 16;
	rCannon.ReloadTime = 20;
	rCannon.Cost = 130;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 30;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 2.5;
	rCannon.TradeOff = false;
	rCannon.hp = 55.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 400.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "cannon";
	rCannon.picture = "cannons8";
	rCannon.caliber = 24;
	rCannon.ReloadTime = 24;
	rCannon.Weight = 38;
	rCannon.Cost = 165;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 4;
	rCannon.TradeOff = false;
	rCannon.hp = 70.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 500.0);

	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "special";
	rCannon.picture = "cannons9";
	rCannon.caliber = 32;
	rCannon.ReloadTime = 20;
	rCannon.Cost = 350;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 72;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 6.0;
	rCannon.TradeOff = true;
	rCannon.hp = 85.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 750.0);
	
	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "special";
	rCannon.picture = "cannons10";
	rCannon.caliber = 36;
	rCannon.ReloadTime = 24;
	rCannon.Cost = 500;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 90;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 7.5;
	rCannon.TradeOff = true;
	rCannon.hp = 90.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 780.0);
	NETCANNON_CANNON = n - 1;
	
	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "special";
	rCannon.picture = "cannons11";
	rCannon.caliber = 42;
	rCannon.ReloadTime = 30;
	rCannon.Cost = 750;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 30;
	rCannon.Upgrade3 = 60;
	rCannon.Weight = 112;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 9.0;
	rCannon.TradeOff = true;
	rCannon.hp = 100.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 800.0);
	NETCANNON_CULVERINE = n - 1;
	
	makeref(rCannon, NetCannons[n]); n++;
	rCannon.name = "special";
	rCannon.picture = "cannons12";
	rCannon.caliber = 48;
	rCannon.ReloadTime = 40;
	rCannon.Cost = 1000;
	rCannon.Upgrade1 = 0;
	rCannon.Upgrade2 = 200;
	rCannon.Upgrade3 = 400;
	rCannon.Weight = 120;
	rCannon.FireAngMax = 0.60;
	rCannon.FireAngMin = -0.35;
	rCannon.DamageMultiply = 12.0;
	rCannon.TradeOff = true;
	rCannon.hp = 110.0;
	Net_InitCannonsCalcSpeedV0(&rCannon, 760.0);
	NETCANNON_MORTAR = n - 1;

	iNetCannonsNum = n;
}
