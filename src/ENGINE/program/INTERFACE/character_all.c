/// BOAL меню персонажи
#include "interface\character_all.h"

int nCurScrollOfficerNum;

void InitInterface(string iniName)
{
    xi_refCharacter = pchar;
	InterfaceStack.SelectMenu_node = "LaunchMainCharacter"; // запоминаем, что звать по Ф2
	GameInterface.title = "titleCharacter";
	
	FillCharactersScroll();
	FillPassengerScroll();
	GameInterface.StatusLine.LOYALITY.Max = 1;
    GameInterface.StatusLine.LOYALITY.Min = 0;
    GameInterface.StatusLine.LOYALITY.Value = 0;
        
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("frame","ProcessFrame",1);
    SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("TableSelectChange", "CS_TableSelectChange", 0);
	SetEventHandler("eTabControlPress","procTabChange",0);
	SetEventHandler("ExitOfficerMenu","ExitOfficerMenu",0);
	SetEventHandler("OfficerChange","OfficerChange",0);
	SetEventHandler("acceptaddofficer","AcceptAddOfficer",0);
	SetEventHandler("ExitRemoveOfficerMenu","ExitRemoveOfficerMenu",0);
	SetEventHandler("AcceptRemoveOfficer","AcceptRemoveOfficer",0);
	SetEventHandler("ChoosePerk","ChoosePerk",0);
	SetEventHandler("ExitPerkMenu","ExitPerkMenu",0);
	SetEventHandler("AcceptPerk","AcceptPerk",0);

    XI_RegistryExitKey("IExit_F2");
    
    EI_CreateFrame("CHARACTER_BIG_PICTURE_BORDER",369,78,579,315); // take from CHARACTER_BIG_PICTURE
    EI_CreateHLine("CHARACTER_BIG_PICTURE_BORDER", 373,104,575,1, 4);
    
    SetVariable();
    SetCurrentNode("CHARACTERS_SCROLL");
    SetNodeUsing("CHARACTERS_SCROLL",true);
    SetFormatedText("LOYALITY_STR", XI_ConvertString("Loyality"));
    //SetControlsTabMode(1);
    HideSkillChanger();
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("frame","ProcessFrame");
    DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("TableSelectChange", "CS_TableSelectChange");
	DelEventHandler("eTabControlPress","procTabChange");
	DelEventHandler("ExitOfficerMenu","ExitOfficerMenu");
	DelEventHandler("OfficerChange","OfficerChange");
	DelEventHandler("acceptaddofficer","AcceptAddOfficer");
	DelEventHandler("ExitRemoveOfficerMenu","ExitRemoveOfficerMenu");
	DelEventHandler("AcceptRemoveOfficer","AcceptRemoveOfficer");
	DelEventHandler("ChoosePerk","ChoosePerk");
	DelEventHandler("ExitPerkMenu","ExitPerkMenu");
	DelEventHandler("AcceptPerk","AcceptPerk");

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
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
    switch(nodName)
	{
        case "B_SKILLUP":
			if (comName=="click")
			{
                IncreaseSkill(1);
			}
			if (comName=="rclick")
			{
                IncreaseSkill(5);
			}
		break;

		case "B_SKILLDOWN":
	        if (comName=="click")
			{
			    DecreaseSkill(1);
			}
			if (comName=="rclick")
			{
			    DecreaseSkill(5);
			}
		break;
		/////////////////////// menu ///////////////
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

void SetVariable()
{
	FillSkillTables();
	SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\256\face_" + xi_refCharacter.FaceId + ".tga");
	SetFormatedText("HERO_NAME", GetFullName(xi_refCharacter));
	
	if (xi_refCharacter.id == pchar.id)
	{
		SetNewGroupPicture("CHARACTER_NATION_PICTURE", "NATION_ICONS", GetNationNameByType(GetBaseHeroNation()));
		SetNodeUsing("CHARACTER_NATION_PICTURE", true);
	}
	else
	{
	    SetNodeUsing("CHARACTER_NATION_PICTURE", false);
	}
	
	if (bBettaTestMode)
    {
        CreateString(true,"healthCur1",
                     "pchar MaxHP = "    + FloatToString(stf(pchar.Health.maxHP), 1) +
                     " CurHP = "   + FloatToString(stf(pchar.Health.HP), 1) +
                     " WeekDmg = " + FloatToString(stf(pchar.Health.weekDamg), 1) +
                     " DayDmg = "  + FloatToString(stf(pchar.Health.Damg), 1),
                     "INTERFACE_ULTRASMALL",COLOR_NORMAL,10,590, SCRIPT_ALIGN_LEFT,1.0);
    }
    
    // теперь это Лояльность
    GameInterface.StatusLine.LOYALITY.Max   = MAX_LOYALITY;
    GameInterface.StatusLine.LOYALITY.Min   = 0;
    GameInterface.StatusLine.LOYALITY.Value = GetCharacterLoyality(xi_refCharacter);
    if (xi_refCharacter.id == pchar.id)
    {
	    GameInterface.StatusLine.BAR_HEALTH.Max   = 54;
	    GameInterface.StatusLine.BAR_HEALTH.Min   = 1;
	    GameInterface.StatusLine.BAR_HEALTH.Value = makeint(pchar.Health.HP);
	    if (sti(pchar.Health.HP) > 54 )
	    {
	        GameInterface.StatusLine.BAR_HEALTH.Value = 54;
	    }
    }
    else
    {
        GameInterface.StatusLine.BAR_HEALTH.Max = 1;
        GameInterface.StatusLine.BAR_HEALTH.Min = 0;
        GameInterface.StatusLine.BAR_HEALTH.Value = 0;
    }
    GameInterface.StatusLine.BAR_REPUTATION.Max   = REPUTATION_MAX;
    GameInterface.StatusLine.BAR_REPUTATION.Min   = REPUTATION_MIN;
    GameInterface.StatusLine.BAR_REPUTATION.Value = makeint(xi_refCharacter.reputation);
    
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"LOYALITY",0);
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"BAR_HEALTH",0);
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"BAR_REPUTATION",0);
    
    if (sti(xi_refCharacter.Skill.FreeSkill) > 0)
	{
        SetFormatedText("FREE_SKILL", xi_refCharacter.Skill.FreeSkill);
    }
    else
    {
        SetFormatedText("FREE_SKILL", "");
    }
    if (sti(xi_refCharacter.Skill.FreeSPECIAL) > 0)
	{
        SetFormatedText("FREE_SPECIAL", xi_refCharacter.Skill.FreeSPECIAL);
    }
    else
    {
        SetFormatedText("FREE_SPECIAL", "");
    }
}

void ProcessFrame()
{
	if (sti(GameInterface.CHARACTERS_SCROLL.current)!=nCurScrollNum && GetCurrentNode() == "CHARACTERS_SCROLL")
	{
		nCurScrollNum = sti(GameInterface.CHARACTERS_SCROLL.current);
		SetButtonsState();
		return;
	}
	if (sti(GameInterface.PASSENGERSLIST.current)!= nCurScrollOfficerNum && GetCurrentNode() == "PASSENGERSLIST")
	{
		nCurScrollOfficerNum = sti(GameInterface.PASSENGERSLIST.current);
		SetOfficersSkills();
		return;
	}
}

