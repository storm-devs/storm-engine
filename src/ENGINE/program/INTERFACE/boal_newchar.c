// Начало игры
// boal 23.01.2004 Выбор геро
string totalInfo;
int idLngFile = -1;
int heroQty = 0;

void InitInterface(string iniName)
{
	int i;
    GameInterface.title = "titleCharacterSelect";

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
    
    if (!CheckAttribute(&NullCharacter, "HeroParam.HeroType") || !CheckAttribute(&NullCharacter, "HeroParam.nation"))
    {   // иначе уже загружен и выбран ГГ, смотрим настройки и идем обратно
		startHeroType = GetHeroNum();
	}
	
	CreateString(true,"CharacterName","",FONT_NORMAL,COLOR_NORMAL,156,68,SCRIPT_ALIGN_CENTER,1.0);
	CreateString(true,"CharacterType","",FONT_NORMAL,COLOR_NORMAL,158,370,SCRIPT_ALIGN_CENTER,1.0);
	
	CreateString(true,"Level_ComplexityName", XI_ConvertString("m_Complexity"), "INTERFACE_ULTRASMALL", COLOR_NORMAL,330,40,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"Level_ComplexityType", GetLevelComplexity(Level_Complexity),"INTERFACE_ULTRASMALL",COLOR_NORMAL,430,40,SCRIPT_ALIGN_LEFT,0.8);
	
    heroQty   = sti(GetNewMainCharacterParam("hero_qty"));
    
    if (CheckAttribute(&NullCharacter, "HeroParam.HeroType") && CheckAttribute(&NullCharacter, "HeroParam.nation"))
    {
		SetVariableFromLast();
	}
	else
	{
	    SetVariable();
	}
	
	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
    SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
}

void ProcessStartNewGameExit()
{
    IDoExit(RC_INTERFACE_DO_OPTIONS);
    
    SetEventHandler(EVENT_END_VIDEO,"ISTART_SelCharacter",0);
	PostEvent(EVENT_END_VIDEO,0);
    EndCancelInterface(true);
}

void ProcessSetGameOptionsExit()
{
    NullCharacter.ReturnBackToNewGame = true;
    
	IDoExit(RC_INTERFACE_DO_OPTIONS);
    EndCancelInterface(true);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);
	
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		EndCancelInterface(false);
	} else {
		EndCancelInterface(true);
	}
	if( !CheckAttribute(&InterfaceStates,"InstantExit") || sti(InterfaceStates.InstantExit)==false )
	{
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
    DelEventHandler("InterfaceBreak","ProcessCancelExit");
    DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");

	interfaceResultCommand = exitCode;
}

void ProcessCommandExecuteLeft()
{
    if (startHeroType > 1)
    {
       startHeroType--;
    }
    else
    {
       startHeroType = heroQty;
    }
    SetVariable();
}

void ProcessCommandExecuteRight()
{
    if (startHeroType < heroQty)
    {
       startHeroType++;
    }
    else
    {
       startHeroType = 1;
    }
    SetVariable();
}

void ProcessCommandExecuteTypeLeft()
{
    idLngFile = LanguageOpenFile("HeroDescribe.txt");

    NullCharacter.HeroParam.HeroType = GetCharacterType(NullCharacter.HeroParam.HeroType, -1);

    GameInterface.strings.CharacterType = XI_ConvertString(NullCharacter.HeroParam.HeroType);

    totalInfo = LanguageConvertString(idLngFile, NullCharacter.HeroParam.HeroType);
    SetFormatedText("INFO_TEXT", totalInfo);
    LanguageCloseFile(idLngFile);
}

void ProcessCommandExecuteTypeRight()
{
    idLngFile = LanguageOpenFile("HeroDescribe.txt");

    NullCharacter.HeroParam.HeroType = GetCharacterType(NullCharacter.HeroParam.HeroType, 1);

    GameInterface.strings.CharacterType = XI_ConvertString(NullCharacter.HeroParam.HeroType);

    totalInfo = LanguageConvertString(idLngFile, NullCharacter.HeroParam.HeroType);
    SetFormatedText("INFO_TEXT", totalInfo);
    LanguageCloseFile(idLngFile);
}

void ProcessCommandExecuteNationLeft()
{
	NullCharacter.HeroParam.nation = sti(NullCharacter.HeroParam.nation) - 1;
	
    if (sti(NullCharacter.HeroParam.nation) < 0)
    {
        NullCharacter.HeroParam.nation = 4;
    }
    
    SetNewPicture("CAPTIVE_FLAG", "interfaces\flag_" + NationShortName(sti(NullCharacter.HeroParam.nation))+ ".tga");
}

