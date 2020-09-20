object NSFortModel, NSFortBlots;

void NetServer_DeleteFortEnvironment()
{
	DeleteClass(NetServer_GetCLient(NET_FORTCLIENT));
	DelEventHandler("NetServer_FortSelectCannonType", "NetServer_FortSelectCannonType");
	DelEventHandler("NetServer_OnFortHit", "NetServer_OnFortHit");
	DelEventHandler("NetServer_OnFortCannonDamage", "NetServer_OnFortCannonDamage");
}

void NetServer_CreateFortEnvironment(int iSMsg)
{
	int iIslandIndex = Net_GetIslandIndex(NetServer.IslandID);

	if (NetServer.GameType != NETGAME_FORT || iIslandIndex < 0)
	{
		NMAddByte(iSMsg, false);		// no fort
		return;
	}

	bool bSuccess = false;
	ref rIsland = Net_GetIslandByIndex(iIslandIndex);
	aref arReload; makearef(arReload, rIsland.reload);
	int iNumReloads = GetAttributesNum(arReload);
	for (int i=0; i<iNumReloads; i++)
	{
		aref arLocator = GetAttributeN(arReload, i);
		ReloadProgressUpdate();
		if (CheckAttribute(arLocator, "fort"))
		{
			if (!CheckAttribute(arLocator, "fort.model") || arLocator.fort.model == "")
			{
				Trace("Island.id = " + rIsland.id + ", and reload." + GetAttributeName(arLocator) + ", doesn't have fort.model value.");
				continue;
			}

			ref rFort = NetServer_GetCLient(NET_FORTCLIENT);

			rFort.Server = true;
			CreateEntity(rFort, "NetFort");
			LayerAddObject("net_execute", rFort, 1);

			CreateEntity(&NSFortModel, "MODELR");
			string sNSFortModel = rIsland.filespath.models + "\" + arLocator.fort.model;

			SetTexturePath(0, rIsland.filespath.models + "\");

			SendMessage(&NSFortModel, "ls", MSG_MODEL_SET_LIGHT_PATH, NetServer_WhrGetLightingPath());
			SendMessage(&NSFortModel, "ls", MSG_MODEL_LOAD_GEO, sNSFortModel);
			SendMessage(&Island, "li", MSG_ISLAND_ADD_FORT,  &NSFortModel);
			LayerAddObject("fort_cannon_trace", &NSFortModel, 1);

			SetTexturePath(0, "");

			NSFortModel.Pos.x = arLocator.x;
			NSFortModel.Pos.z = arLocator.z;

			rFort.MaxCannonDamageDistance = MIN_CANNON_DAMAGE_DISTANCE;		// in meters

			rFort.Fort.Cannons.Type.Cannon = NETCANNON_CANNON;
			rFort.Fort.Cannons.Type.Culverine = NETCANNON_CULVERINE;
			rFort.Fort.Cannons.Type.Mortar = NETCANNON_MORTAR;

			ref rShip = Net_GetShipByName("FORT");
			rFort.Ship.Type = Net_GetShipIndex("FORT");
			
			rFort.Ship.HP = 100.0;
			rFort.Ship.Crew.Quantity = sti(rShip.MaxCrew);
			rFort.Ship.Crew.MinRatio = 1.0;
			rFort.Ship.Crew.Morale = 70;
			rFort.Ship.Name = arLocator.fortname;

			rFort.Ship.Cannons.Charge.Type = 3;	// bombs

			rFort.FriendlyTeam = 0;		// friendly team index

			// skills
			rFort.Skills.Accuracy = 10;
			rFort.Skills.Cannons = 10;
			rFort.Skills.Sailing = 5;
			rFort.Skills.Defence = 5;
			rFort.Skills.Repair = 5;

			// setup goods for fort
			for (int j=0; j<Net_GetGoodsNum(); j++)
			{
				ref rGood = Net_GetGoodByIndex(j);
				string sGood = rGood.Name;
				rFort.Ship.Goods.(sGood) = 10000;
			}
			rFort.Perks.Gunner = 0;
			rFort.Perks.GunnerCamicadze = 0;
			rFort.Perks.GunnerSniper = 0;
			rFort.Perks.BoardingMaster = 0;
			rFort.Perks.Musketeer = 0;
			rFort.Perks.BoardingDevil = 0;

			SendMessage(rFort, "laaii", MSG_NET_ADD_FORT, rIsland, arLocator, &NSFortModel, &NSFortBlots);

			//SendMessage(&Flag, "lil", MSG_FLAG_INIT, &NSFortModel, iNation); // install flag on fort
			bSuccess = true;
			NMAddByte(iSMsg, true);
			NMAddString(iSMsg, GetAttributeName(arLocator), 16);

			SetEventHandler("NetServer_FortSelectCannonType", "NetServer_FortSelectCannonType", 0);
			SetEventHandler("NetServer_OnFortHit", "NetServer_OnFortHit", 0);
			SetEventHandler("NetServer_OnFortCannonDamage", "NetServer_OnFortCannonDamage", 0);

			break;
		}
	}

	if (!bSuccess)
	{
		NMAddByte(iSMsg, false);		// no fort
	}
}

float NetServer_FortSelectCannonType()
{
	ref rFort = NetServer_GetCLient(NET_FORTCLIENT);

	rFort.Ship.Cannons.Type = GetEventData();

	NetServer_CannonRecalculateParameters(rFort);
	NetServer_CalculateFireTime(rFort);
	NetServer_CalculateReloadTime(rFort);

	return stf(rFort.Ship.Cannons.SpeedV0);
}

float NetServer_OnFortCannonDamage()
{
	int iCannonType = GetEventData();
	int iTotalCannons = GetEventData();
	int iDamagedCannons = GetEventData();
	int iBallClientID = GetEventData();
	float fDamage = GetEventData();
	float fDistanceRatio = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	if (iBallClientID == NET_FORTCLIENT) { return fDamage; }

	ref rFort = NetServer_GetCLient(NET_FORTCLIENT);

	fDamage += 0.12 * fDistanceRatio;

	if (fDamage >= 1.0)
	{
		fDamage = 1.0;
		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_FORT);
		NMAddByte(iSMsg, NSC_FORT_CANNON_DESTROY);
		NMAddFloat(iSMsg, x);
		NMAddFloat(iSMsg, y);
		NMAddFloat(iSMsg, z);
		NetServer_SendMessage(DST_ALL, iSMsg, false);
		NMDelete(iSMsg);

		iDamagedCannons++;
		
		rFort.Ship.HP = 100.0 * (1.0 - (iDamagedCannons / iTotalCannons));

		// check for fort defeat
		if (iDamagedCannons >= iTotalCannons / 2)
		{
			rFort.Ship.HP = -1.0;
		}
	}

	return fDamage;
}

void NetServer_OnFortHit()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_BALL);
	NMAddByte(iSMsg, NSC_BALL_HIT_FORT);
	NMAddWord(iSMsg, iServerCurrentBallIndex);
	NMAddFloat(iSMsg, x);
	NMAddFloat(iSMsg, y);
	NMAddFloat(iSMsg, z);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);
}