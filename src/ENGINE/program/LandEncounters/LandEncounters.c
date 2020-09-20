#include "LandEncounters\LEnc_monsters.c"
#include "LandEncounters\LEnc_login.c"
#include "LandEncounters\LandEnc_init.c"

void InitLandRandomEncounters()
{
	
	InitLandEncounters();
	InitLandEncTemplates();
}


ref GetLandEncByID(string id)
{
	if (id=="") return &LandEncounters[0];

	for(int n = 0; n<MAX_LAND_ENCOUNTERS; n++)
	{
		if(LandEncounters[n].id == id) return &LandEncounters[n];
	}

	return &LandEncounters[0];
}

ref GetLandEncTemplateByID(string id)
{
	if (id=="") return &LandEncTemplate[0];

	for(int n = 0; n<MAX_LANDENCTEMPLATES; n++)
	{
		if(LandEncTemplate[n].id == id) return &LandEncTemplate[n];
	}

	return &LandEncTemplate[0];
}
