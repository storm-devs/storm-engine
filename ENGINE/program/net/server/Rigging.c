object NSSail;

void NetServer_DeleteRiggingEnvironment()
{
	DeleteClass(&NSSail);

	DelEventHandler("NetServer_GetSailTextureData", "NetClient_procGetSailTextureData");
	DelEventHandler("NetServer_GetRiggingData", "NetClient_procGetRiggingData");
	DelEventHandler("NetServer_OnShipSailDamage", "NetServer_OnShipSailDamage");
	DelEventHandler("NetServer_OnShipSailChangeState", "NetServer_OnShipSailChangeState");
}

void NetServer_CreateRiggingEnvironment(int iMsg)
{
	NSSail.Server = true;
	CreateEntity(&NSSail, "NetSail");
	LayerAddObject("net_execute", &NSSail, iClientShipPriorityExecute + 1);
	NSSail.MinSpeed = 0.05;

	SetEventHandler("NetServer_GetSailTextureData", "NetClient_procGetSailTextureData", 0);
	SetEventHandler("NetServer_GetRiggingData", "NetClient_procGetRiggingData", 0);
	SetEventHandler("NetServer_OnShipSailDamage", "NetServer_OnShipSailDamage", 0);
	SetEventHandler("NetServer_OnShipSailChangeState", "NetServer_OnShipSailChangeState", 0);
}

void NetServer_OnShipSailChangeState()
{
	int wOurClientID = GetEventData();
	int iSailIndex = GetEventData();
	int iSailState = GetEventData();

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SHIP);
	NMAddByte(iSMsg, NSC_SHIP_SAIL_STATE);
	NMAddClientID(iSMsg, wOurClientID);
	NMAddDword(iSMsg, iSailIndex);	// OPTIMIZATION
	NMAddDword(iSMsg, iSailState);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);
}

void NetServer_OnShipSailDamage()
{
	int wOurClientID = GetEventData();
	int wBallClientID = GetEventData();

	ref rBallClient = NetServer_GetCLient(wBallClientID);
	rBallClient.Stat.Hits = sti(rBallClient.Stat.Hits) + 1;

	/*int wOurClientID = GetEventData();
	int wBallClientID = GetEventData();
	int iSailIndex = GetEventData();
	int iSailState = GetEventData();

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_SHIP);
	NMAddByte(iSMsg, NSC_SHIP_SAIL_STATE);
	NMAddClientID(iSMsg, wOurClientID);
	NMAddDword(iSMsg, iSailIndex);	// OPTIMIZATION
	NMAddDword(iSMsg, iSailState);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);*/
}

bool NetServer_OnShipSailRepair( int wOurClientID )
{
	bool bNotEnoughSailcloth = true;

	ref	rOurClient = NetServer_GetCLient(wOurClientID);
	ref rBaseShip = Net_GetShipByIndex(sti(rOurClient.Ship.Type));
	int iActiveSeconds = sti(rOurClient.Skills.Repair.Active) - 1;
	int iSkillRepair = sti(rOurClient.Skills.Repair);

	float fMaxSP = stf(rBaseShip.SP);
	float fCurSP = stf(rOurClient.Ship.SP);
	if( fCurSP < fMaxSP )
	{
		float fPercentSPRestore = fMaxSP * Bring2Range(0.0, 0.2, 0.1, 1.0, iSkillRepair / NETSKILL_MAX) / 30.0;
		if( fCurSP+fPercentSPRestore > fMaxSP ) {
			fPercentSPRestore = fMaxSP - fCurSP;
		}

		int i,rq, j,gq;
		string tmpstr;
		aref arRoot,arGroup,arSail;
		makearef(arRoot, rOurClient.ship.sails);
		rq = GetAttributesNum(arRoot);

		for(i=0; i<rq; i++)
		{
			arGroup = GetAttributeN(arRoot, i);
			gq = GetAttributesNum(arGroup);
			for(j=0; j<gq; j++)
			{
				arSail = GetAttributeN(arGroup, j);

				if( CheckAttribute(arSail,"mastFall") )
				{
					tmpstr = "ship.masts."+arSail.mastFall;
					if( CheckAttribute(rOurClient,tmpstr) && stf(rOurClient.(tmpstr))>=1.0 )	{continue;}
				}
				fPercentSPRestore -= Net_OneSailDmgRepair(rOurClient,arGroup,arSail,fPercentSPRestore);
				if(fPercentSPRestore<=0.0) {break;}
				if(!CheckAttribute(rOurClient,"ship.sails")) break;
				if( GetAttributesNum(arRoot) != rq ) break;
				if( GetAttributesNum(arGroup) != gq )
				{
					gq = GetAttributesNum(arGroup);
					j--;
				}
			}
			if( fPercentSPRestore <= 0.0 ) {break;}
			if( !CheckAttribute(rOurClient,"ship.sails") ) break;
			if( GetAttributesNum(arRoot) != rq )
			{
				rq = GetAttributesNum(arRoot);
				i--;
			}
		}

		rOurClient.ship.sp = Net_CalculateShipSP(rOurClient);
		bNotEnoughSailcloth = false;
	}

	return bNotEnoughSailcloth;
}

float Net_OneSailDmgRepair(ref chref, aref arSailNode, aref arSail, float fDmgRepair)
{
	if(fDmgRepair<=0.0) return 0.0;
	float fSailDmg = stf(arSail.dmg);
	if(fDmgRepair>=fSailDmg)
	{
		DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arSailNode), sti(arSail.hd), sti(arSail.hc) );
		DeleteAttribute(arSailNode,GetAttributeName(arSail));
		if( GetAttributesNum(arSailNode)==0 )
		{
			DeleteAttribute(chref,"ship.sails."+GetAttributeName(arSailNode));
		}
		return fSailDmg;
	}

	fSailDmg -= fDmgRepair;
	float sailDmgMax = 100.0 * stf(arSail.sp);//GetCharacterShipSP(chref);
	int iAfterHole = GetNeedHoleFromDmg( fSailDmg, sailDmgMax, sti(arSail.mhc) );
	if( sti(arSail.hc) > iAfterHole )
	{
		arSail.hd = DeleteOneSailHole( sti(chref.index), GetAttributeName(arSail), GetAttributeName(arSailNode), sti(arSail.hd), sti(arSail.hc)-iAfterHole );
		arSail.hc = iAfterHole;
	}
	arSail.dmg = fSailDmg;
	return fDmgRepair;
}
