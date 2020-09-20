/// BOAL меню наций
int curNationIdx;

void InitInterface(string iniName)
{
    InterfaceStack.SelectMenu_node = "LaunchNationRelation"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleNationRelation";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	// boal 18.12.2003 -->
	// делаем заточку под 5 или 6 наций и любую морду (64) гл. героя - универсальность залог успеха!
	// введем коэфф. для автопомтроения матрицы
	int     i;
	int     dy= 10;
    int     dx=16;
    int     sx=45;
    int     sy=100;
	int     sz = 56;
	string  picName  = "pic";
	string  groupName;
	int     picCount = 1;
	int     deltaX = 170;
	int     deltaY = 220;
	int     max_nt = MAX_NATIONS;
	
	ref chref = GetMainCharacter();
    int j;
    
	if (bBettaTestMode)
	{   // со столбцом ГГ
		SetNewPicture("ICON_1", "interfaces\PORTRAITS\64\face_" + chref.faceId+ ".tga");
		SetNewPicture("ICON_2", "interfaces\PORTRAITS\64\face_" + chref.faceId+ ".tga");
		for (i=0; i< max_nt; i++)
		{
	        groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name, deltaX +dx+(dx+sz)*i, sy, deltaX + dx+ (dx+sz)*i+sz, sy+sz);
		    picCount++;
		}
		for (i=0; i< max_nt; i++)
		{
		    groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name,  sx, deltaY-10 +dy+(dy+sz)*i, sx+sz, deltaY-10 +dy+(dy+sz)*i+sz);
		    picCount++;
		}
		// Y
		for (i=0; i< max_nt; i++)
		{
	        CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation2MainCharacter(i)), deltaX -sz, deltaY +(dy+sz)*i, deltaX , deltaY +(dy+sz)*i+sz);
		    picCount++;
		}
		//X
		for (i=0; i< max_nt; i++)
		{
	        CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation2MainCharacter(i)), deltaX +dx+(dx+sz)*i, deltaY-sz-4, deltaX + dx+ (dx+sz)*i+sz, deltaY-4);
		    picCount++;
		}
		//GetNationRelation
		for (j=0; j< max_nt; j++)
		{
		    for (i=0; i< max_nt; i++)
		    {
		        if (i != j)
		        {
		            CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation(j, i)), deltaX +dx+(dx+sz)*i, deltaY +(dy+sz)*(j), deltaX + dx+ (dx+sz)*i+sz, deltaY+(dy+sz)*(j)+sz);
		            picCount++;
		        }
		    }
		}
	}
	else
	{// без столбца ГГ
     	SetNewPicture("ICON_1", "");
		SetNewPicture("ICON_2", "");
		sx=75;
    	sy=120;
		sz = 60;
		
		deltaX = 140;
		deltaY = 200;
		for (i=0; i< max_nt; i++)
		{
	        groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name, deltaX +dx+(dx+sz)*i, sy, deltaX + dx+ (dx+sz)*i+sz, sy+sz);
		    picCount++;
		}
		for (i=0; i< max_nt; i++)
		{
		    groupName = "NATIONS";
	        CreateImage(picName+picCount, groupName, Nations[i].Name,  sx, deltaY-10 +dy+(dy+sz)*i, sx+sz, deltaY-10 +dy+(dy+sz)*i+sz);
		    picCount++;
		}
  		for (j=0; j< max_nt; j++)
		{
		    for (i=0; i< max_nt; i++)
		    {
		        if (i != j)
		        {
		            CreateImage(picName+picCount, "relations", GetRelationName(GetNationRelation(j, i)), deltaX +dx+(dx+sz)*i, deltaY +(dy+sz)*(j), deltaX + dx+ (dx+sz)*i+sz, deltaY+(dy+sz)*(j)+sz);
		            picCount++;
		        }
		    }
		}
	}
    // boal 18.12.2003 <--
	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("FlagsPress","FlagsProcess",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("MouseRClickUP","HideInfo",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
    
    /////////////
    CreateString(true,"titul", XI_ConvertString("Hunter_type"),"DIALOG2",COLOR_NORMAL,609,100,SCRIPT_ALIGN_LEFT,1.0);
    
    CreateString(true,"EngH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,700,134,SCRIPT_ALIGN_CENTER,1.0);
    CreateString(true,"FraH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,700,190,SCRIPT_ALIGN_CENTER,1.0);
    CreateString(true,"SpaH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,700,246,SCRIPT_ALIGN_CENTER,1.0);
    CreateString(true,"HolH","","INTERFACE_ULTRASMALL",COLOR_NORMAL,700,302,SCRIPT_ALIGN_CENTER,1.0);

    CalculateHunter();
    
    CreateString(true,"Flag1","Флаг на корабле","DIALOG2",COLOR_NORMAL,630,432,SCRIPT_ALIGN_LEFT,1.0);
    curNationIdx = sti(chref.nation);
    SetNewNation(0);
    XI_RegistryExitKey("IExit_F2");
}

void CalculateHunter()
{

     ref mc = GetMainCharacter();

     CreateImage("Eng", "NATIONS", "ENGLAND",  610, 136, 640,168 );
     CreateImage("Fra", "NATIONS", "FRANCE",  610, 192, 640, 224);
     CreateImage("Spa", "NATIONS", "SPAIN",  610, 248, 640, 282);
     CreateImage("Hol", "NATIONS", "HOLLAND",  610, 304, 640, 336);

     GameInterface.strings.EngH = XI_ConvertString("England");
     GameInterface.strings.FraH = XI_ConvertString("France");
     GameInterface.strings.SpaH = XI_ConvertString("Spain");
     GameInterface.strings.HolH = XI_ConvertString("Holland");

     SetFormatedText("Eng_TEXT", GetNationReputation(pchar, ENGLAND));
     SetFormatedText("Fra_TEXT", GetNationReputation(pchar, FRANCE));
     SetFormatedText("Spa_TEXT", GetNationReputation(pchar, SPAIN));
     SetFormatedText("HOL_Text", GetNationReputation(pchar, HOLLAND));
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("FlagsPress","FlagsProcess");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("MouseRClickUP","HideInfo");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
        DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}

void ShowInfoWindow()
{
	string sHeader = "TEST";
	string sNode = GetCurrentNode();

	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	sPicture = "none";
	sGroup = "none";
	sGroupPicture = "none";

	switch(sNode)
	{
		case "FlagPic":
			sHeader = XI_ConvertString("Nation");
			sText1 = GetRPGText("Nation_hint");
		break;
		
		case "INFO_CLICK":
			sHeader = XI_ConvertString("Hunter_type");
			sText1 = GetRPGText("Hunter_hint");
		break;
	}
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
}

void HideInfo()
{
	CloseTooltip();
	SetCurrentNode("OK_BUTTON");
}

void FlagsProcess()
{
	// boal 04.04.2004 -->
	bool bTmpBool = true;
	int i, cn;
	ref chref;
	
	if (CheckAttribute(pchar, "DisableChangeFlagMode")) return; // нефиг менять файл за 3 секунды сразу
	
	if (!bBettaTestMode)
	{
    	//if(LAi_group_IsActivePlayerAlarm()) bTmpBool = false;
    	//if(!LAi_IsCharacterControl(GetMainCharacter())) bTmpBool = false;
    	if (bLandInterfaceStart)  bTmpBool = false; // на суше нельзя в принципе
    	if (bSeaActive && !CheckEnemyCompanionDistance2GoAway(false)) bTmpBool = false; // компаньон под ударом
        // проверка класса кораблей в эскадре -->
        /*for (i=0; i<4; i++)
    	{
    		cn = GetCompanionIndex(GetMainCharacter(), i);
    		if ( cn>=0 )
    		{
    			chref = GetCharacter(cn);
                if (GetCharacterShipClass(chref) <= 2) bTmpBool = false;
    		}
    	}  */
        // проверка класса кораблей в эскадре <--
	}
	if (bDisableMapEnter || !bTmpBool)
	{
        if (!bBettaTestMode)
        {
            PlaySound("interface\knock.wav");
            return;
        }
	}
	// смена флага
	Sea_ClearCheckFlag(); // сбросить всем в море проверку смотрения на флаг.
	pchar.DisableChangeFlagMode = true; //закрываем Флаг
	DoQuestFunctionDelay("FreeChangeFlagMode", 15.0); // ролик + запас
	
	switch (curNationIdx)
	{
    	case ENGLAND:	EnglandProcess();	break;
    	case FRANCE:	FranceProcess();	break;
    	case SPAIN:		SpainProcess();	break;
    	case PIRATE:	PirateProcess();	break;
    	case HOLLAND:	HollandProcess();	break;
	}
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
    switch(nodName)
	{
        case "LEFTCHANGE_NATION":
    		if(comName=="activate" || comName=="click")
    		{
				if (!CheckAttribute(pchar, "DisableChangeFlagMode"))
				{
					SetNewNation(-1);
				}
    		}
    	break;

    	case "RIGHTCHANGE_NATION":
    		if(comName=="activate" || comName=="click")
    		{
                if (!CheckAttribute(pchar, "DisableChangeFlagMode"))
                {
					SetNewNation(1);
				}
    		}
    	break;
		case "I_CHARACTER_2":
			if(comName=="click")
			{
			    nodName = "I_CHARACTER";
			}
		break;
		case "I_SHIP_2":
			if(comName=="click")
			{
			    nodName = "I_SHIP";
			}
		break;
		case "I_QUESTBOOK_2":
			if(comName=="click")
			{
			    nodName = "I_QUESTBOOK";
			}
		break;
		case "I_TRADEBOOK_2":
			if(comName=="click")
			{
			    nodName = "I_TRADEBOOK";
			}
		break;
		case "I_NATIONS_2":
			if(comName=="click")
			{
			    nodName = "I_NATIONS";
			}
		break;
		case "I_ITEMS_2":
			if(comName=="click")
			{
			    nodName = "I_ITEMS";
			}
		break;
	}
	// boal new menu 31.12.04 -->
	if (nodName == "I_CHARACTER" || nodName == "I_SHIP" ||
	    nodName == "I_QUESTBOOK" || nodName == "I_TRADEBOOK" ||
		nodName == "I_NATIONS" || nodName == "I_ITEMS")
	{
		if(comName=="click")
		{
            InterfaceStates.ReloadMenuExit = true;
			IDoExit(RC_INTERFACE_ANY_EXIT);
			PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			return;
		}
	}
	// boal new menu 31.12.04 -->
}
void SetNewNation(int add)
{
    ref   mchar = GetMainCharacter();
    bool  ok, ok2;
    
    curNationIdx = curNationIdx + add;
    if (curNationIdx < 0) curNationIdx = 4;
    if (curNationIdx > 4) curNationIdx = 0;
    //CreateImage("FlagPic", "NATIONS",GetNationNameByType(curNationIdx), 648, 455, 715, 522);
    SetNewGroupPicture("FlagPic", "NATIONS", GetNationNameByType(curNationIdx));
    
    if (IsCharacterPerkOn(mchar,"FlagPir")  ||
	    IsCharacterPerkOn(mchar,"FlagEng")  ||
		IsCharacterPerkOn(mchar,"FlagFra")  ||
		IsCharacterPerkOn(mchar,"FlagSpa")  ||
		IsCharacterPerkOn(mchar,"FlagHol"))
    {
        SetSelectable("FLAGS",true);
        ok  = !IsCharacterPerkOn(mchar,"Flag" + NationShortName(curNationIdx)) && (sti(mchar.nation) != curNationIdx);
        ok2 =  true;
        if (isMainCharacterPatented())
        {
            ok2 = (sti(Items[sti(mchar.EquipedPatentId)].Nation) != curNationIdx);
        }
        if (ok && ok2)
        {
            SetNewNation(add);
        }
        if (sti(mchar.nation) == curNationIdx)
        {
            SetSelectable("FLAGS",false);
        }
    }
    else
    {
        SetSelectable("FLAGS",false);
	    SetNodeUsing("RIGHTCHANGE_NATION",false);
	    SetNodeUsing("LEFTCHANGE_NATION",false);
    }
}
void PirateProcess()
{
    //DoQuestCheckDelay("pir_flag_rise", 1.0);
    PChar.GenQuest.VideoAVI        = "Pirate";
    PChar.GenQuest.VideoAfterQuest = "pir_flag_rise";

    DoQuestCheckDelay("PostVideo_Start", 0.2);
	ProcessExitCancel();
}

void EnglandProcess()
{
    //DoQuestCheckDelay("eng_flag_rise", 1.0);
    PChar.GenQuest.VideoAVI        = "England";
    PChar.GenQuest.VideoAfterQuest = "eng_flag_rise";

    DoQuestCheckDelay("PostVideo_Start", 0.2);
    ProcessExitCancel();
}

void FranceProcess()
{
    PChar.GenQuest.VideoAVI        = "France";
    PChar.GenQuest.VideoAfterQuest = "fra_flag_rise";

    DoQuestCheckDelay("PostVideo_Start", 0.2);
	ProcessExitCancel();
}

void SpainProcess()
{
    PChar.GenQuest.VideoAVI        = "Spain";
    PChar.GenQuest.VideoAfterQuest = "spa_flag_rise";

    DoQuestCheckDelay("PostVideo_Start", 0.2);
    ProcessExitCancel();
}

void HollandProcess()
{
    PChar.GenQuest.VideoAVI        = "Holland";
    PChar.GenQuest.VideoAfterQuest = "hol_flag_rise";

    DoQuestCheckDelay("PostVideo_Start", 0.2);
	ProcessExitCancel();
}