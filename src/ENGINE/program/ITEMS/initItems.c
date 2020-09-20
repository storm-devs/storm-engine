int InitItems()
{
	InitButtons();
	InitRandItems();
	ref itm;
	int n;

	for(n=0; n<ITEMS_QUANTITY; n++)
	{
		makeref(itm,Items[n]);
		itm.id = "item"+(n+1);
		itm.describe = "";
		itm.index = n;
		itm.model = "";
		itm.shown = false;
		itm.startLocation = "";
		itm.startLocator = "";
		itm.useLocation = "";
		itm.useLocator = "";
		itm.describe = "";
        itm.Weight = 0.0; // boal
        itm.uniq  = false;
        itm.price = 1;
        itm.ItemType = "VARIETY";
	}
	n = 0;
	// в начате идут все без заполнения трупов
//==================PATENTS
    makeref(itm,Items[n]);
	itm.id = "patent_eng";
    itm.groupID = PATENT_ITEM_TYPE;
	itm.name = "itmname_patent_england";
	itm.describe = "itmdescr_patent_england";
	itm.model = "";
    itm.quest = "eng_flag_rise";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Nation = ENGLAND; //форма солдат
	itm.TitulCur = 1; // текущ звание
	itm.TitulCurNext = 0; // счетчик званий
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

    makeref(itm,Items[n]);
	itm.id = "patent_fra";
    itm.groupID = PATENT_ITEM_TYPE;
	itm.name = "itmname_patent_france";
	itm.describe = "itmdescr_patent_france";
	itm.model = "";
    itm.quest = "fra_flag_rise";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
	itm.price = 0;
	itm.Nation = FRANCE; //форма солдат
	itm.TitulCur = 1; // текущ звание
	itm.TitulCurNext = 0; // счетчик званий
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

    makeref(itm,Items[n]);
	itm.id = "patent_spa";
    itm.groupID = PATENT_ITEM_TYPE;
	itm.name = "itmname_patent_spain";
	itm.describe = "itmdescr_patent_spain";
	itm.model = "";
    itm.quest = "spa_flag_rise";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
	itm.price = 0;
	itm.Nation = SPAIN; //форма солдат
	itm.TitulCur = 1; // текущ звание
	itm.TitulCurNext = 0; // счетчик званий
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

    makeref(itm,Items[n]);
	itm.id = "patent_hol";
    itm.groupID = PATENT_ITEM_TYPE;
	itm.name = "itmname_patent_holland";
	itm.describe = "itmdescr_patent_holland";
	itm.model = "";
    itm.quest = "hol_flag_rise";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
	itm.price = 0;
	itm.Nation = HOLLAND; //форма солдат
	itm.TitulCur = 1; // текущ звание
	itm.TitulCurNext = 0; // счетчик званий
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

/////////////////////////////////////////////////////////////////
//                                     WEAPONS   выбор оружия у офицера завязан на порядковый номер в списке, а не на крутизну!!!
////////////////////////////////////////////////////////////////
    ///////////////////////////  FencingLight   ////////////////////////////
    makeref(itm,Items[n]);
	itm.id = "unarmed"; // руки
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_unarmed";
	itm.describe = "itmdescr_unarmed";
	itm.model = "unarmed";
	itm.folder = "items";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_1";
	itm.price = 1;
	itm.Weight = 0.0;
	itm.dmg_min = 1.0;
	itm.dmg_max = 15.0;
	itm.piercing = 1;
	itm.minlevel = 0;
	itm.rare = 0.0001;
	itm.block = 0;
	itm.param.time = 0.05;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
    // boal 21.05.2004 -->
    makeref(itm,Items[n]);
	itm.id = "rabble"; // кочерга
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_rabble";
	itm.describe = "itmdescr_rabble";
	itm.model = "kocherg";
	itm.folder = "items";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_10";
	itm.price = 5;
	itm.Weight = 4;
	itm.dmg_min = 1.0;
	itm.dmg_max = 25.0;
	itm.piercing = 5;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 19;
	itm.param.time = 0.05;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	// boal 21.05.2004 <--

    makeref(itm,Items[n]);
	itm.id = "Lead1"; // кастет
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_lead1";
	itm.describe = "itmdescr_lead1";
	itm.model = "lead1";
	itm.folder = "items";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_1";
	itm.price = 50;
	itm.Weight = 0.5;
	itm.dmg_min = 2.0;
	itm.dmg_max = 30.0;
	itm.piercing = 7;
	itm.minlevel = 0;
	itm.rare = 0.0001;
	itm.block = 0;
	itm.param.time = 0.05;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade5";  //Кинжал
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade5";
	itm.describe = "itmdescr_blade5";
	itm.folder = "items";
	itm.model = "blade5";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 80;
	itm.Weight = 1;
	// boal 19.01.2004 <--
	itm.dmg_min = 3.0;
	itm.dmg_max = 35.0;
	itm.piercing = 25;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 5;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade2";  //Мушкетерская рапира.   "Бретта"
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade2";
	itm.describe = "itmdescr_blade2";
	itm.folder = "items";
	itm.model = "blade2";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 120;
	itm.Weight = 3;
	// boal 19.01.2004 <--
	itm.dmg_min = 5.0;
	itm.dmg_max = 40.0;
	itm.piercing = 15;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 10;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade6";  //Скьявона
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade6";
	itm.describe = "itmdescr_blade6";
	itm.folder = "items";
	itm.model = "blade6";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 200;
	itm.Weight = 2.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 8.0;
	itm.dmg_max = 46.0;
	itm.piercing = 15;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 15;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade9"; //Итальянская рапира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade9";
	itm.describe = "itmdescr_blade9";
	itm.folder = "items";
	itm.model = "blade9";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 600;
	itm.Weight = 2.8;
	// boal 19.01.2004 <--
	itm.dmg_min = 10.0;
	itm.dmg_max = 45.0;
	itm.piercing = 20;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 15;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade14";  //Французская рапира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade14";
	itm.describe = "itmdescr_blade14";
	itm.folder = "items";
	itm.model = "blade14";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 400;
	itm.Weight = 3.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 11.0;
	itm.dmg_max = 48.0;
	itm.piercing = 20;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 60;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
    itm.FencingType = "FencingLight";
    itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade19";   //Германская рапира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade19";
	itm.describe = "itmdescr_blade19";
	itm.folder = "items";
	itm.model = "blade19";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 700;
	itm.Weight = 2.6;
	// boal 19.01.2004 <--
	itm.dmg_min = 12.0;
	itm.dmg_max = 50.0;
	itm.piercing = 20;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 60;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade22"; //Английская рапира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade22";
	itm.describe = "itmdescr_blade22";
	itm.folder = "items";
	itm.model = "blade22";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 900;
	itm.Weight = 2.7;
	// boal 19.01.2004 <--
	itm.dmg_min = 13.0;
	itm.dmg_max = 53.0;
	itm.piercing = 44;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 73;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade23"; //Испанская рапира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade23";
	itm.describe = "itmdescr_blade23";
	itm.folder = "items";
	itm.model = "blade23";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 1300;
	itm.Weight = 2.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 14.0;
	itm.dmg_max = 55.0;
	itm.piercing = 50;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 75;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
    makeref(itm,Items[n]);
	itm.id = "arm1"; //Боевые когти
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_arm1";
	itm.describe = "itmdescr_arm1";
	itm.folder = "items";
	itm.model = "arm1";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 3300;
	itm.Weight = 3.7;
	// boal 19.01.2004 <--
	itm.dmg_min = 20.0;
	itm.dmg_max = 60.0;
	itm.piercing = 65.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 70.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade27"; //Клинок Генри Моргана
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade27";
	itm.describe = "itmdescr_blade27";
	itm.folder = "items";
	itm.model = "blade27";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_8";
	// boal 19.01.2004 -->
	itm.price = 10000;
	itm.Weight = 2.9;
	// boal 19.01.2004 <--
	itm.dmg_min = 18.0;
	itm.dmg_max = 65.0;
	itm.piercing = 99.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 99.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingLight";
	itm.ItemType = "WEAPON";
	n++;
	
	///////////////////////////  Fencing   ////////////////////////////
	makeref(itm,Items[n]);
	itm.id = "blade1"; //Старая сабля
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade1";
	itm.describe = "itmdescr_blade1";
	itm.model = "blade1";
	itm.folder = "items";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 50;
	itm.Weight = 4.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 6.0;
	itm.dmg_max = 45.0;
	itm.piercing = 10;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 5;
	itm.param.time = 0.05;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade4";  //Абордажная сабля
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade4";
	itm.describe = "itmdescr_blade4";
	itm.folder = "items";
	itm.model = "blade4";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 100;
	itm.Weight = 4.6;
	// boal 19.01.2004 <--
	itm.dmg_min = 8.0;
	itm.dmg_max = 50.0;
	itm.piercing = 30;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 1;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "topor3";  //Топорик
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_topor3";
	itm.describe = "itmdescr_topor3";
	itm.folder = "items";
	itm.model = "topor3";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 150;
	itm.Weight = 6;
	// boal 19.01.2004 <--
	itm.dmg_min = 10.0;
	itm.dmg_max = 60.0;
	itm.piercing = 14.0;
	itm.minlevel = 1;
	itm.rare = 0.01;
	itm.block = 2.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade3"; //Малкус   Стальная сабля с тяжелым лезвием, способная разрубить любую кольчугу.
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade3";
	itm.describe = "itmdescr_blade3";
	itm.folder = "items";
	itm.model = "blade3";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 200;
	itm.Weight = 5.3;
	// boal 19.01.2004 <--
	itm.dmg_min = 12.0;
	itm.dmg_max = 55.0;
	itm.piercing = 60;
	itm.minlevel = 0;
	itm.rare = 0.1;
	itm.block = 5;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade7"; //Ятаган
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade7";
	itm.describe = "itmdescr_blade7";
	itm.folder = "items";
	itm.model = "blade7";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 250;
	itm.Weight = 4.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 13.0;
	itm.dmg_max = 55.0;
	itm.piercing = 18;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 5;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade12"; //Персидская сабля
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade12";
	itm.describe = "itmdescr_blade12";
	itm.folder = "items";
	itm.model = "blade12";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 300;
	itm.Weight = 6;
	// boal 19.01.2004 <--
	itm.dmg_min = 14.0;
	itm.dmg_max = 65.0;
	itm.piercing = 30;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 20;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
    makeref(itm,Items[n]);
	itm.id = "blade18"; //Персидская сабля с острым изогнутым клинком, распространенная от Марокко до Индии.
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade18";
	itm.describe = "itmdescr_blade18";
	itm.folder = "items";
	itm.model = "blade18";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 400;
	itm.Weight = 5.4;
	// boal 19.01.2004 <--
	itm.dmg_min = 16.0;
	itm.dmg_max = 60.0;
	itm.piercing = 35;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 35;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade30";//Жало
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade30";
	itm.describe = "itmdescr_blade30";
	itm.folder = "items";
	itm.model = "blade30";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 600;
	itm.Weight = 4.1;
	// boal 19.01.2004 <--
	itm.dmg_min = 18.0;
	itm.dmg_max = 63.0;
	itm.piercing = 55.0;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 40.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade31"; //Кавказкая шашка
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade31";
	itm.describe = "itmdescr_blade31";
	itm.folder = "items";
	itm.model = "blade31";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 800;
	itm.Weight = 4.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 19.0;
	itm.dmg_max = 64.0;
	itm.piercing = 50.0;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 65.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade24"; //Ураган
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade24";
	itm.describe = "itmdescr_blade24";
	itm.folder = "items";
	itm.model = "blade24";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_8";
	// boal 19.01.2004 -->
	itm.price = 1500;
	itm.Weight = 4.3;
	// boal 19.01.2004 <--
	itm.dmg_min = 20.0;
	itm.dmg_max = 66.0;
	itm.piercing = 58.0;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 45.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade25";  //Арабская сабля
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade25";
	itm.describe = "itmdescr_blade25";
	itm.folder = "items";
	itm.model = "blade25";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_8";
	// boal 19.01.2004 -->
	itm.price = 1700;
	itm.Weight = 4.2;
	// boal 19.01.2004 <--
	itm.dmg_min = 21.0;
	itm.dmg_max = 65.0;
	itm.piercing = 60.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 70.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade34"; //Нимча
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade34";
	itm.describe = "itmdescr_blade34";
	itm.folder = "items";
	itm.model = "blade34";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 2000;
	itm.Weight = 5.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 23.0;
	itm.dmg_max = 70.0;
	itm.piercing = 90.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 70.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
    makeref(itm,Items[n]);
	itm.id = "blade26";  //Цербер
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade26";
	itm.describe = "itmdescr_blade26";
	itm.folder = "items";
	itm.model = "blade26";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_8";
	// boal 19.01.2004 -->
	itm.price = 2500;
	itm.Weight = 4.7;
	// boal 19.01.2004 <--
	itm.dmg_min = 24.0;
	itm.dmg_max = 70.0;
	itm.piercing = 88.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 72.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade32"; //Килидж
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade32";
	itm.describe = "itmdescr_blade32";
	itm.folder = "items";
	itm.model = "blade32";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 3000;
	itm.Weight = 4.4;
	// boal 19.01.2004 <--
	itm.dmg_min = 25.0;
	itm.dmg_max = 70.0;
	itm.piercing = 90.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 92.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "Fencing";
	itm.ItemType = "WEAPON";
	n++;
	
	///////////////////////////  FencingHeavy   ////////////////////////////
	makeref(itm,Items[n]);
	itm.id = "blade10"; //Абордажный палаш
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade10";
	itm.describe = "itmdescr_blade10";
	itm.folder = "items";
	itm.model = "blade10";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 300;
	itm.Weight = 7;
	// boal 19.01.2004 <--
	itm.dmg_min = 10.0;
	itm.dmg_max = 60.0;
	itm.piercing = 50;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 10;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade35";  //Палаш
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade35";
	itm.describe = "itmdescr_blade35";
	itm.folder = "items";
	itm.model = "blade35";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 400;
	itm.Weight = 9;
	// boal 19.01.2004 <--
	itm.dmg_min = 12.0;
	itm.dmg_max = 65.0;
	itm.piercing = 65.0;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 25.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade8";  //Мальтийский меч
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade8";
	itm.describe = "itmdescr_blade8";
	itm.folder = "items";
	itm.model = "blade8";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 450;
	itm.Weight = 7.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 14.0;
	itm.dmg_max = 65.0;
	itm.piercing = 35;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 20;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "blade11"; //Шотландский палаш
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade11";
	itm.describe = "itmdescr_blade11";
	itm.folder = "items";
	itm.model = "blade11";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 500;
	itm.Weight = 8;
	// boal 19.01.2004 <--
	itm.dmg_min = 16.0;
	itm.dmg_max = 70.0;
	itm.piercing = 60;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 25;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "blade13"; //Эсток
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade13";
	itm.describe = "itmdescr_blade13";
	itm.folder = "items";
	itm.model = "blade13";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 600;
	itm.Weight = 6.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 18.0;
	itm.dmg_max = 70.0;
	itm.piercing = 65;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 20;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "topor1";  //Топор
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_topor1";
	itm.describe = "itmdescr_topor1";
	itm.folder = "items";
	itm.model = "topor1";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 650;
	itm.Weight = 10;
	// boal 19.01.2004 <--
	itm.dmg_min = 20.0;
	itm.dmg_max = 80.0;
	itm.piercing = 70.0;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 3.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade15";  //Итальянский меч
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade15";
	itm.describe = "itmdescr_blade15";
	itm.folder = "items";
	itm.model = "blade15";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 700;
	itm.Weight = 8.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 22.0;
	itm.dmg_max = 75.0;
	itm.piercing = 55;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 25;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "blade16"; //Испанский меч
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade16";
	itm.describe = "itmdescr_blade16";
	itm.folder = "items";
	itm.model = "blade16";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 800;
	itm.Weight = 9.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 22.0;
	itm.dmg_max = 80.0;
	itm.piercing = 45;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 35;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "blade17"; //Фламберг
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade17";
	itm.describe = "itmdescr_blade17";
	itm.folder = "items";
	itm.model = "blade17";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 900;
	itm.Weight = 11.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 24.0;
	itm.dmg_max = 90.0;
	itm.piercing = 65;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.block = 10;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade20"; //Атилл
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade20";
	itm.describe = "itmdescr_blade20";
	itm.folder = "items";
	itm.model = "blade20";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 1000;
	itm.Weight = 8.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 25.0;
	itm.dmg_max = 93.0;
	itm.piercing = 65;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 30;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "blade21"; //Фальчион
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade21";
	itm.describe = "itmdescr_blade21";
	itm.folder = "items";
	itm.model = "blade21";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_3";
	// boal 19.01.2004 -->
	itm.price = 1100;
	itm.Weight = 10.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 26.0;
	itm.dmg_max = 100.0;
	itm.piercing = 75;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 25;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "blade33"; //Французский меч
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade33";
	itm.describe = "itmdescr_blade33";
	itm.folder = "items";
	itm.model = "blade33";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 1300;
	itm.Weight = 9;
	// boal 19.01.2004 <--
	itm.dmg_min = 27.0;
	itm.dmg_max = 95.0;
	itm.piercing = 70.0;
	itm.minlevel = 1;
	itm.rare = 0.001;
	itm.block = 60.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "blade28";  //Танат
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_blade28";
	itm.describe = "itmdescr_blade28";
	itm.folder = "items";
	itm.model = "blade28";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 3000;
	itm.Weight = 9.3;
	// boal 19.01.2004 <--
	itm.dmg_min = 28.0;
	itm.dmg_max = 100.0;
	itm.piercing = 85.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 75.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "topor2";  //Секира
	itm.groupID = BLADE_ITEM_TYPE;
	itm.name = "itmname_topor2";
	itm.describe = "itmdescr_topor2";
	itm.folder = "items";
	itm.model = "topor2";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 3500;
	itm.Weight = 12.5;
	// boal 19.01.2004 <--
	itm.dmg_min = 35.0;
	itm.dmg_max = 120.0;
	itm.piercing = 95.0;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.block = 20.0;
	itm.param.time = 0.1;
	itm.param.colorstart = argb(64, 64, 64, 64);
	itm.param.colorend = argb(0, 32, 32, 32);
	itm.FencingType = "FencingHeavy";
	itm.ItemType = "WEAPON";
	n++;

//GUNS

	makeref(itm,Items[n]);
	itm.id = "pistol1";
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol1";
	itm.describe = "itmdescr_pistol1";
	itm.folder = "items";
	itm.model = "pistol1";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 100;
	itm.Weight = 3;
	// boal 19.01.2004 <--
	itm.chargeQ = 1;
	itm.chargespeed = 8;
	itm.dmg_min = 20.0;
	itm.dmg_max = 100.0;
	itm.accuracy = 30;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "pistol2";
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol2";
	itm.describe = "itmdescr_pistol2";
	itm.folder = "items";
	itm.model = "pistol2";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 400;
	itm.Weight = 4;
	// boal 19.01.2004 <--
	itm.chargeQ = 1;
	itm.chargespeed = 12;
	itm.dmg_min = 30.0;
	itm.dmg_max = 120.0;
	itm.accuracy = 50;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "pistol3"; //Мортира
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol3";
	itm.describe = "itmdescr_pistol3";
	itm.folder = "items";
	itm.model = "pistol3";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 800;
	itm.Weight = 6;
	// boal 19.01.2004 <--
	itm.chargeQ = 1;
	itm.chargespeed = 22;
	itm.dmg_min = 50.0;
	itm.dmg_max = 175.0;
	itm.accuracy = 20;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "pistol6";  // 2х
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol6";
	itm.describe = "itmdescr_pistol6";
	itm.folder = "items";
	itm.model = "pistol6";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 3600;
	itm.Weight = 5;
	// boal 19.01.2004 <--
	itm.chargeQ = 2;
	itm.chargespeed = 20;
	itm.dmg_min = 35.0;
	itm.dmg_max = 110.0;
	itm.accuracy = 60;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "pistol5"; //Бретерский Пистолет
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol5";
	itm.describe = "itmdescr_pistol5";
	itm.folder = "items";
	itm.model = "pistol5";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_6";
	itm.price = 3800;
	// boal 19.01.2004 -->
	itm.Weight = 3;
	// boal 19.01.2004 <--
	itm.chargeQ = 1;
	itm.chargespeed = 16;
	itm.dmg_min = 40.0;
	itm.dmg_max = 170.0;
	itm.accuracy = 80;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;

	makeref(itm,Items[n]);
	itm.id = "pistol4"; // 4х
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol4";
	itm.describe = "itmdescr_pistol4";
	itm.folder = "items";
	itm.model = "pistol4";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_6";
	itm.shown = 4;
	itm.price = 5000;
	// boal 19.01.2004 -->
	itm.Weight = 6;
	// boal 19.01.2004 <--
	itm.chargeQ = 4;
	itm.chargespeed = 28;
	itm.dmg_min = 30.0;
	itm.dmg_max = 120.0;
	itm.accuracy = 50;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;

    makeref(itm,Items[n]);
	itm.id = "mushket"; //Мушкетон
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_mushket";
	itm.describe = "itmdescr_mushket";
	itm.folder = "items";
	itm.model = "mushket";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_10";
	// boal 19.01.2004 -->
	itm.price = 10000;
	itm.Weight = 13;
	// boal 19.01.2004 <--
	itm.chargeQ = 1;
	itm.chargespeed = 10;
	itm.dmg_min = 100.0;
	itm.dmg_max = 300.0;
	itm.accuracy = 70;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;
	
    makeref(itm,Items[n]);
	itm.id = "pistol7";  //шотган
	itm.groupID = GUN_ITEM_TYPE;
	itm.name = "itmname_pistol7";
	itm.describe = "itmdescr_pistol7";
	itm.folder = "items";
	itm.model = "pistol7";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_8";
	itm.shown = 1;
	itm.price = 25000;
	// boal 19.01.2004 -->
	itm.Weight = 9;
	// boal 19.01.2004 <--
	itm.chargeQ = 4;
	itm.chargespeed = 52;
	itm.dmg_min = 100.0;
	itm.dmg_max = 300.0;
	itm.accuracy = 80;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "WEAPON";
	n++;
	
	// <<<< ---------------  QUEST ITEMS  --------------------

	makeref(itm,Items[n]);
	itm.id = "gold";
	itm.name = "itmname_gold";
	itm.describe = "itmdescr_gold";
	itm.model = "";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	n++;

	makeref(itm,Items[n]);
	itm.id = "Chest";
	itm.name = "itmname_chest";
	itm.describe = "itmdescr_chest";
	itm.model = "stat1";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_4";
	itm.price = 15000; // квест ростовщика и клады
	itm.Weight = 15; // boal
	n++;

	makeref(itm,Items[n]);
	itm.id = "Coins";
	itm.name = "itmname_coins";
	itm.describe = "itmdescr_coins";
	itm.model = "purse";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
    // boal 19.01.2004 -->
	itm.price = 1000;
	itm.Weight = 0.1;
	//itm.Monster.rare = 0.05;
    //itm.Monster.min = 1;
    //itm.Monster.max = 10;
    // boal 19.01.2004 <--
    itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "icollection";
	itm.name = "itmname_incas_collection";
	itm.describe = "itmdescr_incas_collection";
	itm.model = "";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
	itm.price = 47000;
	// boal 19.01.2004 -->
	itm.Weight = 50;
	// boal 19.01.2004 <--
	n++;

	// boal для генератора -->
	makeref(itm,Items[n]);
	itm.id = "letter_1";
	itm.name = "itmname_letter_1";
	itm.describe = "itmdescr_letter_1";
	itm.model = "";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "letter_2";
	itm.name = "itmname_letter_2";
	itm.describe = "itmdescr_letter_2";
	itm.model = "";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "letter_open";
	itm.name = "itmname_letter_open";
	itm.describe = "itmdescr_letter_open";
	itm.model = "";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

    // boal для генератора <--

	makeref(itm,Items[n]); //записка
	itm.id = "letter_notes";
	itm.name = "itmname_letter_notes";
	itm.describe = "itmdescr_letter_notes";
	itm.model = "";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]); //вексель
	itm.id = "Order";
	itm.name = "itmname_order";
	itm.describe = "itmdescr_order";
	itm.model = "";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_8";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "letter_Albermal";
	itm.name = "itmname_letter_Albermal";
	itm.describe = "itmdescr_letter_Albermal";
	itm.model = "";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_12";
	itm.price = 0;
	itm.Weight = 0.1;
	n++; 

    // eddy торговые грамоты <--
	makeref(itm,Items[n]);
	itm.id = "EngTradeLicence";
	itm.name = "itmname_EngTradeLicence";
	itm.describe = "itmdescr_EngTradeLicence";
	itm.model = "";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "FraTradeLicence";
	itm.name = "itmname_FraTradeLicence";
	itm.describe = "itmdescr_FraTradeLicence";
	itm.model = "";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "SpaTradeLicence";
	itm.name = "itmname_SpaTradeLicence";
	itm.describe = "itmdescr_SpaTradeLicence";
	itm.model = "";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "HolTradeLicence";
	itm.name = "itmname_HolTradeLicence";
	itm.describe = "itmdescr_HolTradeLicence";
	itm.model = "";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]); //общая верительная грамота для губеров.
	itm.id = "Trust";
	itm.name = "itmname_Trust";
	itm.describe = "itmdescr_Trust";
	itm.model = "";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_9";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;
	// eddy торговые грамоты <--

	makeref(itm,Items[n]); //секретная депеша по квесту мэра
	itm.id = "sicretLetter";
	itm.name = "itmname_sicretLetter";
	itm.describe = "itmdescr_sicretLetter";
	itm.model = "";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_12";