void SetButtonsState()
{
	string attributeName = "pic" + (nCurScrollNum+1);
	
	HideSkillChanger();
	if(GameInterface.CHARACTERS_SCROLL.(attributeName).character != "0")
	{
		int iCharacter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
		//WriteNewData();
		xi_refCharacter = &characters[iCharacter];
		//ReMappingSkills();
		//SetSkills();
		SetVariable();
	}
	else
	{
        xi_refCharacter = pchar;
        SetVariable();
	}
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	sPicture = "-1";
	string sAttributeName;
	int nChooseNum = -1;
	switch (sCurrentNode)
	{
		case "CHARACTERS_SCROLL":
			nChooseNum = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CHARACTERS_SCROLL", 2 );
			string attributeName = "pic" + (nChooseNum+1);
			int iCharacter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
            sHeader = XI_ConvertString("passengership");
			switch(nChooseNum)
			{
				case 0:
					sHeader = GetCharacterFullName(pchar.id);
					sText1 = "";
					sText2 = XI_ConvertString("It's you");
				break;

				case 1:
					sHeader = XI_ConvertString("navigator");
					sText1 = XI_ConvertString("Navigator_Descr");
					sText2 = XI_ConvertString("Navigator_Up");
				break;

				case 2:
					sHeader = XI_ConvertString("boatswain");
					sText1 = XI_ConvertString("Boatswain_Descr");
					sText2 = XI_ConvertString("Boatswain_Up");
				break;

				case 3:
					sHeader = XI_ConvertString("cannoner");
					sText1 = XI_ConvertString("Cannoner_Descr");
					sText2 = XI_ConvertString("Cannoner_Up");
				break;

				case 4:
					sHeader = XI_ConvertString("doctor");
					sText1 = XI_ConvertString("Doctor_Descr");
					sText2 = XI_ConvertString("Doctor_Up");
				break;

				case 5:
					sHeader = XI_ConvertString("treasurer");
					sText1 = XI_ConvertString("Treasurer_Descr");
					sText2 = XI_ConvertString("Treasurer_Up");
				break;

				case 6:
					sHeader = XI_ConvertString("carpenter");
					sText1 = XI_ConvertString("Carpenter_Descr");
					sText2 = XI_ConvertString("Carpenter_Up");
				break;

				case 7:
					sHeader = XI_ConvertString("fighter");
					sText1 = XI_ConvertString("Officer_Descr");
					sText2 = XI_ConvertString("Officer_Up");
				break;

				case 8:
					sHeader = XI_ConvertString("fighter");
					sText1 = XI_ConvertString("Officer_Descr");
					sText2 = XI_ConvertString("Officer_Up");
				break;

				case 9:
					sHeader = XI_ConvertString("fighter");
					sText1 = XI_ConvertString("Officer_Descr");
					sText2 = XI_ConvertString("Officer_Up");
				break;
			}
			if(iCharacter != 0)
			{
				ref rchr = &Characters[iCharacter];
				iItem = GetMoneyForOfficerFull(rchr);
				if (iItem > 0)
				{
				    sText2 = "Оплата " + FindRussianMoneyString(iItem) + " в месяц";
				}
				if(FindFellowtravellers(pchar,rchr) != FELLOWTRAVEL_COMPANION)
				{
					sText1 = GetCharacterFullName(Characters[iCharacter].id);

					if (CheckAttribute(&Characters[iCharacter], "prisoned") && Characters[iCharacter].prisoned == true)
					{
                        sHeader = GetRPGText("Prisoner");
						sText2 = GetRPGText("Prisoner_desc");
					}
				}
				else
				{
					if(CheckAttribute(rchr, "quest.convoyquest.money"))
					{
						sText3 = "";//SelectEscortQuestInfo(rchr);
					}
					sText1 = GetCharacterFullName(Characters[iCharacter].id);
					sHeader = XI_ConvertString("companionship");
				}
				sPicture = "interfaces\portraits\64\face_" + rchr.FaceId + ".tga"
			}


			if(sPicture == "-1")
			{
				sPicture = "interfaces/portraits/128/" + sHeader + ".tga";
			}
            sText3 = "Клик по списку - назначение или съем офицера.";
            if (bBettaTestMode)
            {
                sText3 += "   " +  Characters[iCharacter].id;
            }
		break;
		case "TABLE_SPECIAL":
		    sHeader = XI_ConvertString("Characteristics");
		    sText1  = GetRPGText(GameInterface.(CurTable).(CurRow).UserData.ID);
		break;
		case "TABLE_SKILL_1":
		    sHeader = XI_ConvertString("Personal skill");
		    sText1  = GetRPGText(GameInterface.(CurTable).(CurRow).UserData.ID);
		break;
		case "TABLE_SKILL_2":
		    sHeader = XI_ConvertString("Ship skill");
		    sText1  = GetRPGText(GameInterface.(CurTable).(CurRow).UserData.ID);
		break;
		case "LOYALITY_STR":
		    sHeader = XI_ConvertString("Loyality");
		    sText1  = GetRPGText("Loyality");
		break;
		case "TABBTN_1":
		    sHeader = XI_ConvertString("Personal abilities");
		    sText1  = GetRPGText("Personal_abilities");
		break;
		case "TABBTN_2":
		    sHeader = XI_ConvertString("Ship abilities");
		    sText1  = GetRPGText("Ship_abilities");
		break;
		case "TABLE_OTHER":
		    sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetRPGText(GameInterface.(CurTable).(CurRow).UserData.ID);
		break;
		case "TABLE_PERKS":
		    sHeader = GetRPGText("TABLE_PERKS");
		    sText1  = GetRPGText("TABLE_PERKS_desc");
		break;
		case "PERK_TABLE_NEED":
		    sHeader = GetRPGText("PERK_TABLE_NEED");
		    sText1  = GetRPGText("PERK_TABLE_NEED_desc");
		break;
		case "CHARACTER_NATION_PICTURE":
		    sHeader = XI_ConvertString("Nation");
			sText1 = GetRPGText("Nation_hint");
		break;
	}
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);

}

void HideInfoWindow()
{
	CloseTooltip();
}

