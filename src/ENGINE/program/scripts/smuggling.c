// SLiB Новый файл для ВМЛ, все от К3 потерто

int ChangeContrabandRelation(ref pchar, int _val)
{
   pchar.questTemp.Contraband.relation = makeint(pchar.questTemp.Contraband.relation) + _val;
   if (makeint(pchar.questTemp.Contraband.relation) > 99) pchar.questTemp.Contraband.relation = 99;
   if (makeint(pchar.questTemp.Contraband.relation) < 0) pchar.questTemp.Contraband.relation = 0;
   
   return makeint(pchar.questTemp.Contraband.relation);
}

// работа с контрабандой
int FindFirstContrabandGoods(ref _refCharacter)
{
	int i;
	int curStoreIdx = GetCharacterCurrentStore(_refCharacter); 
	if(curStoreIdx>=0)
	{
		_refCharacter.FindContrabandGoods.StoreIdx = curStoreIdx;
		for(i=0; i<GOODS_QUANTITY; i++)
		{
			if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME )
			{
				if( GetSquadronGoods(_refCharacter,i)>0 )
				{
					_refCharacter.FindContrabandGoods.GoodsIdx = i;
					return i;
				}
			}
		}
		DeleteAttribute(_refCharacter,"FindContrabandGoods");
		return -1;
	}
	int curLocIdx = FindLocation(_refCharacter.location);
	if(curLocIdx<0) return -1;
	int curIslandIdx = GetIslandIdxByLocationIdx(curLocIdx);
	if(curIslandIdx<0) return -1;
	aref islRef; makearef(islRef,Islands[curIslandIdx].Trade.Contraband);
	int n, iMax;
	iMax   = GetAttributesNum(islRef); //fix
	for(i=0; i<iMax; i++)
	{
		n = sti(GetAttributeValue(GetAttributeN(islRef,0)));
		if( GetSquadronGoods(_refCharacter,n)>0 )
		{
			_refCharacter.FindContrabandGoods.IslandIdx = curIslandIdx;
			_refCharacter.FindContrabandGoods.GoodsIdx = i;
			return n;
		}
	}
	return -1;
}

int FindNextContrabandGoods(ref _refCharacter)
{
	int i,n;
	if(CheckAttribute(_refCharacter,"FindContrabandGoods.StoreIdx"))
	{
		if(CheckAttribute(_refCharacter,"FindContrabandGoods.GoodsIdx"))
		{
			int curStoreIdx = sti(_refCharacter.FindContrabandGoods.StoreIdx);
			if(curStoreIdx>=0)
			{
				for(i=sti(_refCharacter.FindContrabandGoods.GoodsIdx)+1; i<GOODS_QUANTITY; i++)
				{
					if( GetStoreGoodsType(&Stores[curStoreIdx],i) == TRADE_TYPE_CONTRABAND_NAME )
					{
						if( GetSquadronGoods(_refCharacter,i)>0 )
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return i;
						}
					}
				}
			}
		}
	}
	else
	{
		if(CheckAttribute(_refCharacter,"FindContrabandGoods.IslandIdx"))
		{
			if(CheckAttribute(_refCharacter,"FindContrabandGoods.GoodsIdx"))
			{
				int curIslandIdx = sti(_refCharacter.FindContrabandGoods.IslandIdx);
				if(curIslandIdx>=0)
				{
					int curGoodsNum = sti(_refCharacter.FindContrabandGoods.GoodsIdx) + 1;
					aref islRef; makearef(islRef,Islands[curIslandIdx].Trade.Contraband);
					for(i=curGoodsNum; i<GetAttributesNum(islRef); i++)      // это мне сомнительно, походу тут баг, но эта ветка не работает, тк магазин есть, тк разговор в таверне идет
					{
						n = GetAttributeValue(GetAttributeN(islRef,i));
						if( GetSquadronGoods(_refCharacter,n)>0 )
						{
							_refCharacter.FindContrabandGoods.GoodsIdx = i;
							return n;
						}
					}
				}
			}
		}
	}
	DeleteAttribute(_refCharacter,"FindContrabandGoods");
	return -1;
}

