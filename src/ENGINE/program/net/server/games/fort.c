bool NetServer_CheckGameOver_Fort()
{
	int i;

	// check if fort dead...
	ref rFort = NetServer_GetClient(NET_FORTCLIENT);
	if (stf(rFort.Ship.HP) <= 0.0)
	{
		NetServer.iWinnerTeam = 1;
		return true;
	}

	// calculate survived ppl
	int iSurvivedTeams[2];
	for (i=0; i<2; i++) { iSurvivedTeams[i] = 0; }
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{ 
		if (!sti(NSClients[i].Use)) { continue; } 
		if (stf(NSClients[i].Ship.hp) <= 0.0) { continue; } 

		int iTeam = sti(NSClients[i].Team);
		iSurvivedTeams[iTeam] = iSurvivedTeams[iTeam] + 1;
	}

	// check if fort command dead... [0]
	if (iSurvivedTeams[0] == 0 && iSurvivedTeams[1] > 0)
	{
		// playlogic want it to disable
		//NetServer.iWinnerTeam = 1;
		//return true;
	}

	// check if fort attackers dead... [1]
	if (iSurvivedTeams[0] > 0 && iSurvivedTeams[1] == 0)
	{
		NetServer.iWinnerTeam = 0;
		return true;
	}

	// if both teams dead
	if (iSurvivedTeams[0] == 0 && iSurvivedTeams[1] == 0)
	{
		NetServer.iWinnerTeam = 0;
		return true;
	}

	return false;
}