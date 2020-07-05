
//#define MAX_CHARACTER_SKILL	10
int nCurScrollNum;
ref xi_refCharacter;

int nLeadership;
int nFencing;
int nGun;
int nSailing;
int nAccuracy;
int nCannons;
int nGrappling;
int nRepair;
int nDefence;
int nCommerce;
int nSneak;


void InitInterface(string iniName)
{
	locCameraSleep(true);
	SetTimeScale(0.0);
	EngineLayersOffOn(true);

	GameInterface.title = "titleHireOfficer";
	
	FillScroll();
	
	int iCharacter = GameInterface.PASSENGERSLIST.pic1.character;
	xi_refCharacter = &characters[iCharacter];
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	SetVariable();

	CreateString(true,"SkillLeadership","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,147+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillSailing","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,182+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillFencing","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,216+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillGun","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,250+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillCannons","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,284+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillAccuracy","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,318+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillGrappling","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,352+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillDefence","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,386+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillRepair","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,420+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillCommerce","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,454+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillSneak","",FONT_BOLD_NUMBERS,COLOR_NORMAL,372+70,488+60,SCRIPT_ALIGN_RIGHT,0.7);
		
	CreateString(true,"SkillLeadership2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,147+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillSailing2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,182+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillFencing2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,216+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillGun2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,250+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillCannons2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,284+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillAccuracy2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,318+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillGrappling2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,352+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillDefence2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,386+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillRepair2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,420+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillCommerce2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,454+60,SCRIPT_ALIGN_RIGHT,0.7);
	CreateString(true,"SkillSneak2","",FONT_BOLD_NUMBERS,COLOR_NORMAL,408+70,488+60,SCRIPT_ALIGN_RIGHT,0.7);
	
	CreateString(true,"SkillLeadershipT",XI_ConvertString("Leadership"),FONT_NORMAL,COLOR_NORMAL,268+70,149+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillSailingT",XI_ConvertString("Sailing"),FONT_NORMAL,COLOR_NORMAL,268+70,183+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillFencingT",XI_ConvertString("Fencing"),FONT_NORMAL,COLOR_NORMAL,268+70,217+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillGunT",XI_ConvertString("Gun"),FONT_NORMAL,COLOR_NORMAL,268+70,251+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillCannonsT",XI_ConvertString("Cannons"),FONT_NORMAL,COLOR_NORMAL,268+70,285+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillAccuracyT",XI_ConvertString("Accuracy"),FONT_NORMAL,COLOR_NORMAL,268+70,319+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillGrapplingT",XI_ConvertString("Grappling"),FONT_NORMAL,COLOR_NORMAL,268+70,353+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillDefenceT",XI_ConvertString("Defence"),FONT_NORMAL,COLOR_NORMAL,268+70,387+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillRepairT",XI_ConvertString("Repair"),FONT_NORMAL,COLOR_NORMAL,268+70,421+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillCommerceT",XI_ConvertString("Commerce"),FONT_NORMAL,COLOR_NORMAL,268+70,455+60,SCRIPT_ALIGN_LEFT,0.8);
	CreateString(true,"SkillSneakT",XI_ConvertString("Sneak"),FONT_NORMAL,COLOR_NORMAL,268+70,489+60,SCRIPT_ALIGN_LEFT,0.8);
	

	SetpcharVariable();
	
	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); 
	SetEventHandler("exitCancel","ProcessCancelExit",0); 
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0); 
	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("HireOfficer","HireOfficer",0); 


	CreateString(true,"FreeSkillPointsText",XI_ConvertString("Free Skill Points"),FONT_CAPTION,COLOR_NORMAL,303,176,SCRIPT_ALIGN_LEFT,1.0);
	CreateString(true,"CharacterInfoText",XI_ConvertString("HireHistory"),FONT_CAPTION,COLOR_NORMAL,511,176,SCRIPT_ALIGN_LEFT,1.0);

	string sText;
	sText = " " + XI_ConvertString("Level");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("hitpoints");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("current experience");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("next rank");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Fame");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("money quantity");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Nation");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Salary");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Bounty");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Loyality");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);


	SetFormatedText("MAIN_CAPTION",XI_ConvertString("Hire Officer"));
	//SetFormatedText("MAIN_WINDOW_CAPTION",XI_ConvertString("Hire Officer"));
}

