int nTeleportShowData;
object objTeleport;

#event_handler("QuickTeleport","StartQuickTeleport");
#event_handler(EVENT_LOCATION_LOAD,"WQuickTeleport");

void InitTeleport()
{
	nTeleportShowData = 0;
	CreateEntity(&objTeleport,"TMPTELEPORT");
	LayerAddObject("execute",&objTeleport,-1);
	LayerAddObject("realize",&objTeleport,-1);
	SetEventHandler("TeleportChoose","DoTeleportChoose",0);
	SetEventHandler("TeleportStart","DoTeleportStart",0);
}

void DoTeleportStart()
{
	SetTeleportData(0);
}

void DoTeleportChoose()
{
	aref datRef,selRef;
	int chooseNum = GetEventData();
	string attrName = "id"+chooseNum;
	ref rch = GetMainCharacter();
	int n;
	aref ar1,ar2;

	ref tmp_chref;

	switch(nTeleportShowData)
	{
	case 0:
		teleportType=chooseNum;
		if(chooseNum==2)
		{	SetTeleportData(10);
		}
		else
		{	SetTeleportData(1);
		}
		break;
	case 1:
		if(CheckAttribute(objTeleport,"data."+attrName))
		{
			tel_location_id = objTeleport.data.(attrName);
		}
		else
		{
			tel_location_id = "";
		}
		sTeleportLocName = tel_location_id;
		if(teleportType==1)
		{
			SetCharacterShipLocation(rch,sTeleportLocName);
			n = FindIslandBySeaLocation(sTeleportLocName);
			if(n!=-1)
			{
				/* Commented by Max -> WorlMap is changed
				wdmCurrentIsland = Islands[n].id;
				makearef(ar1,worldMap.islands);
				for(n=0; n<GetAttributesNum(ar1); n++)
				{
					ar2 = GetAttributeN(ar1,n);
					if(ar2.name==wdmCurrentIsland)
					{
						worldMap.playerShipX = ar2.position.x;
						worldMap.playerShipZ = ar2.position.z;
						break;
					}
				}
				*/
			}
			DoReloadCharacterToLocation(rch.location,rch.location.group,rch.location.locator);
		}
		else
		{
			SetTeleportData(2);
		}
		break;
	case 2:
		makearef(datRef,objTeleport.data);
		attrName = GetAttributeName(GetAttributeN(datRef,chooseNum));
		locatorGroup = objTeleport.data.(attrName);
		SetTeleportData(3);
		break;
	case 3:
		if(CheckAttribute(objTeleport,"data."+attrName))
		{
			locatorName = objTeleport.data.(attrName);
		}
		else
		{
			locatorName = "";
		}
		DoReloadCharacterToLocation(tel_location_id,locatorGroup,locatorName);
		break;

	case 10:
		tel_dialogFileName = objTeleport.data.(attrName);
		SetTeleportData(11);
		break;

	case 11:
		tel_dialogNodeName = objTeleport.data.(attrName);
		n = GetCharacterIndex("DilogChecking");
		if( n<0 || !IsEntity(&Characters[n]) )
		{
			tmp_chref = LAi_CreateFantomCharacter("Man6_1","","");
			LAi_SetStayType(tmp_chref);
			tmp_chref.id = "DilogChecking";
			tmp_chref.lastname = "";
			tmp_chref.model = "";
			tmp_chref.headModel = "";
		}
		else
		{
			tmp_chref = GetCharacter(n);
		}
		tmp_chref.name = tel_dialogFileName;
		tmp_chref.Dialog.Filename = tel_dialogFileName;
		tmp_chref.Dialog.CurrentNode = tel_dialogNodeName;
		DialogMain(tmp_chref);
		break;
	}
}

string tel_dialogFileName = "";
string tel_dialogNodeName = "";

int teleportType;
string tel_location_id = "";
string locatorGroup = "";
string locatorName = "";