//	itm.shown = 0;
	itm.price = 0;
	// boal 19.01.2004 -->
	itm.Weight = 0.1;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "LegransMap";
	itm.name = "itmname_LegransMap";
	itm.describe = "itmdescr_LegransMap";
	itm.model = "";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_10";
	itm.price = 0;
	// boal 19.01.2004 -->
	itm.Weight = 0.1;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;  

    makeref(itm,Items[n]);
	itm.id = "Bible";
	itm.name = "itmname_bible";
	itm.describe = "itmdescr_bible";
	itm.model = "";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_9";
//	itm.shown = 0;
	itm.price = 0;
	// boal 19.01.2004 -->
	itm.Weight = 2;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;

    makeref(itm,Items[n]);
	itm.id = "ShipsJournal";
	itm.name = "itmname_ShipsJournal";
	itm.describe = "itmdescr_ShipsJournal";
	itm.model = ""; 
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_4";
//	itm.shown = 0;
	itm.price = 0;
	// boal 19.01.2004 -->
	itm.Weight = 2;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;
	
	// ==> Предметы по квесту Аскольда
    makeref(itm,Items[n]);
	itm.id = "Powder_mummie";
	itm.name = "itmname_Powder_mummie";
	itm.describe = "itmdescr_Powder_mummie";
	itm.model = "pursel";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_12";
	itm.shown = 0;
	// boal 19.01.2004 -->
	itm.Weight = 0.5;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;
	
    makeref(itm,Items[n]); // Крест Антониу де Соуза
	itm.id = "DeSouzaCross";
	itm.name = "itmname_DeSouzaCross";
	itm.describe = "itmdescr_DeSouzaCross";
	itm.model = "sapphire";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_12";
	// boal 19.01.2004 -->
	itm.Weight = 0.3;
	itm.price = 0;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "DOjeronRing";
	itm.name = "itmname_DOjeronRing";
	itm.describe = "itmdescr_DOjeronRing";
	itm.model = "RingJew";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_9";
	itm.Weight = 0.3;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "BlackLabel";
	itm.name = "itmname_BlackLabel";
	itm.describe = "itmdescr_BlackLabel";
	itm.model = "";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_8";
	itm.Weight = 0.1;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "Term_arm";
	itm.name = "itmname_Term_arm";
	itm.describe = "itmdescr_Term_arm";
	itm.model = "pursel";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_12";
	itm.price = 0;
	itm.Weight = 5.5;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "Malyava";
	itm.name = "itmname_Malyava";
	itm.describe = "itmdescr_Malyava";
	itm.model = "";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_12";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "PortmansBook";
	itm.name = "itmname_PortmansBook";
	itm.describe = "itmdescr_PortmansBook";
	itm.model = "";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_1";
	itm.price = 0;
	itm.Weight = 0.2;
	itm.ItemType = "QUESTITEMS";
	n++;

	////////////////////////////////////////////////////////////////////////////////////////////		
	//здесь начинаются квестовые предметы, заюзаные в локаторах item
	//оформлять эти предметы ТОЛЬКО ЗДЕСЬ, до конца сектора квестовых предметов
	////////////////////////////////////////////////////////////////////////////////////////////

	ItemsForLocators_start = n; //начальный номер для спец. предметов

	makeref(itm,Items[n]);
	itm.id = "Rock_letter";
	itm.name = "itmname_Rock_letter";
	itm.describe = "itmdescr_Rock_letter";
	itm.model = "StoneMap";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_4";
	itm.shown = 0; //сначала скрижали невидимы, т.е. взять их нельзя до прох.квеста Аскольда.
	string PutLocation, PutLocator;   //рендом скрижалей.
    int WherePut = rand(0);
    switch(WherePut)
    {
        case 0:
            PutLocation = "Pearl_Grot";
            PutLocator  = "item"+(rand(2)+1);
        break;
        case 1:
            PutLocation = "Caiman_Grot";
            PutLocator  = "item"+(rand(2)+1);
        break;
        case 2:
            PutLocation = "Terks_Grot";
            PutLocator  = "item"+(rand(2)+1);
        break;
        case 3:
            PutLocation = "Dominica_Grot";
            PutLocator  = "item"+(rand(2)+1);
        break;
        case 4:
            PutLocation = "Temple";
            PutLocator  = "item1";
        break;
    }
	itm.startLocation = PutLocation;
	itm.startLocator = PutLocator;
	itm.useLocation = "Guadeloupe_Cave";
	itm.useLocator = "button01";
	itm.price = 0;
	// boal 19.01.2004 -->
	itm.Weight = 0.2;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;  
	
	makeref(itm,Items[n]);
	itm.id = "Azzy_bottle";
	itm.name = "itmname_Azzy_bottle";
	itm.describe = "itmdescr_Azzy_bottle";
	itm.model = "pursel";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_12";
	itm.shown = 1; //до поры локатор не реагирует на заступы
	itm.useLocator = "button01";
	// boal 19.01.2004 -->
	itm.Weight = 2.5;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "ShipyardsMap";
	itm.name = "itmname_ShipyardsMap";
	itm.describe = "itmdescr_ShipyardsMap";
	itm.model = "shipyardsMap";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_12";
	itm.price = 5000;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "MayorsRing";
	itm.name = "itmname_MayorsRing";
	itm.describe = "itmdescr_MayorsRing";
	itm.model = "RingSimGold";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_2";
	itm.Weight = 0.3;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "UsurersJew";
	itm.name = "itmname_UsurersJew";
	itm.describe = "itmdescr_UsurersJew";
	itm.model = "Diamond";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_2";
	itm.Weight = 0.1;
	itm.price = 0;
	itm.JevelType = "бриллиант";
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "OpenBook";
	itm.name = "itmname_OpenBook";
	itm.describe = "itmdescr_OpenBook";
	itm.model = "OpenBook";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_4";
	itm.Weight = 0.2;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	itm.shown = false;
	itm.startLocation = "PuertoPrincipe_townhall";
	itm.startLocator = "item1";
	n++;

	makeref(itm,Items[n]);
	itm.id = "Ladder";
	itm.name = "itmname_Ladder";
	itm.describe = "itmdescr_Ladder";
	itm.model = "Ladder";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_12";
	itm.Weight = 0.2;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "key1";
	itm.name = "itmname_Key1";
	itm.describe = "itmdescr_Key1";
	itm.model = "keysimple";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_12";
	itm.Weight = 0.0;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "key2";
	itm.name = "itmname_Key2";
	itm.describe = "itmdescr_Key2";
	itm.model = "keymiddle";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_12";
	itm.Weight = 0.0;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);
	itm.id = "key3";
	itm.name = "itmname_Key3";
	itm.describe = "itmdescr_Key3";
	itm.model = "keygrand";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_12";
	itm.Weight = 0.0;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);  //ключ от сундука по штурму Панамы
	itm.id = "keyPanama";
	itm.name = "itmname_keyPanama";
	itm.describe = "itmdescr_keyPanama";
	itm.model = "keygrand";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_12";
	itm.shown = 0; //сначала ключ невидим
	itm.startLocation = "Panama_prison";
	itm.startLocator = "item1";
	itm.Weight = 0.0;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);  //ключ для линейки ГПК
	itm.id = "keyQuestLSC";
	itm.name = "itmname_keyQuestLSC";
	itm.describe = "itmdescr_keyQuestLSC";
	itm.model = "keymiddle";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_12";
	itm.shown = 0; //сначала ключ невидим
	itm.startLocation = "Villemstad_houseSp5";
	itm.startLocator = "item2";
	itm.Weight = 0.0;
	itm.price = 0;
	itm.ItemType = "QUESTITEMS";
	n++;

	makeref(itm,Items[n]);	//письмо для линейки ГПК
	itm.id = "letter_LSC";
	itm.name = "itmname_letter_LSC";
	itm.describe = "itmdescr_letter_LSC";
	itm.model = "letter";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_4";
	itm.price = 0;
	itm.Weight = 0.1;
	itm.ItemType = "QUESTITEMS";
	n++;
	
    //homo лекарство по линейке Блада
    makeref(itm,Items[n]);
	itm.id = "migraine_potion"; // лекарство от Мигрени
	itm.name = "itmname_migraine_potion";
	itm.describe = "itmdescr_migraine_potion";
	itm.model = "balsam";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_1";
	itm.shown = 0;
	itm.Weight = 0.4;
	itm.ItemType = "QUESTITEMS";
	n++;
	//homo
    makeref(itm,Items[n]);
	itm.id = "MsStid_ring"; // Колечко
	itm.name = "itmname_MsStid_ring";
	itm.describe = "itmdescr_MsStid_ring";
	itm.model = "RingJew";//pursel
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_7";
	itm.shown = 0;
	itm.Weight = 0.4;
	itm.ItemType = "QUESTITEMS";
	n++;
	//homo
    makeref(itm,Items[n]);
	itm.id = "Griffins_Weapon"; // оружие Гриффина
	itm.name = "itmname_Griffins_Weapon";
	itm.describe = "itmdescr_Griffins_Weapon";
	itm.model = "";
	itm.picIndex =14;
	itm.picTexture = "ITEMS_13";
	itm.shown = 0;
	itm.Weight = 3.4;
	itm.ItemType = "QUESTITEMS";
	n++;
	//homo
    makeref(itm,Items[n]);
	itm.id = "Weapon_for_escape"; //оружие для побега Блада
	itm.name = "itmname_Weapon_for_escape";
	itm.describe = "itmdescr_Weapon_for_escape";
	itm.model = "";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_13";
	itm.shown = 0;
	itm.Weight = 10;
	itm.ItemType = "QUESTITEMS";
	n++;


    makeref(itm,Items[n]);
	itm.id = "Ascold_rabble"; // монтировка
	itm.name = "itmname_Ascold_rabble";
	itm.describe = "itmdescr_Ascold_rabble";
	itm.model = "kocherg";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_12";
	itm.shown = 0;
	itm.useLocation = "Guadeloupe_Cave";
	itm.useLocator = "button02";
	// boal 19.01.2004 -->
	itm.Weight = 1;
	// boal 19.01.2004 <--
	itm.ItemType = "QUESTITEMS";

	ItemsForLocators_end = n; //конечный номер для спец. предметов
	n++;

	// <<<< ---------------  QUEST ITEMS  --------------------

    // КАРТЫ //////////////////////////////////////////////////
    makeref(itm,Items[n]);
	itm.id = "map_bad";       // to del
    itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_bad";
	itm.describe = "itmdescr_map_bad";
	itm.model = "";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_10";
