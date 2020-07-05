object MerchantParam;
bool trap = false;

string CreateMerchant(int ination)
{
    ref chref, sld;
    aref  MerPrm;

    makearef(MerPrm, MerchantParam);

    //for (int n = 0; n<=4; n++ )
    //{

        //Map_ReleaseQuestEncounter( NationShortName(sti(MerPrm.nation))+"_QuestMerchant");
        //Group_DeleteGroup("Sea_"+NationShortName(sti(MerPrm.nation))+"_QuestMerchant");
    //}
    
    int iChar = NPC_GenerateCharacter(NationShortName(sti(MerPrm.nation))+"_QuestMerchant", "off_spa_2", "man", "man", 5, sti(MerPrm.nation), 6, true));
    makeref(chref, Characters[iChar]);
    SetMerchantShip(chref, sti(MerPrm.QuestGoodsIdx));
	SetFantomParamHunter(chref); //крутые парни
	string sGroup = "Sea_"+chref.id
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    Group_SetType(sGroup,"trade");
    //Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    
    ref rGroup = Group_GetGroupByID(sGroup);
    rGroup.Task = AITASK_MOVE;
    Group_LockTask(sGroup);
    
    Group_AddCharacter(sGroup, chref.id);
    Group_SetGroupCommander(sGroup, chref.id);

    chref.mapEnc.type = "trade";
    chref.mapEnc.Name = "торговец";
    //chref.mapEnc.Task = AITASK_MOVE; // ?


    Map_CreateTrader(MerPrm.sColony, MerPrm.sNextColony, chref.id, 5);
    MerPrm.CapIndex = iChar;
    
    string sQuest = NationShortName(sti(MerPrm.nation))+"_QuestMerchant_1";
	pchar.quest.(sQuest).win_condition.l1 = "Character_sink";
    pchar.quest.(sQuest).win_condition.l1.character = chref.id;
    pchar.quest.(sQuest).function= "KillMerchant";
    
    sQuest = NationShortName(sti(MerPrm.nation))+"_QuestMerchant_2";

	pchar.quest.(sQuest).win_condition.l1 = "Character_Capture";
    pchar.quest.(sQuest).win_condition.l1.character = chref.id;
    pchar.quest.(sQuest).function= "CaptureMerchant";
    
    
    
    
    
    int gcount = rand(3)-1;
    if (gcount < 0 || makeint(pchar.rank) < 5) gcount = 0;
    
    for (int i = 0; i<gcount; i++)
    {
        iChar = NPC_GenerateCharacter(NationShortName(sti(MerPrm.nation))+"_QuestMerchantGuard_"+i, "off_spa_2", "man", "man", 5, sti(MerPrm.nation), 20, true));
        makeref(sld, Characters[iChar]);
        SetShipHunter(sld);
        SetFantomParamHunter(sld); //крутые парни
        SetCaptanModelByEncType(sld, "war");
        sld.Ship.Mode = "war";
        Group_AddCharacter(sGroup, sld.id);

    }
    MerPrm.gcount = gcount;
    
    
    return chref.Ship.Name;
}

bool MerchantPrepare(int Mnation)
{

    ref  MerPrm;
    int  i;

    makeref(MerPrm, MerchantParam);

    i = 0;
    MerPrm.nation = PIRATE;
    while (GetNationRelation(sti(MerPrm.nation), Mnation) == RELATION_ENEMY && i < 15)
    {
        MerPrm.nation = rand(PIRATE);
        i++;
    }
    if (i == 15)
    {
        MerPrm.nation = PIRATE;
    }
    
     // goods
    switch (rand(12))
    {
        case 0 :
            MerPrm.QuestGoodsIdx = GOOD_SILVER;
        break;
        case 1 :
            MerPrm.QuestGoodsIdx = GOOD_GOLD;
        break;
        case 2 :
            MerPrm.QuestGoodsIdx = GOOD_COFFEE;
        break;
        case 3 :
            MerPrm.QuestGoodsIdx = GOOD_MAHOGANY;
        break;
        case 4 :
            MerPrm.QuestGoodsIdx = GOOD_SLAVES;
        break;
        case 5 :
            MerPrm.QuestGoodsIdx = GOOD_FOOD;
        break;
        case 6 :
            MerPrm.QuestGoodsIdx = GOOD_WEAPON;
        break;
        case 7 :
            MerPrm.QuestGoodsIdx = GOOD_TOBACCO;
        break;
        case 8 :
            MerPrm.QuestGoodsIdx = GOOD_ALE;
        break;
        case 9 :
            MerPrm.QuestGoodsIdx = GOOD_RUM;
        break;
        case 10 :
            MerPrm.QuestGoodsIdx = GOOD_SUGAR;
        break;
        case 11 :
            MerPrm.QuestGoodsIdx = GOOD_SANDAL;
        break;
        case 12 :
            MerPrm.QuestGoodsIdx = GOOD_PAPRIKA;
        break;

    }
    string HomeColony = All_GetColony();
    int iFColony = FindNonEnemyColonyForAdventure(sti(MerPrm.nation), HomeColony, true));

    if (HomeColony!="none" && iFColony != -1 )
    {
    
        if (rand(1)==1)
        {
            MerPrm.sColony = "none";
            MerPrm.sNextColony = HomeColony;
            Log_TestInfo(MerPrm.sColony+"-->"+MerPrm.sNextColony);
        }
        else
        {
            string FriendColony = Colonies[iFColony].id;
            MerPrm.sColony = HomeColony;
            MerPrm.sNextColony = FriendColony;
            Log_TestInfo(MerPrm.sColony+"-->"+MerPrm.sNextColony);
        }
        return true;
    }
    return false;





}

