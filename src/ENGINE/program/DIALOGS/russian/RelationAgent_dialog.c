#define DIPLOMAT_SUM 80000

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iSumm = 0;
	
	if (!CheckAttribute(npchar, "PatentPrice"))
    {
        npchar.PatentPrice = (8000 + rand(6) * 1000);
    }
    int i;
    string attrLoc;
	ref    sld;
	
    attrLoc   = Dialog.CurrentNode;
    
    if (findsubstr(attrLoc, "SetNationPatent_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.nation = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "patent_2";
 	}
 	
 	if (findsubstr(attrLoc, "SetNationLicence_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.LicenceNation = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "NationLicenceType";
 	}

 	if (findsubstr(attrLoc, "NationLicenceType_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.LicenceType = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "NationLicenceType2";
 	}
 	
 	if (findsubstr(attrLoc, "RelationTo_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
        npchar.quest.relation      = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
		// проверка на уже договор
		attrLoc = "RelationAgent" + GetNationNameByType(sti(npchar.quest.relation));
		if (CheckAttribute(Pchar, "GenQuest." + attrLoc) && sti(Pchar.GenQuest.(attrLoc)) == true)
		{
		    Dialog.CurrentNode = "RelationYet";
		}
		else
		{
 	    	Dialog.CurrentNode = "RelationAny_Done";
 	    	npchar.quest.relation.summ = CalculateRelationSum(sti(npchar.quest.relation));
 	    }
 	}
 	
 	if (findsubstr(attrLoc, "CityPay_" , 0) != -1)
 	{
        i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.quest.CityIdx = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 	    Dialog.CurrentNode = "CityInfo";
 	}
 	
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "First time":
			dialog.text = "Рад видеть вас снова.";
			link.l1 = "Мне опять нужны ваши услуги.";
			link.l1.go = "relation";
			link.l2 = "Я уже ухожу.";
			link.l2.go = "exit";
			//линейка ГПК, поиски торгашей -->
			if (pchar.questTemp.LSC == "toLicencer")
			{
				link.l3 = "У меня есть к вам вопрос по поводу Голландской Вест-Индской компании.";
				link.l3.go = "SeekHVIC";
			}
			if (pchar.questTemp.LSC == "toLicencer_2")
			{
				link.l3 = "Хм, прошу прощения за свою дырявую голову. Я снова по поводу Голландской Вест-Индской компании.";
				link.l3.go = "SeekHVIC_3";
			}
			//<-- линейка ГПК, поиски торгашей
			if (npchar.quest.meeting == "0")
			{
				dialog.text = "Позвольте представиться, я являюсь тем человеком, который может замолвить за вас словечко тому или иному губернатору. За плату, естественно, но поверьте, мои услуги самого высшего качества, и вы не пожалеете, если воспользуетесь моими талантами.";
				link.l1 = "Очень интересно, продолжай.";
				link.l1.go = "relation";
				link.l2 = "Может быть, в другой раз.";
				link.l2.go = "exit";
				npchar.quest.meeting = "1";
			}
			
			NextDiag.TempNode = "First time";
		break;

		case "relation":
			dialog.text = "Давайте посмотрим, чем я могу вам помочь.";
            if (ChangeCharacterNationReputation(pchar, ENGLAND, 0) < 0)
			{
				link.l3 = "Я хочу помириться с Англией.";
				link.l3.go = "RelationTo_0";
			}
			
			if (ChangeCharacterNationReputation(pchar, FRANCE, 0) < 0)
			{
				link.l1 = "Я хочу помириться с Францией.";
				link.l1.go = "RelationTo_1";
			}
			if (ChangeCharacterNationReputation(pchar, SPAIN, 0) < 0)
			{
				link.l2 = "Я хочу помириться с Испанией.";
				link.l2.go = "RelationTo_2";
			}

			if (ChangeCharacterNationReputation(pchar, HOLLAND, 0) < 0)
			{
				link.l4 = "Я хочу помириться с Голландией.";
				link.l4.go = "RelationTo_3";
			}
			if (ChangeContrabandRelation(pchar, 0) <= 5)
            {
                Link.l5 = "Я хочу помириться с контрабандистами.";
				Link.l5.go = "Contraband";
            }
            Link.l8 = "Мне нужент каперский патент.";
			Link.l8.go = "patent_0";
			
			Link.l9 = "Есть торговые лицензии?";
			Link.l9.go = "Licence";
			
			if (isHeroOwnCity(true))
			{
				Link.l10 = "У меня вопрос, связанный с принадлежностью поселений.";
				Link.l10.go = "City_Buy";
			}
			
			link.l99 = "Знаете, я думаю, что обойдусь своими силами.";
			link.l99.go = "exit";
		break;
		
		case "Licence":
			dialog.text = "Всегда в наличии, на разные сроки. Какая торговая лицензия интересует?";
		    link.l1 = "Англии";
			link.l1.go = "SetNationLicence_0";
		    link.l2 = "Франции";
			link.l2.go = "SetNationLicence_1";
		    link.l3 = "Испании";
			link.l3.go = "SetNationLicence_2";
		    link.l4 = "Голландии";
			link.l4.go = "SetNationLicence_3";
			link.l9 = "Знаете, я думаю, что обойдусь своими силами.";
			link.l9.go = "exit";
		break;
		
		case "NationLicenceType":
        	dialog.text = "На какой срок?";
		    link.l1 = "30 дней";
			link.l1.go = "NationLicenceType_30";
		    link.l2 = "60 дней";
			link.l2.go = "NationLicenceType_60";
		    link.l3 = "90 дней";
			link.l3.go = "NationLicenceType_90";
			link.l9 = "Я передумал.";
			link.l9.go = "exit";
		break;
		
		case "NationLicenceType2":
			iSumm = sti(npchar.LicenceType) * (3000 + MOD_SKILL_ENEMY_RATE*500);
        	dialog.text = "Итак, торговая лицензия " + XI_ConvertString(Nations[sti(npchar.LicenceNation)].Name + "Gen") + " сроком на " + sti(npchar.LicenceType) + " дней, цена " + FindRussianMoneyString(iSumm) + ".";
		    link.l1 = "Меня устраивает.";
		    if(makeint(Pchar.money) < iSumm)
            {
				Link.l1.go = "No_money";
			}
            else
			{
				link.l1.go = "NationLicenceType3";
			}
			link.l9 = "Я передумал.";
			link.l9.go = "exit";
		break;
		
		case "NationLicenceType3":
            iSumm = sti(npchar.LicenceType) * (3000 + MOD_SKILL_ENEMY_RATE*500);
			dialog.text = "Вот ваш документ. Входить в порт нужно под дружественным флагом. Помните, патруль может проверить не просрочена ли лицензия.";
			link.l9 = "Спасибо.";
			link.l9.go = "exit";
			AddMoneyToCharacter(pchar, -iSumm);
			GiveNationLicence(sti(npchar.LicenceNation), sti(npchar.LicenceType));
		break;
		
        case "No_money":
			dialog.text = "Отлично! Обратитесь ко мне, когда будет нужная сумма.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
		break;
		
        case "patent_0":
			dialog.text = "Замечательно! Для этого вы должны доказать свою преданность державе" + 
                          " безупречной службой. Отправляйтесь к любому губернатору" + //NationNameGenitive(sti(NPChar.nation)) +
                          " и выполните ряд его заданий. После этого он выдаст вам патент.";
			link.l1 = "Скажите, "+GetAddress_FormToNPC(NPChar) + ", а можно как-нибудь обойти эту формальность? ";
			link.l1.go = "patent_1";
			link.l2 = "Прощайте, "+GetAddress_FormToNPC(NPChar);
			link.l2.go = "exit";
		break;
		
		case "patent_1":
			dialog.text = "Вы имеете в виду подкуп должностного лица!? Хотите, чтобы я выдал вам патент сам?";
			link.l1 = "Именно!";
            if (GetCharacterSkillToOld(PChar, SKILL_FORTUNE) > rand(11) || bBettaTestMode)
            {
			    link.l1.go = "patent_2_give";
			}
			else
			{
                link.l1.go = "patent_2_none";
			}
			link.l2 = "Нет. Всего хорошего, "+GetAddress_FormToNPC(NPChar);
			link.l2.go = "exit";
		break;
		
		case "patent_2_none":
			dialog.text = "В данный момент я не располагаю такими связями, чтобы раздобыть чистый бланк патента со всеми печатями и подписями.";
            link.l1 = "Жаль. Прощайте, "+GetAddress_FormToNPC(NPChar);
			link.l1.go = "exit";
		break;
		
		case "patent_2_give":
			dialog.text = "Хорошо, я смогу это устроить. Какой патент вам нужен?";
			if (GetPatentNation() != ENGLAND)
			{
			    link.l1 = "Англии";
				link.l1.go = "SetNationPatent_0";
			}
			if (GetPatentNation() != FRANCE)
			{
			    link.l2 = "Франции";
				link.l2.go = "SetNationPatent_1";
			}
			if (GetPatentNation() != SPAIN)
			{
			    link.l3 = "Испании";
				link.l3.go = "SetNationPatent_2";
			}
			if (GetPatentNation() != HOLLAND)
			{
			    link.l4 = "Голландии";
				link.l4.go = "SetNationPatent_3";
			}
			link.l9 = "Знаете, я думаю, что обойдусь своими силами.";
			link.l9.go = "exit";
		break;
		
		case "patent_2":
            pchar.PatentPrice = 8000 + (sti(NPChar.PatentPrice) * sti(pchar.rank));
            switch (sti(NPChar.nation))
            {
			    case PIRATE :
                    dialog.text = "Явный баг, если видите - сообщите ALexusB";
                    link.l1 = "Неплохая перспектива. Я согласен на ваши условия!";
                    break;
                case HOLLAND :
                    dialog.text = "Голландия почти мирная страна, у вас станет много друзей и мало врагов. А сам патент обойдется вам всего лишь в "+pchar.PatentPrice+" золотых.";
                    link.l1 = "Неплохая перспектива. Я согласен на ваши условия!";
                    break;
                case FRANCE :
                    dialog.text = "Друзья и враги Франции станут вашими. Вы сможете топить корабли врагов. А сам патент обойдется вам всего лишь в "+pchar.PatentPrice+" золотых.";
                    link.l1 = "Отличная перспектива! Мне плевать, кому служить! Послужу и французам!";
                    break;
                case SPAIN :
                    dialog.text = "У Испании много врагов и они тут же станут вашими! Испанцы - нация морских воинов. Патент обойдется вам всего лишь в "+pchar.PatentPrice+" золотых.";
                    link.l1 = "Вы один предлагаете мне что-то стоящее! Получите эти жалкие гроши за романтику кровавого боя и новые задания высокопоставленных особ!";
                    break;
                case ENGLAND :
                    dialog.text = "Если честно, таким образом генерал-губернатор Англии набивает свой карман. А сам патент обойдется вам всего лишь в "+pchar.PatentPrice+" золотых. ";
                    link.l1 = "Я согласен отдать ему свои кровные деньги, которые добыл контрабандой и абордажами!!";
                    break;
			}

            if(makeint(Pchar.money) < makeint(pchar.PatentPrice))
            {
				Link.l1.go = "No_money";
			}
            else
			{
				link.l1.go = "patent_3";
			}

			link.l2 = "Нет, это не для меня!";
			link.l2.go = "exit";
		break;

		case "patent_3":
            pchar.PatentNation = NationShortName(sti(NPChar.nation));
			dialog.text = "Я рад, что мы договорились. По рукам.";
			link.l1 = "Прощайте, "+GetAddress_FormToNPC(NPChar);
			link.l1.go = "exit";
			AddDialogExitQuest("any_patent_take");
		break;

		case "Contraband":
			Pchar.questTemp.Relations.sum = makeint(0.3 * stf(Pchar.rank)/stf(Pchar.reputation)*DIPLOMAT_SUM);
			dialog.Text = "Хорошо. Это обойдеться в " + Pchar.questTemp.Relations.sum + " золотых.";
			Link.l1 = "Я согласен.";
			if(makeint(Pchar.money) < makeint(Pchar.questTemp.Relations.sum))
			{
				Link.l1.go = "No_money";
			}
			else
			{
				Link.l1.go = "Contraband_Agreed";
			}
			Link.l2 = "Пожалуй, я передумал.";
			Link.l2.go = "exit";
		break;
		
		case "Contraband_Agreed":
			dialog.Text = "Замечательно, я все улажу. Они будут иметь с вами дело.";
			Link.l99 = "Спасибо.";
			Link.l99.go = "exit";
			ChangeContrabandRelation(pchar, 25);
			AddMoneyToCharacter(pchar, -sti(Pchar.questTemp.Relations.sum));
		break;
        // boal <--
		case "RelationAny_Done":
			iSumm = sti(npchar.quest.relation.summ);
			dialog.text = "Хмм... даже не знаю, что сказать. Я, конечно, смогу выполнить вашу просьбу примерения с "+ XI_ConvertString(Nations[sti(npchar.quest.relation)].Name + "Abl") +", но это будет вам стоить " + FindRussianMoneyString(iSumm) + ".";
			if(sti(pchar.money) >= iSumm)
			{
				link.l1 = "Думаю, у меня все равно нет выбора. Так что вот ваши деньги.";
				link.l1.go = "relation3";
			}
			link.l2 = "Нет, это слишком большая сумма. Прощайте.";
			link.l2.go = "exit";
		break;

		case "relation3":
			dialog.text = "Отлично! С вами удивительно легко иметь дело. Можете быть спокойны, по истечении максимум 15 дней ваши дела будут улажены.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -sti(npchar.quest.relation.summ));
			ChangeNationRelationFromRelationAgent(npchar);
			attrLoc = "RelationAgent" + GetNationNameByType(sti(npchar.quest.relation));
            Pchar.GenQuest.(attrLoc) = true;
		break;
		
		case "RelationYet":
			dialog.Text = "За вас уже пошло ходатайство. Ждите, так быстро дела не делаются.";
			Link.l99 = "Спасибо.";
			Link.l99.go = "exit";
		break;
		
		case "City_Buy":
			dialog.Text = "Принадлежность какого поселения вы хотите обсудить?";
			for (i=0; i<MAX_COLONIES; i++)
			{
				if (sti(colonies[i].HeroOwn) == true && sti(colonies[i].isBought) == false)
				{
					sld = GetFortCommander(colonies[i].id);
					attrLoc = "l" + i;
					Link.(attrLoc) = GetCityName(colonies[i].id) + " - " + XI_ConvertString(GetNationNameByType(sti(sld.Default.nation)));
					Link.(attrLoc).go = "CityPay_" + i;
				}
			}
			Link.l99 = "Нет. Ничего.";
			Link.l99.go = "exit";
		break;
		
		case "CityInfo":
            i = sti(NPChar.quest.CityIdx);
            sld = GetFortCommander(colonies[i].id);
            iSumm = TWN_CityCost(colonies[i].id);
			dialog.Text = "Город " + GetCityName(colonies[i].id) + ", принадлежал " + XI_ConvertString(GetNationNameByType(sti(sld.Default.nation)) + "Gen") + ", сумма выкупа для прекращения попыток отбить поселение составит " + FindRussianMoneyString(iSumm) + ".";
   			if(sti(pchar.money) >= iSumm)
			{
			    Link.l1 = "Да, меня это вполне устраивает.";
				Link.l1.go = "City_Buy_End";
			}
			Link.l99 = "Нет, спасибо. Не интересно.";
			Link.l99.go = "exit";
		break;
		
		case "City_Buy_End":
            i = sti(NPChar.quest.CityIdx);
            TWN_RealeseForMoney(colonies[i].id, true);
			dialog.Text = "Хорошо, информация о сделке пошла. Попыток захвата города " + GetCityName(colonies[i].id) + " больше не будет.";
			Link.l2 = "Спасибо. Всего хорошего.";
			Link.l2.go = "exit";
			Link.l3 = "Еще один вопрос.";
			Link.l3.go = "relation";
		break;
		//линейка ГПК
		case "SeekHVIC":
			dialog.text = "Хм, однако... Капитан, а что это вам взбрендило интересоваться этой конторой?";
			link.l1 = "Я выполняю для них один заказ, и возникли вопросы.";
			link.l1.go = "SeekHVIC_1";
		break;
		case "SeekHVIC_1":
			dialog.text = "А с чего вы взяли, что я знаю, где они находятся?";
			link.l1 = "Ладно, хватит дурака валять! Лицензии от этой компании вы откуда берете?";
			link.l1.go = "SeekHVIC_2";
		break;
		case "SeekHVIC_2":
			dialog.text = "У представителя компании здесь, в Карибском море. Кстати, как его зовут? Если вы имеете с ним дела, то должены это знать.";
			Link.l1.edit = 6;
			link.l1 = "";
			link.l1.go = "SeekHVIC_res";
		break;
		case "SeekHVIC_res":
			attrLoc = GetStrSmallRegister(dialogEditStrings[6]);
			if (findsubstr(attrLoc, "оливер" , 0) != -1 && findsubstr(attrLoc, "траст" , 0) != -1)
			{
				dialog.text = "Хм, верно... Ну ладно, вы можете найти его на Сан Мартине, в Мариго. У него там комната в двухэтажном доме. Только не говорите, что это я тебя к нему отправил. Так, на всякий случай...";
				link.l1 = "Без проблем.";
				link.l1.go = "exit";
				LAi_group_Delete("EnemyFight");
				pchar.questTemp.LSC = "toOliverTrast";
				AddQuestRecord("ISS_PoorsMurder", "10");
				LocatorReloadEnterDisable("Marigo_town", "houseH2", false);
				sld = characterFromId("PoorKillSponsor");
				FantomMakeCoolFighter(sld, 90, 90, 90, "blade26", "pistol3", 100);
				sld.nation = HOLLAND;
				sld.city = "Marigo_town";
				sld.location = "Marigo_houseH2";
				sld.location.group = "sit";
				sld.location.locator = "sit1";
				sld.dialog.currentnode = "PKInMarigo";
				LAi_CharacterEnableDialog(sld);
				LAi_RemoveLoginTime(sld);
				sld.standUp = true; //вставать и нападать на врага
				LAi_SetHuberType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
				//ложим второе письмо
				ChangeItemName("letter_LSC", "itmname_letter_LSC_1");
				ChangeItemDescribe("letter_LSC", "itmdescr_letter_LSC_1");
				sld = ItemsFromID("letter_LSC");
				sld.shown = true;
				sld.startLocation = "Marigo_houseH2";
				sld.startLocator = "item1";
			}
			else
			{
				dialog.text = "Никого вы не знаете, так что убирайтесь!";
				link.l1 = "Черт, вылетело из головы...";
				link.l1.go = "exit";
				pchar.questTemp.LSC = "toLicencer_2";
			}
		break;
		case "SeekHVIC_3":
			dialog.text = "Ну и что вы опять от меня хотите?";
			link.l1 = "Я вспомнил имя представителя Голландской Вест-Индской компании!";
			link.l1.go = "SeekHVIC_4";
		break;
		case "SeekHVIC_4":
			dialog.text = "Да?! Ну и как его зовут?";
			Link.l1.edit = 6;
			link.l1 = "";
			link.l1.go = "SeekHVIC_res";
		break;
	}
}

