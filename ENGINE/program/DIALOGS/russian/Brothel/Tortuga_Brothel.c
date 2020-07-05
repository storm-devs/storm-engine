// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;     
	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы, молодой человек?", "Чего ты хочешь, красавчик? Ну хоть задай его."), "Опять вопросы?", "Хи-хи, третий раз на дню одно и то же - вопросы...",
                          "Хм, может ты выберешь какую-нибудь красотку себе? А то у меня уже складывается в отношении тебя вполне определенное мнение...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Хм, да ничего..."), "Не могу... Нет вопросов...",
                      "Да уж, действительно в третий раз... Извини.", "Не сейчас... В другой раз...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			//==> Голл. №3, узнаем о Пеьере Пикардийце.
            if (pchar.questTemp.State == "WormEnglPlans_WormPicarder")
            {
                link.l1 = "Послушай, Пьера Пикардийца нет у тебя в заведении?";
                link.l1.go = "Step_H3_1";
			}
            // ==> квест Аззи.
            if (CheckAttribute(pchar, "questTemp.Azzy.AddSpecial"))
            {
                dialog.text = "Что вам угодно, Сэр?";
    			Link.l1 = "Меня прислал к вам Аззи. Он сказал мне...";
    			Link.l1.go = "Step_1";
            }
            if (pchar.questTemp.Azzy == "HowToKnowAzzy" && !CheckAttribute(pchar, "questTemp.Azzy.AddSpecial"))
            {
                dialog.text = NPCStringReactionRepeat("Что вам угодно, Сэр?", "Опять вы?", "Снова по этому же вопросу?", "Ну что еще?", "block", 0, npchar, Dialog.CurrentNode);
    			Link.l1 = HeroStringReactionRepeat("Послушай, мне срочно нужно найти Аззи!", "Да, опять я! Мне нужен Аззи.", "Снова по этому же вопросу.", "То же самое. Мне нужен Аззи.", npchar, Dialog.CurrentNode);
    			Link.l1.go = DialogGoNodeRepeat("Step_15", "none", "none", "none", npchar, Dialog.CurrentNode);
            }
            if (pchar.questTemp.Azzy == "DestrContract")
            {
                dialog.text = "О-о-о, кого я вижу?!";
    			Link.l1 = "Меня, Екатерина. Но на этот раз я пришел с конкретным предложением.";
    			Link.l1.go = "Step_17";
            }
            if (pchar.questTemp.Azzy == "2DestrContract")
            {
                dialog.text = "Опять ты?";
    			Link.l1 = "Опять я. Как насчет вызова Аззи?";
    			Link.l1.go = "Step_20";
            }
			//жемчужный генератор Шарпа
			if (pchar.questTemp.Sharp == "seekSharp" && GetNpcQuestPastDayWOInit(npchar, "quest.SharpTime") > 7 && !LAi_IsDead(&characters[GetCharacterIndex("Sharp")]))
			{
				link.l1 = "Послушай, " + npchar.name + ", я ищу Шарпа. Ты не знаешь, где он?";
				link.l1.go = "SharpPearl_1";
				SaveCurrentNpcQuestDateParam(npchar, "quest.SharpTime");
			}
            // <== Проверяем поле состояния квестов.
		break;
		//==>Голландская линейка, квест №3.
 	 	case "Step_H3_1":
			dialog.text = NPCStringReactionRepeat("Сейчас нет, но будет сегодня точно.", "Красавчик, ты спрашивал уже меня о Пьере", "Опять о Пьере?", "Послушай, милий мой, хватит уже рассуждать о Пьере. Осмотрись, развлекись...", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("А откуда ты знаешь?", "Да, точно...", "Хм, опять...", "Это обязательно, но, к сожалению, не сейчас...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_H3_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_H3_2":
			dialog.text = "Милый мой, мне ли не знать?! Не было еще дня с момента его возвращения, когда он не появлялся у меня. И каждый день берет новую девку, хоть за такое непостоянство я и увеличиваю тарифы вдвое. Так что, если хочешь его застать, то просто подожди.";
			link.l1 = "Спасибо, но я, пожалуй, побегу.";
			link.l1.go = "exit";
            pchar.questTemp.Talks.Brothel = true;
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "11");
        break;
		//==> Квест Аззи
        case "Step_1":
			dialog.text = "Простите, что перебиваю... Назовите свое имя.";
			Link.l1 = "Меня зовут " + GetFullName(pchar) + ".";
			Link.l1.go = "Step_2";
		break;
        case "Step_2":
			dialog.text = "Я ждала тебя, красавчик. Аззи меня уведомил о том, что я должна кое-что сделать для тебя.";
			Link.l1 = "Да, верно. Даже не знаю, как высказать это 'кое-что'...";
			Link.l1.go = "Step_3";
		break;
        case "Step_3":
			dialog.text = "А почему такое замешательство? Ты же знаешь, кто я. Знаешь, и кто такой Аззи.";
			Link.l1 = "Аззи сказал, что ты ведьма!";
			Link.l1.go = "Step_4";
		break;
        case "Step_4":
			dialog.text = "И довольно неплохая, могу признаться... А ты кем Аззи прходишься, что он так печется о тебе?";
			Link.l1 = "Знакомый просто...";
			Link.l1.go = "Step_5";
		break;
        case "Step_5":
			dialog.text = "А-а-а, понятно... Я то думала, что ты серьезный человек в нашей иерархии. А ты так, мимоходом прикоснулся.\n"+
                     "Давай приступим к делу, красавчик. Аззи сказал, что я должна исправить твое тело под определенные нужды. Чего ты хочешь?";
			Link.l1 = "Хм, знаешь, для начала я бы тебя в таверну пригласил на коктейль... Ты сегодня вечером свободна?";
			Link.l1.go = "Step_6";
		break;
        case "Step_6":
			dialog.text = "Даже и не мечтай, ты мне не интересен. Буду я шашни крутить со знакомым Аззи, когда есть САМ Аззи.\n"+
                     "В общем так, давай быстро и по делу. Я могу сделать тебя сильней, ловчей, повысить твое восприятие, обаяние, выносливость или интеллект.\n"+
                     "Так как должна я поменять твое тело?";
			Link.l1 = "Чтобы я был сильней, ловчей, восприимчивей, харизматичней, умней - в общем, весь набор хочу.";
			Link.l1.go = "Step_7";
		break;
        case "Step_7":
			dialog.text = "Весь набор не получится.";
			Link.l1 = "Как это не получится?! Мало того, что доброму молодцу баньку не истопила, спать не уложила, она еще и жульничает! Аззи сказал...";
			Link.l1.go = "Step_8";
		break;
        case "Step_8":
			dialog.text = "Слушай меня внимательно, добрый молодец! Для того, чтобы сделать тебя сильней, мне нужно приладить к твоему телу новые большие мышцы.\n"+
                     "Для того, чтобы сделать тебя ловчей, мне нужно сменить твои сухожилия на более эластичные.\n"+
                     "Для того, чтобы сделать тебя выносливей, мне нужно заменить твои связки на другие, очень прочные.\n"+
                     "И так далее по каждому пункту. Если все сделать разом, то работать вообще ничего толком не будет. Исключением является удача - это как раз разумное сочетание различных характеристик тела, позволяющее выходить из различных сложных ситуации с честью. То есть, быть удачливым.\n"+
                     "Поэтому не дури, выбирай что-нибудь одно. Пока я добрая...";
			Link.l1 = "Понятно, значит я должен выбрать?";
			Link.l1.go = "Step_9";
		break;
        case "Step_9":
			dialog.text = "Точно. Говори, какую характеристику твоего тела повысить?";
            Link.l1 = "Силу";
			Link.l1.go = "Strength";
            Link.l2 = "Восприятие";
			Link.l2.go = "Perception";
            Link.l3 = "Реакцию";
			Link.l3.go = "Agility";
            Link.l4 = "Лидерство";
			Link.l4.go = "Charisma";
            Link.l5 = "Обучаемость";
			Link.l5.go = "Intellect";
			Link.l6 = "Выносливость";
			Link.l6.go = "Endurance";
            Link.l7 = "Удачу";
			Link.l7.go = "Luck";
		break;
        case "Strength":
			dialog.text =  "Сила, хороший выбор. Ну что ж, готовим мышцы - и будем менять. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_S, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_S, 1);
			}
		break;
        case "Perception":
			dialog.text =  "Восприятие, очень хорошо. Ну что ж, готовим новые органы чувств - и будем менять. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_P, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_P, 1);
			}
		break;
        case "Endurance":
			dialog.text =  "Вынослвивость, хорошо. Ну что ж, готовим сверхпрочные связки - и будем менять. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_E, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_E, 1);
			}
		break;
        case "Charisma":
			dialog.text =  "Обояние? Хм, хороший выбор, красавчик! Ну что ж, готовим симпатичную мордашку с мужественными чертами лица - и будем менять. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_C, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_C, 1);
			}
		break;
        case "Intellect":
			dialog.text =  "Интеллект, хороший выбор. Ну что ж, готовим новые хорошие мозги - и будем менять. Не бойся, твою память останется при тебе. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_I, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_I, 1);
			}
		break;
        case "Agility":
			dialog.text =  "Ловкость, хорошо. Ну что ж, готовим новые сверхэластичные связки - и будем менять. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_A, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_A, 1);
			}
		break;
        case "Luck":
			dialog.text =  "Хочешь быть удачливым, пират? И правильно. Ну что ж, готовим все новое - мышцы, связки, сухожилия, глаза, уши - в общем все. Каждое не с выдающимим характеристиками, но очень даже ничего. Полагаю, откладывать нет смысла, приступаем прямо сейчас.";
			Link.l1 = "Приступаем к чему?";
			Link.l1.go = "Step_10";
			if (AddSPECIALValue(pchar, SPECIAL_L, 0) == SPECIAL_MAX)
			{
                Link.l1.go = "NotPosblAdd";
			}
			else
			{
			    AddSPECIALValue(pchar, SPECIAL_L, 1);
			}
		break;
        case "Step_10":
			dialog.text =  "Как к чему? К операции, красавчик. Резать тебя будем! Ха-ха-ха!!";
			Link.l1 = "Что-то все это мне не нравится...";
			Link.l1.go = "Step_11";
		break;
        case "Step_11":
			dialog.text =  "Да ты не бойся, боли не будет. Так что, быстренько тебе печень меняем, складочки подрезаем...";
			Link.l1 = "И что, я за этим еще и наблюдать должен?!";
			Link.l1.go = "Step_12";
		break;
        case "Step_12":
			dialog.text =  "Экий ты корсар мнительный. Стольких людей на тот свет отправил, а своей крови боишься.";
			Link.l1 = "Слушай, ведьма, я ничего и никого не боюсь! Так что ты аккуратней в выражениях. А опасаюсь я за свой рассудок, умом тронутся можно - на такие вещи смотреть!";
			Link.l1.go = "Step_13";
		break;
        case "Step_13":
			dialog.text =  "Ну ладно, сделаем все под наркозом, ха-ха-ха! Вот, выпей настой. Очнешься ровно через два дня - будет твой заказ выполнен. И имей в виду, что впредь я знать тебя не знаю.";
			Link.l1 = "Эх, где наша не пропадала! Давай свою отраву. И осторожней там со мной...";
			Link.l1.go = "exit";
			AddDialogExitQuest("Azzy_PlusSkill");
		break;
        case "NotPosblAdd":
			dialog.text =  "Давай посмотрим, что ты имеешь... Нет красавчик, достойной замены у меня нет. Извини, складские запасы ограничены, да и тебе грех жаловаться на то, что есть... Выбирай что-нибудь другое.";
			Link.l1 = "Хм, я польщен... Так что, могу опять подумать над выбором?";
			Link.l1.go = "Step_9";
		break;
        case "Step_15":
			dialog.text =  "Я не знаю, о ком вы говорите.";
			Link.l1 = "Хватит издеваться надо мной! Мне нужен Аззи. Как я могу найти его?";
			Link.l1.go = "Step_16";
		break;
        case "Step_16":
			dialog.text =  "Не кричите на весь город, сударь! Аззи нельзя найти, лично я смогу его увидеть только в Вальпургиеву ночь. Я ничем не могу вам помочь.";
			Link.l1 = "Чертова ведьма!";
			Link.l1.go = "exit";
		break;
        case "Step_17":
			dialog.text =  "Интересно послушать.";
			Link.l1 = "Я предлагаю тебе вызвать Аззи с помощью трех хрустальных черепов тольтеков.";
			Link.l1.go = "Step_18";
		break;
        case "Step_18":
			dialog.text =  "Хм, я слышала об этих вещицах. Используются тольтеками для мистических ритуалов. Но для вызова демона нужен еще один предмет - странная вещь.";
			Link.l1 = "Об этом я знаю. Так ты готова провести ритуал?";
			Link.l1.go = "Step_19";
		break;
        case "Step_19":
			dialog.text =  "Я могу проести его, если у тебя есть эти предметы. Но только предметы эти я потом оставлю себе в качестве платы за работу.  И еще, демоны не любят, когда их дергают не по делу. Так можно и исчезнуть навсегда...";
			Link.l1 = "По поводу Аззи не беспокойся, у меня есть его обещание не трогать меня в случае его вызова. И предметы я оставляю тебе.";
			Link.l1.go = "Step_20";
		break;
        case "Step_20":
			dialog.text =  "Я готова провести ритуал. Предметы у тебя?";
            if (CheckCharacterItem(pchar, "sculMa1") && CheckCharacterItem(pchar, "sculMa2") && CheckCharacterItem(pchar, "sculMa3") && CheckCharacterItem(pchar, "indian22"))
            {
    			Link.l1 = "Да, они у меня. Приступай прямо сейчас!";
    			Link.l1.go = "Step_22";
            }
            else
            {
    			Link.l1 = "Нет, я ищу.";
    			Link.l1.go = "Step_21";
            }
		break;
        case "Step_21":
			dialog.text =  "Найдешь - приходи, все сделаем.";
			Link.l1 = "Понял, буду искать.";
			Link.l1.go = "exit";
			pchar.questTemp.Azzy = "2DestrContract";
		break;
        case "Step_22":
			dialog.text =  "Итак, начнем! Ха-ха-ха (читает заклинание)";
			Link.l1 = "Эх, ну и корявый же язык...";
			Link.l1.go = "Step_23";
		break;
        case "Step_23":
            TakeItemFromCharacter(pchar, "sculMa1");
            TakeItemFromCharacter(pchar, "sculMa2");
            TakeItemFromCharacter(pchar, "sculMa3");
            TakeItemFromCharacter(pchar, "indian22");
            pchar.questTemp.Azzy = "3DestrContract";
            DeleteAttribute(pchar, "questTemp.Azzy.Oliver")
            ChangeCharacterAddressGroup(characterFromId("Azzy"), pchar.location, "goto", "goto2");
            DoQuestCheckDelay("Azzy_IsFree", 2.0);
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
		break;
		// капитан Шарп, грабеж жемчужных промыслов
		case "SharpPearl_1":
			if (rand(1) || bBettaTestMode)
			{
				if (sti(pchar.questTemp.Sharp.brothelChance) < 9)
				{	//футболим геймера в город
					pchar.questTemp.Sharp.City = GetSharpCity();
					pchar.questTemp.Sharp.City.rumour = true; //флаг дачи слуха
					dialog.text = "Насколько мне известно, капитан Шарп отправился в " + XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc") + ".";
					link.l1 = "Хех! Благодарю тебя, " + npchar.name + ". Ты мне здорово помогла!";
					link.l1.go = "exit";
					pchar.questTemp.Sharp = "toSharp_going"; //в бордели больше не заходить
					ReOpenQuestHeader("SharpPearl");
					AddQuestRecord("SharpPearl", "1");
					AddQuestUserData("SharpPearl", "sCity", XI_ConvertString("Colony" + npchar.city + "Dat"));
					AddQuestUserData("SharpPearl", "sTarget", XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc"));
					if (GetIslandByCityName(pchar.questTemp.Sharp.City) != pchar.questTemp.Sharp.City)
					{
						AddQuestUserData("SharpPearl", "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(pchar.questTemp.Sharp.City) + "Dat"));
					}
					//запускаем энкаунтер Шарпа на карте
					group_DeleteGroup("Sharp_Group");
					sld = characterFromId("Sharp");
					string sGroup = "Sharp_Group";
					Group_FindOrCreateGroup(sGroup);
					Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
					Group_LockTask(sGroup);
					Group_AddCharacter(sGroup, sld.id);
					Group_SetGroupCommander(sGroup, sld.id);
					sld.mapEnc.type = "trade";
					sld.mapEnc.worldMapShip = "quest_ship";
					sld.mapEnc.Name = "бриг 'Шарпоносец'";
					string sColony= SelectAnyColony(npchar.city); //колония, откуда плывет Шарп
					int daysQty = GetMaxDaysFromIsland2Island(GetArealByCityName(sColony), GetArealByCityName(pchar.questTemp.Sharp.City))+3; //дней доехать даем с запасом
					Map_CreateTrader(sColony, pchar.questTemp.Sharp.City, sld.id, daysQty);
					Log_TestInfo("Шарпоносец установлен из " + sColony + "  в " + pchar.questTemp.Sharp.City);
				}
				else
				{	//чудо, Шарп здесь, в борделе!
					dialog.text = "Знаю. Он у меня в заведении отдыхает. Могу позвать, если он тебе срочно нужен.";
					link.l1 = "Сделай, одолжение, " + npchar.name + "...";
					link.l1.go = "SharpPearl_2";
				}
			}
			else
			{
				dialog.text = "Не знаю, у меня его давно уже не было.";
				link.l1 = "Понятно... Ну что же, спасибо тебе, " + npchar.name + ".";
				link.l1.go = "exit";
			}
		break;
		case "SharpPearl_2":
			dialog.text = "Барталомью-ю-ша! Зайди ко мне, дорогой, тебя тут спрашивают!";
			link.l1 = "Хех, а Шарп у вас в почете, как я погляжу...";
			link.l1.go = "SharpPearl_3";
		break;
		case "SharpPearl_3":
			chrDisableReloadToLocation = true;
			pchar.questTemp.Sharp.count = 0; //счетчик нулим для след. раза
			pchar.questTemp.Sharp.price = 5000+rand(20)*1000; //цена за наводку
			pchar.questTemp.Sharp.price.evil = rand(1); //добрый или злой, для скидки
			sld = &characters[GetCharacterIndex("Sharp")];
			sld.dialog.currentnode = "BrothelSharp";
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

