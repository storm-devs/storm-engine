// 19.08.05 BOAL полностью переделанная РПГ и форма персонаж
int nPlayerMoney;
int nNextExperience;
int nCurrentExperience;
int nFreeSkillPoints;
int nFreeSPECIALPoints;

int nLeadership; //del
int nFencing;

bool bOnlyShow, bEditHeroPatam;
int curOfficer;
ref xi_refMainChar;
ref xi_refCharacter;
bool ok;

int oldIndex = -1;

string totalInfo;

void InitInterface_R(string iniName,ref pCharacter)
{
	int i;

    // лочим квест и карту
    bQuestCheckProcessFreeze = true;
    
	xi_refMainChar  = GetMainCharacter();
    xi_refCharacter = pCharacter;

    //xi_refMainChar.I_CHAR.Idx        = xi_refMainChar.index; // запоминалки для перевызова
    //xi_refMainChar.I_CHAR.curOfficer = 0;

    if (CheckAttribute(xi_refMainChar, "I_CHAR.curOfficer"))
    {
        curOfficer = sti(xi_refMainChar.I_CHAR.curOfficer);
    }
    else {curOfficer = 0;}
    
    // boal оптимизация скилов -->
    DelBakSkillAttr(GetMainCharacter());
    ClearCharacterExpRate(GetMainCharacter());
    //RefreshCharacterSkillExpRate(GetMainCharacter());
    // boal оптимизация скилов <--
    
    GameInterface.title = "titleCharacter";
    GameInterface.SkillChanger.current = 0;

    GameInterface.StatusLine.EXP.Max   = "1"; // Это лояльность
    GameInterface.StatusLine.EXP.Min   = "0";
    GameInterface.StatusLine.EXP.Value = "0";

    GameInterface.StatusLine.HEALTH_EXP.Max   = 54;
    GameInterface.StatusLine.HEALTH_EXP.Min   = 1;
    GameInterface.StatusLine.HEALTH_EXP.Value = makeint(xi_refMainChar.Health.HP);
    if (sti(xi_refMainChar.Health.HP) > 54 )
    {
        GameInterface.StatusLine.HEALTH_EXP.Value = 54;
    }
    
    GameInterface.StatusLine.REPUTAT_EXP.Max   = REPUTATION_MAX;
    GameInterface.StatusLine.REPUTAT_EXP.Min   = REPUTATION_MIN;
    GameInterface.StatusLine.REPUTAT_EXP.Value = makeint(xi_refMainChar.reputation);
    
	if( CheckAttribute(&InterfaceStack,"CharacterView_pos") ) GameInterface.skillchanger.current = InterfaceStack.CharacterView_pos;
    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
    
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"TEXTWINDOW",5);
    
    SetNodeUsing("StartNewGame",false);
    
    if (CheckAttribute(xi_refMainChar, "EditHeroPatam"))
	{
	    bEditHeroPatam = true;
	    DeleteAttribute(xi_refMainChar, "EditHeroPatam");
	    SetNodeUsing("StartNewGame", true);
	}
	else
	{
    	bEditHeroPatam = false; // определить тип работы
	}
	
	if (CheckAttribute(xi_refMainChar, "SystemInfo.OnlyShowCharacter"))
	{
	    bOnlyShow = true;
	    DeleteAttribute(xi_refMainChar, "SystemInfo.OnlyShowCharacter");
	}
	else
	{
    	bOnlyShow = false; // определить тип работы
	}
	if (bOnlyShow || bEditHeroPatam)
	{
        SetNodeUsing("I_SHIP_2",false);
	    SetNodeUsing("I_SHIPHOLD_2",false);
	    SetNodeUsing("I_PASSENGER_2",false);
	    SetNodeUsing("I_QUESTBOOK_2",false);
	    SetNodeUsing("I_TRADEBOOK_2",false);
	    SetNodeUsing("I_NATIONS_2",false);
	    SetNodeUsing("I_ITEMS_2",false);
	    
		SetNodeUsing("I_SHIP",false);
	    SetNodeUsing("I_SHIPHOLD",false);
	    SetNodeUsing("I_PASSENGER",false);
	    SetNodeUsing("I_QUESTBOOK",false);
	    SetNodeUsing("I_TRADEBOOK",false);
	    SetNodeUsing("I_NATIONS",false);
	    SetNodeUsing("I_ITEMS",false);
	    SetNodeUsing("I_MAPS",false);
	    SetNodeUsing("B_STATISTIC",false);
	    SetNodeUsing("Statistic_IMAGES",false);
	    SetNodeUsing("Statistic_string",false);
	    SetNodeUsing("STR_BUTTONS",false);
	}
    ok = (GetOfficersQuantity(xi_refMainChar) == 0) && (GetCompanionQuantity(xi_refMainChar) == 1) && (GetPassengersQuantity(xi_refMainChar) == 0);
	if (bOnlyShow || ok || bEditHeroPatam)
	{
		SetNodeUsing("LEFTCHANGE_CHARACTER",false);
		SetNodeUsing("RIGHTCHANGE_CHARACTER",false);
	}

	SetNodeUsing("B_SKILLUP",false);
	SetNodeUsing("B_SKILLDown",false);
	SetNodeUsing("B_ABILITIES",false);

	// boal statistic info 16.12.2003 -->
	//SetNodeUsing("B_STATISTIC",false);
	// boal statistic info 16.12.2003 <--

    CreateString(true,"CurrentExperience","",FONT_NORMAL,COLOR_NORMAL,310,309,SCRIPT_ALIGN_CENTER,0.80);
    CreateString(true,"NextExperience","",FONT_NORMAL,COLOR_NORMAL,93,244,SCRIPT_ALIGN_CENTER,1.0); //weight
    CreateString(true,"Money","",FONT_NORMAL,COLOR_MONEY,93,282,SCRIPT_ALIGN_CENTER,0.95);
    
    CreateString(true,"CharacterName","","INTERFACE_SMALL",COLOR_NORMAL,287,43,SCRIPT_ALIGN_CENTER,1.0);
    CreateString(true,"Reputation","","INTERFACE_SMALL",COLOR_NORMAL,287,67,SCRIPT_ALIGN_CENTER,1.0);
    // boal -->
    CreateString(true,"FreeAbilitiesPoints","",FONT_NORMAL,COLOR_NORMAL,665,39,SCRIPT_ALIGN_RIGHT,1.0);  // self_perk
    CreateString(true,"FreeAbilitiesPoints_ship","",FONT_NORMAL,COLOR_NORMAL,665,210,SCRIPT_ALIGN_RIGHT,1.0); //ship_perk
    
    CreateString(true,"FreeSkillPoints","",FONT_NORMAL,COLOR_NORMAL,670, 10,SCRIPT_ALIGN_RIGHT,1.0); //cheat
    CreateString(true,"FreeSPECIALPoints","",FONT_NORMAL,COLOR_NORMAL,170, 40,SCRIPT_ALIGN_RIGHT,1.0); // free point

    // boal <--
    CreateString(true,"Rank","",FONT_BOLD_NUMBERS,COLOR_NORMAL,383,267,SCRIPT_ALIGN_RIGHT,0.95);
    CreateString(true,"MaxHP","",FONT_NORMAL,COLOR_NORMAL,223,274,SCRIPT_ALIGN_LEFT,1.0);

	// авто рисовалка скилов -->
	for (i = 1; i < 6; i++) //личные
	{
	    CreateString(true,"Skill" + GetSkillNameByIdx(i),"",FONT_BOLD_NUMBERS,COLOR_NORMAL,648,66 + (i-1)*28,SCRIPT_ALIGN_RIGHT,0.85);
	    CreateString(true,"Skill" + GetSkillNameByIdx(i) + "_exp","","INTERFACE_SMALL",COLOR_NORMAL,592,69 + (i-1)*28,SCRIPT_ALIGN_RIGHT,1.0);
	    CreateString(true,"Skill" + GetSkillNameByIdx(i) + "_add","","INTERFACE_SMALL",COLOR_NORMAL,672,72 + (i-1)*28,SCRIPT_ALIGN_RIGHT,0.75);// boal 15/01/04 бонусовые очки от +1 вещей
	}
	
	for (i = 6; i < 15; i++) // корабельные
	{
	    CreateString(true,"Skill" + GetSkillNameByIdx(i),"",FONT_BOLD_NUMBERS,COLOR_NORMAL,648,236 + (i-6)*28,SCRIPT_ALIGN_RIGHT,0.85);
	    CreateString(true,"Skill" + GetSkillNameByIdx(i) + "_exp","","INTERFACE_SMALL",COLOR_NORMAL,592,240 + (i-6)*28,SCRIPT_ALIGN_RIGHT,1.0);
	    CreateString(true,"Skill" + GetSkillNameByIdx(i) + "_add","","INTERFACE_SMALL",COLOR_NORMAL,672,244 + (i-6)*28,SCRIPT_ALIGN_RIGHT,0.75);// boal 15/01/04 бонусовые очки от +1 вещей
	}

	for (i = 15; i < 22; i++) // характеристики
	{
	    CreateString(true,"SPECIAL" + GetSkillNameByIdx(i),"",FONT_BOLD_NUMBERS,COLOR_NORMAL,147,65 + (i-15)*23,SCRIPT_ALIGN_RIGHT,0.75);
	    CreateString(true,"SPECIAL" + GetSkillNameByIdx(i) + "_add","","INTERFACE_SMALL",COLOR_NORMAL,169,71 + (i-15)*23,SCRIPT_ALIGN_RIGHT,0.75);
	}
    // авто рисовалка скилов <--

    CreateString(true,"healthCur","","INTERFACE_SMALL",COLOR_NORMAL,95,323,SCRIPT_ALIGN_CENTER,1.0); // здоровье
    CreateString(true,"TitulCur","","INTERFACE_SMALL",COLOR_NORMAL,290,331,SCRIPT_ALIGN_CENTER,1.0); // звание

    if (bBettaTestMode)
    {
        CreateString(true,"healthCur1",
                     "MaxHP = "    + FloatToString(stf(xi_refMainChar.Health.maxHP), 1) +
                     " CurHP = "   + FloatToString(stf(xi_refMainChar.Health.HP), 1) +
                     " WeekDmg = " + FloatToString(stf(xi_refMainChar.Health.weekDamg), 1) +
                     " DayDmg = "  + FloatToString(stf(xi_refMainChar.Health.Damg), 1) +
                     " SaveCount = "  + xi_refMainChar.SystemInfo.SaveCount +
                     " LoadCount = "  + xi_refMainChar.SystemInfo.LoadCount,
                     "INTERFACE_ULTRASMALL",COLOR_NORMAL,10,496, SCRIPT_ALIGN_LEFT,1.0);
    }
	SetVariable();

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
    SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("frame","ProcessFrame",1);
}


