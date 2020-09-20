object NetRanks[11];
float NetRewardAccuracy[3];
float NetRewardVitality[3];
float NetRewardVictorious[3];
int iNetRanksNum;

void Net_InitRank(int n, string sName, int iSunk, int iWon)
{
	NetRanks[n].Rank = sName;
	NetRanks[n].Sunk = iSunk;
	NetRanks[n].Won = iWon;
}

void Net_InitRewards()
{
	Net_InitRank(0, "Cadet", 5, 0);
	Net_InitRank(1, "Seaman", 15, 0);
	Net_InitRank(2, "Boatswain", 30, 5);
	Net_InitRank(3, "Lieutenant", 50, 10);
	Net_InitRank(4, "3rank_Captain", 75, 15);
	Net_InitRank(5, "2rank_Captain", 100, 20);
	Net_InitRank(6, "1rank_Captain", 150, 30);
	Net_InitRank(7, "Rear_Admiral", 200, 40);
	Net_InitRank(8, "Vice_Admiral", 300, 50);
	Net_InitRank(9, "Admiral", 1000, 100);
	Net_InitRank(10, "Navarch", 1000000000, 250);	// stupid tricks

	iNetRanksNum = 11;

	// Rewards: Accuracy
	NetRewardAccuracy[0] = 0.55;	// 1st rank
	NetRewardAccuracy[1] = 0.45;	// 2nd rank
	NetRewardAccuracy[2] = 0.35;	// 3rd rank

	// Rewards: Vitality
	NetRewardVitality[0] = 0.95;
	NetRewardVitality[1] = 0.85;
	NetRewardVitality[2] = 0.75;

	// Rewards: Victorious
	NetRewardVictorious[0] = 0.95;
	NetRewardVictorious[1] = 0.85;
	NetRewardVictorious[2] = 0.75;
}

int Net_GetRewardAccuracy(int iHits, int iMisses)
{
	float fAccuracy = 0.0;
	if (iHits + iMisses > 0) { fAccuracy = makefloat((iHits) / (iHits + iMisses)); }

	for (int i=0; i<3; i++) { if (NetRewardAccuracy[i] <= fAccuracy) return i + 1; }
	return 0;
}

int Net_GetRewardVitality(int iSunks, int iWrecks)
{
	float fVitality = 0.0;
	if (iSunks + iWrecks > 0) { fVitality = makefloat((iSunks) / (iSunks + iWrecks)); }

	for (int i=0; i<3; i++) { if (NetRewardVitality[i] <= fVitality) return i + 1; }
	return 0;
}

int Net_GetRewardVictorious(int iWon, int iLost)
{
	float fVictorious = 0.0;
	if (iWon + iLost > 0) { fVictorious = makefloat((iWon) / (iWon + iLost)); }

	for (int i=0; i<3; i++) { if (NetRewardVictorious[i] <= fVictorious) return i + 1; }
	return 0;
}

int Net_GetRankNum(int iSunk, int iWon)
{
	for (int i=0; i<iNetRanksNum; i++)
	{
		int iThisSunk = sti(NetRanks[i].Sunk);
		int iThisWon = sti(NetRanks[i].Won);
		
		if (iSunk < iThisSunk || iWon < iThisWon) { return i; }
	}

	return 10;
}

