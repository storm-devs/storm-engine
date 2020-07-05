bool NetServer_CheckGameOver_Naval()
{
	int i;
	int iTeams[4], iTeamsDamage[4];

	for (i=0; i<4; i++) { iTeams[i] = 0; iTeamsDamage[i] = 0; }

	// return true if only one team alive - so they win the battle
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{ 
		if (!sti(NSClients[i].Use)) { continue; } 
		int iTeam = sti(NSClients[i].Team);

		iTeamsDamage[iTeam] = iTeamsDamage[iTeam] + sti(NSClients[i].Stat.DamageInflicted);

		if (stf(NSClients[i].Ship.hp) <= 0.0) { continue; } 

		iTeams[iTeam] = iTeams[iTeam] + 1;
	}

	// check if only one team alive
	int iBestDamageTeam = 0;
	int iBestDamage = -1;
	int iNumAliveTeams = 0;
	for (i=0; i<4; i++) 
	{ 
		if (iBestDamage < iTeamsDamage[iTeam])
		{
			iBestDamage = iTeamsDamage[iTeam];
			iBestDamageTeam = i;
		}

		if (iTeams[i] != 0)
		{
			iNumAliveTeams = iNumAliveTeams + 1;
			NetServer.iWinnerTeam = i;
		}
	}

	if (iNumAliveTeams > 1) { return false; }

	if (iNumAliveTeams == 0) // all teams dead
	{
		NetServer.iWinnerTeam = iBestDamageTeam;
		return true;
	}

	return true;
}