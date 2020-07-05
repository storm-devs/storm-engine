// to_do
/*

int SelectCareerQuest(aref chr)
{
	int m = 0;

	if(CheckAttribute(chr, "CurrentQuest.Type"))
	{
		m = sti(chr.CurrentQuest.Type);
	}
	else
	{
		int iNation = sti(chr.nation);
		
		string sNation = GetNationNameByType(iNation);
		string sRank = sNation + "_career_counter";

		int iCareer = sti(pchar.(sRank));
		object CareerQuests[MAX_SHABLONS];

		for(int i = 0; i < MAX_SHABLONS; i++)
		{
			if(sti(QuestsShablons[i].l.career) != -1)
			{
				if(sti(QuestsShablons[i].l.career) <= iCareer)
				{
					CareerQuests[m].quest = i;
					m++;
				}
			}
		}

		if(m == 0)
		{
			return -1;
		}
		m = m-1;
		m = rand(m);
		m = sti(CareerQuests[m].quest);
	}

	return m;
}

void CareerInit()
{
	Career[0].rank = "Sergeant";
	Career[0].pirate = "Vagrant";
	Career[0].points = 0;
	Career[0].money = 500;

	Career[1].rank = "Lieutenant";
	Career[1].pirate = "Hunter";
	Career[1].points = 10;
	Career[1].money = 1000;

	Career[2].rank = "Captain";
	Career[2].pirate = "Sea Wolf";
	Career[2].points = 20;
	Career[2].money = 2000;

	Career[3].rank = "Commodore";
	Career[3].pirate = "Lord of the seas";
	Career[3].points = 40;
	Career[3].money = 4000;

	Career[4].rank = "Viscount";
	Career[4].pirate = "PirateHead";
	Career[4].points = 80;
	Career[4].money = 8000;
}

void UpdateCareerTime()
{
	int iNation = sti(pchar.nation);
	string patent = GetNationNameByType(iNation) + "_career_counter";
	if(CheckAttribute(pchar, patent))
	{
		pchar.(patent).time = sti(pchar.(patent).time) + 1;
	}
}

bool CheckForNationService(int iNation)
{
	int iOwnNation = sti(pchar.nation);
	string sQuest = GetNationNameByType(iNation);
	sQuest = sQuest + "_line_counter";
	
	if (!CheckAttribute(pchar, sQuest))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void PlayerChangeNation(aref chr)
{
	int iNation = sti(chr.nation);
	string sNation = GetNationNameByType(iNation);
	
	string scareer = sNation + "_line_counter";
	
	pchar.(scareer) = "0";
	pchar.nation = iNation;
	
	scareer = GetNationNameByType(iNation);
	scareer = scareer + "_career_counter";
	pchar.(scareer) = 1;

	string patent = "Patent_" + GetNationNameByType(iNation);
	GiveItem2Character(pchar, patent);
	
	for (int i = 0; i<MAX_NATIONS; i++)
	{
		if (i != iNation)
		{
			patent = "Patent_" + GetNationNameByType(i);
			if (CheckAttribute(pchar, "items."+patent))
			{
				TakeItemFromCharacter(pchar, patent);
			}
		}
	}
	
	DeleteQuestHeader("Career");
	SetQuestHeader("Career");
	string sNationGen = sNation + "Gen";
	AddQuestUserDataForTitle("Career", "sNationGen", XI_ConvertString(sNationGen));
	AddQuestRecord("Career", "1");
	sNation = "from " + sNation;
	pchar.quest.work = "0";
	AddQuestUserData("Career", "sNationGen", XI_ConvertString(sNationGen));

	SetRelationsForCareer(iNation);

	string srank = pchar.(scareer);
	pchar.quest.(srank).win_condition.l1 = "timer";
	pchar.quest.(srank).win_condition.l1.date.day = GetAddingDataDay(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.year = GetAddingDataYear(0, 0, 30);
	pchar.quest.(srank).win_condition.l1.date.hour = rand(23);
	pchar.quest.(srank).win_condition = "career_money";
	pchar.quest.(srank).function = "CareerMoney";

	GenerateHovernorQuests(chr);
	chr.quest.questflag.model = SetModelFlag(chr);
}


int ChangeCharacterCareerProgress(int iNation, int iDecr)
{
	string scareer = GetNationNameByType(iNation);
	
	scareer = scareer + "_line_counter";
	
	iDecr = iDecr  * (1.0 + (0.75 * iGameArcade));
	int iOldCareer = sti(pchar.(scareer));
	pchar.(scareer) =  iOldCareer + iDecr;
	int iNewCareer = sti(pchar.(scareer));


	if(iNewCareer < 0)
	{
		iNewCareer = 0;
	}
	if(iNewCareer > 400)
	{
		iNewCareer = 400;
	}
	
	int isCareer = 0;

	int iPoints;

	scareer = GetNationNameByType(iNation);
	scareer = scareer + "_career_counter";	

	for(int i = 0; i < MAX_CAREER_STEPS; i++)
	{
		if( i > 4)
		{
			break;
		}
		if(!CheckAttribute(&Career[i], "points"))
		{
			Career[i].points = 0;
		}
		if(iNewCareer > sti(Career[i].points))
		{
			if(sti(pchar.(scareer)) < i)
			{
				isCareer = 1;
			}
		}
	}

	if(isCareer > 0)
	{
		if(iOldCareer < iNewCareer)
		{
			//PlayerNextRank(iNation, 1);
			return 1;
		}
		else
		{
			//PlayerNextRank(iNation, -1);
			return -1;
		}
	}
	return 0;
}

int CheckForNextRank(aref chr)
{
	int inewrank;

	if(CheckAttribute(chr, "career.change"))
	{
		inewrank = sti(chr.career.change);
	}
	else
	{
		inewrank = 0;
	}
	chr.career.change = 0;
	return inewrank;
}

void PlayerNextRank(aref chr)
{
	int iNation = sti(chr.nation);

	string scareer = GetNationNameByType(iNation);
	
	scareer = scareer + "_career_counter";
	
	pchar.(scareer) = sti(pchar.(scareer)) + 1;
	
	ChangeCharacterFame(pchar, 1);
}

string SelectCurrenCareerName(int iNation)
{
	string scareer = GetNationNameByType(iNation);
	
	scareer = scareer + "_career_counter";

	int iCareer = sti(pchar.(scareer));

	string sResult = Career[iCareer].rank;

	return sResult;
}

void CareerJournal(int iNation, int iDecr)
{
	string sNation = GetNationNameByType(iNation);
	sNation = "from " + sNation;

	if(iDecr == 1)
	{
		AddQuestRecord("Career", "2");
	}
	else
	{
		AddQuestRecord("Career", "3");
	}

	string sRank = sNation + "_career_counter";

	int iCareer = sti(pchar.(sRank));

	sRank = Career[iCareer].rank;

	AddQuestUserData("Career", "sNationGen", XI_ConvertString(sNation));
	AddQuestUserData("Career", "rank", sRank);
}

void SetRelationsForCareer(int iNation)
{
	int relation;
	for (int i=0; i<MAX_NATIONS; i++)
	{
		relation = GetNationRelation(iNation, i);
		SetNationRelation2MainCharacter(i, relation);
	}
}

void SetRelationsForCareerinNewGame(int iNation)
{
	SetNationRelation2MainCharacter(iNation, RELATION_FRIEND);
	string sNation = GetNationNameByType(iNation);
	pchar.reputation.(sNation) = 51;
}

void CareerMoney(string sQuest)
{
	if (sti(pchar.nation) != PIRATE)
	{
		string sItem = "Patent_" + GetNationNameByType(sti(pchar.nation));
		if (CheckCharacterItem(pchar, sItem))
		{
			LaunchCareerMoney();
		}
	}
}

bool CheckForLineQuest(aref chr)
{
	int iNation = sti(chr.nation);

	string sQuest = GetNationNameByType(iNation);
	sQuest = sQuest + "_career_counter";
	
	if (!CheckAttribute(pchar, sQuest))
	{
		return false;
	}
	
	int iRank = sti(pchar.(sQuest));

	string sRank = "rank" + iRank+1;
	string sStep = "l";
	for(int i = 1; i < 4; i++)
	{
		sStep = "l"+i;
		if(NationsState[iNation].(sRank).(sStep) == "notdone")
		{
			return true;
		}
	}

}

string SelectQuestLink(aref chr)
{
	int iNation = sti(chr.nation);

	string sQuest = GetNationNameByType(iNation);
	sQuest = sQuest + "_career_counter";
	
	int iRank = sti(pchar.(sQuest));

	string sRank = "rank" + iRank;
	string sStep = "l";
	for(int i = 1; i < 4; i++)
	{
		sStep = "l"+i;
		if(NationsState[iNation].(sRank).(sStep) == "notdone")
		{
			return NationsState[iNation].(sRank).(sStep).dialoglink;
		}
	}
}
*/