int SetShipTypeMerchant(ref Cap)
{
    int iShip, hcrew, irank;

    if(makeint(pchar.rank) > 10)
    {
        irank = rand(2) + 3;
    }

    if(makeint(pchar.rank) > 6 && makeint(pchar.rank) < 11)
    {
        irank = rand(2);
    }

    switch (sti(Cap.nation))
    {
        case ENGLAND:
	        switch (irank)
	        {
	            case 0: iShip = SHIP_BRIG;     break;
	            case 1: iShip = SHIP_BRIG;     break;
	            case 2: iShip = SHIP_FLEUT;    break;
	            case 3: iShip = SHIP_GALEON_L; break;
	            case 4: iShip = SHIP_PINNACE;  break;
	            case 5: iShip = SHIP_LINESHIP; break;

	        }
        break;

        case FRANCE:
	        switch (irank)
	        {
	            case 0: iShip = SHIP_FLEUT;     break;
	            case 1: iShip = SHIP_GALEON_L;  break;
	            case 2: iShip = SHIP_GALEON_L;  break;
	            case 3: iShip = SHIP_PINNACE;   break;
	            case 4: iShip = SHIP_PINNACE;   break;
	            case 5: iShip = SHIP_LINESHIP;  break;

	        }
        break;

        case SPAIN:
	        switch (irank)
	        {
	            case 0: iShip = SHIP_CARAVEL;     break;
	            case 1: iShip = SHIP_BRIG;        break;
	            case 2: iShip = SHIP_PINNACE;     break;
	            case 3: iShip = SHIP_GALEON_H;    break;
	            case 4: iShip = SHIP_LINESHIP;    break;
	            case 5: iShip = SHIP_WARSHIP;     break;
	        }
        break;

        case HOLLAND:
	        switch (irank)
	        {
	            case 0: iShip = SHIP_BRIG     break;
	            case 1: iShip = SHIP_FLEUT    break;
	            case 2: iShip = SHIP_FLEUT    break;
	            case 3: iShip = SHIP_GALEON_L break;
	            case 4: iShip = SHIP_PINNACE  break;
	            case 5: iShip = SHIP_PINNACE  break;

	        }
        break;

        case PIRATE:
	        switch (irank)
	        {
	            case 0: iShip = SHIP_BRIG     break;
	            case 1: iShip = SHIP_BRIG     break;
	            case 2: iShip = SHIP_FLEUT    break;
	            case 3: iShip = SHIP_GALEON_L break;
	            case 4: iShip = SHIP_PINNACE  break;
	            case 5: iShip = SHIP_CORVETTE break;
	        }
        break;
    }

    if(makeint(pchar.rank) < 7)
    {
        iShip = SHIP_LUGGER + rand(makeint(SHIP_BARQUE - SHIP_LUGGER));
        irank = 0;
    }

    SetRandomNameToShip(Cap);
    Cap.Ship.Type = GenerateShip(iShip, 1);
    SetBaseShipData(Cap);

    hcrew = GetMaxCrewQuantity(Cap);
    SetCrewQuantity(Cap, hcrew);
    
    DeleteAttribute(Cap,"ship.sails");
    DeleteAttribute(Cap,"ship.masts");
    DeleteAttribute(Cap,"ship.blots");

    if (sti(Cap.nation == PIRATE)) //navy fix -- приведение типов.
    {
        Fantom_SetCannons(Cap, "pirate");
        Fantom_SetBalls(Cap, "pirate");
    }
    else
    {
        Fantom_SetCannons(Cap, "trade");
        Fantom_SetBalls(Cap, "trade");
    }
    
    return irank;
}