/* BEGIN MOD : Stone-D 19/07/2003; Modified by Viper 31/08/2003 */
void SetNextParty(bool bRight)
{
	int i,cn,ci;
	int CmpQ = 3;
	int PrtQ = 0;
	int PsgQ = 0;
	int OffQ = 3;
	int CmpOffQ[4];
	CmpOffQ[0] = 0; //this one is crap
	CmpOffQ[1] = 0;
	CmpOffQ[2] = 0;
	CmpOffQ[3] = 0;
	ref compChar;

	for(i=1;i<4;i++) 
	{
		ci = GetCompanionIndex(xi_refMainChar,i);
		if(ci > -1) 
		{
			CmpOffQ[i] = 3;
		}
	}
	CmpOffQ[0] = CmpOffQ[1] + CmpOffQ[2] + CmpOffQ[3];

	PsgQ = GetPassengersQuantity(xi_refMainChar);
	PrtQ = CmpQ + PsgQ + CmpOffQ[0] + OffQ;
    bool StartOver = true;
    while (StartOver)
	{
		if(bRight) { curOfficer++; }
		else { curOfficer--; }

		if(curOfficer>PrtQ) curOfficer=0;
		if(curOfficer<0) curOfficer=PrtQ;

		if(curOfficer <= OffQ)
		{
			cn = GetOfficersIndex(xi_refMainChar, curOfficer);
		}
		else
		{
			if(curOfficer <= (OffQ + CmpQ + CmpOffQ[0]))
			{
				if(curOfficer <= (OffQ + CmpOffQ[1] + 1))
				{
                    ci = GetCompanionIndex(xi_refMainChar, 1);
					compChar = GetCharacter(ci);
					cn = GetOfficersIndex(compChar, curOfficer-(OffQ+1));
				}
				else
				{
					if(curOfficer <= (OffQ + CmpOffQ[1] + CmpOffQ[2] + 2))
					{
						ci = GetCompanionIndex(xi_refMainChar, 2);
						compChar = GetCharacter(ci);
						cn = GetOfficersIndex(compChar,curOfficer-(OffQ+CmpOffQ[1]+2));
					}
					else
					{
						ci = GetCompanionIndex(xi_refMainChar,3);
						compChar = GetCharacter(ci);
						cn = GetOfficersIndex(compChar,curOfficer-(OffQ+CmpOffQ[1]+CmpOffQ[2]+3));
					}
				}

				if(curOfficer == (OffQ + 1))                           cn = GetCompanionIndex(xi_refMainChar,1);
				if(curOfficer == (OffQ + CmpOffQ[1] + 2))              cn = GetCompanionIndex(xi_refMainChar,2);
				if(curOfficer == (OffQ + CmpOffQ[1] + CmpOffQ[2] + 3)) cn = GetCompanionIndex(xi_refMainChar,3);
			}
			else
			{
				cn = GetPassenger(xi_refMainChar,curOfficer-(CmpQ+OffQ+CmpOffQ[0]+1));
				if ( IsOfficer(GetCharacter(cn)) )
	            {
	                continue;
	            }
			}
		}
		if( cn<0 )
	    {
	        continue;
	    }
	    xi_refCharacter = GetCharacter(cn);
	    if(CheckAttribute(xi_refCharacter,"prisoned")) continue; // boal

	    StartOver = false;
	}

    if(curOfficer!=0)
    {
        xi_refCharacter = GetCharacter(cn);
    }
    else
    {
        xi_refCharacter = xi_refMainChar;
    }
    
    xi_refMainChar.I_CHAR.curOfficer = curOfficer; // boal fix
}

