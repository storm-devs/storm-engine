
#define DELIVERY_TRADE_QUEST		0
#define DELIVERY_PASSENGER_QUEST	1
#define SINK_SHIP_QUEST				2
#define ESCORT_SHIP_QUEST			3
#define DELIVERY_ITEM_QUEST			4
#define CAPTURE_FORT_QUEST			5
#define HUNT_FOR_SLAVES_QUEST		6
#define KILL_CHARACTER_QUEST		7
#define TALK_WITH_CHARACTER_QUEST	8
#define LOOK_AT_SOMETHING_QUEST		9


object QuestsShablons[MAX_SHABLONS];
object Quests[MAX_QUESTS];

void InitQuestsShablons()
{
	QuestsShablons[DELIVERY_TRADE_QUEST].id = "DeliveryTrade";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.goods = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.goods.quantity = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.money = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.target_npc = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.parent_npc = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.isdelayed = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.taken = "-1";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.time = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.logbook = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.iscontinue = "0";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.target_colony = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.parent_colony = "";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.contribution = "0";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.swapped = "0";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.ishovernor = "0";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.careerreward = "1";
	QuestsShablons[DELIVERY_TRADE_QUEST].l.career = "1";

	QuestsShablons[DELIVERY_PASSENGER_QUEST].id = "DeliveryPassenger";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.passenger = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.money = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.isdelayed = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.parent_npc = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.taken = "-1";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.time = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.logbook = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.iscontinue = "0";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.target_colony = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.parent_colony = "";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.contribution = "0";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.swapped = "0";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.ishovernor = "0";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.isfight = "0";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.careerreward = "1";
	QuestsShablons[DELIVERY_PASSENGER_QUEST].l.career = "1";

	QuestsShablons[SINK_SHIP_QUEST].id = "SinkShip";
	QuestsShablons[SINK_SHIP_QUEST].l.shipchar = "";
	QuestsShablons[SINK_SHIP_QUEST].l.capturechar = "";
	QuestsShablons[SINK_SHIP_QUEST].l.item = "";
	QuestsShablons[SINK_SHIP_QUEST].l.stepsquantity = "";
	QuestsShablons[SINK_SHIP_QUEST].l.encounters = "";
	QuestsShablons[SINK_SHIP_QUEST].l.money = "";
	QuestsShablons[SINK_SHIP_QUEST].l.parent_npc = "";
	QuestsShablons[SINK_SHIP_QUEST].l.taken = "-1";
	QuestsShablons[SINK_SHIP_QUEST].l.time = "";
	QuestsShablons[SINK_SHIP_QUEST].l.logbook = "";
	QuestsShablons[SINK_SHIP_QUEST].l.iscontinue = "0";
	QuestsShablons[SINK_SHIP_QUEST].l.target_colony = "";
	QuestsShablons[SINK_SHIP_QUEST].l.parent_colony = "";
	QuestsShablons[SINK_SHIP_QUEST].l.contribution = "0";
	QuestsShablons[SINK_SHIP_QUEST].l.ishovernor = "0";
	QuestsShablons[SINK_SHIP_QUEST].l.isfight = "0";
	QuestsShablons[SINK_SHIP_QUEST].l.careerreward = "1";
	QuestsShablons[SINK_SHIP_QUEST].l.career = "2";

	QuestsShablons[ESCORT_SHIP_QUEST].id = "EscortShip";
	QuestsShablons[ESCORT_SHIP_QUEST].l.shipchar = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.isnewship = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.encounters = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.money = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.parent_npc = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.taken = "-1";
	QuestsShablons[ESCORT_SHIP_QUEST].l.time = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.logbook = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.iscontinue = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.target_colony = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.parent_colony = "";
	QuestsShablons[ESCORT_SHIP_QUEST].l.contribution = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.isdelayed = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.swapped = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.ishovernor = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.isfight = "0";
	QuestsShablons[ESCORT_SHIP_QUEST].l.careerreward = "1";
	QuestsShablons[ESCORT_SHIP_QUEST].l.career = "1";

	QuestsShablons[DELIVERY_ITEM_QUEST].id = "DeliveryItem";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.item = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.money = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.isdelayed = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.target_npc = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.parent_npc = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.taken = "-1";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.time = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.logbook = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.iscontinue = "0";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.target_colony = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.parent_colony = "";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.contribution = "0";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.swapped = "0";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.ishovernor = "0";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.isfight = "0";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.careerreward = "1";
	QuestsShablons[DELIVERY_ITEM_QUEST].l.career = "1";

	QuestsShablons[CAPTURE_FORT_QUEST].id = "Fort";
	QuestsShablons[CAPTURE_FORT_QUEST].l.fortchar = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.encounters = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.money = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.parent_npc = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.taken = "-1";
	QuestsShablons[CAPTURE_FORT_QUEST].l.time = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.logbook = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.iscontinue = "0";
	QuestsShablons[CAPTURE_FORT_QUEST].l.target_colony = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.parent_colony = "";
	QuestsShablons[CAPTURE_FORT_QUEST].l.contribution = "0";
	QuestsShablons[CAPTURE_FORT_QUEST].l.ishovernor = "0";
	QuestsShablons[CAPTURE_FORT_QUEST].l.isfight = "0";
	QuestsShablons[CAPTURE_FORT_QUEST].l.careerreward = "5";
	QuestsShablons[CAPTURE_FORT_QUEST].l.career = "3";

	QuestsShablons[HUNT_FOR_SLAVES_QUEST].id = "HuntForSlaves";
	QuestsShablons[HUNT_FOR_SLAVES_QUEST].l.slaves = "";
	QuestsShablons[HUNT_FOR_SLAVES_QUEST].l.money = "";
	QuestsShablons[HUNT_FOR_SLAVES_QUEST].l.target_colony = "";
	QuestsShablons[HUNT_FOR_SLAVES_QUEST].l.parent_npc = "";
	QuestsShablons[HUNT_FOR_SLAVES_QUEST].l.career = "-1";

	QuestsShablons[KILL_CHARACTER_QUEST].id = "KillCharacter";
	QuestsShablons[KILL_CHARACTER_QUEST].l.victim = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.money = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.isdelayed = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.parent_npc = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.taken = "-1";
	QuestsShablons[KILL_CHARACTER_QUEST].l.time = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.logbook = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.iscontinue = "0";
	QuestsShablons[KILL_CHARACTER_QUEST].l.target_colony = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.parent_colony = "";
	QuestsShablons[KILL_CHARACTER_QUEST].l.contribution = "0";
	QuestsShablons[KILL_CHARACTER_QUEST].l.swapped = "0";
	QuestsShablons[KILL_CHARACTER_QUEST].l.ishovernor = "0";
	QuestsShablons[KILL_CHARACTER_QUEST].l.isfight = "0";
	QuestsShablons[KILL_CHARACTER_QUEST].l.careerreward = "1";
	QuestsShablons[KILL_CHARACTER_QUEST].l.career = "1";

	QuestsShablons[TALK_WITH_CHARACTER_QUEST].id = "TalkWithCharacter";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.money = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.isdelayed = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.parent_npc = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.target_npc = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.taken = "-1";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.time = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.logbook = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.iscontinue = "0";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.target_colony = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.parent_colony = "";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.contribution = "0";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.swapped = "0";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.ishovernor = "0";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.isfight = "0";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.careerreward = "1";
	QuestsShablons[TALK_WITH_CHARACTER_QUEST].l.career = "1";

	QuestsShablons[LOOK_AT_SOMETHING_QUEST].id = "LookAtSomething";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.earth_locator = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.sea_locator = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.money = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.target_colony = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.target_island = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.parent_npc = "";
	QuestsShablons[LOOK_AT_SOMETHING_QUEST].l.career = "-1";
}

void InitQuests()
{
	for (int i = 0; i < MAX_QUESTS; i++)
	{
		Quests[i].id = "-1";
	}
}

int FindFirstEmptyQuest()
{
	
	string sQuestStep;

	for(int i = 0; i < MAX_QUESTS; i++)
	{
		/*
		sQuestStep = "l"+i;
		if(!CheckAttribute(pchar, "quests." + sQuestStep))
		{
			return i;
		}
		*/
		if(sti(Quests[i].id) == -1)
		{
			return i;
		}
	}
	return - 1;
}