int GetCharacterCurrentStore(ref _refCharacter)
{
	bool bOk2 = CheckAttribute(Pchar, "quest.Contraband.Active") && (sti(Pchar.quest.Contraband.Active) == true); // в сделке, значит магазин уже ясен
	if (bOk2 && CheckAttribute(Pchar, "GenQuest.Contraband.StoreIdx"))
	{
		return sti(Pchar.GenQuest.Contraband.StoreIdx);
	}
	int i, idx;
	int curLocNum = FindLocation(_refCharacter.location);
	if(curLocNum<0) return -1;

    idx = -1;
	if (CheckAttribute(&Locations[curLocNum], "fastreload")) // в таверне должно быть всегда
	{
		string locGroupId = Locations[curLocNum].fastreload;
		for (i=0; i<STORE_QUANTITY; i++)
		{
			if (Stores[i].group == locGroupId)
			{
				idx = i;
				break;
			}
		}
	}
	else
	{
        idx = SHIP_STORE; // нет контры там, всегда пошлют, если вдруг нет fastreload в таверне
	}

    Pchar.GenQuest.Contraband.StoreIdx = idx; // запомним, для бухты, когда в сделку пойдем
	return  idx;
}
// to_do
int GetCurrentIslandGoodsPrice(int GoodsType)
{
	int curStoreIdx = GetCharacterCurrentStore(GetMainCharacter());   // вернет в бухте из запоминалки
	if(curStoreIdx<0) return 0;
	return GetStoreGoodsPrice(&Stores[curStoreIdx],GoodsType,PRICE_TYPE_SELL,GetMainCharacter(), 1);
}
// выбор куда идти
string SelectSmugglingLocation()
{
    ref CurIsland;
	int n;
	string TargetLocation
	
	n = GetCharacterCurrentIsland(Pchar);
	if (n < 0) 
	{
		TargetLocation = "None";
	}
	else
	{
		CurIsland = GetIslandByIndex(n);
		TargetLocation = GetIslandRandomShoreId(CurIsland.id);
    }
    Pchar.Quest.SelectedSmugglingLocation = TargetLocation;
    return TargetLocation;
}