//	itm.shown = 0;
	itm.price = 1000;
	// boal 19.01.2004 -->
	itm.Weight = 0.1;
	itm.imageTga = "map_1";
	itm.imageType = "cabin_map";
	// boal 19.01.2004 <--
    itm.rare = 0.005;
    itm.ItemType = "MAP";
	n++;

    makeref(itm,Items[n]);
	itm.id = "map_normal";
    itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_normal";
	itm.describe = "itmdescr_map_normal";
	itm.model = "";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_9";
//	itm.shown = 0;
	itm.price = 3050;
	// boal 19.01.2004 -->
	itm.Weight = 0.1;
	itm.imageTga = "map_2";
	itm.imageType = "cabin_map";
	// boal 19.01.2004 <--
	itm.rare = 0.001;
	itm.ItemType = "MAP";
	n++;
	
	// boal 270904 -->
	makeref(itm,Items[n]);
	itm.id = "map_bermudas";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_bermudas";
	itm.describe = "itmdescr_map_bermudas";
	itm.model = "";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_11";
	itm.price = 1300;
	itm.Weight = 0.2;
	itm.imageTga = "bermudas";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_jam";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_jam";
	itm.describe = "itmdescr_map_jam";
	itm.model = "";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "Jamaica";
	itm.imageType = "";
	itm.MapIsland = "Redmond";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_cayman";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_cayman";
	itm.describe = "itmdescr_map_cayman";
	itm.model = "";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_11";
	itm.price = 2000;
	itm.Weight = 0.2;
	itm.imageTga = "kayman";
	itm.imageType = "";
	itm.MapIsland = "Cayman";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_barbados";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_barbados";
	itm.describe = "itmdescr_map_barbados";
	itm.model = "";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "barbados";
	itm.imageType = "";
	itm.MapIsland = "Oxbay";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_TORTUGA";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_TORTUGA";
	itm.describe = "itmdescr_map_TORTUGA";
	itm.model = "";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_13";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "TORTUGA";
	itm.imageType = "";
	itm.MapIsland = "Oxbay";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);    ////
	itm.id = "map_Curacao";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_Curacao";
	itm.describe = "itmdescr_map_Curacao";
	itm.model = "";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "curacao";
	itm.imageType = "";
	itm.MapIsland = "Douwesen";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_martiniqua";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_martiniqua";
	itm.describe = "itmdescr_map_martiniqua";
	itm.model = "";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "martinica";
	itm.imageType = "";
	itm.MapIsland = "FalaiseDeFleur";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_dominica";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_dominica";
	itm.describe = "itmdescr_map_dominica";
	itm.model = "";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_11";
	itm.price = 2000;
	itm.Weight = 0.2;
	itm.imageTga = "dominica";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_trinidad";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_trinidad";
	itm.describe = "itmdescr_map_trinidad";
	itm.model = "";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_13";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "trinidad";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_puerto";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_puerto";
	itm.describe = "itmdescr_map_puerto";
	itm.model = "";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_13";
	itm.price = 1200;
	itm.Weight = 0.2;
	itm.imageTga = "p_rico";
	itm.imageType = "";
	itm.MapIsland = "IslaMuelle";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_cuba";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_cuba";
	itm.describe = "itmdescr_map_cuba";
	itm.model = "";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_11";
	itm.price = 6200;
	itm.Weight = 0.2;
	itm.imageTga = "cuba";
	itm.imageType = "";
	itm.MapIsland = "Cuba";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_Pearl";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_Pearl";
	itm.describe = "itmdescr_map_Pearl";
	itm.model = "";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_13";
	itm.price = 6500;
	itm.Weight = 0.2;
	itm.imageTga = "Pearl";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_hisp";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_hisp";
	itm.describe = "itmdescr_map_hisp";
	itm.model = "";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_11";
	itm.price = 5000;
	itm.Weight = 0.2;
	itm.imageTga = "hispaniola";
	itm.imageType = "";
	itm.MapIsland = "Gaity";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_nevis";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_nevis";
	itm.describe = "itmdescr_map_nevis";
	itm.model = "";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_13";
	itm.price = 900;
	itm.Weight = 0.2;
	itm.imageTga = "nevis";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_beliz";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_beliz";
	itm.describe = "itmdescr_map_beliz";
	itm.model = "";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_11";
	itm.price = 2200;
	itm.Weight = 0.2;
	itm.imageTga = "beliz";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);    /////
	itm.id = "map_guad";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_guad";
	itm.describe = "itmdescr_map_guad";
	itm.model = "";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "guadelupa";
	itm.imageType = "";
	itm.MapIsland = "Guadelupa";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);    /////
	itm.id = "map_santa";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_santa";
	itm.describe = "itmdescr_map_santa";
	itm.model = "";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_13";
	itm.price = 1900;
	itm.Weight = 0.2;
	itm.imageTga = "santa";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_antigua";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_antigua";
	itm.describe = "itmdescr_map_antigua";
	itm.model = "";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "antigua";
	itm.imageType = "";
	itm.MapIsland = "Antigua";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_terks";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_terks";
	itm.describe = "itmdescr_map_terks";
	itm.model = "";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_13";
	itm.price = 3000;
	itm.Weight = 0.2;
	itm.imageTga = "terks";
	itm.imageType = "";
	itm.MapIsland = "Terks";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_sm";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_sm";
	itm.describe = "itmdescr_map_sm";
	itm.model = "";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_13";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "s_martin";
	itm.imageType = "";
	itm.MapIsland = "Sent";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_maine_1";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_maine_1";
	itm.describe = "itmdescr_map_maine_1";
	itm.model = "";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_11";
	itm.price = 10000;
	itm.Weight = 0.2;
	itm.imageTga = "maine_1";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_maine_2";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_maine_2";
	itm.describe = "itmdescr_map_maine_2";
	itm.model = "";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_11";
	itm.price = 10000;
	itm.Weight = 0.2;
	itm.imageTga = "maine_2";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_panama";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_panama";
	itm.describe = "itmdescr_map_panama";
	itm.model = "";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_13";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "panama";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_maracaibo";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_maracaibo";
	itm.describe = "itmdescr_map_maracaibo";
	itm.model = "";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "maracaibo";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "map_cumana";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_cumana";
	itm.describe = "itmdescr_map_cumana";
	itm.model = "";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_11";
	itm.price = 1000;
	itm.Weight = 0.2;
	itm.imageTga = "cumana";
	itm.imageType = "";
	itm.MapIsland = "";
	itm.ItemType = "MAP";
	n++;
	
	// boal 270904 <--
	
    // boal карты генератора клада -->
    makeref(itm,Items[n]);
	itm.id = "map_full";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_full";
	itm.describe = "itmdescr_map_full";
	itm.model = "";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_7";
	itm.price = 0; // 0 test
	itm.Weight = 0.4;
	itm.imageTga = "none";
	itm.imageType = "MapRead"; // текстовая форма
	itm.mapType = "Full";
	itm.ItemType = "MAP";
	n++;
	
    makeref(itm,Items[n]);
	itm.id = "map_part1";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_part1";
	itm.describe = "itmdescr_map_part1";
	itm.model = "";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_7";
	itm.price = 0;
	itm.Weight = 0.2;
	itm.imageTga = "none";
	itm.imageType = "MapRead"; // текстовая форма
	itm.mapType = "Part";
	itm.ItemType = "MAP";
	n++;

    makeref(itm,Items[n]);
	itm.id = "map_part2";
	itm.groupID = MAPS_ITEM_TYPE;
	itm.name = "itmname_map_part2";
	itm.describe = "itmdescr_map_part2";
	itm.model = "";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_7";
	itm.price = 0;
	itm.Weight = 0.2;
	itm.imageTga = "none";
	itm.imageType = "MapRead"; // текстовая форма
	itm.mapType = "Part";
	itm.ItemType = "MAP";
	n++;
    // boal карты генератора клада <--


    //  ЗАПОЛНЕНИЕ ТРУПОВ
    ItemDeadStartCount = n;
    trace("Заполнение трупов с предмета "+ItemDeadStartCount);
    
    
    makeref(itm,Items[n]);
	itm.id = STATUE1; //+1
	itm.name = "itmname_statue1";
	itm.describe = "itmdescr_statue1";
	itm.model = "stat1";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_1";
	//itm.shown = false;
	itm.price = 20;
	itm.Weight = 35;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;
	