void SetMerchantShip(ref Cap, int igoods)
{
    int hcrew, irank;
    int killMax;

    irank = SetShipTypeMerchant(Cap);
    SetRandomNameToCharacter(Cap);
    SetCaptanModelByEncType(Cap, "trade");//
    Cap.Ship.Mode = "Trade";
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ref MerPrm;
    makeref(MerPrm, MerchantParam);
    int killM = Statistic_AddValue(GetMainCharacter(), "Rob"+NationShortName(sti(MerPrm.nation))+"MerchantOnMap", 0)
    if (MOD_SKILL_ENEMY_RATE > 5) // 20/01/07 homo
    {
        killMax = makeint(3+(10 - MOD_SKILL_ENEMY_RATE)/2);
    }
    else
    {
        killMax = 6;
    }
    if  (killM >= killMax && rand(1)== 0 )
    {
        //Cap.AlwaysEnemy = true;
        //на тест
        SetCaptanModelByEncType(Cap, "war");
        //Cap.Ship.Mode = "war";
        Cap.greeting = "soldier_arest";
        Cap.DontRansackCaptain = true; //квестовые не сдаютс€
        Cap.dialog.filename = "Capitans_dialog.c";
		Cap.dialog.currentnode = "MerchantTrap_Abordage";
		Cap.DeckDialogNode = "First time";
        Statistic_AddValue(GetMainCharacter(), "Rob"+NationShortName(sti(MerPrm.nation))+"MerchantOnMap", -( killM + 1));

        hcrew = GetMaxCrewQuantity(Cap);
        ChangeCrewExp(pchar, "Soldiers", 90);  // за счет солдат - засада дл€ √√
        SetCrewQuantityOverMax(Cap, 2*hcrew);
        trap = true;
    }
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (!trap)
    {
        int iSpace = GetCharacterFreeSpace(Cap, igoods);
    
        Log_TestInfo("irank = "+irank);
        float del = makefloat(7-irank+rand(1))/5;
        if (del > 1.0 ) del = 1;
        Log_TestInfo("goods load = "+iSpace+"/"+makeint(iSpace*del));
        iSpace = makeint(iSpace*del); // чтоб не так жирно было
        Fantom_SetCharacterGoods(Cap, igoods, iSpace);
    }
}

string All_GetColony();
{


    int locnum = FindLocation(pchar.location);
    if (locnum != -1 )
    {
        return locations[locnum].townsack;
    }
    if (!IsEntity(worldMap))
    {
        return Sea_FindNearColony();
    }
    return "none";
}

void KillMerchant(string temp)
{
    AddTemplRumour("MerchantRumour", id_counter+1);
    DieMerchant(5);
}


void CaptureMerchant(string temp)
{
    int qind;
    ref MerPrm;
    makeref(MerPrm, MerchantParam);
    if (trap)
    {
        qind = 6;
        Log_TestInfo("TRADER_TRAP!!!");
        string stext1 = "ѕрошел слух, что лихой пират - "+GetFullName(pchar)+", угодил в западню, в которую его заманили "+NationNameSK(sti(MerPrm.ination))+"ие власти. ќни подослали к нему ''торговое'' судно, в трюме которого, спр€тались вооруженные до зубов "+NationNameSK(sti(MerPrm.ination))+"ие  солдаты. Ќо "+pchar.name+" не растер€лс€, он их попросту перебил - всех до единого! ¬от какой удалой пират наш "+GetFullName(pchar)+".";
        string stext2 = "Ћюди говор€т, что дерзкий пират - "+GetFullName(pchar)+", повадилс€ грабить "+NationNameSK(sti(MerPrm.ination))+"ие торговые корабли. » тогда наш √енерал-губернатор решил покончить с ним. ќн снар€ди  корабль, замаскированный под торговца с несколькими дес€тками хорошо обученных солдат, из ≈вропы, и подослал к нему. Ќо кровожадный пират перебил их всех и вновь улизнул от правосуди€!";
        AddSimpleRumour(stext1, sti(MerPrm.ination)+10, 30, 3);
        AddSimpleRumour(stext2, sti(MerPrm.ination), 30, 3);

    }
    else
    {
       qind = 4;
       AddTemplRumour("MerchantRumour", id_counter+1);
    }
    DieMerchant(qind);



}


