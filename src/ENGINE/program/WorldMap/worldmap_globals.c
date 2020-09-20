

#define WDM_MAP_TO_SEA_SCALE				50
#define WDM_MAP_ENCOUNTERS_TO_SEA_SCALE		25	// boal	18.02.06 80 - это много, нужно обязательнй бой, когда догнали	// 80 VANO был тут, чисто для того чтобы корабли ближе появлялись в море, а то они на горизонте тусуются

#define WDM_NONE_ISLAND				""

#define WDM_ETYPE_MERCHANT	0
#define WDM_ETYPE_FOLLOW	1
#define WDM_ETYPE_WARRING	2


object worldMap;

//=========================================================================================
//Quest functions  - Квестовые энкаунтеры. Все эвенты приходят с параметром characterID
//=========================================================================================

//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//торговец - в случае успешного достижения пункта назначения придёт эвент Map_TraderSucces
void Map_CreateTrader(string beginlocator, string endLocator, string characterID, int TimeOut)  //boal
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID); //boal fix
	encField.type = "trader";
	encField.characterID = characterID;
	encField.beginlocator = beginlocator;
	encField.endLocator = endLocator;
	encField.TimeOut = TimeOut; //boal
	worldMap.addQuestEncounters = "updateQuest";
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		pchar.worldmap.shipcounter = 0;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) + 1;
}

// boal 04/10/06
//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//торговец - в случае успешного достижения пункта назначения придёт эвент Map_TraderSucces
void Map_CreateTraderXZ(float x1, float z1, float x2, float z2, string characterID, int TimeOut)  
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID); 
	encField.type = "trader";
	encField.XZGoto = true;
	encField.characterID = characterID;
	encField.x1 = x1;
	encField.z1 = z1;
	encField.x2 = x2;
	encField.z2 = z2;
	encField.TimeOut = TimeOut; //boal
	worldMap.addQuestEncounters = "updateQuest";
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		pchar.worldmap.shipcounter = 0;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) + 1;
}

//военный (догоняющий). TimeOut в днях, если -1, то неиспользуеться.
//При таймауте придёт эвент Map_WarriorEnd
void Map_CreateWarrior(string beginLocator, string characterID, int TimeOut)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	encField.type = "warrior";
	encField.characterID = characterID;
	encField.beginlocator = beginlocator;
	encField.TimeOut = TimeOut;
	worldMap.addQuestEncounters = "updateQuest";
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		pchar.worldmap.shipcounter = 0;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) + 1;
}

//воюющий. TimeOut в днях. При таймауте придёт эвент Map_BattleEnd
//если же игрок вошел в этот энкаунтер до истечения таймаута удалиться энкоунтер из карты
void Map_CreateBattle(string characterID, int iEnemyNation, int TimeOut)
{
	aref encField = wdmFindOrCreateQuestEncounter(characterID);
	encField.type = "battle";
	encField.characterID = characterID;
	encField.iEnemyNation = iEnemyNation;
	encField.TimeOut = TimeOut;
	worldMap.addQuestEncounters = "updateQuest";
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		pchar.worldmap.shipcounter = 0;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) + 1;
}

//Удалить квестового энкоунтера
void Map_ReleaseQuestEncounter(string characterID)
{
	//Просматриваем список создаваемых
	aref encs, at;
	string aname;
	makearef(encs, worldMap.addQuestEncounters);
	int num = GetAttributesNum(encs);
	for(int cnt = 0; cnt < num; cnt++)
	{
		for(int i = 0; i < num; i++)
		{
			at = GetAttributeN(encs, i);
			aname = GetAttributeName(at);
			aname = "addQuestEncounters." + aname + ".characterID";
			if(worldMap.(aname) == characterID)
			{
				DeleteAttribute(&worldMap, aname);
				break;
			}
		}
		if(i >= num)
		{
			break;
		}
	}
	//Просматриваем список созданных
	makearef(encs, worldMap.encounters);
	num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref enc = GetAttributeN(encs, i);
		if(CheckAttribute(enc, "quest.chrID") == 0) continue;
		if(enc.quest.chrID == characterID)
		{
			wdmDeleteLoginEncounter(GetAttributeName(enc));
		}
        num--; //fix
        i--;
	}
	worldMap.deleteUpdate = "";
}

//=========================================================================================
//Util functions
//=========================================================================================

//Storm functions

int wdmGetNumberStorms()
{
	return MakeInt(worldMap.storm.num);
}

//Ship encounter functions

int wdmGetNumberShipEncounters()
{
	return MakeInt(worldMap.encounter.num);
}

