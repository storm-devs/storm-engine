//Здесь прописываются шаблоны цепочек слухов

#define MAX_TEMPL 15 // количество шаблонов
object templat[MAX_TEMPL];   // шаблонные слухи с развитием

void TemplateLoad()
{
	int n;
	ref tpl;
	n = 0;

///////////////////////////////////////////GoldFleet////////////////////////////////////////
//Испанский золотой конвой

	makeref(tpl, templat[n]);
	//снаряжение конвоя
	tpl.name = "Init_GoldFleet";
	tpl.texts = 1;
	tpl.state = 1;
	tpl.tip = "trader,tavern";
	tpl.rep = "none";
	tpl.nonation.n0 = SPAIN;
	tpl.starttime = 30;
	tpl.actualtime = 120;
	tpl.event = "GoldFleetMonth";
	tpl.next = "none";
	tpl.function = "OnInit_GoldFleet";
	n++;

	makeref(tpl, templat[n]);
	//Выход конвоя из Порто Белло
	tpl.name = "Start_GoldFleet";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.nonation.n0 = SPAIN;
	tpl.starttime = 2;
	tpl.actualtime = 20;
	tpl.loginfo.begin = "Rumour_GoldFleet";
	tpl.loginfo.textnum = "1"
	tpl.event = "GoldFleet";
	tpl.next = "End_GoldFleet";
	tpl.function = "";
	n++;

	makeref(tpl, templat[n]);
	//гибель конвоя
	tpl.name = "Defeat_GoldFleet";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 30;
//	tpl.loginfo.end = "Rumour_GoldFleet";
//	tpl.loginfo.textnum = "4"
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "";
	n++;

	makeref(tpl, templat[n]);
	//Атака конвоя
	tpl.name = "Attak_GoldFleet";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "all";
	tpl.rep = "none";
	//tpl.onlynation = SPAIN; //fix
	tpl.starttime = 0;
	tpl.actualtime = 30;
	tpl.loginfo.end = "Rumour_GoldFleet";
	tpl.loginfo.text = "13"
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "";
	n++;

	makeref(tpl, templat[n]);
	//прибыте конвоя в Гавану
	tpl.name = "End_GoldFleet";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 30;
	tpl.loginfo.end = "Rumour_GoldFleet";
	tpl.loginfo.textnum = "7"
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "";
	n++;
///////////////////////////////////////////MerchantOnMap////////////////////////////////////////
//Наводка на купца

	makeref(tpl, templat[n]);
	//Наводка
	tpl.name = "MerchantOnMap";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "habitue,tavern";
	tpl.rep = "bad";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "MerchantOnMap";
	tpl.next = "none";
	tpl.function = "";
	n++;

	makeref(tpl, templat[n]);
	//Гибель купца
	tpl.name = "MerchantRumour";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 5;
	tpl.event = "OnMerchantDie";
	tpl.next = "none";
	tpl.function = "";
	n++;

////////////////////////////////////////Разные слухи///////////////////////////////////
	makeref(tpl, templat[n]);
	//Цены в магазине
	tpl.name = "GetColonyPrices";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "trader,tavern";
	tpl.rep = "good";
	tpl.starttime = 0;
	tpl.actualtime = 30;
	tpl.event = "GetPrices";
	tpl.next = "none";
	tpl.function = "";
	n++;


//////////////////////////////////////////Дуэли///////////////////////////////////////
//Дуэль - трус.
	makeref(tpl, templat[n]);
	tpl.name = "DuelCoward";
	tpl.texts = 3;
	tpl.state = 5;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "OnInit_DuelCoward";
	n++;

//Дуэль - супергерой.
	makeref(tpl, templat[n]);
	tpl.name = "DuelHero";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "DuelHero";
	tpl.next = "none";
	tpl.function = "";
	n++;

///////////////////////////////////////////ПГГ////////////////////////////////////////
//ПГГ - без корабля, в таверне тусуется.
	makeref(tpl, templat[n]);
	tpl.name = "WaitInTavern";
	tpl.texts = 3;
	tpl.state = 3;
	tpl.tip = "citizen,habitue,trader,tavern";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "PGG_OnInit_WaitInTavern";
	n++;

//ПГГ - пашет на мэра.
	makeref(tpl, templat[n]);
	tpl.name = "WorkOnMayor";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "citizen,habitue,trader,tavern";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "PGG_OnInit_WorkOnMayor";
	n++;

//ПГГ - на пути к острову.
	makeref(tpl, templat[n]);
	tpl.name = "SailToIsland";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "PGG_OnInit_SailToIsland";
	n++;

//ПГГ - перевозка груза.
	makeref(tpl, templat[n]);
	tpl.name = "WorkOnStore";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "all";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 0;
	tpl.event = "none";
	tpl.next = "none";
	tpl.function = "PGG_OnInit_WorkOnStore";
	n++;

//ПГГ - с контрой.
	makeref(tpl, templat[n]);
	tpl.name = "WorkWithContra";
	tpl.texts = 3;
	tpl.state = 1;
	tpl.tip = "habitue";
	tpl.rep = "none";
	tpl.starttime = 0;
	tpl.actualtime = 2;
	tpl.event = "PGG_WorkWithContra";
	tpl.next = "none";
	tpl.function = "PGG_OnInit_WorkWithContra";
	n++;
///////////////////////////////////////////////////////////////////////////////////////////////////////
}