void FillSkillTables()
{
	int     i;
	string  row, skillName;
    int     diff, skillVal;
    
    // boal оптимизация скилов -->
    DelBakSkillAttr(xi_refCharacter);
    ClearCharacterExpRate(xi_refCharacter);
    RefreshCharacterSkillExpRate(xi_refCharacter);
    
    SetEnergyToCharacter(xi_refCharacter);
    // boal оптимизация скилов <--
    
    if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_self") )
		xi_refCharacter.perks.FreePoints_self = 0;
    if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_ship") )
		xi_refCharacter.perks.FreePoints_ship = 0;
		
    SetFormatedText("TABSTR_1", XI_ConvertString("Personal_abilities") + " " +xi_refCharacter.perks.FreePoints_self);
    SetFormatedText("TABSTR_2", XI_ConvertString("Ship_abilities") + " " + xi_refCharacter.perks.FreePoints_ship);
    SetControlsTabMode(1);

	GameInterface.TABLE_SPECIAL.select = 0;
	GameInterface.TABLE_SPECIAL.hr.td1.str = "";
	SetFormatedText("STR_1", XI_ConvertString("Characteristics"));

	for (i=1; i<=7; i++)
	{
	    row = "tr" + i;
	    skillName = GetSkillNameByTRIdx("SPECIALType", i);
	    
        GameInterface.TABLE_SPECIAL.(row).UserData.ID = skillName;
		GameInterface.TABLE_SPECIAL.(row).td1.fontidx = 0;
		GameInterface.TABLE_SPECIAL.(row).td1.textoffset = "0,-6";
		//GameInterface.TABLE_SPECIAL.(row).td1.align = "center";
		GameInterface.TABLE_SPECIAL.(row).td2.textoffset = "5,0";
		GameInterface.TABLE_SPECIAL.(row).td3.align = "center";
		GameInterface.TABLE_SPECIAL.(row).td4.scale = 0.90;
		GameInterface.TABLE_SPECIAL.(row).td4.fontidx = 1;
        
		GameInterface.TABLE_SPECIAL.(row).td1.str = XI_ConvertString(skillName + "T");
		GameInterface.TABLE_SPECIAL.(row).td2.str = XI_ConvertString(skillName);
		skillVal = GetSkillValue(xi_refCharacter, SPECIAL_TYPE, skillName);
		GameInterface.TABLE_SPECIAL.(row).td4.str = skillVal;
		// рассчет драйна
		diff = GetCharacterSPECIAL(xi_refCharacter, skillName) - skillVal;
		if (diff == 0)
		{
     		GameInterface.TABLE_SPECIAL.(row).td3.str = "";
     		DeleteAttribute(&GameInterface, "TABLE_SPECIAL." + row + ".td3.color");
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.TABLE_SPECIAL.(row).td3.str = "(+" + diff + ")";
	          GameInterface.TABLE_SPECIAL.(row).td3.color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.TABLE_SPECIAL.(row).td3.str = "(" + diff + ")";
	          GameInterface.TABLE_SPECIAL.(row).td3.color = argb(255,255,196,196);
	       }
		}
	}
    GameInterface.TABLE_SKILL_1.select = 0;
    GameInterface.TABLE_SKILL_1.hr.td1.str = "";
	SetFormatedText("STR_2", XI_ConvertString("Personal skill"));
	for (i=1; i<=7; i++)
	{
	    row = "tr" + i;
	    skillName = GetSkillNameByTRIdx("SelfType", i);
	    
        GameInterface.TABLE_SKILL_1.(row).UserData.ID = skillName;
		GameInterface.TABLE_SKILL_1.(row).td1.icon.group = "ICONS_SPEC";
	    GameInterface.TABLE_SKILL_1.(row).td1.icon.image = skillName + " skill icon";
	    GameInterface.TABLE_SKILL_1.(row).td1.icon.width = 30;
    	GameInterface.TABLE_SKILL_1.(row).td1.icon.height = 30;
    	GameInterface.TABLE_SKILL_1.(row).td1.icon.offset = "-4, 0";
		GameInterface.TABLE_SKILL_1.(row).td2.align = "left";
		GameInterface.TABLE_SKILL_1.(row).td2.scale = 0.75;
		GameInterface.TABLE_SKILL_1.(row).td2.textoffset = "3,0";
		GameInterface.TABLE_SKILL_1.(row).td5.fontidx = 0;
		GameInterface.TABLE_SKILL_1.(row).td5.scale = 0.65;
		GameInterface.TABLE_SKILL_1.(row).td5.align = "left";
		GameInterface.TABLE_SKILL_1.(row).td3.scale = 0.75;
		GameInterface.TABLE_SKILL_1.(row).td4.scale = 0.85;

		GameInterface.TABLE_SKILL_1.(row).td2.str = XI_ConvertString(skillName);
		skillVal = GetSkillValue(xi_refCharacter, SKILL_TYPE, skillName);
		GameInterface.TABLE_SKILL_1.(row).td5.str = skillVal;
		// рассчет драйна
		diff = GetSummonSkillFromName(xi_refCharacter, skillName) - skillVal;
		
		if (skillVal < SKILL_MAX)
		{
			GameInterface.TABLE_SKILL_1.(row).td3.str = makeint(GetSkillValueExp(xi_refCharacter, skillName) * 100.0 / makefloat(skillVal * GetCharacterExpRate(xi_refCharacter, skillName))) + "%";
		}
		else
		{
		    GameInterface.TABLE_SKILL_1.(row).td3.str = "";
		}
		if (diff == 0)
		{
     		GameInterface.TABLE_SKILL_1.(row).td4.str = "";
     		DeleteAttribute(&GameInterface, "TABLE_SKILL_1." + row + ".td4.color");
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.TABLE_SKILL_1.(row).td4.str = "(+" + diff + ")";
	          GameInterface.TABLE_SKILL_1.(row).td4.color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.TABLE_SKILL_1.(row).td4.str = "(" + diff + ")";
	          GameInterface.TABLE_SKILL_1.(row).td4.color = argb(255,255,196,196);
	       }
		}
	}
	GameInterface.TABLE_SKILL_2.select = 0;
	GameInterface.TABLE_SKILL_2.hr.td1.str = "";
	SetFormatedText("STR_3", XI_ConvertString("Ship skill"));
	for (i=1; i<=7; i++)
	{
	    row = "tr" + i;
	    skillName = GetSkillNameByTRIdx("ShipType", i);

        GameInterface.TABLE_SKILL_2.(row).UserData.ID = skillName;
		GameInterface.TABLE_SKILL_2.(row).td1.icon.group = "ICONS_SPEC";
	    GameInterface.TABLE_SKILL_2.(row).td1.icon.image = skillName + " skill icon";
	    GameInterface.TABLE_SKILL_2.(row).td1.icon.width = 30;
    	GameInterface.TABLE_SKILL_2.(row).td1.icon.height = 30;
    	GameInterface.TABLE_SKILL_2.(row).td1.icon.offset = "-1, 0";
		GameInterface.TABLE_SKILL_2.(row).td2.align = "left";
		GameInterface.TABLE_SKILL_2.(row).td2.scale = 0.75;
		GameInterface.TABLE_SKILL_2.(row).td2.textoffset = "3,0";
		GameInterface.TABLE_SKILL_2.(row).td5.fontidx = 0;
		GameInterface.TABLE_SKILL_2.(row).td5.scale = 0.65;
		GameInterface.TABLE_SKILL_2.(row).td5.align = "left";
		GameInterface.TABLE_SKILL_2.(row).td3.scale = 0.75;
		GameInterface.TABLE_SKILL_2.(row).td4.scale = 0.85;

		GameInterface.TABLE_SKILL_2.(row).td2.str = XI_ConvertString(skillName);
		skillVal = GetSkillValue(xi_refCharacter, SKILL_TYPE, skillName);
		GameInterface.TABLE_SKILL_2.(row).td5.str = skillVal;
		// рассчет драйна
		diff = GetSummonSkillFromName(xi_refCharacter, skillName) - skillVal;

		if (skillVal < SKILL_MAX)
		{
			GameInterface.TABLE_SKILL_2.(row).td3.str = makeint(GetSkillValueExp(xi_refCharacter, skillName) * 100.0 / makefloat(skillVal * GetCharacterExpRate(xi_refCharacter, skillName))) + "%";
		}
		else
		{
		    GameInterface.TABLE_SKILL_2.(row).td3.str = "";
		}
		if (diff == 0)
		{
     		GameInterface.TABLE_SKILL_2.(row).td4.str = "";
     		DeleteAttribute(&GameInterface, "TABLE_SKILL_2." + row + ".td4.color");
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.TABLE_SKILL_2.(row).td4.str = "(+" + diff + ")";
	          GameInterface.TABLE_SKILL_2.(row).td4.color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.TABLE_SKILL_2.(row).td4.str = "(" + diff + ")";
	          GameInterface.TABLE_SKILL_2.(row).td4.color = argb(255,255,196,196);
	       }
		}
	}
	////  остальное
	GameInterface.TABLE_OTHER.select = 0;
	GameInterface.TABLE_OTHER.hr.td1.str = "";
	for (i=1; i<=9; i++)
	{
	    row = "tr" + i;

	    GameInterface.TABLE_OTHER.(row).td1.icon.width = 26;
    	GameInterface.TABLE_OTHER.(row).td1.icon.height = 26;
    	GameInterface.TABLE_OTHER.(row).td1.icon.offset = "-1, 0";
		GameInterface.TABLE_OTHER.(row).td2.align = "left";
		GameInterface.TABLE_OTHER.(row).td2.scale = 0.85;
		GameInterface.TABLE_OTHER.(row).td2.textoffset = "3,0";
		GameInterface.TABLE_OTHER.(row).td3.align = "right";
	}
	GameInterface.TABLE_OTHER.tr1.UserData.ID = "Rank";
	GameInterface.TABLE_OTHER.tr1.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr1.td1.icon.image = "Rank";
	GameInterface.TABLE_OTHER.tr1.td2.str = XI_ConvertString("Rank");
	GameInterface.TABLE_OTHER.tr1.td3.str = sti(xi_refCharacter.rank);
	
	GameInterface.TABLE_OTHER.tr2.UserData.ID = "Life";
	GameInterface.TABLE_OTHER.tr2.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr2.td1.icon.image = "Life";
	GameInterface.TABLE_OTHER.tr2.td2.str = XI_ConvertString("Life");
	GameInterface.TABLE_OTHER.tr2.td3.str = MakeInt(LAi_GetCharacterHP(xi_refCharacter)) + " / " + MakeInt(LAi_GetCharacterMaxHP(xi_refCharacter));

    GameInterface.TABLE_OTHER.tr3.UserData.ID = "Health";
	GameInterface.TABLE_OTHER.tr3.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr3.td1.icon.image = "Health";
	GameInterface.TABLE_OTHER.tr3.td2.str = XI_ConvertString("Health");
	GameInterface.TABLE_OTHER.tr3.td3.str = GetHealthName(xi_refCharacter);
	GameInterface.TABLE_OTHER.tr3.td3.scale = 0.75;
	
	if (GetHealthNum(xi_refCharacter) >= GetHealthMaxNum(xi_refCharacter))
    {
        GameInterface.TABLE_OTHER.tr3.td3.color = SetAlphaIntoColor(COLOR_MONEY, GetAlphaFromSkill(10));
    }
    else
    {
        GameInterface.TABLE_OTHER.tr3.td3.color = SetAlphaIntoColor(COLOR_NORMAL, GetAlphaFromSkill(makeint(GetHealthNum(xi_refCharacter)* 1.9)));
    }
    
    GameInterface.TABLE_OTHER.tr4.UserData.ID = "Energy";
	GameInterface.TABLE_OTHER.tr4.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr4.td1.icon.image = "Energy";
	GameInterface.TABLE_OTHER.tr4.td2.str = XI_ConvertString("Energy");
	GameInterface.TABLE_OTHER.tr4.td3.str = sti(Lai_CharacterGetEnergy(xi_refCharacter)) + " / " + sti(LAi_GetCharacterMaxEnergy(xi_refCharacter));
	diff = sti(LAi_GetCharacterMaxEnergy(xi_refCharacter) - GetCharacterMaxEnergyABSValue(xi_refCharacter));
	if (diff != 0)
	{
	   if (diff > 0)
	   {
          GameInterface.TABLE_OTHER.tr4.td3.color = argb(255,196,255,196);
       }
       else
       {
          GameInterface.TABLE_OTHER.tr4.td3.color = argb(255,255,196,196);
       }
	}
	else
	{
		DeleteAttribute(&GameInterface, "TABLE_OTHER.tr4.td3.color");
	}
	
	GameInterface.TABLE_OTHER.tr5.UserData.ID = "Money";
	GameInterface.TABLE_OTHER.tr5.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr5.td1.icon.image = "Money";
	GameInterface.TABLE_OTHER.tr5.td2.str = XI_ConvertString("Money");
	GameInterface.TABLE_OTHER.tr5.td3.str = MakeMoneyShow(sti(xi_refCharacter.Money), MONEY_SIGN,MONEY_DELIVER);
	//GameInterface.TABLE_OTHER.tr5.td3.scale = 0.95;
	//GameInterface.TABLE_OTHER.tr5.td3.color = SetAlphaIntoColor(COLOR_MONEY, GetAlphaFromSkill(10));

    GameInterface.TABLE_OTHER.tr6.UserData.ID = "Reputation";
	GameInterface.TABLE_OTHER.tr6.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr6.td1.icon.image = "Reputation";
	GameInterface.TABLE_OTHER.tr6.td2.str = XI_ConvertString("Reputation");
	GameInterface.TABLE_OTHER.tr6.td3.str = XI_ConvertString(GetReputationName(sti(xi_refCharacter.reputation)));
	GameInterface.TABLE_OTHER.tr6.td3.scale = 0.78;
	
	GameInterface.TABLE_OTHER.tr7.UserData.ID = "weight";
	GameInterface.TABLE_OTHER.tr7.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr7.td1.icon.image = "weight";
	GameInterface.TABLE_OTHER.tr7.td2.str = XI_ConvertString("weight");
	GameInterface.TABLE_OTHER.tr7.td3.str = FloatToString(GetItemsWeight(xi_refCharacter), 1) + " / "+GetMaxItemsWeight(xi_refCharacter);
	
    GameInterface.TABLE_OTHER.tr8.UserData.ID = "Title";
	GameInterface.TABLE_OTHER.tr8.td1.icon.group = "ICONS_CHAR";
	GameInterface.TABLE_OTHER.tr8.td1.icon.image = "Title";
	
	DeleteAttribute(&GameInterface, "TABLE_OTHER.tr8.td2");
	GameInterface.TABLE_OTHER.tr8.td2.str = XI_ConvertString("Title");
	GameInterface.TABLE_OTHER.tr8.td2.align = "left";
	GameInterface.TABLE_OTHER.tr8.td2.scale = 0.85;
	GameInterface.TABLE_OTHER.tr8.td2.textoffset = "3,0";

    DeleteAttribute(&GameInterface, "TABLE_OTHER.tr8.td3");
    GameInterface.TABLE_OTHER.tr8.td3.str = "";
	GameInterface.TABLE_OTHER.tr8.td3.scale = 0.9;
	
	if (xi_refCharacter.id == pchar.id && isMainCharacterPatented())
    {
        GameInterface.TABLE_OTHER.tr8.td3.str = GetAddress_FormTitle(sti(Items[sti(pchar.EquipedPatentId)].Nation), sti(Items[sti(pchar.EquipedPatentId)].TitulCur));
        GameInterface.TABLE_OTHER.tr8.td2.icon.group = "NATIONS";
		GameInterface.TABLE_OTHER.tr8.td2.icon.image = GetNationNameByType(sti(Items[sti(pchar.EquipedPatentId)].Nation));
		GameInterface.TABLE_OTHER.tr8.td2.icon.offset = "60, 1";
		GameInterface.TABLE_OTHER.tr8.td2.icon.width = 24;
		GameInterface.TABLE_OTHER.tr8.td2.icon.height = 24;
    }
    else
    {
        GameInterface.TABLE_OTHER.tr8.td3.str = "нет звания";
    }
	
	GameInterface.TABLE_OTHER.tr9.UserData.ID = "NextExp";
	GameInterface.TABLE_OTHER.tr9.td1.icon.group = "ICONS_CHAR";
    GameInterface.TABLE_OTHER.tr9.td1.icon.image = "NextExp";
	GameInterface.TABLE_OTHER.tr9.td2.str = XI_ConvertString("NextExp");
	GameInterface.TABLE_OTHER.tr9.td3.str = "";
	
	// прорисовка
	Table_UpdateWindow("TABLE_SPECIAL");
    Table_UpdateWindow("TABLE_SKILL_1");
    Table_UpdateWindow("TABLE_SKILL_2");
    Table_UpdateWindow("TABLE_PERKS");
    Table_UpdateWindow("TABLE_OTHER");
}

