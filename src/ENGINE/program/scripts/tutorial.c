
#define MAX_TUTORIALS				11

#define SHIP_SAILING_TUTORIAL		0
#define FENCING_TUTORIAL			1
#define	DELIVERY_TRADE_TUTORIAL		2
#define	SHIP_ESCORT_TUTORIAL		3
#define	SLAVES_TUTORIAL				4
#define	DELIVERY_PASSENGER_TUTORIAL	5
#define	TRADE_OPERATIONS_TUTORIAL	6
#define	SINK_SHIP_TUTORIAL			7
#define	HIRE_CREW_TUTORIAL			8
#define	OFFICERS_TUTORIAL			9
#define	SHIP_UPGRADE_TUTORIAL		10

object TutorialsList[MAX_TUTORIALS];

void InitTutorials()
{
	//0 управление кораблем
	//1 фехтование
	//2 квест на доставку груза
	//3 квест на сопровождение корабля
	//4 квест на доставку рабов
	//5 квест на доставку пассажира
	//6 торговля (импорт/экспорт)
	//7 квест на потопление судна
	//8 найм команды
	//9 найм офицеров
	//10 апгрейды корабля

	TutorialsList[SHIP_SAILING_TUTORIAL].completed = 0;
	TutorialsList[SHIP_SAILING_TUTORIAL].link = "ShipSailingTutorial";

	TutorialsList[FENCING_TUTORIAL].completed = 0;
	TutorialsList[FENCING_TUTORIAL].link = "FencingTutorial";
	
	TutorialsList[DELIVERY_TRADE_TUTORIAL].completed = 0;
	TutorialsList[DELIVERY_TRADE_TUTORIAL].link = "DeliveryTradeTutorial";

	TutorialsList[SHIP_ESCORT_TUTORIAL].completed = 0;
	TutorialsList[SHIP_ESCORT_TUTORIAL].link = "ShipEscortTutorial";

	TutorialsList[SLAVES_TUTORIAL].completed = 0;
	TutorialsList[SLAVES_TUTORIAL].link = "SlavesTutorial";

	TutorialsList[DELIVERY_PASSENGER_TUTORIAL].completed = 0;
	TutorialsList[DELIVERY_PASSENGER_TUTORIAL].link = "DeliveryPassengerTutorial";

	TutorialsList[TRADE_OPERATIONS_TUTORIAL].completed = 0;
	TutorialsList[TRADE_OPERATIONS_TUTORIAL].link = "TradeOperationsTutorial";

	TutorialsList[SINK_SHIP_TUTORIAL].completed = 0;
	TutorialsList[SINK_SHIP_TUTORIAL].link = "SinkShipTutorial";

	TutorialsList[HIRE_CREW_TUTORIAL].completed = 0;
	TutorialsList[HIRE_CREW_TUTORIAL].link = "HireCrewTutorial";

	TutorialsList[OFFICERS_TUTORIAL].completed = 0;
	TutorialsList[OFFICERS_TUTORIAL].link = "OfficersTutorial";

	TutorialsList[SHIP_UPGRADE_TUTORIAL].completed = 0;
	TutorialsList[SHIP_UPGRADE_TUTORIAL].link = "ShipUpgradeTutorial";
}

void EmptySelectedTutorials()
{
	for(int i = 0; i < MAX_TUTORIALS; i++)
	{
		if(CheckAttribute(&TutorialsList[i], "selected"))
		{
			DeleteAttribute(&TutorialsList[i], "selected");
		}
	}
}

string SelectTutorial()
{
	string sFirtsNode = "First Time";

	object RandTut[MAX_TUTORIALS];
	int m = 0;

	for(int i = 0; i < MAX_TUTORIALS; i++)
	{
		if(sti(TutorialsList[i].completed) ==  0 && !CheckAttribute(&TutorialsList[i], "selected"))
		{
			RandTut[m].link = TutorialsList[i].link;
			RandTut[m].turorial = i;
			m++;
		}
	}

	if(m != 0)
	{
		m = rand(m - 1);
		TutorialsList[sti(RandTut[m].turorial)].selected = 1;
		sFirtsNode = RandTut[m].link;
	}

	return sFirtsNode;

}

void CompleteTutorial(aref chr)
{
	string sQuest = "";
	for(int i = 0; i < MAX_TUTORIALS; i++)
	{
		if(TutorialsList[i].link == chr.tutorial.link)
		{
			TutorialsList[i].completed = 1;
			sQuest = TutorialsList[i].link;
			//SetQuestHeader(sQuest);
			SetQuestHeaderBottom(sQuest);
			SetQuestHeaderColor(sQuest, argb(255,192,192,255));
			AddQuestRecord(sQuest, "1");
			DeleteAttribute(chr, "tutorial");
			AddPartyExp(pchar, 150);
			chr.quest.questflag.model = "";
			DeleteAttribute(chr, "quest.questflag");
			break;
		}
	}
}

void CompleteSmugglerTutorial(aref chr)
{
	string sQuest = "ContrabandTutorial";
	SetQuestHeader(sQuest);
	SetQuestHeaderColor(sQuest, argb(255,192,192,255));
	AddQuestRecord(sQuest, "1");
	DeleteAttribute(chr, "tutorial");
	AddPartyExp(pchar, 150);
	chr.quest.questflag.model = "";
	DeleteAttribute(chr, "quest.questflag");

	if(CheckForContraband(chr) == true)
	{
		chr.Dialog.CurrentNode = "Smuggler_offer";
	}
	else
	{
		chr.Dialog.CurrentNode = "Smuggler_nothing";
	}
	pchar.smuggler_tutorial_complete = 1;

	if(!CheckAttribute(pchar, "isSmuggPossible"))
	{
		pchar.isSmuggPossible = 1;
	}

}