bool wdmSetCurrentShipData(int shipIndex)
{
	worldMap.encounter.cur = shipIndex;
	int i = MakeInt(worldMap.encounter.cur);
	if(i < 0 || i != shipIndex) return 0;
	return 1;
}

//Создание энкоунтеров...

//Торговец
bool wdmCreateMerchantShip(float kSpeed)
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_MERCHANT, worldMap.island, &i1, &i2) == false) return false;
	//Создаём энкоунтера
	string encID = "";
	bool res = wdmCreateMerchantShipByIndex(kSpeed, i1, &encID, "", "", 5+rand(5)); //boal new //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Торговец
bool wdmCreateMerchantShipByIndex(float kSpeed, int index, ref encID, string from, string to, int timeOutInDays)  // boal
{
	//Нация энкоунтера (моделька)
	string nationShipName = wdmEncounterModelName(index);
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	// boal правки в ядре -->
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); 
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lsssff", MSG_WORLDMAP_CREATEENC_MER, nationShipName, from, to, kSpeed, timeOutInSec);
	// boal <--
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	//Вернём идентификатор
	encID = worldMap.EncounterID1;
	return res;
}
// boal 04/10/06
bool wdmCreateMerchantShipXZByIndex(float kSpeed, int index, ref encID, float x1, float z1, float x2, float z2, int timeOutInDays)  
{
	//Нация энкоунтера (моделька)
	string nationShipName = wdmEncounterModelName(index);
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); 
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lsffffff", MSG_WORLDMAP_CREATEENC_MER_XZ, nationShipName, x1, z1, x2, z2, kSpeed, timeOutInSec);
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	//Вернём идентификатор
	encID = worldMap.EncounterID1;
	return res;
}

//Преследователь
bool wdmCreateFollowShip(float kSpeed)
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_FOLLOW, worldMap.island, &i1, &i2) == false) return false;
	//Создаём энкоунтера
	string encID = "";
	bool res = wdmCreateFollowShipByIndex(kSpeed, i1, &encID, 5+rand(5)); //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Преследователь
bool wdmCreateFollowShipByIndex(float kSpeed, int index, ref encID, int timeOutInDays)
{
	//Нация энкоунтера
	string nationShipName = wdmEncounterModelName(index);
	//проверим нацию
	ref n = GetMapEncounterRef(index);
	int iNation = PIRATE;
	if(CheckAttribute(n, "nation"))
	{
		iNation = sti(n.Nation);
	}
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = false;
	if(GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY)
	{
		res = SendMessage(&worldMap, "lsssff", MSG_WORLDMAP_CREATEENC_MER, nationShipName, "", "", kSpeed, timeOutInSec); // boal new
	}
	else
	{   // boal - код этот полный абзац, я Вам, господа акеловцы, аплодирую - метод Следовать дает убегание - и так весь код. плакаль
		// погано, что метод имеет право быть, но название должно быть иным, нужно чтоб мирные не доставали патрулями.
		res = SendMessage(&worldMap, "lsff", MSG_WORLDMAP_CREATEENC_FLW, nationShipName, kSpeed, timeOutInSec);
	}
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	encID = worldMap.EncounterID1;
	return res;
}

//Преследователь НАСТОЯЩИЙ
bool wdmCreateRealFollowShipByIndex(float kSpeed, int index, ref encID, int timeOutInDays)
{
	//Нация энкоунтера
	string nationShipName = wdmEncounterModelName(index);
	//проверим нацию
	ref n = GetMapEncounterRef(index);
	int iNation = PIRATE;
	if(CheckAttribute(n, "nation"))
	{
		iNation = sti(n.Nation);
	}
	//Создадим ссылку на атрибуты
	ref mapEncSlotRef = GetMapEncounterRef(index);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = false;
	/*if(GetNationRelation2MainCharacter(iNation) != RELATION_ENEMY)
	{
		res = SendMessage(&worldMap, "lsssf", MSG_WORLDMAP_CREATEENC_MER, nationShipName, "", "", kSpeed);
	}
	else
	{ */  // boal - код этот полный абзац, я Вам, господа акеловцы, аплодирую - метод Следовать дает убегание - и так весь код. плакаль
		res = SendMessage(&worldMap, "lsff", MSG_WORLDMAP_CREATEENC_FLW, nationShipName, kSpeed, timeOutInSec);
	//}
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef, worldMap.EncounterID1);
	encID = worldMap.EncounterID1;
	return res;
}