void CS_TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurTable = sControl;
    CurRow   =  "tr" + (iSelected);
    //sti(GameInterface.(sControl).(sRow).index);
 	NullSelectTable("TABLE_SPECIAL");
    NullSelectTable("TABLE_SKILL_1");
    NullSelectTable("TABLE_SKILL_2");
    NullSelectTable("TABLE_PERKS");
    NullSelectTable("TABLE_OTHER");
    // перерисует "порог опыта"
    GameInterface.TABLE_OTHER.tr9.td3.str = "";
    if (CurTable == "TABLE_OTHER" && CurRow == "tr1")
    {
    	GameInterface.TABLE_OTHER.tr9.td3.str = GetCharacterRankRateCur(xi_refCharacter) + "/" + GetCharacterRankRate(xi_refCharacter);
    }
    sti(xi_refCharacter.rank_exp)
	if (CurTable == "TABLE_SKILL_1" || CurTable == "TABLE_SKILL_2")
	{
        string skillName;
        int skillVal;
        skillName = GameInterface.(CurTable).(CurRow).UserData.ID;
        skillVal = GetSkillValue(xi_refCharacter, SKILL_TYPE, skillName);
		if (skillVal != SKILL_MAX && skillVal != 0)
		{
	       GameInterface.TABLE_OTHER.tr9.td3.str = sti(GetSkillValueExp(xi_refCharacter, skillName))+ "/" + makeint(skillVal * GetCharacterExpRate(xi_refCharacter, skillName));
		}
	}
	Table_UpdateWindow("TABLE_OTHER");
	// установим скилчанжер, если нужно
	HideSkillChanger();
    SetSkillArrows();
}