////////////////////////////////////////////////////////////////////////////////
//                Контрабандисты НАЧАЛО
////////////////////////////////////////////////////////////////////////////////
void PlaceSmugglersOnShore(string LocationId)
{
	ref Smuggler, player;
	int RandCounter;
	int i;
	
	player = GetMainCharacter();

	string Model;
	
	player.GenQuest.Smugglers_Group = "Smugglers_1";
    LAi_group_Register(player.GenQuest.Smugglers_Group);
    
	for (i = 1; i <= 3; i++)
    {
        Model = "pirate_" + (rand(9)+1);
		Smuggler = GetCharacter(NPC_GenerateCharacter("Rand_Smug0" + i, Model, "man", "man", 5, PIRATE, 4, true)); // 4 дня, потом сами пропадут
		SetFantomParam(Smuggler);
		LAi_SetWarriorTypeNoGroup(Smuggler);
		LAi_warrior_DialogEnable(Smuggler, true);
		LAi_warrior_SetStay(Smuggler, true);
		Smuggler.Dialog.Filename = "Smuggler_OnShore_dialog.c";
		Smuggler.Dialog.CurrentNode = "First time";
		Smuggler.greeting = "Gr_Smuggler_OnShore";
		LAi_group_MoveCharacter(Smuggler, player.GenQuest.Smugglers_Group);
		ChangeCharacterAddressGroup(Smuggler, LocationID, "Smugglers", "Smuggler0" + i);
    }
	
	AddGeometryToLocation(LocationID, "smg");
	
	LAi_group_SetRelation(LAI_GROUP_PLAYER, player.GenQuest.Smugglers_Group, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(player.GenQuest.Smugglers_Group, "CoastalGuards", LAI_GROUP_NEITRAL); // патруль на берегу

	SetTimerCondition("Rand_Smuggling", 0, 0, 4, false);  // через 4 дня всех убрать
	// задание на завал контры
	if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
	{
		pchar.quest.KillSmugglers.win_condition.l1 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l1.character = "Rand_Smug01";
		pchar.quest.KillSmugglers.win_condition.l2 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l2.character = "Rand_Smug02";
		pchar.quest.KillSmugglers.win_condition.l3 = "NPC_Death";
		pchar.quest.KillSmugglers.win_condition.l3.character = "Rand_Smug03";
		pchar.quest.KillSmugglers.win_condition = "KillSmugglers_after";
	}
}


void RemoveSmugglersFromShore()
{
	ref Smuggler;
	int RandCounter, i, cn;
	string sLoc;

	DeleteAttribute(pchar, "quest.Contraband.Active");
	CloseQuestHeader("Gen_Contraband"); 

	if (CheckAttribute(pchar, "Quest.SelectedSmugglingLocation"))
	{
		sLoc = pchar.Quest.SelectedSmugglingLocation;
	}
//navy --> PGG
	else
	{
		if (CheckAttribute(pchar, "questTemp.PGGContra"))
		{
			sLoc = pchar.questTemp.PGGContra.Loc;
			Smuggler = &Characters[sti(pchar.questTemp.PGGContra)];
			ChangeCharacterAddressGroup(Smuggler, "None", "", "");
			Smuggler.location.from_sea = "";
			Smuggler.PGGAi.location.town = Smuggler.PGGAi.location.town.back;
			DeleteAttribute(pchar, "questTemp.PGGContra");
			DeleteAttribute(Smuggler, "PGGAi.location.town.back");
			CloseQuestHeader("Gen_StrangeInfo");
			//LAi_group_Delete("PGGTmp");
			pchar.Quest.PGG_WithSmugglers_Dead.Over = "yes";
			CloseQuestHeader("Gen_StrangeInfo");
		}
	}
//navy <--
	RemoveGeometryFromLocation(sLoc, "smg");

	pchar.quest.KillSmugglers_outShore.win_condition.l1 = "ExitFromLocation";
	pchar.quest.KillSmugglers_outShore.win_condition.l1.location = pchar.location;
	pchar.quest.KillSmugglers_outShore.win_condition = "KillSmugglers_outShore";
	for (i=1; i<=3; i++) //eddy
	{
		cn = GetCharacterIndex("Rand_Smug0"+i);
		if (cn != -1)
		{
			characters[cn].lifeDay = 0;
		}
	}
	LAi_group_SetRelation(LAI_GROUP_PLAYER, "CoastalGuards", LAI_GROUP_NEITRAL);
}

void SetCoastalGuardPursuit()
{
	ref Smuggler;
	int i;
	int iNation = sti(pchar.GenQuest.Contraband.GuardNation);// Нация патруля
	string Model;
	
	for (i = 1; i <= 3; i++)
    {
        Model = "off_" + NationShortName(iNation) + "_" + (rand(1) + 1);
		Smuggler = GetCharacter(NPC_GenerateCharacter("Coastal_Captain0" + i, Model, "man", "man", 5, iNation, 3, true)); // 3 дня, потом сами пропадут
		SetFantomParam(Smuggler);
		SelectCoastalGuardShip(Smuggler);
		Smuggler.Coastal_Captain = true; // если по нему палить, НЗГ не будет
		Smuggler.AlwaysEnemy = true;
		Smuggler.DontRansackCaptain = true;
		Smuggler.AlwaysSandbankManeuver = true;
		Group_addCharacter("Coastal_Guards", Smuggler.id);
		SetCharacterRelationBoth(sti(Smuggler.index), GetMainCharacterIndex(), RELATION_ENEMY);
		if (makeint(Pchar.rank) < 6 && i == 1 && GetCompanionQuantity(Pchar) == 1) break;
		if (makeint(Pchar.rank) < 9 && i == 2 && GetCompanionQuantity(Pchar) < 3) break;
    }
	Group_SetGroupCommander("Coastal_Guards", "Coastal_Captain01");

	Group_SetPursuitGroup("Coastal_Guards", PLAYER_GROUP);
	Group_SetAddress("Coastal_Guards", Islands[GetCharacterCurrentIsland(Pchar)].id, "", "");
	Group_SetTaskAttack("Coastal_Guards", PLAYER_GROUP);
	Group_LockTask("Coastal_Guards");
}


void StopCoastalGuardPursuit()
{
	ref rCap;
	int i, cn;
	
	for (i = 1; i <= 3; i++)
	{
		cn = GetCharacterIndex("Coastal_Captain0" + i);
		if (cn != -1)
		{
			rCap = GetCharacter(cn);
			if (rCap.location == Pchar.location && bSeaActive) return; // если в море погоня, то не трем, только на выход на карту.
			ChangeCharacterAddressGroup(rCap, "None", "", "");
			setCharacterShipLocation(rCap, "None");
		}
	}
	group_DeleteGroup("Coastal_Guards");
}

void SelectCoastalGuardShip(ref rCharacter)
{
	SetShipHunter(rCharacter);
}