//Воюющие корабли
bool wdmCreateWarringShips()
{
	//Получим описание энкоунтера в море
	int i1 = -1;
	int i2 = -1;
	if(GenerateMapEncounter(WDM_ETYPE_WARRING, worldMap.island, &i1, &i2) == false) 
	{
		ReleaseMapEncounters();
		return false;
	}
	//Создаём энкоунтера
	string encID1 = "";
	string encID2 = "";
	bool res = wdmCreateWarringShipsByIndex(i1, i2, &encID1, &encID2, 5+rand(5));  //homo 07/10/06
	//Очищаем массив энкоунтеров
	ReleaseMapEncounters();
	return res;
}

//Воюющие корабли
bool wdmCreateWarringShipsByIndex(int index1, int index2, ref encID1, ref encID2, int timeOutInDays)
{
	//Нации энкоунтера
	string nationShipName1 = wdmEncounterModelName(index1);
	string nationShipName2 = wdmEncounterModelName(index2);
	//Создадим ссылки на атрибуты
	ref mapEncSlotRef1 = GetMapEncounterRef(index1);
	ref mapEncSlotRef2 = GetMapEncounterRef(index2);
	//Время жизни в секундах
	float daysPerSec = 24.0/stf(worldMap.date.hourPerSec); // Boal супер бага была - делили на 24. Математику в школе прогуливали?
	float timeOutInSec = daysPerSec*timeOutInDays;
	//Создаём реального энкоунтера
	bool res = SendMessage(&worldMap, "lssf", MSG_WORLDMAP_CREATEENC_WAR, nationShipName1, nationShipName2, timeOutInSec);
	//Копируем данные
	WdmCopyEncounterData(mapEncSlotRef1, worldMap.EncounterID1);
	WdmCopyEncounterData(mapEncSlotRef2, worldMap.EncounterID2);
	encID1 = worldMap.EncounterID1;
	encID2 = worldMap.EncounterID2;
	return res;
}

//Шторм
void wdmCreateStorm()
{
    if(CheckAttribute(pchar,"worldmapencountersoff") == 1)
	{  // boal убиралка шторма
		if(sti(pchar.worldmapencountersoff)) return;
	}
	int isTornado = 0;
	if(worldMap.island == WDM_NONE_ISLAND)
	{
		int r = rand(1000);
		if(r != 0)
		{
			if(r < 450)
			{
				isTornado = 1;
			}
		}
	}
	// тест торнадор
	isTornado = 1;
	// конец теста
	SendMessage(&worldMap, "ll", MSG_WORLDMAP_CREATESTORM, isTornado);
}

//Получить модельку кораблика по индексу нации
string wdmEncounterModelName(int encIndex)
{
	ref n = GetMapEncounterRef(encIndex);
	if(CheckAttribute(n, "worldMapShip") != 0)
	{
		return n.worldMapShip;
	}
	return "ship";
}

//Скопировать данные об энкоутере
void WdmCopyEncounterData(ref mapEncSlotRef, string encStringID)
{
	encStringID = "encounters." + encStringID + ".encdata";
	worldMap.(encStringID) = "";
	aref destRef;
	makearef(destRef, worldMap.(encStringID));
	CopyAttributes(destRef, mapEncSlotRef);
}

//Зарезервировать место для добавления нового энкоунтера
aref wdmCreateNewQuestEncDescription()
{
	string gname, aname;
	aref encs, at;
	makearef(encs, worldMap.addQuestEncounters);
	int num = GetAttributesNum(encs);
	for(int cnt = 0; cnt < num + 10; cnt++)
	{	
		gname = "e" + cnt;
		for(int i = 0; i < num; i++)
		{
			at = GetAttributeN(encs, i);
			aname = GetAttributeName(at);
			if(aname == gname)
			{
				break;
			}
		}
		if(i >= num)
		{
			break;
		}
	}
	aref retVal;
	makearef(retVal, worldMap.addQuestEncounters.(gname));
	return retVal;
}

//// boal & homosapienz
void  wdmEmptyAllDeadQuestEncounter()
{
    aref encs;
    string sdel,aname;
	bool isWMap = IsEntity(worldMap);
	
    makearef(encs, worldMap.encounters);

    int num = GetAttributesNum(encs);
    for(int i = 0; i < num; i++)
    {
        aref enc = GetAttributeN(encs, i);
        if(CheckAttribute(enc, "quest.chrID"))
        {
            int iChar = GetCharacterIndex(enc.quest.chrID)
            sdel  = "encounters."+GetAttributeName(enc);
            if (iChar == -1 || CharacterIsDead(&characters[iChar]))
            {
	            if (!isWMap)
				{
					DeleteAttribute(&worldMap, sdel);
	                num = GetAttributesNum(encs);
	                pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	            }
	            else
	            {
	            	 enc.livetime = 0;
	            }
	        }
        }
    }

   	makearef(encs, worldMap.addQuestEncounters);
    num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref at = GetAttributeN(encs, i);
		sdel = GetAttributeName(at);
		aname = "addQuestEncounters." + sdel + ".characterID";
		if (CheckAttribute(&worldMap, aname))
		{
	        iChar = GetCharacterIndex(worldMap.(aname));
	        if (iChar == -1 || CharacterIsDead(&characters[iChar]))
	        {
	            DeleteAttribute(&worldMap, "addQuestEncounters." + sdel);
	            num = GetAttributesNum(encs);
	            pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	        }
        }
    }
}