void SetSkillArrows()
{
   	bool ok;
	ok = CurTable == "TABLE_SKILL_1" || CurTable == "TABLE_SKILL_2";
	if (sti(xi_refCharacter.skill.freeskill) > 0 && ok)
	{
	    if (GetSkillValue(xi_refCharacter, SKILL_TYPE, GameInterface.(CurTable).(CurRow).UserData.ID) > 1)
        {
			if (CurTable == "TABLE_SKILL_1")
			{
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 690 ,105 + 32*(iSelected-1), 722, 105 + 28 + 32*(iSelected-1), 0);
			}
			else
			{
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 690 ,364 + 32*(iSelected-1), 722,364 + 28 + 32*(iSelected-1), 0);
			}
			SetNodeUsing("B_SKILLDown", true);
        }
        if (GetSkillValue(xi_refCharacter, SKILL_TYPE, GameInterface.(CurTable).(CurRow).UserData.ID) < SKILL_MAX)
        {
            if (CurTable == "TABLE_SKILL_1")
			{
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 717,105 + 32*(iSelected-1),749,105 + 28 + 32*(iSelected-1), 0);
			}
			else
			{
				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 717,364 + 32*(iSelected-1),749,364 + 28 + 32*(iSelected-1), 0);
			}
            SetNodeUsing("B_SKILLUP",  true);
        }
	}
	ok = sti(xi_refCharacter.skill.freeskill) > 0 || sti(xi_refCharacter.skill.FreeSPECIAL) > 0;
	if (ok && CurTable == "TABLE_SPECIAL")
	{
	    if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GameInterface.(CurTable).(CurRow).UserData.ID) > 1)
        {
			SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 265 ,105 + 32*(iSelected-1), 297, 105 + 28 + 32*(iSelected-1), 0);
			SetNodeUsing("B_SKILLDown", true);
        }
        if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GameInterface.(CurTable).(CurRow).UserData.ID) < SPECIAL_MAX)
        {
			SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 288,105 + 32*(iSelected-1),320,105 + 28 + 32*(iSelected-1), 0);
            SetNodeUsing("B_SKILLUP",  true);
        }
	}
}

void procTabChange()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();
	SetCurrentNode("TABLE_PERKS");
	if( sNodName == "TABBTN_1" )
	{
		SetControlsTabMode( 1 );
		return;
	}
	if( sNodName == "TABBTN_2" )
	{
		SetControlsTabMode( 2 );
		return;
	}
}