void SetVariable()
{
	int k;
	
    RefreshCharacterSkillExpRate(xi_refCharacter);
    
    nPlayerMoney = sti(xi_refCharacter.Money);
    // boal -->
    if(!CheckAttribute(xi_refCharacter, "rank_exp"))
	{
		xi_refCharacter.rank_exp = 0;
	}
    nCurrentExperience = sti(xi_refCharacter.rank_exp);
    nNextExperience = GetCharacterRankRate(xi_refCharacter);//CalculateExperienceFromRank(sti(xi_refCharacter.rank)+1);
    // boal <--
    nFreeSkillPoints    = sti(xi_refCharacter.Skill.FreeSkill);
    if(!CheckAttribute(xi_refCharacter, "Skill.FreeSPECIAL"))
	{
		xi_refCharacter.Skill.FreeSPECIAL = 0;
	}
    nFreeSPECIALPoints  = sti(xi_refCharacter.Skill.FreeSPECIAL);
    
	int nFreeAbilities = 0;
	int nFreeAbilities_ship = 0;
	if (CheckAttribute(xi_refCharacter,"perks.FreePoints_self") )
		nFreeAbilities = sti(xi_refCharacter.perks.FreePoints_self);
    if (CheckAttribute(xi_refCharacter,"perks.FreePoints_ship") )
		nFreeAbilities_ship = sti(xi_refCharacter.perks.FreePoints_ship);

    // теперь это Лояльность
    if (CheckAttribute(xi_refCharacter, "loyality"))
    {
        if (sti(xi_refCharacter.loyality) > 30)
        {
            xi_refCharacter.loyality = 31;
        }
        if (sti(xi_refCharacter.loyality) < 0)
        {
            xi_refCharacter.loyality = 0;
        }
        GameInterface.StatusLine.EXP.Max = 31;
        GameInterface.StatusLine.EXP.Min = 0;
        GameInterface.StatusLine.EXP.Value = sti(xi_refCharacter.loyality);
        //Log_TestInfo("loyality " + xi_refCharacter.id + " = " +  xi_refCharacter.loyality);
        if (bBettaTestMode)
        {
            CreateString(true, "loyality1",
                     "loyality = "    + xi_refCharacter.loyality,
                     "INTERFACE_ULTRASMALL",COLOR_NORMAL,10,473, SCRIPT_ALIGN_LEFT,1.0);
        }
    }
    else
    {
        GameInterface.StatusLine.EXP.Max = 1;
        GameInterface.StatusLine.EXP.Min = 0;
        GameInterface.StatusLine.EXP.Value = 0;
        if (bBettaTestMode)
        {
            CreateString(true, "loyality1", "", "INTERFACE_ULTRASMALL",COLOR_NORMAL,10,473, SCRIPT_ALIGN_LEFT,1.0);
        }
    }

    GameInterface.strings.CurrentExperience        = nCurrentExperience + "/" + nNextExperience;
    GameInterface.strings.NextExperience           = "" +FloatToString(GetItemsWeight(xi_refCharacter), 1) + " / "+GetMaxItemsWeight(xi_refCharacter); // boal Это теперь вес
    GameInterface.strings.Money                    = MakeMoneyShow(nPlayerMoney, MONEY_SIGN,MONEY_DELIVER);
    GameInterface.strings.CharacterName            = GetFullName(xi_refCharacter);
    GameInterface.strings.Reputation               = XI_ConvertString(GetReputationName(sti(xi_refCharacter.reputation)));
	GameInterface.strings.FreeAbilitiesPoints      = nFreeAbilities;
	GameInterface.strings.FreeAbilitiesPoints_ship = nFreeAbilities_ship;
    GameInterface.strings.Rank                     = xi_refCharacter.rank;
	GameInterface.strings.MaxHP                    = MakeInt(LAi_GetCharacterHP(xi_refCharacter)) + "/" + MakeInt(LAi_GetCharacterMaxHP(xi_refCharacter));
	if (nFreeSkillPoints > 0)
	{
        GameInterface.strings.FreeSkillPoints          = nFreeSkillPoints;
    }
    else
    {
        GameInterface.strings.FreeSkillPoints          = "";
    }
    if (nFreeSPECIALPoints > 0)
	{
        GameInterface.strings.FreeSPECIALPoints        = nFreeSPECIALPoints;
    }
    else
    {
        GameInterface.strings.FreeSPECIALPoints        = "";
    }
    CreateImage("NationTitul", "", "", 0, 0, 0, 0);
    if (xi_refCharacter.index == xi_refMainChar.index)
    {
        GameInterface.strings.healthCur = GetHealthName(xi_refCharacter);
        if (GetHealthNum(xi_refCharacter) >= GetHealthMaxNum(xi_refCharacter))
        {
            ChangeStringColor("healthCur", SetAlphaIntoColor(COLOR_MONEY, GetAlphaFromSkill(10)));
        }
        else
        {
            ChangeStringColor("healthCur", SetAlphaIntoColor(COLOR_NORMAL, GetAlphaFromSkill(makeint(GetHealthNum(xi_refCharacter)* 1.9))));
        }
        GameInterface.strings.TitulCur = "нет звания";
        ChangeStringColor("TitulCur", SetAlphaIntoColor(COLOR_NORMAL,GetAlphaFromSkill(1)));
        if (isMainCharacterPatented())
        {
            GameInterface.strings.TitulCur = GetAddress_FormTitle(sti(Items[sti(xi_refMainChar.EquipedPatentId)].Nation), sti(Items[sti(xi_refMainChar.EquipedPatentId)].TitulCur));
            ChangeStringColor("TitulCur", SetAlphaIntoColor(COLOR_NORMAL,GetAlphaFromSkill(10)));
            CreateImage("NationTitul","NATIONS",GetNationNameByType(sti(Items[sti(xi_refMainChar.EquipedPatentId)].Nation)), 204, 329, 230, 355);
        }
        SetNodeUsing("HEALTHSTAT",true);
        SetNodeUsing("HEALTHLINES",true);
        SetNodeUsing("TITLE_NAME",true);
        SetNodeUsing("HEALTH_EXP",true);
        SetNodeUsing("REPUTAT_EXP",true);
    }
    else
    {
		if (CheckAttribute(xi_refCharacter, "quest.officertype"))
		{ // офицер с типом
		    GameInterface.strings.TitulCur = xi_refCharacter.quest.officertype;
	        ChangeStringColor("TitulCur", SetAlphaIntoColor(COLOR_NORMAL,GetAlphaFromSkill(10)));
	        SetNodeUsing("TITLE_NAME", true);
		}
		else
		{
            GameInterface.strings.TitulCur = "";
            SetNodeUsing("TITLE_NAME", false);
		}
		GameInterface.strings.healthCur = "";
        //SetNodeUsing("HEALTHSTAT",false);
        //SetNodeUsing("HEALTHLINES",false);
        SetNodeUsing("HEALTH_EXP",false);
        SetNodeUsing("REPUTAT_EXP",false);
    }

	for (k = 1; k < 22; k++)
	{
		if (k <15)
		{
            SetSkillShow("Skill" + GetSkillNameByIdx(k), GetSkillValue(xi_refCharacter, SKILL_TYPE, GetSkillNameByIdx(k)));
            SetSkill_ExpShow(GetSkillNameByIdx(k), GetSkillValue(xi_refCharacter, SKILL_TYPE, GetSkillNameByIdx(k)));
		}
		else
		{
            SetSPECIALShow("SPECIAL" + GetSkillNameByIdx(k), GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GetSkillNameByIdx(k)));
            SetSPECIALDrainShow(GetSkillNameByIdx(k), GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GetSkillNameByIdx(k)));
		}
	}
    // boal <--
	SetNewPicture("FACEPICT", "interfaces\portraits\256\face_" + xi_refCharacter.FaceId + ".tga");
	SetNewPicture("TYPEPICT", ""); // boal fix
	if( xi_refCharacter.id == xi_refMainChar.id )
	{
		SetNewPicture("TYPEPICT", "");
	}
	else
	{
	// Viper - CharSheetEnh Start
		if ( IsCompanion(xi_refCharacter) ) SetNewPicture("TYPEPICT", "interfaces\IsCompanion.tga");
		if ( IsOfficer(xi_refCharacter) )
        {
            if (makeint(xi_refCharacter.AbordageMode) == 1)
            {
                SetNewPicture("TYPEPICT", "interfaces\Viper_IsOfficer.tga");
            }
            else
            {
                SetNewPicture("TYPEPICT", "interfaces\Boal_IsOfficer.tga");
            }
        }
		if ( IsOfficerCompanion(xi_refCharacter) ) SetNewPicture("TYPEPICT", "interfaces\Viper_IsOfficerCompanion.tga");
	// Viper - CharSheetEnh End
	}

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"exp",0);
	oldIndex = -1;
}