void SetTeleportData(int tshowType)
{
	string attrName,locName;
	ref locatorRef;
	int locationNum;
	aref groupRef,locRef;
	int i,n;

	aref arList;
	aref arData;

	nTeleportShowData = tshowType;
	DeleteAttribute(&objTeleport,"");

	switch(tshowType)
	{
	case 0: // Выбор телепорт персонажа или корабля
		objTeleport.data.id1 = "character teleport";
		objTeleport.data.id2 = "ship teleport";
		objTeleport.data.id3 = "character dialog";
		break;
	case 1: // Выбор локации
		for(i = 0; i < MAX_LOCATIONS; i++)
		{
			if(Locations[i].id != "")
			{
				attrName = "id"+i;
				objTeleport.data.(attrName) = Locations[i].id;
			}
		}
		break;
	case 2: // Выбор группы локаторов
		locationNum = FindLocation(tel_location_id);
		if(locationNum!=-1)
		{
			makeref(locatorRef,Locations[locationNum]);
			if(CheckAttribute(locatorRef,"boarding") && locatorRef.boarding=="true")
			{
				DoReloadCharacterToLocation(tel_location_id,"rld","loc0");
				nTeleportShowData = 0;
				return;
			}
			if(CheckAttribute(locatorRef,"reload"))
			{
				objTeleport.data.id1 = "reload";
			}
			if(CheckAttribute(locatorRef,"camdetector"))
			{
				objTeleport.data.id2 = "camdetector";
			}
			if(CheckAttribute(locatorRef,"characters"))
			{
				objTeleport.data.id3 = "characters";
			}
			if(CheckAttribute(locatorRef,"goto"))
			{
				objTeleport.data.id4 = "goto";
			}
		}
		break;
	case 3: // Выбор локатора
		Trace("~~~~~~~ loc id = "+tel_location_id + " group id = "+locatorGroup);
		locationNum = FindLocation(tel_location_id);
		if(locationNum!=-1)
		{
			makeref(locatorRef,Locations[locationNum]);
			if(CheckAttribute(locatorRef,locatorGroup))
			{
				makearef(groupRef,locatorRef.(locatorGroup));
				n = GetAttributesNum(groupRef);
				for(i=0; i<n; i++)
				{
					attrName = "id"+i;
					locRef = GetAttributeN(groupRef,i);
					//locName = GetAttributeName(locRef);
					objTeleport.data.(attrName) = locRef.name;
				}
			}
		}
		break;
	case 10: // выбор диалога
		object objFileFinder;
		objFileFinder.dir = "PROGRAM\DIALOGS";
		objFileFinder.mask = "*.c";
		CreateEntity(&objFileFinder,"FINDFILESINTODIRECTORY");
		makearef(arList,objFileFinder.filelist);
		if( GetAttributesNum(arList)>0 )
		{
			makearef(arData,objTeleport.data);
			CopyAttributes(arData,arList);
		}
		break;
	case 11: // Выбор нода
		object objNodeFinder;
		objNodeFinder.file = "PROGRAM\DIALOGS\" + tel_dialogFileName;
		CreateEntity(&objNodeFinder,"FINDDIALOGNODES");
		makearef(arList,objNodeFinder.nodelist);
		if( GetAttributesNum(arList)>0 )
		{
			makearef(arData,objTeleport.data);
			CopyAttributes(arData,arList);
		}
		break;
	}

	makearef(locRef,objTeleport.data);
	SendMessage(&objTeleport,"la",42222,locRef);
}

void StartQuickTeleport()
{
	if(tel_location_id!="" && locatorGroup!="" && locatorName!="")
		DoReloadCharacterToLocation(tel_location_id,locatorGroup,locatorName);
}

void WQuickTeleport()
{
	ref chref = GetMainCharacter();
	tel_location_id = chref.location;
	locatorGroup = chref.location.group;
	locatorName = chref.location.locator;
}