void SetControlsTabMode(int nMode)
{
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;

	string sPic1 = "TabSelected";
	string sPic2 = sPic1;

	switch (nMode)
	{
		case 1: //
			sPic1 = "TabDeSelected";
			nColor1 = argb(255,255,255,255);
			// порог опыта
			if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_self_exp"))
            {
               xi_refCharacter.perks.FreePoints_self_exp = 0;
            }
            GameInterface.TABLE_OTHER.tr9.td3.str = sti(xi_refCharacter.perks.FreePoints_self_exp)+ "/" + GetFreePoints_SelfRate(xi_refCharacter);
		break;
		case 2:
			sPic2 = "TabDeSelected";
			nColor2 = argb(255,255,255,255);
			// порог опыта
			if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_ship_exp"))
            {
               xi_refCharacter.perks.FreePoints_ship_exp = 0;
            }
            GameInterface.TABLE_OTHER.tr9.td3.str = sti(xi_refCharacter.perks.FreePoints_ship_exp)+ "/" + GetFreePoints_ShipRate(xi_refCharacter);
		break;
	}
    Table_UpdateWindow("TABLE_OTHER"); // порог опыта
    
	SetNewGroupPicture("TABBTN_1", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_2", "TABS", sPic2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_1", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_2", 8,0,nColor2);

	FillControlsList(nMode);
	HideSkillChanger();
}

void FillControlsList(int nMode)
{
	switch (nMode)
	{
	    case 1: FillPerksTable("Self", true); break;  // Личные перки
	    case 2: FillPerksTable("Ship", true); break;  // Корабельные перки
	}
}

void SetOfficersSkills()
{
	string sCharacter = "pic"+(sti(GameInterface.PASSENGERSLIST.current)+1);
	if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter + ".character"))
		{
			sCharacter = GameInterface.PASSENGERSLIST.(sCharacter).character;
			ref otherchr = &characters[sti(sCharacter)];
	        SetSPECIALMiniTable("TABLE_SMALLSKILL", otherchr);
	        SetOTHERMiniTable("TABLE_SMALLOTHER", otherchr);
	        SetFormatedText("OFFICER_NAME", GetFullName(otherchr));
	        SetSelectable("ACCEPT_ADD_OFFICER", true);
        }
        else
        {
            Table_Clear("TABLE_SMALLSKILL", false, true, true);
            Table_Clear("TABLE_SMALLOTHER", false, true, true);
            SetFormatedText("OFFICER_NAME", "");
            SetSelectable("ACCEPT_ADD_OFFICER", false);
        }
	}
	else
    {
        Table_Clear("TABLE_SMALLSKILL", false, true, true);
        Table_Clear("TABLE_SMALLOTHER", false, true, true);
        SetFormatedText("OFFICER_NAME", "");
        SetSelectable("ACCEPT_ADD_OFFICER", false);
    }
}

void FillPassengerScroll()
{
	int i, howWork;
	string faceName;
	string attributeName;
	string PsgAttrName;
	int _curCharIdx;
	ref _refCurChar;
	aref pRef, pRef2;
	bool  ok;
	
	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollOfficerNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;
	makearef(pRef,pchar.Fellows.Passengers);

	int nListSize = GetPassengersQuantity(pchar);
	int nListSizeFree = GetFreePassengersQuantity(pchar);

	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ListSize = nListSizeFree + 2;

	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";

	FillFaceList("PASSENGERSLIST.ImagesGroup", pchar, 2); // passengers

	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";

	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		PsgAttrName = "id"+(i+1);
		makearef(pRef2,GameInterface.PASSENGERSLIST.(attributeName));

		_curCharIdx = sti(pRef.(PsgAttrName));

		if (_curCharIdx!=-1)
		{
			ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
			if (!ok && GetRemovable(&characters[_curCharIdx]))
			{
                // совместители должностей -->
                howWork = 1;
                if (CheckCharacterPerk(&characters[_curCharIdx], "ByWorker"))
                {
                    howWork = 2;
                }
                if (CheckCharacterPerk(&characters[_curCharIdx], "ByWorker2"))
                {
                    howWork = 3;
                }
                ok = !CheckAttribute(&characters[_curCharIdx], "isfree") || sti(characters[_curCharIdx].isfree) < howWork;
                PsgAttrName = GetOfficerTypeByNum(nCurScrollNum);
				// совместители должностей <--
				if (ok && !CheckAttribute(&characters[_curCharIdx], PsgAttrName))
				{
					GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
					GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
					GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
					m++;
				}
			}
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m + 2; // не знаю зачем, но для совместимости с "было"
}

void ExitOfficerMenu()
{
	XI_WindowShow("OFFICERS_WINDOW", false);
	XI_WindowDisable("OFFICERS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CHARACTERS_SCROLL");
}

void OfficerChange()
{
    string attributeName = "pic" + (nCurScrollNum+1);

	if(GameInterface.CHARACTERS_SCROLL.(attributeName).character != "0")
	{
		int iCharacter = sti(GameInterface.CHARACTERS_SCROLL.(attributeName).character);
		xi_refCharacter = &characters[iCharacter];
		if (isOfficerInShip(xi_refCharacter, true) && xi_refCharacter.id != pchar.id)
		{
			XI_WindowShow("REMOVE_OFFICER_WINDOW", true);
			XI_WindowDisable("REMOVE_OFFICER_WINDOW", false);
			XI_WindowDisable("MAIN_WINDOW", true);

			SetCurrentNode("REMOVE_CANCEL_OFFICER");
		}
	}
	else
	{
	    if (nCurScrollNum <= 9 && nCurScrollNum != 0)
		{
			FillPassengerScroll();
		    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
		    SetCurrentNode("PASSENGERSLIST");
			ProcessFrame();
			SetOfficersSkills();

			XI_WindowShow("OFFICERS_WINDOW", true);
			XI_WindowDisable("OFFICERS_WINDOW", false);
			XI_WindowDisable("MAIN_WINDOW", true);
		}
	}
}

void AcceptAddOfficer()
{
	int iCurrentNode = nCurScrollNum;
	bool bNeedFollow = false;
	bool bOk;
	
	string attributeName2 = "pic"+(nCurScrollOfficerNum+1);

    if (checkAttribute(GameInterface, "PASSENGERSLIST."+attributeName2 + ".character"))
    {
		int iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);

		if (!CheckAttribute(&characters[iChar], "isfree"))
		{
			characters[iChar].isfree = 1;
		}
		else
		{
		    characters[iChar].isfree = sti(characters[iChar].isfree) + 1; // совместители
		}
		bOk = (Characters[iChar].location != pchar.location);  // ниже локация перебивается на ГГ
		switch (nCurScrollNum)
		{
			case 1:
				pchar.Fellows.Passengers.navigator = iChar;
			break;

			case 2:
				pchar.Fellows.Passengers.boatswain = iChar;
			break;

			case 3:
				pchar.Fellows.Passengers.cannoner = iChar;
			break;

			case 4:
				pchar.Fellows.Passengers.doctor = iChar;
			break;

			case 5:
				pchar.Fellows.Passengers.treasurer = iChar;
			break;

			case 6:
				pchar.Fellows.Passengers.carpenter = iChar;
			break;

			case 7:
				SetOfficersIndex(pchar, 1, iChar);
				bNeedFollow = true;
			break;

			case 8:
				SetOfficersIndex(pchar, 2, iChar);
				bNeedFollow = true;
			break;

			case 9:
				SetOfficersIndex(pchar, 3, iChar);
				bNeedFollow = true;
			break;
		}
        attributeName2 = GetOfficerTypeByNum(nCurScrollNum);
    	characters[iChar].(attributeName2) = true; // совместитель дожности
    	//SetCharacterTask_FollowCharacter(&Characters[iChar], PChar);
    	if (bNeedFollow) // только для офов
    	{
	    	//if (Characters[iChar].location.group == "sit")
	    	//{// найм прямо в таверне
	    	//if (bOk && IsEntity(loadedLocation) && !CheckAttribute(loadedLocation, "DisableOfficers"))
	    	if (IsEntity(loadedLocation) && loadedLocation.type == "tavern")
	    	{   //  пусть всегда будут появляться
	    	    PlaceCharacter(&Characters[iChar], "goto", "random_must_be_near");
	    	}
	    	LAi_tmpl_SetFollow(&Characters[iChar], GetMainCharacter(), -1.0);
    	}
		FillCharactersScroll();
		GameInterface.CHARACTERS_SCROLL.current = iCurrentNode;
	}
	ExitOfficerMenu();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CHARACTERS_SCROLL",-1);
	ProcessFrame();
	SetVariable();
}