//SPYGLASSES

	makeref(itm,Items[n]);
	itm.id = "spyglass1";
	itm.groupID = SPYGLASS_ITEM_TYPE;
	itm.name = "itmname_spyglass1";
	itm.describe = "itmdescr_spyglass1";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 50;
	itm.Weight = 1;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.15;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.folder = "items";
	itm.model = "spyglass4";
	itm.scope.texture = "eye_BadTub.tga";
	itm.scope.zoom = 2.5;
	itm.scope.time_activate = 500;
	itm.scope.time_update = 150;
	itm.scope.show.nation = 1;
	itm.scope.show.cannons = 0;
	itm.scope.show.ship_name = 1;
	itm.scope.show.ship_type = 1;
	itm.scope.show.hull = 0;
	itm.scope.show.sail = 0;
	itm.scope.show.crew = 0;
	itm.scope.show.speed = 1;
	itm.scope.show.charge = 0;
	itm.scope.show.cannontype = 0;
	itm.scope.show.captain_skills = 0;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "spyglass2";
	itm.groupID = SPYGLASS_ITEM_TYPE;
	itm.name = "itmname_spyglass2";
	itm.describe = "itmdescr_spyglass2";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_6";
	// boal 19.01.2004 -->
	itm.price = 300;
	itm.Weight = 1.5;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.folder = "items";
	itm.model = "spyglass4";
	itm.scope.texture = "eye_CommonTub.tga";
	itm.scope.zoom = 4.0;
	itm.scope.time_activate = 500;
	itm.scope.time_update = 150;
	itm.scope.show.nation = 1;
	itm.scope.show.cannons = 0;
	itm.scope.show.ship_name = 1;
	itm.scope.show.ship_type = 1;
	itm.scope.show.hull = 0;
	itm.scope.show.sail = 1;
	itm.scope.show.crew = 0;
	itm.scope.show.speed = 1;
	itm.scope.show.charge = 0;
	itm.scope.show.cannontype = 0;
	itm.scope.show.captain_skills = 0;
	itm.minlevel = 1;
	itm.rare = 0.07;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "spyglass3";
	itm.groupID = SPYGLASS_ITEM_TYPE;
	itm.name = "itmname_spyglass3";
	itm.describe = "itmdescr_spyglass3";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_6";
	itm.price = 900;
	// boal 19.01.2004 -->
	itm.Weight = 2;

    itm.Solder_o.rare = 0.15;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    // boal 19.01.2004 <--
	itm.folder = "items";
	itm.model = "spyglass4";
	itm.scope.texture = "eye_GoodTub.tga";
	itm.scope.zoom = 7.0;
	itm.scope.time_activate = 500;
	itm.scope.time_update = 150;
	itm.scope.show.nation = 1;
	itm.scope.show.cannons = 1;
	itm.scope.show.ship_name = 1;
	itm.scope.show.ship_type = 1;
	itm.scope.show.hull = 1;
	itm.scope.show.sail = 1;
	itm.scope.show.crew = 0;
	itm.scope.show.speed = 1;
	itm.scope.show.charge = 0;
	itm.scope.show.cannontype = 0;
	itm.scope.show.captain_skills = 0;
	itm.minlevel = 1;
	itm.rare = 0.05;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "spyglass4";
	itm.groupID = SPYGLASS_ITEM_TYPE;
	itm.name = "itmname_spyglass4";
	itm.describe = "itmdescr_spyglass4";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_1";
	itm.folder = "items";
	itm.model = "spyglass4";
	itm.price = 2500;
	// boal 19.01.2004 -->
	itm.Weight = 4;

    itm.Solder_o.rare = 0.05;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    // boal 19.01.2004 <--
	itm.scope.texture = "eye_BestTub.tga";
	itm.scope.zoom = 10.0;
	itm.scope.time_activate = 500;
	itm.scope.time_update = 150;
	itm.scope.show.nation = 1;
	itm.scope.show.cannons = 1;
	itm.scope.show.ship_name = 1;
	itm.scope.show.ship_type = 1;
	itm.scope.show.hull = 1;
	itm.scope.show.sail = 1;
	itm.scope.show.crew = 1;
	itm.scope.show.speed = 1;
	itm.scope.show.charge = 1;
	itm.scope.show.cannontype = 0;
	itm.scope.show.captain_skills = 1;
	itm.minlevel = 1;
	itm.rare = 0.01;
	itm.ItemType = "SUPPORT";
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "spyglass5";
	itm.groupID = SPYGLASS_ITEM_TYPE;
	itm.name = "itmname_spyglass5";
	itm.describe = "itmdescr_spyglass5";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_1";
	itm.folder = "items";
	itm.model = "spyglass4";
	itm.price = 42500;
	// boal 19.01.2004 -->
	itm.Weight = 7;

    itm.Solder_o.rare = 0.0001;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    // boal 19.01.2004 <--
	itm.scope.texture = "eye_BestTub.tga";
	itm.scope.zoom = 15.0;
	itm.scope.time_activate = 500;
	itm.scope.time_update = 150;
	itm.scope.show.nation = 1;
	itm.scope.show.cannons = 1;
	itm.scope.show.ship_name = 1;
	itm.scope.show.ship_type = 1;
	itm.scope.show.hull = 1;
	itm.scope.show.sail = 1;
	itm.scope.show.crew = 1;
	itm.scope.show.speed = 1;
	itm.scope.show.charge = 1;
	itm.scope.show.cannontype = 1;
	itm.scope.show.captain_skills = 1;
	itm.minlevel = 1;
	itm.rare = 0.0001;
	itm.ItemType = "SUPPORT";
	n++;