void ProcessCancelExit()
{
	if (bEditHeroPatam && nFreeSPECIALPoints > 0)
	{
	    PlaySound("interface\knock.wav");   // все распределить нужно
	}
	else
	{
		interfaceResultCommand = RC_INTERFACE_CHARACTER_EXIT;
		CharInterfaceExit(true);
	}
}

void CharInterfaceExit(bool flag)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
    DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("frame","ProcessFrame");

	WriteNewData();
	if (bEditHeroPatam)
	{
	    DoQuestCheckDelay("newMainHero_StartGame", 0.6);
	}
    EndCancelInterface(flag);
    PostEvent("StopQuestCheckProcessFreeze", 400);//boal 230804 заморозка проверки квестов
}

void WriteNewData()
{
    xi_refCharacter.Skill.FreeSkill    = nFreeSkillPoints;
    xi_refCharacter.Skill.FreeSPECIAL  = nFreeSPECIALPoints;
    // boal оптимизация скилов -->
    DelBakSkillAttr(xi_refCharacter);
    ClearCharacterExpRate(xi_refCharacter);
    RefreshCharacterSkillExpRate(xi_refCharacter);
    // boal оптимизация скилов <--
}


void IncreaseSkill()
{
	int     cIdx, iValue;
	string  sSkillName;

    cIdx = sti(GameInterface.SkillChanger.current);
	if (cIdx < 15)
	{
        if (nFreeSkillPoints>0)
        {
            sSkillName = GetSkillNameByIdx(cIdx);
    		if (GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) == 0)	{return;}
    	    if (GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) < SKILL_MAX)
    	    {
    	        iValue = AddCharacterSkill(xi_refCharacter, sSkillName, 1);
    			SetSkillShow("Skill" + sSkillName, iValue);
    			SetSkill_ExpShow(sSkillName, iValue);
    			SetNodeUsing("B_SKILLUP", iValue < SKILL_MAX);
    			SetNodeUsing("B_SKILLDown", iValue > 1);
    	    }
    	    else return;

            nFreeSkillPoints--;
            if (nFreeSkillPoints > 0)
            {
    		    GameInterface.strings.FreeSkillPoints = nFreeSkillPoints;
            }
            else
            {
    		    GameInterface.strings.FreeSkillPoints = "";
            }
    		SetExpTotalShowSkillCur();
		}
	}
	else
	{//SPECIAL
        if (nFreeSPECIALPoints>0)
        {
            sSkillName = GetSkillNameByIdx(cIdx);
    	    if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName) < GetSPECIAL_MaxHeroType(xi_refCharacter, sSkillName))
    	    {
    	        iValue = AddSPECIALValue(xi_refCharacter, sSkillName, 1);
    			SetSkillShow("SPECIAL" + sSkillName, iValue);
                SetSPECIALDrainShow(sSkillName, iValue);
    			SetNodeUsing("B_SKILLUP", iValue < GetSPECIAL_MaxHeroType(xi_refCharacter, sSkillName));
    			SetNodeUsing("B_SKILLDown", iValue > GetSPECIAL_MinHeroType(xi_refCharacter, sSkillName));
    			ClearCharacterExpRate(xi_refCharacter);
    	    }
    	    else return;

            nFreeSPECIALPoints--;
            if (nFreeSPECIALPoints > 0)
            {
    		    GameInterface.strings.FreeSPECIALPoints = nFreeSPECIALPoints;
    		}
    		else
    		{
                GameInterface.strings.FreeSPECIALPoints = "";
    		}
    		if (bEditHeroPatam)
			{
			    InitStartParam(xi_refCharacter);
			}
    		SetExpTotalShowSkillCur();
		}
	}
}