void ExitRemoveOfficerMenu()
{
	XI_WindowShow("REMOVE_OFFICER_WINDOW", false);
	XI_WindowDisable("REMOVE_OFFICER_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("CHARACTERS_SCROLL");
}

void AcceptRemoveOfficer()
{
    int iCurrentNode = nCurScrollNum;
	string attributeName2 = "pic"+(nCurScrollNum+1);

	int iChar = sti(GameInterface.CHARACTERS_SCROLL.(attributeName2).character);

    characters[iChar].isfree = sti(characters[iChar].isfree) - 1; // совместители
	if (sti(characters[iChar].isfree) <= 0)
	{
		DeleteAttribute(&characters[iChar], "isfree");
	}

	switch (nCurScrollNum)
	{
		case 1:
			pchar.Fellows.Passengers.navigator = -1;
		break;

		case 2:
			pchar.Fellows.Passengers.boatswain = -1;
		break;

		case 3:
			pchar.Fellows.Passengers.cannoner = -1;
		break;

		case 4:
			pchar.Fellows.Passengers.doctor = -1;
		break;

		case 5:
			pchar.Fellows.Passengers.treasurer = -1;
		break;

		case 6:
			pchar.Fellows.Passengers.carpenter = -1;
		break;

		case 7:
			RemoveOfficersIndex(pchar, GetOfficersIndex(pchar, 1));
		break;

		case 8:
			RemoveOfficersIndex(pchar, GetOfficersIndex(pchar, 2));
		break;

		case 9:
			RemoveOfficersIndex(pchar, GetOfficersIndex(pchar, 3));
		break;
	}
    attributeName2 = GetOfficerTypeByNum(nCurScrollNum);
    DeleteAttribute(&characters[iChar], attributeName2); // совместитель дожности
    
	FillCharactersScroll();
	GameInterface.CHARACTERS_SCROLL.current = iCurrentNode;
	ExitRemoveOfficerMenu();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"CHARACTERS_SCROLL",-1);
	SetVariable();
}

void HideSkillChanger()
{
    SetNodeUsing("B_SKILLUP",  false);
	SetNodeUsing("B_SKILLDown",  false);
}

void IncreaseSkill(int _add)
{
	int     iValue;
	string  sSkillName;

    sSkillName = GameInterface.(CurTable).(CurRow).UserData.ID;
    if (CurTable != "TABLE_SPECIAL")
    {
		if (sti(xi_refCharacter.skill.freeskill) < _add) _add = sti(xi_refCharacter.skill.freeskill);
		if ((GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) + _add) > SKILL_MAX)
		{
		    _add = SKILL_MAX - GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName);
		}
	    if (_add > 0)
	    {
	        iValue = AddCharacterSkill(xi_refCharacter, sSkillName, _add);
			xi_refCharacter.skill.freeskill = sti(xi_refCharacter.skill.freeskill) - _add;
	    }
	    else return;
    }
    else
    {
        if (sti(xi_refCharacter.skill.FreeSPECIAL) < _add) _add = sti(xi_refCharacter.skill.FreeSPECIAL);
		if ((GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName) + _add) > SPECIAL_MAX)
		{
		    _add = SPECIAL_MAX - GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName);
		}
	    if (_add > 0)
	    {
	        iValue = AddSPECIALValue(xi_refCharacter, sSkillName, _add);
			xi_refCharacter.skill.FreeSPECIAL = sti(xi_refCharacter.skill.FreeSPECIAL) - _add;
	    }
	    else return;
    }
	SetVariable();
	SetSkillArrows();
}

void DecreaseSkill(int _add)
{
	int     iValue;
	string  sSkillName;

    sSkillName = GameInterface.(CurTable).(CurRow).UserData.ID;
    if (CurTable != "TABLE_SPECIAL")
    {
	    if (GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) <= _add)
	    {
	        _add = GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) - 1;
	    }
	    if (_add > 0)
	    {
	        iValue = AddCharacterSkill(xi_refCharacter, sSkillName, -_add);
			xi_refCharacter.skill.freeskill = sti(xi_refCharacter.skill.freeskill) + _add;
	    }
	    else return;
    }
    else
    {
        if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName) <= _add)
	    {
	        _add = GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName) - 1;
	    }
	    if (_add > 0)
	    {
	        iValue = AddSPECIALValue(xi_refCharacter, sSkillName, -_add);
			xi_refCharacter.skill.FreeSPECIAL = sti(xi_refCharacter.skill.FreeSPECIAL) + _add;
	    }
	    else return;
    }
    SetVariable();
    SetSkillArrows();
}
///////////////////////////////////////  PERKS /////////////////////////
void FillPerksTable(string _type, bool _refresh)
{
	int perksQ, i, n;
	string perkName, row, icoGroup;
	aref arPerksRoot;

    Table_Clear("TABLE_PERKS", false, true, false);
	GameInterface.TABLE_PERKS.hr.td1.str = "";
	if (_refresh)
	{
	    GameInterface.TABLE_PERKS.select = 0;
	    GameInterface.TABLE_PERKS.top = 0;
	}
	SetFormatedText("STR_4", XI_ConvertString("Abilities"));

	makearef(arPerksRoot,ChrPerksList.list); // общий список
	perksQ = GetAttributesNum(arPerksRoot);
	n = 1;
	for(i=0; i<perksQ; i++)
	{
	    row = "tr" + n;

        perkName = GetAttributeName(GetAttributeN(arPerksRoot,i));
        if (xi_refCharacter.id == pchar.id && CheckAttribute(arPerksRoot, perkName + ".NPCOnly")) continue;
        if (xi_refCharacter.id != pchar.id && CheckAttribute(arPerksRoot, perkName + ".PlayerOnly")) continue;
        if (CheckAttribute(arPerksRoot, perkName + ".Hiden")) continue;
        
        if (!CheckAttribute(arPerksRoot, perkName + ".BaseType")) // to_DO
        {
            arPerksRoot.(perkName).BaseType = "self";
        }
		if (CheckAttribute(arPerksRoot, perkName + ".BaseType") && arPerksRoot.(perkName).BaseType == _type)
		{
			GameInterface.TABLE_PERKS.(row).UserData.ID   = perkName;
			GameInterface.TABLE_PERKS.(row).UserData.Type = _type;
            if (CheckCharacterPerk(xi_refCharacter, perkName))
            {
                icoGroup = "PERK_ENABLE";
                GameInterface.TABLE_PERKS.(row).td2.color = argb(255,255,255,255);
            }
            else
            {
                icoGroup = "PERK_DISABLE";
                if (xi_refCharacter.id == pchar.id && GetOfficersPerkUsing(pchar, perkName))
                {   // есть у офа, но нет у ГГ
                	GameInterface.TABLE_PERKS.(row).td2.color = argb(255,196,255,196);
                }
                else
                {
                	GameInterface.TABLE_PERKS.(row).td2.color = argb(255,196,196,196);
                }
            }
			GameInterface.TABLE_PERKS.(row).td1.icon.group  = icoGroup;
			GameInterface.TABLE_PERKS.(row).td1.icon.image  = perkName;
			GameInterface.TABLE_PERKS.(row).td1.icon.width  = 28;
		    GameInterface.TABLE_PERKS.(row).td1.icon.height = 28;
		    GameInterface.TABLE_PERKS.(row).td1.icon.offset = "-1, 1";

			GameInterface.TABLE_PERKS.(row).td2.str = GetConvertStr(perkName, "AbilityDescribe.txt");
			//GameInterface.TABLE_PERKS.(row).td2.textoffset = "0,-6";
			GameInterface.TABLE_PERKS.(row).td2.scale = 0.80;
			n++;
		}
	}

	// прорисовка
	Table_UpdateWindow("TABLE_PERKS");
}