void DieMerchant(int qind)
{
    ref MerPrm;
    makeref(MerPrm, MerchantParam);

    if (sti(MerPrm.ination)!= PIRATE )
    {
        Statistic_AddValue(GetMainCharacter(), "Rob"+NationShortName(sti(MerPrm.ination))+"MerchantOnMap", 1);
    }

    AddQuestRecord("MerchantOnMap", qind);
    AddQuestUserData("MerchantOnMap", "sShipName", MerPrm.ShipName);
    AddQuestUserData("MerchantOnMap", "sQuestGoodsIdx", MerPrm.QuestGoodsIdx);
    AddQuestUserData("MerchantOnMap", "snation", MerPrm.nation);

    CloseQuestHeader("MerchantOnMap");
    Map_ReleaseQuestEncounter(NationShortName(sti(MerPrm.ination))+"_QuestMerchant");
    //fix homo
    trap = false; //fix
    MerPrm.active = false;

}

void ClearMerchant(string stemp)
{
    ref MerPrm;
    makeref(MerPrm, MerchantParam);
    CloseQuestHeader("MerchantOnMap");
    Map_ReleaseQuestEncounter(NationShortName(sti(MerPrm.ination))+"_QuestMerchant");
    Group_DeleteGroup("Sea_"+NationShortName(sti(MerPrm.ination))+"_QuestMerchant");
    MerPrm.active = false;
}

string GenerateMerchant()
{
        ref MerPrm, sld;
        ref CurTpl
        string reztext, tresult, sdirect;
        int lngFileID;
        string HomeColony = All_GetColony();
        int iNation = sti(Colonies[FindColony(HomeColony)].nation)
        if ( MerchantPrepare(iNation) )
        {

            makeref(MerPrm, MerchantParam);
            MerPrm.ShipName = CreateMerchant(iNation);


            if (MerPrm.sColony == "none")
            {
                MerPrm.sColony = Colonies[FindNonEnemyColonyForAdventure(iNation, HomeColony, true)].id;
                sdirect = "To";
            }
            Characters[sti(MerPrm.CapIndex)].FromColony = MerPrm.sColony;


            int tNum = TplNameToNum("MerchantOnMap");
            makeref(CurTpl,  templat[tNum]);
            lngFileID = LanguageOpenFile("RumourTexts.txt");
            string tid = CurTpl.name+sdirect+"_t"+rand(sti(CurTpl.texts)-1);
            reztext = LanguageConvertString(lngFileID, tid);
            MerPrm.ination = MerPrm.nation; // fix
            MerPrm.nation = NationNameGenitive(sti(MerPrm.nation));
            string GoodName = "seg_"+goods[sti(MerPrm.QuestGoodsIdx)].name;
            MerPrm.QuestGoodsIdx = GetConvertStr(GoodName, "ShipEatGood.txt");
            MerPrm.sColony = GetConvertStr(MerPrm.sColony+" Town", "LocLables.txt");
            MerPrm.sNextColony = GetConvertStr(MerPrm.sNextColony+" Port", "LocLables.txt");

            tresult += GetAssembledString( reztext,	MerPrm);


            ReOpenQuestHeader("MerchantOnMap");
            AddQuestRecord("MerchantOnMap", 1+rand(2));
    		AddQuestUserData("MerchantOnMap", "snation", MerPrm.nation);
    		AddQuestUserData("MerchantOnMap", "sQuestGoodsIdx", MerPrm.QuestGoodsIdx);
    		AddQuestUserData("MerchantOnMap", "sColony", MerPrm.sColony);
    		AddQuestUserData("MerchantOnMap", "sNextColony", MerPrm.sNextColony);
    		AddQuestUserData("MerchantOnMap", "sShipName", MerPrm.ShipName);
    		
            string sQuest = "ClearMerchant";
            SetTimerCondition(sQuest, 0, 0, 6, false);
            pchar.quest.(sQuest).win_condition = "ClearMerchant";
            pchar.quest.(sQuest).function= "ClearMerchant";
            MerPrm.active = true;
            //AddQuestRecord();
            //CloseQuestHeader();
            return tresult;
        }
        else
        {
            return NO_RUMOUR_TEXT[rand(4)];
        }
}

string AfteMerchantRumour()
{
    ref MerPrm,CurTpl;
    string reztext, tresult;
    makeref(MerPrm, MerchantParam);
    int tNum = TplNameToNum("MerchantRumour");

    makeref(CurTpl,  templat[tNum]);
    int lngFileID = LanguageOpenFile("RumourTexts.txt");
    string tid = CurTpl.name+"_t"+rand(sti(CurTpl.texts)-1);
    reztext = LanguageConvertString(lngFileID, tid);
    tresult += GetAssembledString( reztext,	MerPrm);
    if (tresult!="") return tresult;
    else
        {
            return NO_RUMOUR_TEXT[rand(4)];
        }

}