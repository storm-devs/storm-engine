
bool NetClient_IsTeam(int wOurCharacterID, int wOtherCharacterID)
{
	return false;
}

bool NetClient_IsCLient(int wOurCharacterID)
{
	if (wOurCharacterID == DST_INVALID) { return false; }
	if (sti(NCClients[wOurCharacterID].Use)) { return true; } 
	return false;
}

bool NetClient_IsClientDead(ref rClient)
{
	return sti(rClient.Dead);
}

ref NetClient_GetClient(int wOurCharacterID) { return &NCClients[wOurCharacterID]; }
int NetClient_GetThisClientID() { return wClientID; }
ref NetClient_GetThisClient() { return &NCClients[wClientID]; }