void ChoosePerk()
{
	bool ok = true;
	bool showCondition = true;
	string  perkName = GameInterface.(CurTable).(CurRow).UserData.ID;
	string  descr    = GetConvertStr("perk" + perkName, "AbilityDescribe.txt");
	aref rootItems;
	int    i;
	string row, perkCond, icoGroup;
	
	SetNewGroupPicture("PERK_PICTURE", "PERK_ENABLE", perkName);
    SetFormatedText("PERK_WINDOW_CAPTION", GetConvertStr(perkName, "AbilityDescribe.txt"));
    
    if (CheckAttribute(&ChrPerksList, "list." + perkName + ".OfficerType"))
    {
        descr += "\n\n Способность будет работать и в том случае, если назначен офицер " + XI_ConvertString(ChrPerksList.list.(perkName).OfficerType)+ ", у которого она есть.";
    }
    SetFormatedText("PERK_WINDOW_TEXT", descr);
    SetVAligmentFormatedText("PERK_WINDOW_TEXT");
    
    if (GameInterface.(CurTable).(CurRow).UserData.Type == "self")
    {
        SetFormatedText("PERK_TEXT_FREE", XI_ConvertString("Personal abilities") + ": " +xi_refCharacter.perks.FreePoints_self);
		if (sti(xi_refCharacter.perks.FreePoints_self) <= 0) ok = false;
	}
	else
	{
		SetFormatedText("PERK_TEXT_FREE", XI_ConvertString("Ship abilities") + ": " + xi_refCharacter.perks.FreePoints_ship);
		if (sti(xi_refCharacter.perks.FreePoints_ship) <= 0) ok = false;
    }
	// проверка на необходимы перки -->
	if (CheckAttribute(&ChrPerksList, "list." + perkName + ".condition"))
	{
        makearef(rootItems, ChrPerksList.list.(perkName).condition);
		Table_Clear("PERK_TABLE_NEED", false, true, false);
		GameInterface.PERK_TABLE_NEED.select = 0;
		GameInterface.PERK_TABLE_NEED.hr.td1.str = "";
		GameInterface.PERK_TABLE_NEED.top = 0;
		for (i=0; i<GetAttributesNum(rootItems); i++)
    	{
    	    row = "tr" + (i+1);
    	    perkCond = GetAttributeName(GetAttributeN(rootItems, i));
    	    if (CheckCharacterPerk(xi_refCharacter, perkCond))
            {
                icoGroup = "PERK_ENABLE";
                GameInterface.PERK_TABLE_NEED.(row).td2.color = argb(255,255,255,255);
            }
            else
            {
                icoGroup = "PERK_DISABLE";
                GameInterface.PERK_TABLE_NEED.(row).td2.color = argb(255,196,196,196);
                ok = false;
            }
			GameInterface.PERK_TABLE_NEED.(row).td1.icon.group  = icoGroup;
			GameInterface.PERK_TABLE_NEED.(row).td1.icon.image  = perkCond;
			GameInterface.PERK_TABLE_NEED.(row).td1.icon.width  = 24;
		    GameInterface.PERK_TABLE_NEED.(row).td1.icon.height = 24;
		    GameInterface.PERK_TABLE_NEED.(row).td1.icon.offset = "-2, 3";

			GameInterface.PERK_TABLE_NEED.(row).td2.str = GetConvertStr(perkCond, "AbilityDescribe.txt");
			GameInterface.PERK_TABLE_NEED.(row).td2.scale = 0.68;
    	}
		Table_UpdateWindow("PERK_TABLE_NEED");
	}
	else
	{
	    showCondition = false;
	}
	// проверка на необходимы перки <--
	XI_WindowShow("PERK_WINDOW", true);
	XI_WindowDisable("PERK_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	
	if (CheckCharacterPerk(xi_refCharacter, perkName))
 	{
 	    SetNodeUsing("PERK_ON", true);
 	    ok = false; // уже есть
 	}
 	else
 	{
 	    SetNodeUsing("PERK_ON", false);
 	    SetNewGroupPicture("PERK_PICTURE", "PERK_DISABLE", perkName);
 	}
 	
    SetSelectable("PERK_ACCEPT", ok);
    SetNodeUsing("PERK_TABLE_NEED", showCondition);
	SetCurrentNode("PERK_CANCEL");
}

void ExitPerkMenu()
{
	XI_WindowShow("PERK_WINDOW", false);
	XI_WindowDisable("PERK_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("TABLE_PERKS");
}

void AcceptPerk()
{
    string  perkName = GameInterface.(CurTable).(CurRow).UserData.ID;
    if (GameInterface.(CurTable).(CurRow).UserData.Type == "self")
    {
		xi_refCharacter.perks.FreePoints_self = sti(xi_refCharacter.perks.FreePoints_self) - 1;
	}
	else
	{
		xi_refCharacter.perks.FreePoints_ship = sti(xi_refCharacter.perks.FreePoints_ship) - 1;
    }
    if (SetCharacterPerk(xi_refCharacter, perkName))
    {   // перерисуем персонажа
    	FillSkillTables();	
    }
    // перерисуем все -->
    SetFormatedText("TABSTR_1", XI_ConvertString("Personal_abilities") + " " +xi_refCharacter.perks.FreePoints_self);
    SetFormatedText("TABSTR_2", XI_ConvertString("Ship_abilities") + " " + xi_refCharacter.perks.FreePoints_ship);
    FillPerksTable(GameInterface.(CurTable).(CurRow).UserData.Type, false);
    // перерисуем все <--
	ExitPerkMenu();
}