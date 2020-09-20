
void UpdateFame()
{	
	pchar.days = sti(pchar.days) + 1;
	int iDays = sti(pchar.days);
	int iChecker = iGameArcade + 4;
	if(iDays == iChecker)
	{
		pchar.days = 0;
		ChangeCharacterFame(pchar, -1);
	}
}

void ShipFameUp()
{

	ref chr;
	int cn;
	for(int i = 0; i<COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			chr = &characters[cn];
			chr.shipfame.shipname = chr.ship.name;
			chr.shipfame.fame = sti(chr.shipfame.fame)+1;
		}
	}
}

void ShipFameErase(aref chr)
{


	chr.shipfame.shipname = pchar.ship.name;
	chr.shipfame.fame = 0;
}

int GetShipFame(aref chr)
{
	if (checkAttribute(chr, "shipfame"))
	{
		return sti(chr.shipfame.fame);
	}
	return 0;
}
