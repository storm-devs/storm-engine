// to_do for del
#define MAX_SHIP_UPGRADES	9

#define	SHIP_HULL_UPGRADE_1		0
#define	SHIP_HULL_UPGRADE_2		1
#define	SHIP_HULL_UPGRADE_3		2

#define	SHIP_SAILS_UPGRADE_1	3
#define	SHIP_SAILS_UPGRADE_2	4
#define	SHIP_SAILS_UPGRADE_3	5

#define	SHIP_CANNONS_UPGRADE_1	6
#define	SHIP_CANNONS_UPGRADE_2	7
#define	SHIP_CANNONS_UPGRADE_3	8

object ShipUpgrades[MAX_SHIP_UPGRADES]

void InitShipUpgrades()
{
	ShipUpgrades[SHIP_HULL_UPGRADE_1].id = "hull1";
	ShipUpgrades[SHIP_HULL_UPGRADE_1].hullhp = 0;
	ShipUpgrades[SHIP_HULL_UPGRADE_1].speedrate = 0;
	ShipUpgrades[SHIP_HULL_UPGRADE_1].turnrate = 0;
	ShipUpgrades[SHIP_HULL_UPGRADE_1].cost = 3;

	ShipUpgrades[SHIP_HULL_UPGRADE_2].id = "hull2";
	ShipUpgrades[SHIP_HULL_UPGRADE_2].hullhp = 5;
	ShipUpgrades[SHIP_HULL_UPGRADE_2].speedrate = -5;
	ShipUpgrades[SHIP_HULL_UPGRADE_2].turnrate = -5;
	ShipUpgrades[SHIP_HULL_UPGRADE_2].cost = 6;

	ShipUpgrades[SHIP_HULL_UPGRADE_3].id = "hull3";
	ShipUpgrades[SHIP_HULL_UPGRADE_3].hullhp = 10;
	ShipUpgrades[SHIP_HULL_UPGRADE_3].speedrate = -10;
	ShipUpgrades[SHIP_HULL_UPGRADE_3].turnrate = -10;
	ShipUpgrades[SHIP_HULL_UPGRADE_3].cost = 12;

	ShipUpgrades[SHIP_SAILS_UPGRADE_1].id = "sails1";
	ShipUpgrades[SHIP_SAILS_UPGRADE_1].dmgtosails = 0;
	ShipUpgrades[SHIP_SAILS_UPGRADE_1].speedrate = 0;
	ShipUpgrades[SHIP_SAILS_UPGRADE_1].turnrate = 0;
	ShipUpgrades[SHIP_SAILS_UPGRADE_1].cost = 2;

	ShipUpgrades[SHIP_SAILS_UPGRADE_2].id = "sails2";
	ShipUpgrades[SHIP_SAILS_UPGRADE_2].dmgtosails = 5;
	ShipUpgrades[SHIP_SAILS_UPGRADE_2].speedrate = 2.5;
	ShipUpgrades[SHIP_SAILS_UPGRADE_2].turnrate = 2.5;
	ShipUpgrades[SHIP_SAILS_UPGRADE_2].cost = 5;

	ShipUpgrades[SHIP_SAILS_UPGRADE_3].id = "sails3";
	ShipUpgrades[SHIP_SAILS_UPGRADE_3].dmgtosails = 10;
	ShipUpgrades[SHIP_SAILS_UPGRADE_3].speedrate = 5;
	ShipUpgrades[SHIP_SAILS_UPGRADE_3].turnrate = 5;
	ShipUpgrades[SHIP_SAILS_UPGRADE_3].cost = 10;

	ShipUpgrades[SHIP_CANNONS_UPGRADE_1].id = "cannons1";
	ShipUpgrades[SHIP_CANNONS_UPGRADE_1].blastchance = 0;
	ShipUpgrades[SHIP_CANNONS_UPGRADE_1].cost = 500;

	ShipUpgrades[SHIP_CANNONS_UPGRADE_2].id = "cannons2";
	ShipUpgrades[SHIP_CANNONS_UPGRADE_2].blastchance = 0.2;
	ShipUpgrades[SHIP_CANNONS_UPGRADE_2].cost = 1000;

	ShipUpgrades[SHIP_CANNONS_UPGRADE_3].id = "cannons3";
	ShipUpgrades[SHIP_CANNONS_UPGRADE_3].blastchance = 0.4;
	ShipUpgrades[SHIP_CANNONS_UPGRADE_3].cost = 2000;
}

int FindShipUpgrade(string sUpgrade)
{
	for(int i = 0; i < MAX_SHIP_UPGRADES; i++)
	{
		if(ShipUpgrades[i].id == sUpgrade)
		{
			return i;
		}
	}
	return -1;
}
