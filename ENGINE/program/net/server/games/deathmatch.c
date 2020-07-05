bool NetServer_CheckGameOver_DeathMatch()
{
	int i;
	int iNumAlive = 0;
	// if only one client alive - game over
	for (i=0; i<NET_MAXCLIENTS; i++) 
	{ 
		if (!sti(NSClients[i].Use)) { continue; } 
		if (stf(NSClients[i].Ship.hp) <= 0.0) { continue; } 
		iNumAlive++;
		NetServer.wWinnerClientID = NSClients[i].id;
	}
	if (iNumAlive == 1) { return true; }

	if (iNumAlive == 0) // all dead - check one client who did most damage
	{
		int iMaxDamage = -1;
		for (i=0; i<NET_MAXCLIENTS; i++) 
		{ 
			if (!sti(NSClients[i].Use)) { continue; } 
			int iDamage = sti(NSClients[i].Stat.DamageInflicted);
			if (iDamage < iMaxDamage) { continue; }

			iMaxDamage = iDamage;
			NetServer.wWinnerClientID = NSClients[i].id;
		}
		return true;		
	}

	return false;
}