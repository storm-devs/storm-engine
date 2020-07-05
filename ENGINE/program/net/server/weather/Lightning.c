int iServerLightningSubTexX, iServerLightningSubTexY;

void NetServer_WhrDeleteLightningEnvironment()
{
	DelEventHandler("NetServer_Lightning_DoIt", "NetServer_Lightning_DoIt");
}

void NetServer_WhrCreateLightningEnvironment()
{
	aref aCurWeather = NetServer_GetCurrentWeather();
	aref aLightning; makearef(aLightning, aCurWeather.Lightning);
	
	if (sti(aLightning.Enable) != true) return;

	SetEventHandler("NetServer_Lightning_DoIt", "NetServer_Lightning_DoIt", 0);

	iServerLightningSubTexX = Whr_GetLong(aLightning, "SubTexX");
	iServerLightningSubTexY = Whr_GetLong(aLightning, "SubTexY");

	PostEvent("NetServer_Lightning_DoIt", 1000 + rand(1000));
}

void NetServer_Lightning_DoIt()
{
	int iSMsg;

	int iNextTimeLightning = 200 + rand(2000);

	// next lightning
	PostEvent("NetServer_Lightning_DoIt", iNextTimeLightning);
	
	aref aCurWeather = NetServer_GetCurrentWeather();

	// FIX-ME : about all client ships center
	float cx = stf(Camera.Pos.x);
	float cz = stf(Camera.Pos.z);

	float fDist = 1000.0 + frnd() * 2000.0;
	if (rand(30) == 15) { fDist = 20.0 + frnd() * 200.0; }	// nearest lightning
	float fAngle = frnd() * PIm2;
	int iTimeSound = fDist / 333.0;

	float fTime = frnd() * 0.1 + 0.1;
	float x = cx + fDist * sin(fAngle);
	float y = 590.0;
	float z = cz + fDist * cos(fAngle);

	int iSubTexture = rand(iServerLightningSubTexX * iServerLightningSubTexY - 1);

	iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_WEATHER);
	NMAddByte(iSMsg, NSC_WEATHER_ADD_LIGHTNING);
	NMAddDword(iSMsg, iServerTime + iNextTimeLightning);
	NMAddWord(iSMsg, iSubTexture);
	NMAddFloat(iSMsg, fTime);
	NMAddFloat(iSMsg, x);
	NMAddFloat(iSMsg, y);
	NMAddFloat(iSMsg, z);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);

	// find nearest ships and do damage
	for (int i=0; i<NET_MAXCLIENTS; i++)
	{
		if (!sti(NSClients[i].Use)) { continue; }
		ref rClient, rShip; 
		aref arCShip; 

		rClient = NetServer_GetClient(i);
		makearef(arCShip, rClient.Ship);
		rShip = Net_GetShipByIndex(sti(arCShip.Type));

		float sx = stf(arCShip.Pos.x);
		float sy = stf(arCShip.Pos.y);
		float sz = stf(arCShip.Pos.z);
		float bx = stf(arCShip.BoxSize.x);
		float by = stf(arCShip.BoxSize.y);
		float bz = stf(arCShip.BoxSize.z);

		fDist = GetDistance2D(x, z, sx, sz);
		float fMinLightningDist = 20.0 + bz / 1.5;
		if (fDist < fMinLightningDist)
		{
			int iMinBoom = MakeInt(Bring2Range(5.0, 2.0, 1.0, 7.0, stf(rShip.Class))) + rand(2);
			float fCos = cos(stf(arCShip.Ang.y));
			float fSin = sin(stf(arCShip.Ang.y));
			for (int k=0; k<iMinBoom; k++)
			{
				float dx = 0.6 * bx * (frnd() - 0.5);
				float dz = 0.7 * bz * (frnd() - 0.5);
				RotateAroundY(&dx, &dz, fCos, fSin);
				//NetServer_Ship_Serious_Boom(sx + dx, sy + 0.1 * by * frnd(), sz + dz);

				iSMsg = NMCreate();
				NMAddByte(iSMsg, NC_SHIP);
				NMAddByte(iSMsg, NSC_SHIP_SERIOUS_BOOM);
				NMAddDword(iSMsg, rand(2000) + iServerTime);
				NMAddFloat(iSMsg, sx + dx);
				NMAddFloat(iSMsg, sy + 0.1 * by * frnd());
				NMAddFloat(iSMsg, sz + dz);
				NetServer_SendMessage(DST_ALL, iSMsg, false);
				NMDelete(iSMsg);
			}

			NetServer_ShipApplyHullHitpoints(rClient, iMinBoom * 40.0, NETKILL_BY_LIGHTNING, DST_INVALID);
			NetServer_ShipApplyCrewHitpoints(rClient, MakeFloat(rand(3)), DST_INVALID);

			// fireplaces test
			int iNumFirePlaces = sti(rClient.Ship.NumFirePlaces);
			float fCurr = 0.0;
			float fFirePlaces = Bring2Range(1.0, 0.1, 0.0, fMinLightningDist, fDist);
			float fTest = (1.0 - fFirePlaces) * iNumFirePlaces;
			for (int j=0; j<iNumFirePlaces; j++)
			{
				fCurr = fCurr + 1.0;
				if (fCurr < fTest) { continue; }

				fCurr = 0.0;
				int iRandStartTime = rand(10000);
				float fTotalFireTime = NetServer_ShipGetTotalFireTime(rClient);

				iSMsg = NMCreate();
				NMAddByte(iSMsg, NC_SHIP);
				NMAddByte(iSMsg, NSC_SHIP_ACTIVATE_FIREPLACE);
				NMAddByte(iSMsg, i);	// ClientID
				NMAddWord(iSMsg, j);	// Fireplace index
				NMAddDword(iSMsg, iRandStartTime + iServerTime);
				NMAddFloat(iSMsg, fTotalFireTime);
				NetServer_SendMessage(DST_ALL, iSMsg, false);
				NMDelete(iSMsg);

				PostEvent("NetServer_ShipFireDamage", iRandStartTime, "lllf", i, DST_INVALID, j, fTotalFireTime);
			}
		}
	}
}