// чистка энкоутеров на карте homosapienz
void  wdmEmptyAllOldEncounter()
{
    aref encs;
    string sdel,aname;
	bool isWMap = IsEntity(worldMap);

    makearef(encs, worldMap.encounters);

    int num = GetAttributesNum(encs);
    aref enc;
    int  i;
    
    for (i = 0; i < num; i++)
    {
        enc = GetAttributeN(encs, i);
        if (CheckAttribute(enc, "needDelete") && enc.needDelete == "wdmEncounterDelete")
        {
	        sdel  = "encounters."+GetAttributeName(enc);
			DeleteAttribute(&worldMap, sdel);
	        num--; //fix
	        i--;
	        if (CheckAttribute(enc, "quest")) pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
        }
    }
}
// время жизни на карте для квестовых энкоутеров
void  wdmUpdateAllEncounterLivetime()
{
    aref encs;
    int ihours;
    string sdel;
    float b,k;
    if(!actLoadFlag)
    {
		ihours = GetQuestPastTimeParam("WordMapEncounters_DailyUpdate");
		if (ihours > 0)
		{
            Log_TestInfo("Прошло дней : "+makeint(ihours/24.0));
            float timeOutInSec = ihours/stf(worldMap.date.hourPerSec);

            makearef(encs, worldMap.encounters);
            int num = GetAttributesNum(encs);
            aref enc;
            int  i;

            for (i = 0; i < num; i++)
            {
                enc = GetAttributeN(encs, i);          // только квестовые
                if (CheckAttribute(enc, "livetime") && CheckAttribute(enc, "quest"))
                {
                    Log_TestInfo(enc.livetime+" - "+timeOutInSec+" : "+stf(stf(enc.livetime) - timeOutInSec));
                    /* По идее можно и координату приращивтаь, чтоб не просто время шло, но еще и плыли.
                    k = (enc.gotoz - enc.z)/(enc.gotox - enc.x);
                    b = enc.z - k*enc.x
                    ...
                    */
                    enc.livetime = stf(stf(enc.livetime) - timeOutInSec);
                    DeleteAttribute(enc, "quest.event"); // удаляем  event = Map_TraderSucces
                    if (sti(enc.livetime)<=0)
                    {
                        //Map_ReleaseQuestEncounter(enc.quest.chrID);
               	        sdel  = "encounters."+GetAttributeName(enc);
                        DeleteAttribute(&worldMap, sdel);
                        pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
                    }

                }
            }
		}
	}
}

// найти сущ случайку для НПС

aref  wdmFindOrCreateQuestEncounter(string _chrId)
{
    aref encs;
    string sdel,aname;
	bool isWMap = IsEntity(worldMap);

    makearef(encs, worldMap.encounters);

    int num = GetAttributesNum(encs);
    for(int i = 0; i < num; i++)
    {
        aref enc = GetAttributeN(encs, i);
        if (CheckAttribute(enc, "quest.chrID"))
        {
            int iChar = GetCharacterIndex(enc.quest.chrID)
            sdel  = "encounters."+GetAttributeName(enc);
            if (iChar == -1 || characters[iChar].id == _chrId)
            {
	            if (!isWMap)
				{
					DeleteAttribute(&worldMap, sdel);
	                num = GetAttributesNum(encs);
	                pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	            }
	            else
	            {
	            	 enc.livetime = 0;
	            }
	        }
        }
    }

   	makearef(encs, worldMap.addQuestEncounters);
    num = GetAttributesNum(encs);
	for(i = 0; i < num; i++)
	{
		aref at = GetAttributeN(encs, i);
		sdel = GetAttributeName(at);
		aname = "addQuestEncounters." + sdel + ".characterID";
		if (CheckAttribute(&worldMap, aname))
		{
	        iChar = GetCharacterIndex(worldMap.(aname));
	        if (iChar == -1 || characters[iChar].id == _chrId)
	        {
	            DeleteAttribute(&worldMap, "addQuestEncounters." + sdel);
	            num = GetAttributesNum(encs);
	            pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	        }
        }
    }

	return wdmCreateNewQuestEncDescription();
}