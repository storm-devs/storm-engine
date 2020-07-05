//Создать фантома основываясь на характуристиках данного персонажа в самом дальнем углу
void LAi_GenerateFantomFromMe(aref chr)
{
	//Проверяем возможность генерации фантома в данной локации
	if(!TestRef(loadedLocation)) return;
	if(!IsEntity(loadedLocation)) return;
	if(LAi_LocationIsFantomsGen(loadedLocation) == false) return;
	//Проверяем возможность перерождения персонажа
	if(LAi_CharacterIsReincarnation(chr) == false) return;
	//Проверяем какую модельку использовать для перерождения
	bool isGen = LAi_CharacterReincarnationMode(chr);
	if(CheckAttribute(chr, "model") == false) isGen = true;
	if(CheckAttribute(chr, "model.animation") == false) isGen = true;
	if(chr.model == "") isGen = true;
	if(chr.model.animation == "") isGen = true;
	string model, ani;
	if(isGen)
	{
		int nat = GetLocationNation(loadedLocation)
		if(nat < 0) nat = PIRATE;
		if(nat >= MAX_NATIONS) nat = PIRATE;
		if(CheckAttribute(&Nations[nat], "fantomModel") == false)
		{
			Trace("Nation (Nations[" + nat + "]) by name " + Nations[nat].name + " not include fantoms");
			return;
		}
		aref fantoms;
		makearef(fantoms, Nations[nat].fantomModel);
		int num = GetAttributesNum(fantoms);
		if(num <= 0)
		{
			Trace("Nation (Nations[" + nat + "]) by name " + Nations[nat].name + " not include have fantoms");
			return;
		}
		num = rand(num - 1);
		string m = GetAttributeName(GetAttributeN(fantoms, num));
		model = fantoms.(m);
		ani = "man";
		m = m + ".ani";
		if(CheckAttribute(fantoms, m) != false)
		{
			ani = fantoms.(m);
		}
	}else{
		model = chr.model;
		ani = chr.model.animation;
	}
	//Сохраняем параметры персонажа
	object tmp;
	CopyAttributes(&tmp, chr);
	//Создаём фантома	
	ref fnt = LAi_CreateFantomCharacterEx(model, ani, LAi_CharacterReincarnationGroup(chr), "");
	string curidx = fnt.index;
	//Устанавливаем параметры предыдущего персонажа
	CopyAttributes(fnt, &tmp);
	// boal оружие дать! 19.01.2004 -->
    // фантомы точные клоны SetFantomParam(fnt);  
	//--> eddy. шаг на увеличение ранга фантома.
    if (CheckAttribute(chr, "chr_ai.reincarnation.step"))
	{
		SetFantomParamFromRank(fnt, sti(chr.rank) + sti(chr.chr_ai.reincarnation.step), true); // бравые орлы
		characters[sti(fnt.baseIndex)].rank = fnt.rank; //в структуру прародителя ранг с наворотом
	}
	else //не даем падать рангу от SetFantomParam
	{
	    LAi_NPC_Equip(fnt, sti(fnt.rank), true, true);
	    LAi_SetCurHPMax(fnt);
	}
	// boal 19.01.2004 <--
	if(CheckAttribute(fnt, "chr_ai.group"))
	{
		LAi_group_MoveCharacter(fnt, fnt.chr_ai.group);
	}else{
		LAi_group_MoveCharacter(fnt, "");
	}
	//Сохраняем модельку и анимацию
	fnt.model = model;
	fnt.model.animation = ani;
	//Инициализируем тип
	if(!CheckAttribute(fnt, "chr_ai.type")) fnt.chr_ai.type = "";
	string chrtype = fnt.chr_ai.type;
	SetRandomNameToCharacter(fnt);
	fnt.id = tmp.id;
	fnt.index = curidx;
	LAi_tmpl_stay_InitTemplate(fnt);
	fnt.chr_ai.type = "";
	if(chrtype != "")
	{
		chrtype = "LAi_type_" + chrtype + "_Init";
		call chrtype(fnt);
	}
	SetNewModelToChar(fnt);   // fix
}