//POTIONS

	makeref(itm,Items[n]);
	itm.id = "potion1";
	itm.name = "itmname_potion1";
	itm.describe = "itmdescr_potion1";
	itm.model = "potion";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_1";
//	itm.shown = false;
    // boal 19.01.2004 -->
	itm.price = 100;
	itm.Weight = 0.8;

    itm.Solder_o.rare = 0.3;
    itm.Solder_o.min = 2;
    itm.Solder_o.max = 6;
    
    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 3;
    
    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 4;
 
    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 7;

  // boal 19.01.2004 <--
	itm.potion.pic = 21;
	itm.potion.tex = 0; // battle_interface\useditems1.tga
	itm.potion.health = 40.0;
	itm.minlevel = 0;
	itm.rare = 0.3;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "potion2";
	itm.name = "itmname_potion2";
	itm.describe = "itmdescr_potion2";
	itm.model = "potionbig";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_1";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 250;
	itm.Weight = 1;

    itm.Solder_o.rare = 0.3;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 3;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 3;
 
    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 5;

  // boal 19.01.2004 <--
	itm.potion.pic = 22;
	itm.potion.tex = 0; // battle_interface\useditems1.tga
	itm.potion.health = 150.0;
	itm.potion.health.speed = 10; // скорость выпивания
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "potion3";
	itm.name = "itmname_potion3";
	itm.describe = "itmdescr_potion3";
	itm.model = "Antidote";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_1";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 200;
	itm.Weight = 0.4;

    itm.Solder_o.rare = 0.05;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.potion.pic = 23;
	itm.potion.tex = 0; // battle_interface\useditems2.tga
	itm.potion.antidote = true;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "potion4";
	itm.name = "itmname_potion4";
	itm.describe = "itmdescr_potion4";
	itm.model = "balsam";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_1";
	// boal 19.01.2004 -->
	itm.price = 450;
	itm.Weight = 0.4;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    
    itm.Solder_o.rare = 0.05;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 3;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

     // boal 19.01.2004 <--
	itm.potion.pic = 24;
	itm.potion.tex = 0; // battle_interface\useditems1.tga
	itm.potion.health = 130.0;
	itm.potion.antidote = true;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

