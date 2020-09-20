#define NUM_PERKS		6
object NetPerks[NUM_PERKS];

int Net_GetPerksNum() { return NUM_PERKS; }
ref Net_GetPerkByIndex(int iPerkIndex) { return &NetPerks[iPerkIndex]; }

int Net_GetPerkCost(int iPerkIndex)
{
	return sti(NetPerks[iPerkIndex].Cost);
}

void Net_InitPerks()
{
	NetPerks[0].Name = "TriggerPester";
	NetPerks[0].Cost = 1000;

	NetPerks[1].Name = "Protector";
	NetPerks[1].Cost = 2000;

	NetPerks[2].Name = "HolyShit";
	NetPerks[2].Cost = 3000;

	NetPerks[3].Name = "Flayer";
	NetPerks[3].Cost = 3000;

	NetPerks[4].Name = "CrazyGunner";
	NetPerks[4].Cost = 4000;

	NetPerks[5].Name = "MuscetFire";
	NetPerks[5].Cost = 15000;
}