void DecreaseSkill()
{
	int     cIdx, iValue;
	string  sSkillName;

    cIdx = sti(GameInterface.SkillChanger.current);
	if (cIdx < 15)
	{
        sSkillName = GetSkillNameByIdx(cIdx);
	    if (GetSkillValue(xi_refCharacter, SKILL_TYPE, sSkillName) > 1)
	    {
	        iValue = AddCharacterSkill(xi_refCharacter, sSkillName, -1);
			SetSkillShow("Skill" + sSkillName, iValue);
			SetSkill_ExpShow(sSkillName, iValue);
			SetNodeUsing("B_SKILLUP", iValue < SKILL_MAX);
			SetNodeUsing("B_SKILLDown", iValue > 1);
	    }
	    else return;

        nFreeSkillPoints++;
        if (nFreeSkillPoints > 0)
        {
		    GameInterface.strings.FreeSkillPoints = nFreeSkillPoints;
        }
        else
        {
		    GameInterface.strings.FreeSkillPoints = "";
        }
		SetExpTotalShowSkillCur();
	}
	else
	{//SPECIAL
        sSkillName = GetSkillNameByIdx(cIdx);
	    if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, sSkillName) > GetSPECIAL_MinHeroType(xi_refCharacter, sSkillName))
	    {
	        iValue = AddSPECIALValue(xi_refCharacter, sSkillName, -1);
			SetSkillShow("SPECIAL" + sSkillName, iValue);
            SetSPECIALDrainShow(sSkillName, iValue);
			SetNodeUsing("B_SKILLUP", iValue < GetSPECIAL_MaxHeroType(xi_refCharacter, sSkillName));
			SetNodeUsing("B_SKILLDown", iValue > GetSPECIAL_MinHeroType(xi_refCharacter, sSkillName));
			ClearCharacterExpRate(xi_refCharacter);
	    }
	    else return;

        nFreeSPECIALPoints++;
        if (nFreeSPECIALPoints > 0)
        {
		    GameInterface.strings.FreeSPECIALPoints = nFreeSPECIALPoints;
		}
		else
		{
            GameInterface.strings.FreeSPECIALPoints = "";
		}
		if (bEditHeroPatam)
		{
		    InitStartParam(xi_refCharacter);
		}
		SetExpTotalShowSkillCur();
	}
}

