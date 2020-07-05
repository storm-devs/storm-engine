
extern void InitColonies();

void ColoniesInit()
{
	if(LoadSegment("Colonies\Colonies_init.c"))
	{
		InitColonies();
		UnloadSegment("Colonies\Colonies_init.c");
	}
}
 
int FindColony(string sColony)
{
	for (int i=0; i<MAX_COLONIES; i++)
	{
		if(Colonies[i].id == sColony)
		{
			return i;
		}
	}
	
	return -1;
}

ref GetColonyByIndex(int iColony);
{
	return &Colonies[iColony];
}