void ProcessBreakExit()
{
	IDoExit(RC_INTERFACE_HIRE_OFFICER_EXIT);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_HIRE_OFFICER_EXIT);
}

void IDoExit(int exitCode)
{
	SetTimeScale(1.0);
	locCameraSleep(false);

	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("HireOfficer","HireOfficer"); 

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
	/*
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	case "B_OK":
		if(comName=="activate" || comName=="click")
		{
			PostEvent("evntDoPostExit",1,"l",RC_INTERFACE_HIRE_OFFICER_EXIT);
		}
	break;
	}
	*/
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	
}

void FillScroll()
{
	int i;
	string faceName;
	string attributeName;
	string PsgAttrName;
	int _curCharIdx;
	ref _refCurChar;
	aref pRef2;
	
	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;
	
	int iBeginOfficer = 0;
	int nListSize = GetHireOfficersQuantity(&iBeginOfficer);
	
	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ListSize = nListSize + 2;
	
	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";
	
	FillFaceList("PASSENGERSLIST.ImagesGroup", pchar, 3); // officers for hire
	
	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";
	
	int m = 0;
	
	for(i=0; i<nListSize; i++)
	{
		_curCharIdx = FindOfficerForFill(i);
	
		if(_curCharIdx!=-1)
		{
			attributeName = "pic" + (m+1);
			PsgAttrName = "character";
			makearef(pRef2,GameInterface.PASSENGERSLIST.(attributeName));
			GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
			GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
			GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
			m++;
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m;
	//SendMessage(&GameInterface,"lsl", MSG_INTERFACE_SCROLL_CHANGE, "PASSENGERSLIST", 0);
}


void ProcessFrame()
{	
	if(sti(GameInterface.PASSENGERSLIST.current)!=nCurScrollNum)
   {
		nCurScrollNum = sti(GameInterface.PASSENGERSLIST.current);
		string attributeName = attributeName = "pic" + (nCurScrollNum+1);
		int iCharacter = GameInterface.PASSENGERSLIST.(attributeName).character;
		xi_refCharacter = &characters[iCharacter];
		SetVariable();
   }
}

void SetVariable()
{
	string sPlayerName = xi_refCharacter.name + " " + xi_refCharacter.lastname;
	
	int nPlayerMoney = sti(xi_refCharacter.Money);
	int nCurrentExperience = sti(xi_refCharacter.Experience);
	int nNextExperience = CalculateExperienceFromRank(sti(xi_refCharacter.rank)+1);
	
	SetSkills();
	
  
	SetFormatedText("CHARACTER_NAME",sPlayerName);

	SetSkillShow("SkillLeadership",nLeadership);
	SetSkillShow("SkillFencing",nFencing);
	SetSkillShow("SkillGun",nGun);
	SetSkillShow("SkillSailing",nSailing);
	SetSkillShow("SkillAccuracy",nAccuracy);
	SetSkillShow("SkillCannons",nCannons);
	SetSkillShow("SkillGrappling",nGrappling);
	SetSkillShow("SkillRepair",nRepair);
	SetSkillShow("SkillDefence",nDefence);
	SetSkillShow("SkillCommerce",nCommerce);
	SetSkillShow("SkillSneak",nSneak);

	SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\256\face_" + xi_refCharacter.FaceId + ".tga");
	
	//SetFormatedText("INFO",Bio[sti(xi_refCharacter.bio)].text);
	

	//SetFormatedText("INFO_TEXT1", XI_ConvertString("OfficerPrice")+ xi_refCharacter.quest.OfficerPrice);	

	//////////GameInterface.strings.OfficerPrice = sti(xi_refCharacter.quest.OfficerPrice);

	if (sti(pchar.money) >= sti(xi_refCharacter.quest.OfficerPrice))
	{
		SetSelectable("B_OK", true);
	}
	else
	{
		SetSelectable("B_OK", false);
	}

	string sText;

	sText = xi_refCharacter.rank;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,0,&sText);

	sText = sti(LAi_GetCharacterHP(xi_refCharacter)) + "/" + sti(LAi_GetCharacterMaxHP(xi_refCharacter));
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,1,&sText);

	sText = nCurrentExperience;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,2,&sText);

	sText = nNextExperience;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,3,&sText);

	sText = XI_ConvertString(GetFameName(sti(xi_refCharacter.fame)));
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,4,&sText);

	sText = nPlayerMoney;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,5,&sText);

	sText = XI_ConvertString(GetNationNameByType(sti(xi_refCharacter.Nation)));
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,6,&sText);


	sText = xi_refCharacter.quest.OfficerPrice);
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,7,&sText);



	if(checkAttribute(xi_refCharacter, "Loyality"))
	{
		sText = XI_ConvertString(GetMoraleName(sti(xi_refCharacter.loyality)));
		sText = sText + " ";
		SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,9,&sText);
	}
	else
	{
		sText = XI_ConvertString(GetMoraleName(99));
		sText = sText + " ";
		SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,9,&sText);
	}

	sText = " " + XI_ConvertString("Level");
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 10,0,&sText);

}