//-----------ADDED BY VERRUCKT BROTHEL MOD------------------
	makeref(itm,Items[n]);
	itm.id = "potionrum";
	itm.name = "itmname_potionrum";
	itm.describe = "itmdescr_potionrum";
    itm.model = "balsam";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_1";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 150;
	itm.Weight = 2;

    itm.Solder_o.rare = 0.1;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Solder.rare = 0.2;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.2;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.potion.pic = 25;
	itm.potion.tex = 0; // battle_interface\useditems3.tga
	itm.potion.health = 50.0;
	itm.potion.antidote = true;
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "potionwine";
	itm.name = "itmname_potionwine";
	itm.describe = "itmdescr_potionwine";
    itm.model = "balsam";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_1";
	// boal 19.01.2004 -->
	itm.price = 500;
	itm.Weight = 1;

    itm.Solder_o.rare = 0.3;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.potion.pic = 26;
	itm.potion.tex = 0; // battle_interface\useditems3.tga
	itm.potion.health = 175.0;
	itm.potion.health.speed = 7; // скорость выпивания
	itm.minlevel = 1;
	itm.rare = 0.1;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

    makeref(itm,Items[n]);
	itm.id = "potion5";
	itm.name = "itmname_potion5";
	itm.describe = "itmdescr_potion5";
    itm.model = "balsam";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_7";
	// boal 19.01.2004 -->
	itm.price = 30;
	itm.Weight = 0.4;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 9;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 9;
    // boal 19.01.2004 <--
	itm.potion.pic = 27;
	itm.potion.tex = 0; // battle_interface\useditems3.tga
	itm.potion.health = 25.0;
	itm.minlevel = 1;
	itm.rare = 0.2;
	itm.SortIndex = 2;
	itm.ItemType = "SUPPORT";
	n++;

//JEWELRY

	makeref(itm,Items[n]);
	itm.id = "jewelry1";
	itm.name = "itmname_jewelry1";
	itm.describe = "itmdescr_jewelry1";
	itm.model = "sapphire";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
    // boal 19.01.2004 -->
	itm.price = 50;
	itm.Weight = 0.2;

    itm.Solder_o.rare = 0.1; 
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 4;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 4;
    
    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    
    itm.Citizen.rare = 0.05;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.05;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry2";
	itm.name = "itmname_jewelry2";
	itm.describe = "itmdescr_jewelry2";
	itm.model = "diamond";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 700;
	itm.Weight = 0.2;

    itm.Solder_o.rare = 0.1;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 4;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry3";
	itm.name = "itmname_jewelry3";
	itm.describe = "itmdescr_jewelry3";
	itm.model = "ruby";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 550;
	itm.Weight = 0.2;

    itm.Solder_o.rare = 0.1;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 4;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry4";
	itm.name = "itmname_jewelry4";
	itm.describe = "itmdescr_jewelry4";
	itm.model = "emerald";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 500;
	itm.Weight = 0.2;

    itm.Solder_o.rare = 0.1;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 4;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry5";  // Gold
	itm.name = "itmname_jewelry5";
	itm.describe = "itmdescr_jewelry5";
	itm.model = "sapphire";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 240;
	itm.Weight = 0.7;

    itm.Solder_o.rare = 0.2;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 4;

    itm.Monster.rare = 0.3;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 2;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry6";
	itm.name = "itmname_jewelry6";
	itm.describe = "itmdescr_jewelry6";
	itm.model = "sapphire";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 100;
	itm.Weight = 0.1;

    itm.Solder.rare = 0.01;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.3;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry7";
	itm.name = "itmname_jewelry7";
	itm.describe = "itmdescr_jewelry7";
	itm.model = "sapphire";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 200;
	itm.Weight = 0.1;

    itm.Solder_o.rare = 0.1;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry8"; // +1 sneak
	itm.name = "itmname_jewelry8";
	itm.describe = "itmdescr_jewelry8";
	itm.model = "sapphire";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 10;
	itm.Weight = 0.1;
    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    
    itm.Warrior.rare = 0.15;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.15;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.25;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.2;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry9"; // +1 sneak
	itm.name = "itmname_jewelry9";
	itm.describe = "itmdescr_jewelry9";
	itm.model = "sapphire";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 80;
	itm.Weight = 0.2;

    itm.Solder.rare = 0.01;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Warrior.rare = 0.03;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.2;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.2;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry10";
	itm.name = "itmname_jewelry10";
	itm.describe = "itmdescr_jewelry10";
	itm.model = "emerald";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 170;
	itm.Weight = 0.1;

    itm.Solder_o.rare = 0.2;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    
    itm.Monster.rare = 0.3;
    itm.Monster.min = 1;
    itm.Monster.max = 1;


    itm.Warrior.rare = 0.02;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.3;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry11";
	itm.name = "itmname_jewelry11";
	itm.describe = "itmdescr_jewelry11";
	itm.model = "pearlbig";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 90;
	itm.Weight = 0.3;

    itm.Solder_o.rare = 0.2;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 5;

    itm.Monster.rare = 0.2;
    itm.Monster.min = 1;
    itm.Monster.max = 6;


    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 6;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 5;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 5;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry12";
	itm.name = "itmname_jewelry12";
	itm.describe = "itmdescr_jewelry12";
	itm.model = "pearlsmall";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 40;
	itm.Weight = 0.1;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 5;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 20;

    itm.Citizen.rare = 0.15;
    itm.Citizen.min = 1;
    itm.Citizen.max = 15;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 5;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.2;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry13";
	itm.name = "itmname_jewelry13";
	itm.describe = "itmdescr_jewelry13";
	itm.model = "sapphire";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 150;
	itm.Weight = 0.3;

    itm.Solder_o.rare = 0.05;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Monster.rare = 0.01;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 2;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry14";
	itm.name = "itmname_jewelry14";
	itm.describe = "itmdescr_jewelry14";
	itm.model = "sapphire";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 200;
	itm.Weight = 0.1;

    itm.Solder_o.rare = 0.05;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Monster.rare = 0.01;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 2;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry15"; // +1 sneak
	itm.name = "itmname_jewelry15";
	itm.describe = "itmdescr_jewelry15";
	itm.model = "sapphire";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 300;
	itm.Weight = 0.5;

    itm.Solder_o.rare = 0.01;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Monster.rare = 0.02;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "jewelry16";
	itm.name = "itmname_jewelry16";
	itm.describe = "itmdescr_jewelry16";
	itm.model = "sapphire";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_2";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 10;
	itm.Weight = 0.3;

    itm.Monster.rare = 0.01;
    itm.Monster.min = 1;
    itm.Monster.max = 4;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 2;

    itm.Citizen_f.rare = 0.4;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 2;
	itm.rare = 0.05;
	n++;

    makeref(itm,Items[n]);
	itm.id = "jewelry17";  // серебро
	itm.name = "itmname_jewelry17";
	itm.describe = "itmdescr_jewelry17";
	itm.model = "pursel";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 110;
	itm.Weight = 0.5;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 5;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 3;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

    makeref(itm,Items[n]);
	itm.id = "jewelry18";
	itm.name = "itmname_jewelry18";
	itm.describe = "itmdescr_jewelry18";
	itm.model = "pursel";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 330;
	itm.Weight = 0.1;

    itm.Solder_o.rare = 0.2;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen_f.rare = 0.25;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 0;
	itm.rare = 0.1;
	n++;

    makeref(itm,Items[n]);
	itm.id = "bullet"; // boal это пуля "mineral1";
	itm.name = "itmname_bullet"; //"itmname_mineral1";
	itm.describe = "itmdescr_bullet";//"itmdescr_mineral1";
	itm.model = "pursel";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_8";
