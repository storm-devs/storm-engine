
bool NetServer_IsTeam(int wOurCharacterID, int wOtherCharacterID)
{
	return false;
}

bool NetServer_IsCLient(int wOurCharacterID)
{
	if (wOurCharacterID == DST_INVALID) { return false; }
	if (GetAttributesNum(NSClients[wOurCharacterID])) { return true; }
	return false;
}

bool NetServer_IsClientDead(ref rClient)
{
	return sti(rClient.Dead);
}

ref NetServer_GetCLient(int wOurCharacterID)
{
	return &NSClients[wOurCharacterID];
}

float NetServer_AddClientExp(ref rClient, float fExp)
{
	rClient.Exp = stf(rClient.Exp) + fExp;
	return stf(rClient.Exp);
}

float NetServer_IsPerkUse(string sPerk, float fOff, float fOn)
{
	if (sti(sPerk)) { return fOn; }
	return fOff;
}

int NetServer_GetNumClients()
{
	int i;
	int iNumClients = 0;
	for (i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }
		iNumClients++;
	}

	return iNumClients;
}

int NetServer_GetNumTeams()
{
	int i;
	int iTeams[4];
	int iNumNetTeams = 0;
	
	for (i=0; i<4; i++) { iTeams[i] = 0; }
	for (i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }

		int iTeam = sti(NSClients[i].Team);
		iTeams[iTeam] = iTeams[iTeam] + 1;
	}
	for (i=0; i<4; i++) { if (iTeams[i] > 0) { iNumNetTeams++; } } 

	return iNumNetTeams;
}