void SetSkills()
{
	nLeadership = sti(xi_refCharacter.Skill.Leadership);
	nFencing = sti(xi_refCharacter.Skill.Fencing);
	nGun = sti(xi_refCharacter.Skill.Gun);	
	nSailing = sti(xi_refCharacter.Skill.Sailing);	
	nAccuracy = sti(xi_refCharacter.Skill.Accuracy);
	nCannons = sti(xi_refCharacter.Skill.Cannons);
	nGrappling = sti(xi_refCharacter.Skill.Grappling);
	nRepair = sti(xi_refCharacter.Skill.Repair);
	nDefence = sti(xi_refCharacter.Skill.Defence);
	nCommerce = sti(xi_refCharacter.Skill.Commerce);
	nSneak = sti(xi_refCharacter.Skill.Sneak);
}

void SetpcharVariable()
{
	
	SetSkillShow("SkillLeadership2",sti(pchar.skill.temp.leadership));
	SetSkillShow("SkillFencing2",sti(pchar.skill.temp.fencing));
	SetSkillShow("SkillGun2",sti(pchar.skill.temp.gun));
	SetSkillShow("SkillSailing2",sti(pchar.skill.temp.sailing));
	SetSkillShow("SkillAccuracy2",sti(pchar.skill.temp.accuracy));
	SetSkillShow("SkillCannons2",sti(pchar.skill.temp.cannons));
	SetSkillShow("SkillGrappling2",sti(pchar.skill.temp.grappling));
	SetSkillShow("SkillRepair2",sti(pchar.skill.temp.repair));
	SetSkillShow("SkillDefence2",sti(pchar.skill.temp.defence));
	SetSkillShow("SkillCommerce2",sti(pchar.skill.temp.commerce));
	SetSkillShow("SkillSneak2",sti(pchar.skill.temp.sneak));
	

	SetFormatedText("INFO_TEXT", XI_ConvertString("OurMoney")+ pchar.money);	

	//GameInterface.strings.OurMoney = sti(pchar.money);
}

void HireOfficer()
{
	
	int iMoney = sti(xi_refCharacter.quest.OfficerPrice);
	AddPassenger(pchar, xi_refCharacter, 0);
	
	officers[FindOfficerStek(xi_refCharacter.id)].hired = "1";
	int iNation = sti(xi_refCharacter.nation);
	xi_refCharacter.loyality = GetCharacterReputation(pchar, iNation);
	xi_refCharacter.isofficer = 1;
	
	//DelFromScroll("PASSENGERSLIST",nCurScrollNum);
	//nCurScrollNum = -1;
	int iBeginOfficer = 0;
	int nListSize = GetRealHireOfficersQuantity(&iBeginOfficer);
	
	if(nListSize == 0)
	{
		ProcessCancelExit();
	}
	else
	{
		FillScroll();
		SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
		SetVariable();
		SetpcharVariable();
	}
	
	AddMoneyToCharacter(pchar, -iMoney);
}