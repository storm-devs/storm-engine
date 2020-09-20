// ship upgrades
object NetShipHullUpgrades[3], NetShipSailUpgrade[3];

void Net_InitUpgrades()
{
	NetShipHullUpgrades[0].str = "hull1";
	NetShipHullUpgrades[0].hp = 0.0;
	NetShipHullUpgrades[0].speedrate = 0.0;
	NetShipHullUpgrades[0].turnrate = 0.0;

	NetShipHullUpgrades[1].str = "hull2";
	NetShipHullUpgrades[1].hp = 5.0;
	NetShipHullUpgrades[1].speedrate = -5.0;
	NetShipHullUpgrades[1].turnrate = -5.0;

	NetShipHullUpgrades[2].str = "hull3";
	NetShipHullUpgrades[2].hp = 10.0;
	NetShipHullUpgrades[2].speedrate = -10.0;
	NetShipHullUpgrades[2].turnrate = -10.0;

	// sails upgrade
	NetShipSailUpgrade[0].str = "common";
	NetShipSailUpgrade[0].dmgtosails = 0.0;
	NetShipSailUpgrade[0].speedrate = 0.0;
	NetShipSailUpgrade[0].turnrate = 0.0;

	NetShipSailUpgrade[1].str = "pat";
	NetShipSailUpgrade[1].dmgtosails = 5.0;
	NetShipSailUpgrade[1].speedrate = 2.5;
	NetShipSailUpgrade[1].turnrate = 2.5;

	NetShipSailUpgrade[2].str = "silk";
	NetShipSailUpgrade[2].dmgtosails = 10.0;
	NetShipSailUpgrade[2].speedrate = 5.0;
	NetShipSailUpgrade[2].turnrate = 5.0;
}