void SetSPECIALShow(string skillName, int skillVal)
{
	GameInterface.strings.(skillName) = skillVal;
	int color = COLOR_NORMAL;
	if(skillVal==SPECIAL_MAX)	color = COLOR_MONEY;
	ChangeStringColor(skillName,SetAlphaIntoColor(color, GetAlphaFromSkill(skillVal))); // 55-255
}

void SetSkill_ExpShow(string skillName, int skillVal)
{
	string skillName_exp = skillName + "_exp";
	string strName       = "Skill" + skillName_exp;
	
    if(!CheckAttribute(xi_refCharacter, "Skill."+skillName_exp))
	{
		xi_refCharacter.Skill.(skillName_exp) = 0;
	}
	if (skillVal == SKILL_MAX || skillVal == 0)
	{
	   GameInterface.strings.(strName) = "";
	}
	else
	{
	   GameInterface.strings.(strName) = makeint(makeint(stf(xi_refCharacter.Skill.(skillName_exp)))*100 / makeint(skillVal * GetCharacterExpRate(xi_refCharacter, skillName))) + "%";
	}

	// boal 15/01/04 бонусовые очки от +1 вещей -->
	strName = "Skill" +skillName + "_add";
	DelBakSkillAttr(xi_refCharacter); // boal оптимизация скилов
	int diff = GetCharacterSkill(xi_refCharacter, skillName) - skillVal;
	if (diff == 0)
	{
	   GameInterface.strings.(strName) = "";
	}
	else
	{
	   if (diff > 0)
	   {
          GameInterface.strings.(strName) = "(+" + diff + ")";
       }
       else
       {
          GameInterface.strings.(strName) = "(" + diff + ")";
       }
	}
	// boal 15/01/04 бонусовые очки от +1 вещей <--
}