void ProcessCommandExecuteNationRight()
{
    NullCharacter.HeroParam.nation = sti(NullCharacter.HeroParam.nation) + 1;

    if (sti(NullCharacter.HeroParam.nation) > 4)
    {
        NullCharacter.HeroParam.nation = 0;
    }

    SetNewPicture("CAPTIVE_FLAG", "interfaces\flag_" + NationShortName(sti(NullCharacter.HeroParam.nation))+ ".tga");
}
void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
    	case "RUNGAME": // 1 hero
    		if(comName=="activate" || comName=="click")
    		{
                 SaveHeroNum();
				 ProcessStartNewGameExit();
    		}
    		if(comName=="leftstep")
    		{
    		    ProcessCommandExecuteLeft();
    		}
    		if(comName=="rightstep")
    		{
                ProcessCommandExecuteRight();
    		}
    	break;
    	
    	case "B_OPTIONS":
    		if(comName=="activate" || comName=="click")
    		{
				 ProcessSetGameOptionsExit();
    		}
    		if(comName=="leftstep")
    		{
    		    ProcessCommandExecuteLeft();
    		}
    		if(comName=="rightstep")
    		{
                ProcessCommandExecuteRight();
    		}
    	break;
    	
    	case "LEFTCHANGE_CHARACTER":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteLeft();
    		}
    	break;

    	case "RIGHTCHANGE_CHARACTER":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteRight();
    		}
    	break;
    	
    	case "LEFTCHANGE_TYPE":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteTypeLeft();
    		}
    	break;

    	case "RIGHTCHANGE_TYPE":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteTypeRight();
    		}
    	break;
    	
    	case "LEFTCHANGE_NATION":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteNationLeft();
    		}
    	break;

    	case "RIGHTCHANGE_NATION":
    		if(comName=="click")
    		{
    		    ProcessCommandExecuteNationRight();
    		}
    	break;
	}
}

void SetVariable()
{
    idLngFile = LanguageOpenFile("HeroDescribe.txt");
    GameInterface.strings.CharacterName = GetNewMainCharacterName();
    
    NullCharacter.HeroParam.HeroType = GetNewMainCharacterType(startHeroType);
    
    GameInterface.strings.CharacterType = XI_ConvertString(NullCharacter.HeroParam.HeroType);
    
    SetNewPicture("FACEPICT", "interfaces\portraits\256\face_" + GetNewMainCharacterFace() + ".tga");
    totalInfo = LanguageConvertString(idLngFile, "hero_" + startHeroType);
    SetFormatedText("INFO_TEXT", totalInfo);
    LanguageCloseFile(idLngFile);
    
    NullCharacter.HeroParam.nation = GetNewMainCharacterNation(startHeroType);
    
    SetNewPicture("CAPTIVE_FLAG", "interfaces\flag_" + NationShortName(sti(NullCharacter.HeroParam.nation))+ ".tga");
}

void SetVariableFromLast()
{
    idLngFile = LanguageOpenFile("HeroDescribe.txt");
    GameInterface.strings.CharacterName = GetNewMainCharacterName();

    GameInterface.strings.CharacterType = XI_ConvertString(NullCharacter.HeroParam.HeroType);

    SetNewPicture("FACEPICT", "interfaces\portraits\256\face_" + GetNewMainCharacterFace() + ".tga");
    totalInfo = LanguageConvertString(idLngFile, "hero_" + startHeroType);
    SetFormatedText("INFO_TEXT", totalInfo);
    LanguageCloseFile(idLngFile);

    SetNewPicture("CAPTIVE_FLAG", "interfaces\flag_" + NationShortName(sti(NullCharacter.HeroParam.nation))+ ".tga");
}


string GetCharacterType(string type, int direction)
{
    switch (type)
	{
        case "Merchant":
            if (direction > 0)
            {
    	    	type = "Master";
    	    }
    	    else
    	    {
    	        type = "SecretAgent";
    	    }
	    break;

	    case "Master":
         	if (direction > 0)
            {
    	    	type = "Corsair";
    	    }
    	    else
    	    {
    	        type = "Merchant";
    	    }
	    break;

	    case "Corsair":
            if (direction > 0)
            {
    	    	type = "Adventurer";
    	    }
    	    else
    	    {
    	        type = "Master";
    	    };
	    break;

	    case "Adventurer":
            if (direction > 0)
            {
    	    	type = "Inquisitor";
    	    }
    	    else
    	    {
    	        type = "Corsair";
    	    }
	    break;

	    case "Inquisitor":
            if (direction > 0)
            {
    	    	type = "SecretAgent";
    	    }
    	    else
    	    {
    	        type = "Adventurer";
    	    }
	    break;
	    
	    case "SecretAgent":
            if (direction > 0)
            {
    	    	type = "Merchant";
    	    }
    	    else
    	    {
    	        type = "Inquisitor";
    	    }
	    break;
	}

	return type;
}