//	itm.shown = false;
	itm.price = 4;//560;
	itm.Weight = 0.1;
	itm.minlevel =0;
	itm.rare = 0.3;
	itm.SortIndex = 1;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral2";
	itm.name = "itmname_mineral2";
	itm.describe = "itmdescr_mineral2";
	itm.model = "sapphire";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_8";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 300;
	itm.Weight = 6;

    itm.Solder.rare = 0.01;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    
    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.02;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral3";
	itm.name = "itmname_mineral3";
	itm.describe = "itmdescr_mineral3";
	itm.model = "sapphire";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_8";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 10;
	itm.Weight = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 4;
    
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral4"; // баклан
	itm.name = "itmname_mineral4";
	itm.describe = "itmdescr_mineral4";
	itm.model = "pursel"; // чтоб был виден
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_8";
//	itm.shown = false;
	itm.price = 25;
	// boal 19.01.2004 -->
	itm.Weight = 2;

    itm.Solder.rare = 0.05;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    
    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.05;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.05;
	n++;

    makeref(itm,Items[n]);
	itm.id = "mineral5";
	itm.name = "itmname_mineral5";
	itm.describe = "itmdescr_mineral5";
	itm.model = "pursel";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 2;
	itm.Weight = 3;

    itm.Monster.rare = 0.01;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.02;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral6";
	itm.name = "itmname_mineral6";
	itm.describe = "itmdescr_mineral6";
	itm.model = "pursel";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 20;
	itm.Weight = 1;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.3;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.001;
    itm.Warrior.min = 1;
    itm.Warrior.max = 5;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 5;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.4;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral7";
	itm.name = "itmname_mineral7";
	itm.describe = "itmdescr_mineral7";
	itm.model = "pursel";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 140;
	itm.Weight = 0.8;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.3;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    
    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.2;
	n++;

    makeref(itm,Items[n]);
	itm.id = "mineral8";
	itm.name = "itmname_mineral8";
	itm.describe = "itmdescr_mineral8";
	itm.model = "pursel";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_9";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 30;
	itm.Weight = 1;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =4;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral9";
	itm.name = "itmname_mineral9";
	itm.describe = "itmdescr_mineral9";
	itm.model = "pursel";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_9";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 80;
	itm.Weight = 1;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.2;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel =2;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "mineral10";
	itm.name = "itmname_mineral10";
	itm.describe = "itmdescr_mineral10";
	itm.model = "pursel";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_9";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 4;
	itm.Weight = 1;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 5;

    itm.Monster.rare = 0.001;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.001;
    itm.Warrior.min = 1;
    itm.Warrior.max = 5;
    // boal 19.01.2004 <--
	itm.minlevel =0;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "sculMa1";
	itm.name = "itmname_sculMa1";
	itm.describe = "itmdescr_sculMa1";
	itm.model = "pursel";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_9";
	itm.price = 5000;
	// boal 19.01.2004 -->
	itm.Weight = 3.5;
	// boal 19.01.2004 <--
	itm.minlevel =5;
	itm.rare = 0.01;
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "sculMa2";
	itm.name = "itmname_sculMa2";
	itm.describe = "itmdescr_sculMa2";
	itm.model = "pursel";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_9";
	itm.price = 7000;
	// boal 19.01.2004 -->
	itm.Weight = 3.5;
	// boal 19.01.2004 <--
	itm.minlevel =1;
	itm.rare = 0.01;
	n++;

	makeref(itm,Items[n]);
	itm.id = "sculMa3";
	itm.name = "itmname_sculMa3";
	itm.describe = "itmdescr_sculMa3";
 	itm.model = "pursel";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_9";
	itm.price = 10000;
	// boal 19.01.2004 -->
	itm.Weight = 3.5;
	// boal 19.01.2004 <--
	itm.minlevel =1;
	itm.rare = 0.01;
	n++;

//INDIAN TRINKETS

	makeref(itm,Items[n]);
	itm.id = "indian1"; // +1 sneak
	itm.name = "itmname_indian1";
	itm.describe = "itmdescr_indian1";
	itm.model = "pursel";
	itm.picIndex = 1;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 30;
	itm.Weight = 3;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.03;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.001;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.005;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian2";
	itm.name = "itmname_indian2";
	itm.describe = "itmdescr_indian2";
	itm.model = "pursel";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 40;
	itm.Weight = 1;

    itm.Solder.rare = 0.02;
    itm.Solder.min = 1;
    itm.Solder.max = 1;

    itm.Monster.rare = 0.03;
    itm.Monster.min = 1;
    itm.Monster.max = 3;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian3";
	itm.name = "itmname_indian3";
	itm.describe = "itmdescr_indian3";
	itm.model = "pursel";
	itm.picIndex = 3;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 100;
	itm.Weight = 0.6;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    
    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;
    
    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian4";
	itm.name = "itmname_indian4";
	itm.describe = "itmdescr_indian4";
	itm.model = "pursel";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 5;
	itm.Weight = 2;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.2;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian5"; // +1 sneak
	itm.name = "itmname_indian5";
	itm.describe = "itmdescr_indian5";
	itm.model = "sapphire";
	itm.picIndex = 5;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 130;
	itm.Weight = 1.5;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.03;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 3;
	itm.rare = 0.05;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian6";
	itm.name = "itmname_indian6";
	itm.describe = "itmdescr_indian6";
	itm.model = "sapphire";
	itm.picIndex = 6;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 600;
	itm.Weight = 1;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.001;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 2;
	itm.rare = 0.005;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian7"; // +1 sneak
	itm.name = "itmname_indian7";
	itm.describe = "itmdescr_indian7";
	itm.model = "sapphire";
	itm.picIndex = 7;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 150;
	itm.Weight = 3.6;

    itm.Monster.rare = 0.07;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.01;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.01;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 4;
	itm.rare = 0.03;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian8";
	itm.name = "itmname_indian8";
	itm.describe = "itmdescr_indian8";
	itm.model = "sapphire";
	itm.picIndex = 8;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 120;
	itm.Weight = 0.6;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 4;
	itm.rare = 0.07;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian9";
	itm.name = "itmname_indian9";
	itm.describe = "itmdescr_indian9";
	itm.model = "sapphire";
	itm.picIndex = 9;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 20;
	itm.Weight = 1;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.2;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 5;
	itm.rare = 0.03;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian11";  // rats god
	itm.name = "itmname_indian11";
	itm.describe = "itmdescr_indian11";
	itm.model = "";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 5000;
	itm.Weight = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 5;
	itm.rare = 0.001;
	n++;
	
	makeref(itm,Items[n]);
	itm.id = "indian10";
	itm.name = "itmname_indian10";
	itm.describe = "itmdescr_indian10";
	itm.model = "sapphire";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 210;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 7;
	itm.rare = 0.03;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian12";
	itm.name = "itmname_indian12";
	itm.describe = "itmdescr_indian12";
	itm.model = "sapphire";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 1230;
	itm.Weight = 10;

    itm.Monster.rare = 0.3;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.1;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.2;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 7;
	itm.rare = 0.001;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian13";
	itm.name = "itmname_indian13";
	itm.describe = "itmdescr_indian13";
	itm.model = "pursel";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 40;
	itm.Weight = 3;

    itm.Monster.rare = 0.05;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.1;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.1;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian14";
	itm.name = "itmname_indian14";
	itm.describe = "itmdescr_indian14";
	itm.model = "pursel";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 110;
	itm.Weight = 3;

    itm.Monster.rare = 0.1;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.05;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.2;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;

    itm.Solder.rare = 0.001;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 10;
	itm.rare = 0.005;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian15";
	itm.name = "itmname_indian15";
	itm.describe = "itmdescr_indian15";
	itm.model = "sapphire";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 500;
	itm.Weight = 30;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 3;
	itm.rare = 0.005;
	n++;

	makeref(itm,Items[n]);
	itm.id = "indian16";
	itm.name = "itmname_indian16";
	itm.describe = "itmdescr_indian16";
	itm.model = "sapphire";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_5";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 15;
	itm.Weight = 3;

    itm.Monster.rare = 0.001;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.001;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.2;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.2;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;

    itm.Solder.rare = 0.1;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 3;
	itm.rare = 0.05;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian17"; // +1 sneak
	itm.name = "itmname_indian17";
	itm.describe = "itmdescr_indian17";
	itm.model = "sapphire";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_9";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 50;
	itm.Weight = 2;

    itm.Monster.rare = 0.06;
    itm.Monster.min = 1;
    itm.Monster.max = 1;

    itm.Warrior.rare = 0.02;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;

    itm.Citizen.rare = 0.01;
    itm.Citizen.min = 1;
    itm.Citizen.max = 1;

    itm.Citizen_f.rare = 0.1;
    itm.Citizen_f.min = 1;
    itm.Citizen_f.max = 1;

    itm.Solder.rare = 0.01
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.05;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian18";
	itm.name = "itmname_indian18";
	itm.describe = "itmdescr_indian18";
	itm.model = "pursel";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 75;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian19";
	itm.name = "itmname_indian19";
	itm.describe = "itmdescr_indian19";
	itm.model = "pursel";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 950;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian20";
	itm.name = "itmname_indian20";
	itm.describe = "itmdescr_indian20";
	itm.model = "pursel";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 750;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian21";
	itm.name = "itmname_indian21";
	itm.describe = "itmdescr_indian21";
	itm.model = "sapphire";
	itm.picIndex = 15;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 25;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;

    makeref(itm,Items[n]);
	itm.id = "indian22";
	itm.name = "itmname_indian22";
	itm.describe = "itmdescr_indian22";
	itm.model = "pursel";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_7";
