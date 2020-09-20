//  Форма смены отношений наций
string totalInfo;
void InitInterface(string iniName)
{
    // лочим квест и карту
    bQuestCheckProcessFreeze = true;
    if(IsEntity(worldMap))
    {
    	wdmLockReload            = true;
    }
    StartAboveForm(true);
    
    GameInterface.title = "titleNationLegend";

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	CalculateNationRelat();

	SetFormatedText("INFO_TEXT",totalInfo);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_SALARY_EXIT );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);
    
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
    PostEvent("StopQuestCheckProcessFreeze", 500);//boal 230804 заморозка проверки квестов
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
    	case "B_OK":
    		if(comName=="activate" || comName=="click")
    		{
                IDoExit(RC_INTERFACE_SALARY_EXIT);
    		}
    	break;
	}
}

void CalculateNationRelat()
{
    ref mainCh = GetMainCharacter();
    int Nation1, Nation2, RelatNat;

    // boal 04.04.04 навел марафет в коде - красота :)
    bool ok1, ok2;
    
    Nation1 = 1;
    Nation2 = 1;
    while (Nation1 == Nation2)
    {
	    switch (Rand(3))
	    {
	        case 0:
		        Nation1 = ENGLAND;
	        break;

	        case 1:
		        Nation1 = FRANCE;
	        break;

	        case 2:
		        Nation1 = HOLLAND;
	        break;

	        case 3:
		        Nation1 = SPAIN;
	        break;
	    }

	    switch (Rand(3))
	    {
	        case 0:
		       Nation2 = SPAIN;
	        break;
	        case 1:
		       Nation2 = HOLLAND;
	        break;
	        case 2:
		        Nation2 = FRANCE;
	        break;
	        case 3:
		        Nation2 = ENGLAND;
	        break;
	    }
    }


    if (GetNationRelation(Nation1, Nation2) == RELATION_ENEMY || GetNationRelation(Nation1, Nation2) == RELATION_FRIEND)
    {   // меняем крайние на середину
        RelatNat = RELATION_NEUTRAL;
    }
    else
    {   // нейтралы - случайно
        switch(Rand(1))
        {
            case 0:
    	       RelatNat = RELATION_FRIEND;
            break;
            case 1:
    	       RelatNat = RELATION_ENEMY;
            break;
        }
    }
//Фикс для эскадр, чтоб на время осады не поменялась на дружественные или нейтральные отношения.////////////////////////
 /*
    ref FortCh, CaptEsc;
    CaptEsc = GetCharacter(GetCharacterIndex("NatCapitan_1"));

    FortCh = FindSiegeFortCommander();
    if(FortCh.id != "none")
    {
        ok1 = (sti(FortCh.nation) == Nation1) && (sti(CaptEsc.nation) == Nation2);
        ok2 = (sti(FortCh.nation) == Nation2) && (sti(CaptEsc.nation) == Nation1);
        if (ok1 || ok2)
        {
            RelatNat = RELATION_ENEMY;
        }
    }     */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SetNationRelationBoth(Nation1, Nation2, RelatNat);

    if (sti(mainCh.nation) == Nation1)
    {
        SetNationRelation2MainCharacter(Nation2, RelatNat);
    }

    if (sti(mainCh.nation) == Nation2)
    {
        SetNationRelation2MainCharacter(Nation1, RelatNat);
    }
    totalInfo = XI_ConvertString(GetNationNameByType(Nation1)) + " и " + XI_ConvertString(GetNationNameByType(Nation2));
    if(RelatNat == RELATION_FRIEND)
    {
        totalInfo = totalInfo + " стали союзниками.";
    }
    if(RelatNat == RELATION_ENEMY)
    {
        totalInfo = totalInfo + " перешли в состояние войны.";
    }
    if(RelatNat == RELATION_NEUTRAL)
    {
        totalInfo = totalInfo + " перешли к нейтральным отношениям.";
    }
    Log_SetStringToLog(totalInfo); // boal fix tavern
    SetNewGroupPicture("Nat1", "NATIONS",  GetNationNameByType(Nation1));
    SetNewGroupPicture("Nat2", "NATIONS", GetNationNameByType(Nation2));
    SetNewGroupPicture("RelNat", "relations", GetRelationName(GetNationRelation(Nation1, Nation2)));
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}