void NetClient_DeleteRiggingEnvironment()
{
	DeleteClass(&NCSail);
	DeleteClass(&NCRope);
	DeleteClass(&NCFlag);
	DeleteClass(&NCVant);

	DelEventHandler("NetClient_GetSailTextureData", "NetClient_procGetSailTextureData");
	DelEventHandler("NetClient_GetRiggingData", "NetClient_procGetRiggingData");
	DelEventHandler("NetClient_OnShipSailDamageSound", "NetClient_OnShipSailDamageSound");
}

void NetClient_CreateRiggingEnvironment(int iMsg)
{
	NCSail.Server = false;
	CreateEntity(&NCSail, "NetSail");
	LayerAddObject("net_execute", &NCSail, iClientShipPriorityExecute + 1);
	LayerAddObject("net_realize", &NCSail, iClientShipPriorityRealize + 1);
	LayerAddObject("sea_reflection", &NCSail, 3);
	LayerAddObject("sails_trace", &NCSail, 10);
	NCSail.MinSpeed = 0.05;

	NCRope.Server = false;
	CreateEntity(&NCRope, "NetRope");
	LayerAddObject("net_execute", &NCRope, iClientShipPriorityExecute + 2);
	LayerAddObject("net_realize", &NCRope, iClientShipPriorityRealize + 2);

	NCFlag.Server = false;
	CreateEntity(&NCFlag, "NetFlag");
	LayerAddObject("net_execute", &NCFlag, iClientShipPriorityExecute + 3);
	LayerAddObject("net_realize", &NCFlag, iClientShipPriorityRealize + 3);
	LayerAddObject("sea_reflection", &NCFlag, 3);

	NCVant.Server = false;
	CreateEntity(&NCVant, "NetVant");
	LayerAddObject("net_execute", &NCVant, iClientShipPriorityExecute + 4);
	LayerAddObject("net_realize", &NCVant, iClientShipPriorityRealize + 4);

	SetEventHandler("NetClient_GetSailTextureData", "NetClient_procGetSailTextureData", 0);
	SetEventHandler("NetClient_GetRiggingData", "NetClient_procGetRiggingData", 0);
	SetEventHandler("NetClient_OnShipSailDamageSound", "NetClient_OnShipSailDamageSound", 0);
}

ref NetClient_procGetSailTextureData()
{
	int st, i, sq;
	ref shref;
	aref arEmbl, arSail;
	string attrName;

	aref arClient = GetEventData();

	DeleteAttribute(&BI_objRetValue, "");
	// sails upgrade
	BI_objRetValue.normalTex = "ships\parus_" + NetShipSailUpgrade[sti(arClient.ship.Upgrades.Sails) - 1].str + ".tga";

	BI_objRetValue.sailscolor = argb(255,255,255,255);
	//BI_objRetValue.geraldTex = "Net\Sails\france.tga";
	//BI_objRetValue.geraldTexPointer = 0; // (IDirect3DTexture8*)

	string nationFileName = "ships\parus_common.tga";
	string tmpStr;

	if( CheckAttribute(&arClient,"ship.sails.color") ) {
		BI_objRetValue.sailscolor = sti(arClient.ship.sails.color);
	}

	st = sti(arClient.Ship.Type);
	if (st!=SHIP_NOTUSED)
	{
		shref = Net_GetShipByIndex(st);
		BI_objRetValue.maxSP = shref.sp;

		if (CheckAttribute(shref, "GeraldSails")) 
		{
			makearef(arEmbl, shref.GeraldSails);
		} else {
			makearef(arEmbl, shref.NetGeraldSails);
		}
		sq = GetAttributesNum(arEmbl);
		for(i=0; i<sq; i++)
		{
			arSail = GetAttributeN(arEmbl,i);
			attrName = GetAttributeName(arSail);
			tmpStr = GetAttributeValue(arSail);
			if(tmpStr=="1")	{BI_objRetValue.(attrName) = nationFileName;}
			else	{BI_objRetValue.(attrName) = tmpStr;}
		}
	}
	// проставляем гербовый знак (в текстуре или при помощи поинтера на текстуру)
	//BI_objRetValue.geraldTex = "ships\gerald\8ball1.tga";
	//BI_objRetValue.geraldTexPointer = 0; // (IDirect3DTexture8*)

	BI_objRetValue.geraldTexPointer = arClient.UserData.Sail.Texture;

	return &BI_objRetValue;
}

ref NetClient_procGetRiggingData()
{
	int i,n;

	int retVal = 0;

	string datName = GetEventData();
	
	if(datName=="GetFlagTexNum")
	{
		i = GetEventData();
		n = GetEventData();
		if(i==true)
		{
			switch(n)
			{
			case ENGLAND: retVal = 1; break;
			case FRANCE: retVal = 2; break;
			case SPAIN: retVal = 4; break;
			case PIRATE: retVal = 5; break;
			case HOLLAND: retVal = 0; break;
			}
		}
		else
		{
			switch(n)
			{
			case ENGLAND: retVal = 1; break;
			case FRANCE: retVal = 2; break;
			case SPAIN: retVal = 4; break;
			case PIRATE: retVal = 5; break;
			case HOLLAND: retVal = 0; break;
			}
		}
	}
	if(datName=="GetFlagTexPointer")
	{
		aref arClient = GetEventData();
		retVal = sti(arClient.UserData.Flag.Texture);
	}
	return &retVal;
}

void NetClient_OnShipSailDamageSound()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	Play3DSound("ball2sail", x, y, z);
}

void NetClient_OnShipSailDamage(int iMsg)
{
	int wOurClientID = NMGetClientID(iMsg);
	int iSailIndex = NMGetDword(iMsg);		// OPTIMIZATION
	int iSailState = NMGetDword(iMsg);

	SendMessage(&NCSail, "lslll", MSG_SAIL_SCRIPT_PROCESSING, "SailMakeHole", wOurClientID, iSailIndex, iSailState);
}