bool LAi_CreateEncounters(ref location)
{
	aref grp, st, at;
	ref chr, rCharacter;
	string encGroup, str, locator, sAreal, sCity;
	int num, i, iChar, iNation, iRank, n, iTemp, iMassive;
	string model[10];
	if (!bLandEncountersGen) //если прерывание на локацию, энкаунтеров не генерим
	{		
		bLandEncountersGen = true;
		return false;
	}
	//Можем ли генерить
	if(!CheckAttribute(location, "locators.encdetector") || !CheckNPCQuestDate(location, "Enc_date") || bDisableLandEncounters || CheckAttribute(location, "DisableEncounters")) return false;
	if (CheckAttribute(location, "fastreload")) return false; //отсекаем локации exitTown у пиратских городов
    //boal 02.09.06 пауза случаек на один раз -->
	if (CheckAttribute(pchar, "GenQuest.Enc2Pause"))
	{
	    DeleteAttribute(pchar, "GenQuest.Enc2Pause");
	    return false;
	}
	// boal <--
	//--> если бухта и в ней контра, то не ставим никого
	if (location.type == "seashore") 
	{
		makearef(st, location.models.always);
		num = GetAttributesNum(st);
		for(i = 0; i < num; i++)
		{
			at = GetAttributeN(st, i);	
			str = GetAttributeName(at);
			if(st.(str) == "smg")
			{
				return false;
			}
		}			
	}
	//<-- если бухта и в ней контра, то не ставим никого
	SetNPCQuestDate(location, "Enc_date"); //запись на дату не сегодня
	//Установить нацию патруля 
	sAreal = GiveArealByLocation(location);
	if (sAreal == "none") return false;
	sCity = GetCityNameByIsland(sAreal);
	if (sCity == "none") return false;
	iNation = GetCityNation(sCity);
	if (iNation == -1 || iNation == PIRATE) return false;
	//группа, куда будем помещать фантомов
	encGroup = LAi_FindRandomLocator("encdetector");
	str = "locators." + encGroup;
	makearef(grp, location.(str));
	switch (rand(3))
	{
		//------------------ Банда рейдеров типа дежурит на грабежах ----------------------
		case 0:
			if (rand(10) > 7) return false;
			num = GetAttributesNum(grp) - rand(3); //кол-во человек в банде
			if (num <= 0 ) num = 1; //если локаторов меньше четырех
			str = "Gang"+ location.index + "_";
			//--> генерим ранг 
			if (sti(pchar.rank) > 6) 
			{
				if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*2.5/num);
				else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*1.6/num);
			}
			else  
			{	//казуалам зеленый свет на начало игры
				if (sti(pchar.rank) > 3) iRank = sti(pchar.rank);
				else 
				{
					iRank = 1;
					num = 1;
				}
			}	
			//<-- генерим ранг 
			//Начинаем перебирать локаторы и логинить фантомов
			model[0] = "pirate_1";
			model[1] = "pirate_2";
			model[2] = "pirate_3";
			model[3] = "pirate_4";
			model[4] = "pirate_5";
			model[5] = "pirate_6";
			model[6] = "pirate_7";
			model[7] = "pirate_8";
			model[8] = "pirate_9";
			model[9] = "pirate_10";
			i = 0;
			while(i < num)
			{
				iMassive = rand(9);
				if (model[iMassive] != "")
				{
					chr = GetCharacter(NPC_GenerateCharacter(str + i, model[iMassive], "man", "man", iRank, iNation, 1, true));
					SetFantomParamFromRank(chr, iRank, true);
					//Получим локатор для логина
					locator = GetAttributeName(GetAttributeN(grp, i));
					ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
					chr.dialog.filename = "Enc_Raiders_dialog.c";
					chr.greeting = "Enc_Raiders"; 
					chr.EncQty = num;
					LAi_SetStayType(chr);
					LAi_SetCheckMinHP(chr, LAi_GetCharacterHP(chr)-1, true, "LandEnc_RaidersBeforeDialog");
					LAi_group_MoveCharacter(chr, "RaidersGroup_" + location.index);
					if (i == 0) 			
					{
						sAreal = "Raiders_" + location.index;
						pchar.GenQuest.(sAreal).name = GetFullName(chr); //имя бандита, будет главарем
						pchar.GenQuest.(sAreal).nation = iNation; //нация для слухов 
					}
					i++;
					model[iMassive] = "";
				}
			}
			str = "EncRaiders_" + location.index;
			pchar.quest.(str).win_condition.l1        = "locator";
			pchar.quest.(str).win_condition.l1.location = location.id;
			pchar.quest.(str).win_condition.l1.locator_group = "encdetector";
			pchar.quest.(str).win_condition.l1.locator = encGroup;
			pchar.quest.(str).win_condition = "LandEnc_RaidersBegin";
			pchar.quest.(str).EncQty = num; //кол-во бандитов
			pchar.quest.(str).LocIdx = location.index; 
			str = "TimeRaiders_" + location.index;
			pchar.quest.(str).win_condition.l1            = "Timer";
			pchar.quest.(str).win_condition.l1.date.day   = GetAddingDataDay(0, 0, 1);
			pchar.quest.(str).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
			pchar.quest.(str).win_condition.l1.date.year  = GetAddingDataYear(0, 0, 1);
			pchar.quest.(str).win_condition               = "LandEnc_RaidersOver";	
			pchar.quest.(str).LocIdx = location.index;
		break;
		//------------------ Спасаем девку в пампасах ----------------------
		case 1:
			if (rand(10) > 4 || location.type == "seashore" || location.type == "mayak") return false;	
			num = GetAttributesNum(grp); //кол-во локаторов 
			if (num <= 0) num = 1;
			if (!CheckAttribute(location, "locators.reload.reloadW_back"))
			{//--------------- обычная девка в джунглях ---------------				
				if (sti(pchar.rank) > 6) 
				{
					if (sti(pchar.rank) > 20) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*2.5/num);
					else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*1.6/num);
				}
				else
				{	//казуалам зеленый свет на начало игры
					if (sti(pchar.rank) > 3) iRank = sti(pchar.rank);
					else iRank = 1;
				}	
				//<-- генерим ранг 
				LAi_group_Delete("EnemyFight");
				LAi_group_Delete("LandEncGroup");
				//Начинаем перебирать локаторы и логинить фантомов
				model[0] = "pirate_1";
				model[1] = "pirate_2";
				model[2] = "pirate_3";
				model[3] = "pirate_4";
				model[4] = "pirate_5";
				model[5] = "pirate_6";
				model[6] = "pirate_7";
				model[7] = "pirate_8";
				model[8] = "pirate_9";
				model[9] = "pirate_10";
				i = 0;
				while(i < num)
				{
					iMassive = rand(9);
					if (model[iMassive] != "")
					{
						//Получим локатор для логина
						locator = GetAttributeName(GetAttributeN(grp, i));
						if (i == 0)
						{					
							iChar =	NPC_GenerateCharacter("CangGirl", "girl_"+(rand(7)+1), "woman", "towngirl", 5, iNation, 1, false);
							chr = &characters[iChar];
							chr.dialog.filename = "Enc_RapersGirl_dialog.c";
							chr.dialog.currentnode = "First time";
							chr.greeting = "Enc_RapersGirl_1";
							ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
							LAi_SetActorType(chr);
							LAi_group_MoveCharacter(chr, "LandEncGroup");
							LAi_ActorDialog(chr, pchar, "", -1, 0);  
							LAi_SetCheckMinHP(chr, LAi_GetCharacterHP(chr)-1, false, "LandEnc_RapersBeforeDialog");
							str = location.index;
							pchar.GenQuest.EncGirl.nation = iNation; //нация для слухов 
							pchar.GenQuest.EncGirl.name = GetFullName(chr); //имя девки
							i++;
							continue;
						}
						chr = GetCharacter(NPC_GenerateCharacter("GangMan_" + i, model[iMassive], "man", "man", iRank, PIRATE, 1, true));
						SetFantomParamFromRank(chr, iRank, true);
						chr.dialog.filename = "Enc_Rapers_dialog.c";
						chr.dialog.currentnode = "First time";
						chr.greeting = "Enc_Raiders";
						ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
						LAi_SetActorType(chr);
						LAi_group_MoveCharacter(chr, "EnemyFight");
						LAi_ActorFollow(chr, &characters[iChar], "", -1);
						LAi_SetCheckMinHP(chr, LAi_GetCharacterHP(chr)-1, false, "LandEnc_RapersBeforeDialog");
						i++;
						model[iMassive] = "";
					}
				}
				sGlobalTemp = "";//состояние квеста на выходе из локации, нулим
				//--> сюрприз для любителей просто свалить из локации
				pchar.quest.LandEnc_RapersBadExit.win_condition.l1 = "ExitFromLocation";
				pchar.quest.LandEnc_RapersBadExit.win_condition.l1.location = pchar.location;
				pchar.quest.LandEnc_RapersBadExit.win_condition = "LandEnc_RapersBadExit";
			}
			else
			{//--------------- замануха в колодец -------------------
				LAi_group_Delete("LandEncGroup");
    			aref arRld, arDis;
				makearef(arRld, Locations[sti(location.index)].reload);
    			int	Qty = GetAttributesNum(arRld);
				str = "none";
				for (int a=0; a<Qty; a++)
    			{
    				arDis = GetAttributeN(arRld, a);
    				if (arDis.name == "reloadW_back")
    				{
						str = arDis.go; //ID смежной локации, куда ведет колодец
    				}
    			}
				if (str == "none") return false; //на всякий случай
				//--> снимаем возможный конфликт с бергларами
				if (sAreal == "SentMartin" || sAreal == "SantaCatalina")
				{					
					iTemp = GetCharacterIndex("BerglarSantaCatalina");
					if (iTemp != -1 && characters[iTemp].location == str) return false;
					iTemp = GetCharacterIndex("BerglarMarigo");
					if (iTemp != -1 && characters[iTemp].location == str) return false;
				}
				//<-- снимаем возможный конфликт с бергларами
				locator = GetAttributeName(GetAttributeN(grp, 1));
				chr =	GetCharacter(NPC_GenerateCharacter("FriendGirl", "girl_"+(rand(7)+1), "woman", "towngirl", 5, iNation, 0, false));
				chr.dialog.filename = "Enc_FriendGirl_dialog.c";
				chr.dialog.currentnode = "First time";
				chr.greeting = "Enc_RapersGirl_1";
				ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
				LAi_SetActorType(chr);
				LAi_group_MoveCharacter(chr, "LandEncGroup");
				LAi_ActorDialog(chr, pchar, "", -1, 0); 
				pchar.GenQuest.EncGirl.locationId = str; //ID локации
				pchar.GenQuest.EncGirl.nation = iNation; //нация для слухов 
				pchar.GenQuest.EncGirl.name = GetFullName(chr); //имя девки
				pchar.quest.Enc_FriendGirl_after.win_condition.l1 = "NPC_Death";
				pchar.quest.Enc_FriendGirl_after.win_condition.l1.character = "FriendGirl";
				pchar.quest.Enc_FriendGirl_after.win_condition = "Enc_FriendGirl_after";				
			}
		break;
		//------------------ Праздношатающиеся перцы ----------------------
		case 2:
			LAi_group_Delete("LandEncGroup");
			if (rand(10) > 9) return false;	
			locator = GetAttributeName(GetAttributeN(grp, 0));
			//Начинаем перебирать локаторы и логинить фантомов
			if (rand(5) > 3 && location.id.label == "ExitTown")
			{
				chr = GetCharacter(NPC_GenerateCharacter("WalkerGirl", "girl_"+(rand(7)+1), "woman", "towngirl", 5, PIRATE, 0, false));
				chr.greeting = "Gr_Woman_Citizen";
			}
			else
			{
				chr = GetCharacter(NPC_GenerateCharacter("Walker", "citiz_"+(rand(9)+1), "man", "man", sti(pchar.rank), PIRATE, 0, true));
				SetFantomParamFromRank(chr, sti(pchar.rank), true);
				chr.greeting = "cit_common";
			}
			chr.dialog.filename = "Enc_Walker.c";
			chr.dialog.currentnode = "First time";
			LAi_SetCitizenType(chr);
			LAi_group_MoveCharacter(chr, "LandEncGroup");
			ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
		break;
		//------------------ Военный патруль ----------------------
		case 3:
			if (rand(10) > 4) return false;
			//--> генерим ранг. военному патрулю палец в рот не клади и на начальных уровнях.
			num = GetAttributesNum(grp); //кол-во человек в патруле
			if (num <= 0) num = 1;
			if (sti(pchar.rank) > 14) iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*2.5/num);
			else iRank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*1.6/num);
			//<-- генерим ранг 
			//Начинаем перебирать локаторы и логинить фантомов
			str = "Patrol"+ location.index + "_";
			string sGroup = "PatrolGroup_" + location.index; //имя группы
			for(i = 0; i < num; i++)
			{
				if (i == 0)
				{					
					chr = GetCharacter(NPC_GenerateCharacter(str + i, "off_"+NationShortName(iNation)+"_"+(rand(1)+1), "man", "man", iRank+2, iNation, 1, true));
					SetFantomParamFromRank(chr, iRank+2, true);
				}
				else
				{
					if (i == 3)
					{
						chr = GetCharacter(NPC_GenerateCharacter(str + i, NationShortName(iNation)+"_mush_"+(rand(2)+1), "man", "mushketer", iRank, iNation, 1, false));
						//SetFantomParamFromRank(chr, iRank, true);
					}
					else
					{
						chr = GetCharacter(NPC_GenerateCharacter(str + i, "sold_"+NationShortName(iNation)+"_"+(rand(7)+1), "man", "man", iRank, iNation, 1, true));
						SetFantomParamFromRank(chr, iRank, true);
					}
				}
				chr.City = sCity;
				chr.CityType = "soldier";				
				chr.greeting = "soldier_arest";
				chr.dialog.filename = "Enc_Patrol.c";
				//Получим локатор для логина
				locator = GetAttributeName(GetAttributeN(grp, i));
				ChangeCharacterAddressGroup(chr, location.id, encGroup, locator);
				chr.EncQty = num;
				LAi_SetStayType(chr);
				LAi_SetCheckMinHP(chr, LAi_GetCharacterHP(chr)-1, false, "LandEnc_PatrolBeforeDialog");
				LAi_group_MoveCharacter(chr, sGroup);
			}
			LAi_group_SetLookRadius(sGroup, 100);
			LAi_group_SetHearRadius(sGroup, 100);

			str = "EncPatrol_" + location.index;
			pchar.quest.(str).win_condition.l1        = "locator";
			pchar.quest.(str).win_condition.l1.location = location.id;
			pchar.quest.(str).win_condition.l1.locator_group = "encdetector";
			pchar.quest.(str).win_condition.l1.locator = encGroup;
			pchar.quest.(str).win_condition = "LandEnc_PatrolBegin";
			pchar.quest.(str).EncQty = num; //кол-во патрульных
			pchar.quest.(str).LocIdx = location.index; 
			str = "TimePatrol_" + location.index;
			pchar.quest.(str).win_condition.l1            = "Timer";
			pchar.quest.(str).win_condition.l1.date.day   = GetAddingDataDay(0, 0, 1);
			pchar.quest.(str).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 1);
			pchar.quest.(str).win_condition.l1.date.year  = GetAddingDataYear(0, 0, 1);
			pchar.quest.(str).win_condition               = "LandEnc_PatrolOver";	
			pchar.quest.(str).LocIdx = location.index;
			str = location.index;
			pchar.GenQuest.(str).nation = iNation; //нация патруля для слухов 
		break;
	}
	return true;
}