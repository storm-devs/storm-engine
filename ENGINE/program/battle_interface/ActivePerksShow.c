
object objActivePerkShower;

#event_handler(EVENT_LOCATION_LOAD,"procLoadIntoNew");

void InitActivePerkShower()
{
	objActivePerkShower.ShowParam.PosRect.left		= sti(showWindow.right) - RecalculateHIcon(48+132);
	objActivePerkShower.ShowParam.PosRect.top		= RecalculateVIcon(426);
	objActivePerkShower.ShowParam.PosRect.right		= sti(showWindow.right) - RecalculateHIcon(48);
	objActivePerkShower.ShowParam.PosRect.bottom	= sti(showWindow.bottom) - RecalculateVIcon(28);
	objActivePerkShower.ShowParam.IconSize.horz		= RecalculateHIcon(48);
	objActivePerkShower.ShowParam.IconSize.vert		= RecalculateVIcon(48);
	objActivePerkShower.ShowParam.IconSpace.horz	= RecalculateHIcon(2);
	objActivePerkShower.ShowParam.IconSpace.vert	= RecalculateVIcon(2);
}

void procLoadIntoNew()
{
	UnloadActivePerkShower();
	SetEventHandler("frame","LoadActivePerkShower",1);
}

void LoadActivePerkShower()
{
	DelEventHandler("frame","LoadActivePerkShower");

	DeleteAttribute(&objActivePerkShower,"PerkList.list");
	DeleteAttribute(&objActivePerkShower,"Textures");

	// информация о текстурах
	if(bSeaActive && !bAbordageStarted)
	{	// морская часть
		objActivePerkShower.Textures.t0.file = "battle_interface\list_icons.tga";
		objActivePerkShower.Textures.t0.horz = 16;
		objActivePerkShower.Textures.t0.vert = 8;
	}
	else
	{	// земная часть
		objActivePerkShower.Textures.t0.file = "battle_interface\LandCommands.tga";
		objActivePerkShower.Textures.t0.horz = 16;
		objActivePerkShower.Textures.t0.vert = 2;
	}

	aref arPerksRoot,arPerk;
	makearef(arPerksRoot,pchar.perks.list);
	int perksQ = GetAttributesNum(arPerksRoot);

	string stmp;
	int idx = 0;
	for(int j=0; j<perksQ; j++)
	{
		arPerk = GetAttributeN(arPerksRoot,j);
		if( CheckAttribute(arPerk,"delay") )
		{
			if( CheckAttribute(arPerk,"active") )
			{
				AddPerkToActiveList(GetAttributeName(arPerk));
			}
		}
	}

	CreateEntity(&objActivePerkShower,"ActivePerkShower");
	if(bSeaActive && !bAbordageStarted)
	{	// морская часть
		LayerAddObject(SEA_EXECUTE,&objActivePerkShower,-1);
		LayerAddObject(SEA_REALIZE,&objActivePerkShower,-1);
	}
	else
	{
		LayerAddObject("execute",&objActivePerkShower,-1);
		LayerAddObject("realize",&objActivePerkShower,-1);
	}
}

void UnloadActivePerkShower()
{
	DeleteClass(&objActivePerkShower);
	DeleteAttribute(&objActivePerkShower,"PerkList");
}

bool IsPerkIntoList(string perkID)
{
	return CheckAttribute(&objActivePerkShower,"PerkList.list."+perkID);
}

void AddPerkToActiveList(string perkID)
{
	if( !IsEntity(&objActivePerkShower) ) return;
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( CheckAttribute(arRoot,perkID) ) return;

	makearef(arCur,arRoot.(perkID));
	arCur.texture = GetTextureIndex(GetPerkTextureName(perkID));
	arCur.pic_idx = GetPerkPictureIndex(perkID);
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"add",arCur);
}

int GetPerkPictureIndex(string perkName)
{
	switch(perkName)
	{
		case "TimeSpeed":
	        if(bSeaActive && !bAbordageStarted)
			{
				return 59;
			}
			else
			{
			    return 15;
			}
		break;
		case "Sink":			return 43; break;
		case "Repair":			return 55; break;
		// boal зачем же так игру кастрировать? Грустно, однако :( -->
		case "Rush":	        return 28;	break;
		case "SandbankManeuver":return 57; break;
		case "LightRepair":		return 54; break;
		case "InstantRepair":	return 55; break;
		case "ImmediateReload":	return 53; break;
		case "Turn180":			return 56; break;
		// boal <--
	}
	trace("WARNING!!! Perk name - "+perkName+" hav`t picture");
	return 0;
}

string GetPerkTextureName(string perkName)
{
	switch(perkName)
	{
		case "TimeSpeed":
			if(bSeaActive && !bAbordageStarted)
			{
				return "battle_interface\list_icons.tga";
			}
			else
			{
			    return "battle_interface\LandCommands.tga";
			}
		break;
		case "Sink":			return "battle_interface\list_icons.tga"; break;
		case "Repair":			return "battle_interface\list_icons.tga"; break;
		case "Rush":			return "battle_interface\LandCommands.tga";	break;
		case "ImmediateReload":	return "battle_interface\list_icons.tga"; break;
		case "InstantBoarding":	return "battle_interface\list_icons.tga"; break;
		case "Turn180":			return "battle_interface\list_icons.tga"; break;
		case "SandbankManeuver":return "battle_interface\list_icons.tga"; break;
		case "LightRepair":		return "battle_interface\list_icons.tga"; break;
		case "InstantRepair":	return "battle_interface\list_icons.tga"; break;
	}
	trace("WARNING!!! Perk name - "+perkName+" hav`t texture");
	return "";
}

int GetTextureIndex(string texName)
{
	aref arTexList,arCurTex;
	makearef(arTexList,objActivePerkShower.Textures);
	int i,q;

	q = GetAttributesNum(arTexList);
	for(i=0; i<q; i++)
	{
		arCurTex = GetAttributeN(arTexList,i);
		if(arCurTex.file == texName) return i;
	}
	trace("WARNING!!! Texture name - "+texName+" hav`t describe");
	return -1;
}

void DelPerkFromActiveList(string perkID)
{
	aref arRoot,arCur;
	makearef(arRoot,objActivePerkShower.PerkList.list);
	if( !CheckAttribute(arRoot,perkID) ) return;

	makearef(arCur,arRoot.(perkID));
	SendMessage(&objActivePerkShower,"lsa",MSG_ACTIVE_PERK_LIST_REFRESH,"del",arCur);
	DeleteAttribute(arRoot,perkID);
}