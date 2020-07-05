void NetServer_OnCamera(int wNetClientID, int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_DECK_CAMERA_UPDATE:
			EntityUpdate(0);
			ref rClient = NetServer_GetClient(wNetClientID);
			rClient.Camera.Dir.x = NMGetFloat(iMsg);
			rClient.Camera.Dir.y = NMGetFloat(iMsg);
			rClient.Camera.Dir.z = NMGetFloat(iMsg);
			EntityUpdate(1);

			rClient.IsCanFire = "";	// IsCanFire and CurrentTargetRelation updated here

			int iOurTeam = sti(rClient.Team);
			int iCurrentTargetTeam = sti(rClient.CurrentTargetTeam);

			int iSMsg = NMCreate();
			NMAddByte(iSMsg, NC_CAMERA);
			NMAddByte(iSMsg, NSC_DECK_CAMERA_UPDATE);
			NMAddByte(iSMsg, sti(rClient.IsCanFire));
			NMAddByte(iSMsg, NetServer_GetTeamRelation(iOurTeam, iCurrentTargetTeam));
			NetServer_SendMessage(wNetClientID, iSMsg, false);
			NMDelete(iSMsg);
		break;
	}
}

int NetServer_GetTeamRelation(int iOurTeam, int iCurrentTargetTeam)
{
	if (iCurrentTargetTeam == -1) return 0;							// nothing selected
	if (sti(NetServer.GameType) == NETGAME_DEATHMATCH) return 2;	// enemy
	if (iOurTeam != iCurrentTargetTeam) return 2;					// enemy
	return 1;														// friend
}