void SetSPECIALDrainShow(string skillName, int skillVal)
{
    string strName;
    // boal бонусовые очки от +1 вещей -->
	strName = "SPECIAL" +skillName + "_add";
	int diff = GetCharacterSPECIAL(xi_refCharacter, skillName) - skillVal;
	if (diff == 0)
	{
	   GameInterface.strings.(strName) = "";
	}
	else
	{
	   if (diff > 0)
	   {
          GameInterface.strings.(strName) = "(+" + diff + ")";
       }
       else
       {
          GameInterface.strings.(strName) = "(" + diff + ")";
       }
	}
	// boal 15/01/04 бонусовые очки от +1 вещей <--
}
void SetExpTotalShowSkill(string skillName, int skillVal)
{
	string skillName_exp = skillName + "_exp";
	
    if (!CheckAttribute(xi_refCharacter, "Skill."+skillName_exp))
	{
		xi_refCharacter.Skill.(skillName_exp) = 0;
	}
	if (skillVal == SKILL_MAX || skillVal == 0)
	{
       GameInterface.strings.CurrentExperience = "";
	}
	else
	{
       GameInterface.strings.CurrentExperience = sti(xi_refCharacter.Skill.(skillName_exp))+ "/" + makeint(skillVal * GetCharacterExpRate(xi_refCharacter, skillName));
	}
}
void SetExpTotalShowRank()
{
	if(!CheckAttribute(xi_refCharacter, "rank_exp"))
	{
		xi_refCharacter.rank_exp = 0;
	}
    nCurrentExperience = sti(xi_refCharacter.rank_exp);
    nNextExperience = GetCharacterRankRate(xi_refCharacter);//CalculateExperienceFromRank(sti(xi_refCharacter.rank)+1);

    //GameInterface.StatusLine.EXP.Max = nNextExperience;
    //GameInterface.StatusLine.EXP.Value = nCurrentExperience;
    GameInterface.strings.CurrentExperience = nCurrentExperience + "/" + nNextExperience;
}
void SetExpTotalShowSkillCur()
{
	if (sti(GameInterface.SkillChanger.current) < 15 && sti(GameInterface.SkillChanger.current) > 0)
	{
	    SetExpTotalShowSkill(GetSkillNameByIdx(sti(GameInterface.SkillChanger.current)),
		                     GetSkillValue(xi_refCharacter, SKILL_TYPE, GetSkillNameByIdx(sti(GameInterface.SkillChanger.current))));
	}
	else
	{
        GameInterface.strings.CurrentExperience = "";
        if (sti(GameInterface.SkillChanger.current) == 22)//личные
        {
            if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_self_exp"))
            {
               xi_refCharacter.perks.FreePoints_self_exp = 0;
            }
            GameInterface.strings.CurrentExperience = sti(xi_refCharacter.perks.FreePoints_self_exp)+ "/" + GetFreePoints_SelfRate(xi_refCharacter);
        }
        if (sti(GameInterface.SkillChanger.current) == 23)//корабельные
        {
            if (!CheckAttribute(xi_refCharacter,"perks.FreePoints_ship_exp"))
            {
               xi_refCharacter.perks.FreePoints_ship_exp = 0;
            }
            GameInterface.strings.CurrentExperience = sti(xi_refCharacter.perks.FreePoints_ship_exp)+ "/" + GetFreePoints_ShipRate(xi_refCharacter);
        }
	}
}
// boal <--

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
		case "LEFTCHANGE_CHARACTER":
			if(comName=="click")
			{
				if(!bOnlyShow)
				{
					WriteNewData();
					SetNextParty(false);
					SetVariable();
				}
			}
		break;

		case "RIGHTCHANGE_CHARACTER":
			if(comName=="click")
			{
				if(!bOnlyShow)
				{
					WriteNewData();
					SetNextParty(true);
					SetVariable();
				}
			}
		break;

		case "SKILLCHANGER":
			if(sti(GameInterface.SkillChanger.current)==0)
			{
	            SetExpTotalShowRank(); // boal
	            if(comName=="rightstep" && !bOnlyShow && !bEditHeroPatam)
				{
					WriteNewData();
					SetNextParty(true);
					SetVariable();
				}
				if(comName=="leftstep" && !bOnlyShow && !bEditHeroPatam)
				{
					WriteNewData();
					SetNextParty(false);
					SetVariable();
				}
			}
		break;

		case "B_SKILLUP":
			if(comName=="click")
			{
				SetExpTotalShowSkillCur(); // boal
	            if(sti(GameInterface.SkillChanger.current)<22) // boal
				{
					IncreaseSkill();
					oldIndex = -1;
					// boal -->
					WriteNewData();
					if (bEditHeroPatam || sti(xi_refCharacter.rank_exp) == 0)
					{
					    SetVariable();
					}
					// boal <--
				}
			}
		break;

		case "B_SKILLDOWN":
	        if(comName=="click")
			{
				SetExpTotalShowSkillCur(); // boal
	            if(sti(GameInterface.SkillChanger.current)<22) // boal
				{
					DecreaseSkill();
					oldIndex = -1;
					// boal -->
					WriteNewData();
					if (bEditHeroPatam || sti(xi_refCharacter.rank_exp) == 0)
					{
					    SetVariable();
					}
					// boal <--
				}
			}
		break;

		case "B_ABILITIES":
			if(comName=="click" && !bOnlyShow && !bEditHeroPatam)
			{
	            GameInterface.AbilityChr = xi_refCharacter.index;
	            GameInterface.AbilityType = "self";
				PostEvent("LaunchIAfterFrame",1,"sl", "AbilityView", 1);
				interfaceResultCommand = RC_INTERFACE_CHARACTER_EXIT;
				CharInterfaceExit(false);
				return;
			}
		break;

		case "B_ABILITIES_SHIP":
			if(comName=="click" && !bOnlyShow && !bEditHeroPatam)
			{
	            GameInterface.AbilityChr = xi_refCharacter.index;
	            GameInterface.AbilityType = "ship";
				PostEvent("LaunchIAfterFrame",1,"sl", "AbilityView", 1);
				interfaceResultCommand = RC_INTERFACE_CHARACTER_EXIT;
				CharInterfaceExit(false);
				return;
			}
		break;

		// boal statistic info 16.12.2003 -->
		case "B_STATISTIC":
		    SetExpTotalShowRank();
			if(comName=="click")
			{
	            GameInterface.AbilityChr = xi_refCharacter.index;
				PostEvent("LaunchIAfterFrame",1,"sl", "StatisticView", 1);
				interfaceResultCommand = RC_INTERFACE_CHARACTER_EXIT;
				CharInterfaceExit(false);
				return;
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
		case "I_SHIPHOLD_2":
			if(comName=="click")
			{
			    nodName = "I_SHIPHOLD";
			}
		break;
		case "I_PASSENGER_2":
			if(comName=="click")
			{
			    nodName = "I_PASSENGER";
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
	if (nodName == "I_CHARACTER" || nodName == "I_SHIP" || nodName == "I_SHIPHOLD" ||
	    nodName == "I_PASSENGER" || nodName == "I_QUESTBOOK" || nodName == "I_TRADEBOOK" ||
		nodName == "I_NATIONS" || nodName == "I_ITEMS" || nodName == "I_MAPS")
	{
		if(comName=="click")
		{
            CharInterfaceExit(false);
			PostEvent("LaunchIAfterFrame",1,"sl", nodName, 2);
			InterfaceStack.SelectMenu_node = nodName;
			return;
		}
	}
	// boal new menu 31.12.04 -->

}

void ProcessFrame()
{
	int  newIndex = sti(GameInterface.SkillChanger.current);
	bool ok;
	if(oldIndex!=newIndex)
	{
		SetNodeUsing("B_SKILLUP",   false);
		SetNodeUsing("B_SKILLDown", false);
		
		totalInfo = "";
		oldIndex = newIndex;
		if (newIndex>0)
		{
            SetExpTotalShowSkillCur();
            if (newIndex < 15 && nFreeSkillPoints>0 && !bOnlyShow && !bEditHeroPatam)
			{
                if (GetSkillValue(xi_refCharacter, SKILL_TYPE, GetSkillNameByIdx(newIndex)) > 1)
                {
					if (newIndex < 6)
					{
						SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 538 ,65 + 28*(newIndex-1), 568, 93+28*(newIndex-1), 0);
					}
					else
					{
						SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 538 ,235 + 28*(newIndex-6), 568,263 + 28*(newIndex-6), 0);
					}
					SetNodeUsing("B_SKILLDown", true);
                }
                if (GetSkillValue(xi_refCharacter, SKILL_TYPE, GetSkillNameByIdx(newIndex)) < SKILL_MAX)
                {
                    if (newIndex < 6)
					{
						SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 568,65 + 28*(newIndex-1),600,93 + 28*(newIndex-1), 0);
					}
					else
					{
						SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 568,235 + 28*(newIndex-6),600,263 + 28*(newIndex-6), 0);
					}
                    SetNodeUsing("B_SKILLUP",  true);
                }
			}
			ok = (nFreeSkillPoints > 0) || (nFreeSPECIALPoints > 0) || (bEditHeroPatam);
			if (newIndex > 14 && newIndex < 22 && ok && !bOnlyShow)
			{
                if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GetSkillNameByIdx(newIndex)) > GetSPECIAL_MinHeroType(xi_refCharacter, GetSkillNameByIdx(newIndex)))
                {
     				SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 75, 65 + 23*(newIndex-15), 95, 88 + 23*(newIndex-15), 0);
					SetNodeUsing("B_SKILLDown", true);
                }
                if (GetSkillValue(xi_refCharacter, SPECIAL_TYPE, GetSkillNameByIdx(newIndex)) < GetSPECIAL_MaxHeroType(xi_refCharacter, GetSkillNameByIdx(newIndex)))
                {
                    SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 95,65 + 23*(newIndex-15),115,88 + 23*(newIndex-15), 0);
                    SetNodeUsing("B_SKILLUP",  true);
                }
			}
			if (newIndex < 22)
			{
			    totalInfo = GetRPGText(GetSkillNameByIdx(newIndex));
			}
			else
			{
	            // списки перков
	            switch (newIndex)
	            {
	                case 22:
	                    totalInfo = "Личные навыки" + NewStr() + GetPerkListText(xi_refCharacter, "self");
	                break;
	                case 23:
	                    totalInfo = "Корабельные навыки" + NewStr() + GetPerkListText(xi_refCharacter, "ship");
	                break;
	            }
			}
			SetFormatedText("TEXTWINDOW",totalInfo);

			SetNodeUsing("B_ABILITIES",      newIndex==22);
			SetNodeUsing("B_ABILITIES_SHIP", newIndex==23); // 24 - Statiscic
		}
	}
}

int GetSPECIAL_MaxHeroType(ref rCharacter, string _spec)
{
	string sTemp, sType;
	if (CheckAttribute(rCharacter, "HeroParam.HeroType"))
	{
		sType = rCharacter.HeroParam.HeroType;
	    sTemp = "Max_" + _spec;
	    if (CheckAttribute(&NullCharacter, "HeroLimitSPEC." + sType + "." + sTemp))
	    {
			return sti(NullCharacter.HeroLimitSPEC.(sType).(sTemp));
	    }
	}
	return SPECIAL_MAX;
}

int GetSPECIAL_MinHeroType(ref rCharacter, string _spec)
{
	string sTemp, sType;
	if (CheckAttribute(rCharacter, "HeroParam.HeroType"))
	{
	    sType = rCharacter.HeroParam.HeroType;
	    sTemp = "Min_" + _spec;
	    if (CheckAttribute(&NullCharacter, "HeroLimitSPEC." + sType + "." + sTemp))
	    {
	        return sti(NullCharacter.HeroLimitSPEC.(sType).(sTemp));
	    }
	}
	return 3;
}