//	itm.shown = false;
	// boal 19.01.2004 -->
	itm.price = 5;
	itm.Weight = 2;

    itm.Monster.rare = 0.005;
    itm.Monster.min = 1;
    itm.Monster.max = 1;
    // boal 19.01.2004 <--
	itm.minlevel = 1;
	itm.rare = 0.001;
	n++;

    // одежда 07.03.05
	makeref(itm,Items[n]);
	itm.id = "suit_1";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = true;
	itm.name = "itmname_suit_1";
	itm.describe = "itmdescr_suit_1";
	itm.folder = "items";
	itm.model = "5";
	itm.picIndex = 4;
	itm.picTexture = "ITEMS_9";
	itm.price = 20;
	itm.Weight = 6;
	itm.CirassLevel = 0.0;
	itm.minlevel = 20;
	itm.rare = 0.001;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "suit_2";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = true;
	itm.name = "itmname_suit_2";
	itm.describe = "itmdescr_suit_2";
	itm.folder = "items";
	itm.model = "6";
	itm.picIndex = 16;
	itm.picTexture = "ITEMS_1";
	itm.price = 20000;
	itm.Weight = 8;
	itm.CirassLevel = 0.0;
	itm.minlevel = 20;
	itm.rare = 0.001;
	itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "suit_3";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = true;
	itm.name = "itmname_suit_3";
	itm.describe = "itmdescr_suit_3";
	itm.folder = "items";
	itm.model = "7";
	itm.picIndex = 2;
	itm.picTexture = "ITEMS_1";
	itm.price = 150000;
	itm.Weight = 7;
	itm.CirassLevel = 0.0;
	itm.minlevel = 20;
	itm.rare = 0.001;
	itm.ItemType = "SUPPORT";
	n++;
	// броня
    makeref(itm,Items[n]);
	itm.id = "cirass1";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = false;
	itm.name = "itmname_cirass1";
	itm.describe = "itmdescr_cirass1";
	itm.folder = "items";
	itm.model = "1";
	itm.picIndex = 14;
	itm.picTexture = "ITEMS_8";
	itm.price = 7000;
	// boal 19.01.2004 -->
	itm.Weight = 30;
	itm.CirassLevel = 0.10;
	// boal 19.01.2004 <--
	itm.minlevel = 2;
	itm.rare = 0.01;

    itm.Solder.rare = 0.01;
    itm.Solder.min = 1;
    itm.Solder.max = 1;
    
    itm.Warrior.rare = 0.01;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "cirass2";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = false;
	itm.name = "itmname_cirass2";
	itm.describe = "itmdescr_cirass2";
	itm.folder = "items";
	itm.model = "2";
	itm.picIndex = 11;
	itm.picTexture = "ITEMS_8";
	itm.price = 15000;
	// boal 19.01.2004 -->
	itm.Weight = 25;
	itm.CirassLevel = 0.18;
	// boal 19.01.2004 <--
	itm.minlevel = 5;
	itm.rare = 0.1;

    itm.Solder_o.rare = 0.01;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;

    itm.Warrior.rare = 0.005;
    itm.Warrior.min = 1;
    itm.Warrior.max = 1;
    itm.ItemType = "SUPPORT";
	n++;

	makeref(itm,Items[n]);
	itm.id = "cirass4";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = false;
	itm.name = "itmname_cirass4";
	itm.describe = "itmdescr_cirass4";
	itm.folder = "items";
	itm.model = "3";
	itm.picIndex = 12;
	itm.picTexture = "ITEMS_8";
	itm.price = 19000;
	// boal 19.01.2004 -->
	itm.Weight = 20;
	itm.CirassLevel = 0.22;
	// boal 19.01.2004 <--
	itm.minlevel = 11;
	itm.rare = 0.1;
	
	itm.Solder_o.rare = 0.003;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    itm.ItemType = "SUPPORT";
	n++;

    makeref(itm,Items[n]);
	itm.id = "cirass3";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = false;
	itm.name = "itmname_cirass3";
	itm.describe = "itmdescr_cirass3";
	itm.folder = "items";
	itm.model = "3";
	itm.picIndex = 13;
	itm.picTexture = "ITEMS_8";
	itm.price = 22000;
	// boal 19.01.2004 -->
	itm.Weight = 23;
	itm.CirassLevel = 0.27;
	// boal 19.01.2004 <--
	itm.minlevel = 9;
	itm.rare = 0.1;

    itm.Solder_o.rare = 0.003;
    itm.Solder_o.min = 1;
    itm.Solder_o.max = 1;
    itm.ItemType = "SUPPORT";
    n++;
            
	makeref(itm,Items[n]);
	itm.id = "cirass5";
	itm.groupID = CIRASS_ITEM_TYPE;
	itm.Clothes = false;
	itm.name = "itmname_cirass5";
	itm.describe = "itmdescr_cirass5";
	itm.folder = "items";
	itm.model = "4";
	itm.picIndex = 10;
	itm.picTexture = "ITEMS_8";
	itm.price = 30000;
	// boal 19.01.2004 -->
	itm.Weight = 17;
	itm.CirassLevel = 0.35;
	// boal 19.01.2004 <--
	itm.minlevel = 20;
	itm.rare = 0.001;
	itm.ItemType = "SUPPORT";
	n++;

    trace("Всего предметов (размерность массива) "+n);
	trace("Начальный специальный предмет: " + Items[ItemsForLocators_start].id);
	trace("Конечный специальный предмет: " + Items[ItemsForLocators_end].id);
	return n;
}

void InitButtons()
{
	/*aref lab3;
	makearef(lab3, Locations[FindLocation("Labirint_3")]);

	lab3.items.b_1.locator = "button01";
	lab3.items.b_1.model   = "b1";
	lab3.items.b_1.itemDeltaY  = 0.0;
	lab3.items.b_1.deltaY  = -0.05;
	lab3.items.b_1.trigged  = false;
	*/
}

void InitRandItems()
{
	ref rnditem;

	makeref(rnditem,RandItems[0]);
	rnditem.id = POTION;
	rnditem.maxQuantity = 4;

	
    makeref(rnditem,RandItems[1]);
	rnditem.id = "Mineral9";  
	rnditem.maxQuantity = 2;

	makeref(rnditem,RandItems[2]);
	rnditem.id = "Mineral10";  
	rnditem.maxQuantity = 2;
	
	makeref(rnditem,RandItems[3]);
	rnditem.id = "jewelry16";
	rnditem.maxQuantity = 15;
	
	makeref(rnditem,RandItems[4]);
	rnditem.id = "jewelry1";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[5]);
	rnditem.id = "jewelry2";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[6]);
	rnditem.id = "jewelry3";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[7]);
	rnditem.id = "jewelry5";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[8]);
	rnditem.id = "jewelry6";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[9]);
	rnditem.id = "jewelry7";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[10]);
	rnditem.id = "jewelry10";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[11]);
	rnditem.id = "jewelry11";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[12]);
	rnditem.id = "jewelry12";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[13]);
	rnditem.id = "jewelry13";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[14]);
	rnditem.id = "potion1";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[15]);
	rnditem.id = "indian4";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[16]);
	rnditem.id = "potion1";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[17]);
	rnditem.id = "potion2";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[18]);
	rnditem.id = "indian13";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[19]);
	rnditem.id = "potion3";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[20]);
	rnditem.id = "potion4";
	rnditem.maxQuantity = 4;

    // boal -->
	makeref(rnditem,RandItems[21]);
	rnditem.id = "pistol3";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[22]);
	rnditem.id = "pistol2";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[23]);
	rnditem.id = "pistol1";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[24]);
	rnditem.id = "blade8";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[25]);
	rnditem.id = "blade7";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[26]);
	rnditem.id = "blade6";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[27]);
	rnditem.id = "blade5";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[28]);
	rnditem.id = "blade4";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[29]);
	rnditem.id = "blade3";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[30]);
	rnditem.id = "blade2";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[31]);
	rnditem.id = "spyglass1";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[32]);
	rnditem.id = "spyglass2";
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[33]);
	rnditem.id = "blade1";
	rnditem.maxQuantity = 1;

    makeref(rnditem,RandItems[34]);
	rnditem.id = "jewelry17";
	rnditem.maxQuantity = 4;

    makeref(rnditem,RandItems[35]);
	rnditem.id = "jewelry18";
	rnditem.maxQuantity = 4;

    makeref(rnditem,RandItems[36]);
	rnditem.id = "bullet"; // boal "Mineral1";
	rnditem.maxQuantity = 20;

	makeref(rnditem,RandItems[37]);
	rnditem.id = "mineral3";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[38]);
	rnditem.id = "Mineral4";  
	rnditem.maxQuantity = 1;

    makeref(rnditem,RandItems[39]);
	rnditem.id = "Mineral5";  
	rnditem.maxQuantity = 1;

	makeref(rnditem,RandItems[40]);
	rnditem.id = "Mineral6";  
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[41]);
	rnditem.id = "Mineral7";  
	rnditem.maxQuantity = 1;

    makeref(rnditem,RandItems[42]);
	rnditem.id = "potion5";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[43]);
	rnditem.id = "potionwine";
	rnditem.maxQuantity = 4;

	makeref(rnditem,RandItems[44]);
	rnditem.id = "potionrum";
	rnditem.maxQuantity = 4;
	// boal <--
}