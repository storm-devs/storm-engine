
#define UPGRADE_WHEAT			0
#define UPGRADE_PIER			1
#define	UPGRADE_MILL			2
#define	UPGRADE_SHIPYARD		3
#define	UPGRADE_ACADEMY			4
#define	UPGRADE_HUTS			5
#define	UPGRADE_STOREHOUSE		6

#define	UPGRADE_FORT			7
#define	UPGRADE_CHURCH			8
#define	UPGRADE_EXPEDITION		9
#define	UPGRADE_GOLDMINES		10
#define	UPGRADE_SILVERMINES		11


void ColonyUpgradesInit()
{
	ColonyUpgrades[UPGRADE_WHEAT].id = "Wheatfield";
	ColonyUpgrades[UPGRADE_WHEAT].l1.name = "Wheatfield";
	ColonyUpgrades[UPGRADE_WHEAT].level = 1;
	ColonyUpgrades[UPGRADE_WHEAT].maxlevel = 1;
	ColonyUpgrades[UPGRADE_WHEAT].colonylevel = 1;
	ColonyUpgrades[UPGRADE_WHEAT].l1.money = 10000;
	ColonyUpgrades[UPGRADE_WHEAT].population_modifier = 5;
	ColonyUpgrades[UPGRADE_WHEAT].food_modifier = 5;
	ColonyUpgrades[UPGRADE_WHEAT].model = 0;
	ColonyUpgrades[UPGRADE_WHEAT].isdestroy = 0;
	ColonyUpgrades[UPGRADE_WHEAT].days = 3;
	ColonyUpgrades[UPGRADE_WHEAT].requred.l1.population = 200;
	ColonyUpgrades[UPGRADE_WHEAT].l1.text = "ѕшеничные пол€ увеличивают прирост продовольстви€ в колонии.";

	ColonyUpgrades[UPGRADE_PIER].id = "Fishingpier";
	ColonyUpgrades[UPGRADE_PIER].l1.name = "Fishingpier";
	ColonyUpgrades[UPGRADE_PIER].level = 1;
	ColonyUpgrades[UPGRADE_PIER].maxlevel = 1;
	ColonyUpgrades[UPGRADE_PIER].colonylevel = 1;
	ColonyUpgrades[UPGRADE_PIER].l1.money = 10000;
	ColonyUpgrades[UPGRADE_PIER].population_modifier = 5;
	ColonyUpgrades[UPGRADE_PIER].food_modifier = 2;
	ColonyUpgrades[UPGRADE_PIER].model = 1;
	ColonyUpgrades[UPGRADE_PIER].isdestroy = 1;
	ColonyUpgrades[UPGRADE_PIER].days = 3;
	ColonyUpgrades[UPGRADE_PIER].requred.l1.population = 100;
	ColonyUpgrades[UPGRADE_PIER].l1.text = "–ыболовецкие причалы увеличивают прирост продовольстви€ в колонии.";

	ColonyUpgrades[UPGRADE_MILL].id = "Mill";
	ColonyUpgrades[UPGRADE_MILL].l1.name = "Mill";
	ColonyUpgrades[UPGRADE_MILL].level = 1;
	ColonyUpgrades[UPGRADE_MILL].maxlevel = 1;
	ColonyUpgrades[UPGRADE_MILL].colonylevel = 1;
	ColonyUpgrades[UPGRADE_MILL].l1.money = 20000;
	ColonyUpgrades[UPGRADE_MILL].population_modifier = 5;
	ColonyUpgrades[UPGRADE_MILL].food_modifier = 2;
	ColonyUpgrades[UPGRADE_MILL].model = 1;
	ColonyUpgrades[UPGRADE_MILL].isdestroy = 1;
	ColonyUpgrades[UPGRADE_MILL].days = 3;
	ColonyUpgrades[UPGRADE_MILL].requred.l1.wheatfield = 1;
	ColonyUpgrades[UPGRADE_MILL].requred.l1.population = 200;
	ColonyUpgrades[UPGRADE_MILL].l1.text = "ћельница увеличивает прирост продовольстви€ в колонии.";

	ColonyUpgrades[UPGRADE_SHIPYARD].id = "Shipyard";
	ColonyUpgrades[UPGRADE_SHIPYARD].l1.name = "Low Shipyard";
	ColonyUpgrades[UPGRADE_SHIPYARD].l2.name = "Middle Shipyard";
	ColonyUpgrades[UPGRADE_SHIPYARD].l3.name = "Hi Shipyard";
	ColonyUpgrades[UPGRADE_SHIPYARD].level = 1;
	ColonyUpgrades[UPGRADE_SHIPYARD].maxlevel = 3;
	ColonyUpgrades[UPGRADE_SHIPYARD].colonylevel = 1;//на каждый уровень
	ColonyUpgrades[UPGRADE_SHIPYARD].l1.money = 40000;
	ColonyUpgrades[UPGRADE_SHIPYARD].l2.money = 60000;
	ColonyUpgrades[UPGRADE_SHIPYARD].l3.money = 80000;
	ColonyUpgrades[UPGRADE_SHIPYARD].shiplistmodifier = 1;//на каждый уровень
	ColonyUpgrades[UPGRADE_SHIPYARD].model = 1;
	ColonyUpgrades[UPGRADE_SHIPYARD].isdestroy = 0;
	ColonyUpgrades[UPGRADE_SHIPYARD].days = 5;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l1.fort = 1;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l1.population = 300;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l2.fort = 2;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l2.population = 600;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l3.fort = 3;
	ColonyUpgrades[UPGRADE_SHIPYARD].requred.l3.population = 900;
	ColonyUpgrades[UPGRADE_SHIPYARD].l1.text = "ѕозвол€ет строить на колонии корабли вплоть до 5-го класса.";
	ColonyUpgrades[UPGRADE_SHIPYARD].l2.text = "ѕозвол€ет строить на колонии корабли вплоть до 4-го класса.";
	ColonyUpgrades[UPGRADE_SHIPYARD].l3.text = "ѕозвол€ет строить на колонии корабли вплоть до 3-го класса.";

	ColonyUpgrades[UPGRADE_ACADEMY].id = "Academy";
	ColonyUpgrades[UPGRADE_ACADEMY].l1.name = "Academy";
	ColonyUpgrades[UPGRADE_ACADEMY].level = 1;
	ColonyUpgrades[UPGRADE_ACADEMY].maxlevel = 1;
	ColonyUpgrades[UPGRADE_ACADEMY].colonylevel = 4;
	ColonyUpgrades[UPGRADE_ACADEMY].l1.money = 50000;
	ColonyUpgrades[UPGRADE_ACADEMY].crewexpmodifier = 5;
	ColonyUpgrades[UPGRADE_ACADEMY].model = 1;
	ColonyUpgrades[UPGRADE_ACADEMY].isdestroy = 1;
	ColonyUpgrades[UPGRADE_ACADEMY].days = 10;
	ColonyUpgrades[UPGRADE_ACADEMY].requred.l1.population = 1000;
	ColonyUpgrades[UPGRADE_ACADEMY].l1.text = "јкадеми€ позвол€ет тренировать матросов и офицеров.";

	ColonyUpgrades[UPGRADE_HUTS].id = "Huts";
	ColonyUpgrades[UPGRADE_HUTS].l1.name = "Huts";
	ColonyUpgrades[UPGRADE_HUTS].level = 1;
	ColonyUpgrades[UPGRADE_HUTS].maxlevel = 1;
	ColonyUpgrades[UPGRADE_HUTS].colonylevel = 2;
	ColonyUpgrades[UPGRADE_HUTS].l1.money = 30000;
	ColonyUpgrades[UPGRADE_HUTS].crewexpmodifier = 5;
	ColonyUpgrades[UPGRADE_HUTS].model = 1;
	ColonyUpgrades[UPGRADE_HUTS].isdestroy = 1;
	ColonyUpgrades[UPGRADE_HUTS].days = 10;
	ColonyUpgrades[UPGRADE_HUTS].loyality = 0.2;//увеличение loyality в день
	ColonyUpgrades[UPGRADE_HUTS].requred.l1.population = 750;
	ColonyUpgrades[UPGRADE_HUTS].l1.text = "Ѕараки позвол€ют тренировать матросов и офицеров, а так же увеличивают ло€льность населени€.";

	ColonyUpgrades[UPGRADE_STOREHOUSE].id = "Storehouse";
	ColonyUpgrades[UPGRADE_STOREHOUSE].l1.name = "Storehouse";
	ColonyUpgrades[UPGRADE_STOREHOUSE].level = 1;
	ColonyUpgrades[UPGRADE_STOREHOUSE].maxlevel = 1;
	ColonyUpgrades[UPGRADE_STOREHOUSE].colonylevel = 2;
	ColonyUpgrades[UPGRADE_STOREHOUSE].l1.money = 40000;
	ColonyUpgrades[UPGRADE_STOREHOUSE].model = 1;
	ColonyUpgrades[UPGRADE_STOREHOUSE].isdestroy = 1;
	ColonyUpgrades[UPGRADE_STOREHOUSE].days = 5;
	ColonyUpgrades[UPGRADE_STOREHOUSE].requred.l1.loyality = 40;
	ColonyUpgrades[UPGRADE_STOREHOUSE].requred.l1.population = 500;
	ColonyUpgrades[UPGRADE_STOREHOUSE].l1.text = "—клады позвол€ют хранить большее количества товара в колонии.";

	ColonyUpgrades[UPGRADE_FORT].id = "Fort";
	ColonyUpgrades[UPGRADE_FORT].l1.name = "Low Fort";
	ColonyUpgrades[UPGRADE_FORT].l2.name = "Middle Fort";
	ColonyUpgrades[UPGRADE_FORT].l3.name = "Hi Fort";
	ColonyUpgrades[UPGRADE_FORT].l4.name = "Super Fort";
	ColonyUpgrades[UPGRADE_FORT].level = 1;
	ColonyUpgrades[UPGRADE_FORT].maxlevel = 4;
	ColonyUpgrades[UPGRADE_FORT].colonylevel = 1;//дл€ каждого уровн€
	ColonyUpgrades[UPGRADE_FORT].l1.money = 200000;
	ColonyUpgrades[UPGRADE_FORT].l2.money = 250000;
	ColonyUpgrades[UPGRADE_FORT].l3.money = 300000;
	ColonyUpgrades[UPGRADE_FORT].l4.money = 350000;
	ColonyUpgrades[UPGRADE_FORT].model = 0;
	ColonyUpgrades[UPGRADE_FORT].isdestroy = 0;
	ColonyUpgrades[UPGRADE_FORT].days = 15;
	ColonyUpgrades[UPGRADE_FORT].requred.l1.population = 500;
	ColonyUpgrades[UPGRADE_FORT].requred.l2.huts = 1;
	ColonyUpgrades[UPGRADE_FORT].requred.l2.population = 800;
	ColonyUpgrades[UPGRADE_FORT].requred.l3.church = 1;
	ColonyUpgrades[UPGRADE_FORT].requred.l3.population = 1000;
	ColonyUpgrades[UPGRADE_FORT].requred.l4.academy = 1;
	ColonyUpgrades[UPGRADE_FORT].requred.l4.population = 1500;
	ColonyUpgrades[UPGRADE_FORT].l1.text = "ѕозвол€ет устанавливать на защитных укреплени€х колонии пушки калибром до 32 фунтов.";
	ColonyUpgrades[UPGRADE_FORT].l2.text = "ѕозвол€ет устанавливать на защитных укреплени€х колонии кульверины и пушки калибром до 36 фунтов.";
	ColonyUpgrades[UPGRADE_FORT].l3.text = "ѕозвол€ет устанавливать на защитных укреплени€х колонии кульверины, мортиры и пушки калибром до 42 фунтов.";
	ColonyUpgrades[UPGRADE_FORT].l4.text = "ѕозвол€ет устанавливать на защитных укреплени€х колонии кульверины, мортиры и пушки калибром до 48 фунтов.";

	ColonyUpgrades[UPGRADE_CHURCH].id = "Church";
	ColonyUpgrades[UPGRADE_CHURCH].l1.name = "Church";
	ColonyUpgrades[UPGRADE_CHURCH].level = 1;
	ColonyUpgrades[UPGRADE_CHURCH].maxlevel = 1;
	ColonyUpgrades[UPGRADE_CHURCH].colonylevel = 3;
	ColonyUpgrades[UPGRADE_CHURCH].l1.money = 60000;
	ColonyUpgrades[UPGRADE_CHURCH].model = 1;
	ColonyUpgrades[UPGRADE_CHURCH].isdestroy = 1;
	ColonyUpgrades[UPGRADE_CHURCH].days = 5;
	ColonyUpgrades[UPGRADE_CHURCH].loyality = 0.5;//увеличение loyality в день
	ColonyUpgrades[UPGRADE_CHURCH].requred.l1.loyality = 60;
	ColonyUpgrades[UPGRADE_CHURCH].requred.l1.population = 800;
	ColonyUpgrades[UPGRADE_CHURCH].l1.text = "÷ерковь увеличивает ло€льность населени€ колонии.";

	ColonyUpgrades[UPGRADE_EXPEDITION].id = "Expedition";
	ColonyUpgrades[UPGRADE_EXPEDITION].l1.name = "Expedition";
	ColonyUpgrades[UPGRADE_EXPEDITION].level = 1;
	ColonyUpgrades[UPGRADE_EXPEDITION].maxlevel = 1;
	ColonyUpgrades[UPGRADE_EXPEDITION].colonylevel = 2;
	ColonyUpgrades[UPGRADE_EXPEDITION].l1.money = 50000;
	ColonyUpgrades[UPGRADE_EXPEDITION].model = 0;
	ColonyUpgrades[UPGRADE_EXPEDITION].isdestroy = 0;
	ColonyUpgrades[UPGRADE_EXPEDITION].days = 10;
	ColonyUpgrades[UPGRADE_EXPEDITION].requred.l1.population = 600;
	ColonyUpgrades[UPGRADE_EXPEDITION].l1.text = "Ёкспедици€ позвол€ет определить есть ли на острове залежи золота или серебра.";

	ColonyUpgrades[UPGRADE_GOLDMINES].id = "Goldmines";
	ColonyUpgrades[UPGRADE_GOLDMINES].l1.name = "Goldmines";
	ColonyUpgrades[UPGRADE_GOLDMINES].level = 1;
	ColonyUpgrades[UPGRADE_GOLDMINES].maxlevel = 1;
	ColonyUpgrades[UPGRADE_GOLDMINES].colonylevel = 3;
	ColonyUpgrades[UPGRADE_GOLDMINES].l1.money = 50000;
	ColonyUpgrades[UPGRADE_GOLDMINES].money_per_day = 500;
	ColonyUpgrades[UPGRADE_GOLDMINES].model = 0;
	ColonyUpgrades[UPGRADE_GOLDMINES].isdestroy = 0;
	ColonyUpgrades[UPGRADE_GOLDMINES].days = 15;
	ColonyUpgrades[UPGRADE_GOLDMINES].requred.l1.expedition = 1;
	ColonyUpgrades[UPGRADE_GOLDMINES].requred.l1.population = 1000;
	ColonyUpgrades[UPGRADE_GOLDMINES].requred.l1.loyality = 80;
	ColonyUpgrades[UPGRADE_GOLDMINES].l1.text = "«олотые шахты позвол€ют колонии получать дополнительную прибыль.";

	ColonyUpgrades[UPGRADE_SILVERMINES].id = "Silvermines";
	ColonyUpgrades[UPGRADE_SILVERMINES].l1.name = "Silvermines";
	ColonyUpgrades[UPGRADE_SILVERMINES].level = 1;
	ColonyUpgrades[UPGRADE_SILVERMINES].maxlevel = 1;
	ColonyUpgrades[UPGRADE_SILVERMINES].colonylevel = 3;
	ColonyUpgrades[UPGRADE_SILVERMINES].l1.money = 25000;
	ColonyUpgrades[UPGRADE_SILVERMINES].money_per_day = 250;
	ColonyUpgrades[UPGRADE_SILVERMINES].model = 0;
	ColonyUpgrades[UPGRADE_SILVERMINES].isdestroy = 0;
	ColonyUpgrades[UPGRADE_SILVERMINES].days = 15;
	ColonyUpgrades[UPGRADE_SILVERMINES].requred.l1.expedition = 1;
	ColonyUpgrades[UPGRADE_SILVERMINES].requred.l1.population = 1000;
	ColonyUpgrades[UPGRADE_SILVERMINES].requred.l1.loyality = 70;
	ColonyUpgrades[UPGRADE_SILVERMINES].l1.text = "—еребр€нные шахты позвол€ют колонии получать дополнительную прибыль.";
}

