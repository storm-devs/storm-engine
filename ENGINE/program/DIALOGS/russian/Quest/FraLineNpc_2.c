// диалоговый файл №2 на фр. линейку
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	int i;
	float locx, locy, locz;
	string sTemp;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
    switch (Dialog.CurrentNode)
    {
        case "First time":
            dialog.text = "Если вы это видите, то это бага. Надо срочно сообщить Эдди!";
            link.l1 = "...";
            link.l1.go = "exit";

//>>>>>>>>>>>>========= Разброс диалогов по персонажам =====================
            //Квест №1, сопровождение Леграна.
            if (pchar.questTemp.State == "Fr1Legran_toLeMaren")// квест №1, базар с бандиами 
            {
    			dialog.text = "Эй, приятель! Нам нужно потолковать с твоим дружком. Отойди-ка в сторонку и не мешай - здоровее будешь...";
    			link.l1 = "Хм, полагаю, что лучше именно вам убраться с дороги. И поберегите свои уши!";
    			link.l1.go = "Step_F1_1";
            }
            //Квест №2, доставка письма в Кюрасао.
            if (pchar.questTemp.State == "Fr2Letter_Arrest")// квест №2, базар с охраной при аресте
            {
    			dialog.text = "Сдайте свое оружие! И не вздумайте сопротивляться!";
    			link.l1 = "Ну хорошо, будь по-вашему. И только потому, что Кюрасао в дружбе с Тортугой...";
    			link.l1.go = "Step_F2_1";
            }
            if (pchar.questTemp.State == "Fr2Letter_PrisonTalk")// квест №2, базар с тюремщиком
            {
    			dialog.text = "Сидишь, клопов кормишь? Завтра и тебя покормят по-королевски, как приговоренного к виселице, а ее уже строят, хе-хе!";
    			link.l1 = "Ну, тебе от этого вряд ли что-то перепадет...";
    			link.l1.go = "Step_F2_2";
            }
            if (pchar.questTemp.State == "Fr2Letter_VisitGovernor")// квест №2, базар с фантомом губера в тюрьме
            {
    			dialog.text = "Здравствуйте, капитан.";
    			link.l1 = "Месье Стэвезант... Какими судьбами?!";
    			link.l1.go = "Step_F2_8";
            }
            if (pchar.questTemp.State == "Fr2Letter_SeekProblems")// квест №2, базар с кэпом галеона, абордаж
            {
    			dialog.text = "Чертов ладрон!";
    			link.l1 = "Потише, испанский пес! Я задам тебе пару вопросов. Ответишь правду - будешь жить.";
    			link.l1.go = "Step_F2_12";
            }
			if (npchar.id == "AnnaDeLeiva")// квест №3, донна Анна в спальне
            {
    			dialog.text = "Боже! Что случилось? Вы прибыли от Бертрана? Что это был за шум?";
    			link.l1 = "Сударыня, не знаю, печальные это для вас новости или нет, но я был вынужден убить вашего супруга. У нас нет времени на переживания и обмороки, нужно немедленно бежать! Генерал-губернатор Тортуги Бертран д'Ожерон ждет вас.";
    			link.l1.go = "Step_F3_1";
            }
			if (npchar.id == "InesDeLasCierras" && pchar.questTemp.State == "Fr5AnnaHant_toHavana")// квест №5, Инесс
            {
    			dialog.text = "Сеньор, кто вы? И что вам нужно в моем доме?";
    			link.l1 = "Сеньора, я прибыл сюда по поручению донны Анны, вашей подруги. Насколько мне известно, вы известили ее о некой опасности...";
    			link.l1.go = "Step_F5_1";
            }
			if (npchar.id == "InesDeLasCierras" && pchar.questTemp.State == "Fr5AnnaHant_SeekHoseBrothers")
            {
    			dialog.text = "Поторопитесь, сеньор. Я уверяю вас - жизнь донны Анны висит на волоске...";
    			link.l1 = "Все будет в порядке, уважаемая Инес, не беспокойтесь.";
    			link.l1.go = "exit";
            }
			if (npchar.id == "InesDeLasCierras" && pchar.questTemp.State == "Fr5AnnaHant_GoodWork")
            {
    			dialog.text = "Ну что, капитан, какие новости?";
    			link.l1 = "Все в порядке, проблем с родственниками донны Анны более не существует...";
    			link.l1.go = "Step_F5_4";
            }
			if (npchar.id == "Captain_1" || npchar.id == "Captain_2")//квест №6, базар с кэпами
            {
    			dialog.text = "Месье, нам нужно на всех парусах идти к Кумане. Не приведи Гоcподь - опаздаем и там не будет реалов! Вы не знаете нашего капитана...";
    			link.l1 = "Хм, кто же не знает Франсуа Олоне? Известный флибустьер...";
    			link.l1.go = "exit";
            }
			if (npchar.id == "RockBrasilian" && pchar.questTemp.State == "Fr7RockBras_toSeekPlace")//квест №7, базар с Бразильцем. 
            {
    			dialog.text = NPCStringReactionRepeat("Вот это да! Кто это осмелился на такое?!", "Ну, давай продолжим наш разговор. Так кто же ты такой, герой нашего времени?", "Продолжим? Так кто ты такой?", "Хм, давай закончим наш разговор. Кто ты такой?", "block", 0, npchar, Dialog.CurrentNode);
    			if (LAi_group_GetTarget(pchar) <= 0)
				{
					link.l1 = HeroStringReactionRepeat("Я осмелился. Зовут меня "+GetFullName(pchar)+". Я пришел, чтобы вытащить тебя отсюда.", "Давай продолжим... Меня зовут "+GetFullName(pchar)+", я пришел за тобой.", "Меня зовут "+GetFullName(pchar)+", я пришел за тобой.", "Давай закончим, не вопрос. Меня зовут "+GetFullName(pchar)+", я пришел за тобой.", npchar, Dialog.CurrentNode);
    				link.l1.go = "Step_F7_1";
				}
				else
				{
					link.l1 = "Сейчас не время говорить, надо разделаться со всеми в этом месте!";
    				link.l1.go = "Step_F7_Late";
				}
				// --> типа опоздал...
				if (GetQuestPastDayParam("questTemp") > 8)
				{
    				dialog.text = "Кто ты?..";
    				link.l1 = "Я пришел за тобой, меня послал д'Ожерон...";
    				link.l1.go = "Step_F7_8";
				}
				else SaveCurrentQuestDateParam("questTemp"); //чтобы не глюкнуло во время боя
				// <-- типа опоздал...
            }
			if (npchar.id == "RockBrasilian" && pchar.questTemp.State == "empty")//квест №7, базар с Бразильцем в резиденции дОжерона. 
            {
    			dialog.text = "Капитан, я еще не имел возможности поблагодарить тебя.";
    			link.l1 = "Все в порядке, капитан Рок...";
    			link.l1.go = "Step_F7_5";
            }
			if (npchar.id == "CaptainGay")//квест №8, базар с кэпом Геем. 
            {
    			dialog.text = "Ты кто такой?";
    			link.l1 = "Неважно, кто я. Тебе привет от Джона Морриса. Он очень хочет познакомиться с твоим судовым журналом.";
    			link.l1.go = "Step_F8_1";
            }			
		break;
//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Спасение трех горожан. Квест №1 **********************************
 		case "Step_F1_1":
			dialog.text = "Ты сам напросился! Прощайся с жизнью...";
			link.l1 = "Эх, не бережете вы свои уши...";
			link.l1.go = "Step_F1_2";
		break;
 		case "Step_F1_2":
			LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
			LAi_SetPlayerType(pchar);
			sld = characterFromId("Legran");
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
			LAi_SetWarriorType(npchar);
			LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Fr1Legran_afterFightBandits");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Арест ГГ у Стэвезанта. Квест №2 **********************************
 		case "Step_F2_1":
			LAi_SetPlayerType(pchar);
            DoReloadCharacterToLocation("Villemstad_prison",  "goto", "goto9");
			SetTempRemoveParam(pchar, "items");
			pchar.questTemp.money = sti(pchar.money)/2;
			RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
			RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
			pchar.money = 0;
			DeleteAttribute(pchar, "items");
			DoQuestCheckDelay("Fr2Letter_intoPrison_1", 20);
			AddQuestRecord("Fra_Line_2_DelivLetter", "2");
            DialogExit();
		break;
 		case "Step_F2_2":
			dialog.text = "Так-так, это ты о чем?";
			link.l1 = "О том, что на мою казнь ты посмотришь задаром. Зато станешь на несколько кругляшек побогаче, если до завтра Стэвезант обыщет капитанскую каюту на моем корабле.";
			link.l1.go = "Step_F2_3";
		break;
 		case "Step_F2_3":
			dialog.text = "А это тебе зачем?";
			link.l1 = "Затем, чтобы он ознакомился с моими бумагами, судовым журналом и прочее. Тогда поймет, что я действительно вестовой генерал-губернатора Тортуги.";
			link.l1.go = "Step_F2_4";
		break;
 		case "Step_F2_4":
			dialog.text = "А-а-а, понятно... Ну что же, это я могу устроить, один из офицеров Стэвезанта - мой тесть.";
			link.l1 = "Ну давай, не пожалеешь. Послушай, а ты не знаешь, почему меня схватили? Ничего понять не могу...";
			link.l1.go = "Step_F2_5";
		break;
 		case "Step_F2_5":
			dialog.text = "Пираты, все они... А по тебе сразу видать, что ты капер.";
			link.l1 = "Так я же и не спорил... Ладно, послушай, не теряй времени, приступай к делу. А то меня точно повесят ни за что.";
			link.l1.go = "Step_F2_6";
		break;
 		case "Step_F2_6":
			dialog.text = "Хорошо. То, что твои судовые бумаги попадут к Стэвезанту - это я тебе обещаю. Ну а там молись, чтобы бумаги эти были убедительными.\n"+
				          "Ну а несколько кругляшков мой тесть сам возьмет из твоей утвари. Не возражаешь?";
			link.l1 = "Конечно, нет! Ну, не теряй времени, друг мой.";
			link.l1.go = "Step_F2_7";
		break;	
 		case "Step_F2_7":
			LAi_SetPlayerType(pchar);
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1", "none", "", "", "", 10);
            DialogExit();
			DoQuestCheckDelay("Fr2Letter_intoPrison_3", 10);
		break;	
 		case "Step_F2_8":
			dialog.text = "Не ёрничайте, капитан, все разъяснилось.\n"+
				          "Один из моих офицеров решил проверить вашу каюту. Мы нашли французский каперский патент, прочли судовой журнал и все встало на свои места.\n"+
						  "Все верно, вы действительно флибустьер на службе Франции. Я не извиняюсь, ибо таков порядок. Вы свободны, капитан.";
			link.l1 = "Очень рад. Глупо было бы умереть ни за что.";
			link.l1.go = "Step_F2_9";
		break;		
 		case "Step_F2_9":
			dialog.text = "Вам вернут все, что было у вас конфисковано. Передавайте мои наилучшие пожелания Бертрану. И будте острожны на обратном пути - в последнее время близ острова творится черт знает что.";
			link.l1 = "Что именно, сударь?";
			link.l1.go = "Step_F2_10";
		break;
 		case "Step_F2_10":
			dialog.text = "Ничего хорошего, капитан, ничего хорошего... Ну да ладно, вам до этого дела нет. Прощайте.";
			link.l1 = "Прощайте, месье генерал-губернатор.";
			link.l1.go = "Step_F2_11";
		break;
 		case "Step_F2_11":
			pchar.questTemp.jailCanMove = true; //даем возможность выйти из тюрьмы
			LAi_SetPlayerType(pchar);
			pchar.questTemp.State = "Fr2Letter_SeekProblems";
			DoReloadCharacterToLocation("Villemstad_prison",  "goto", "goto12");
			AddQuestRecord("Fra_Line_2_DelivLetter", "3");
			RestoreTempRemoveParam(pchar, "items");
			pchar.money = pchar.questTemp.money;
			DeleteAttribute(pchar, "questTemp.money");
			//==> странных типов помещаем в таверну
            LAi_group_Delete("PeaceGroup");
			for (i=1; i<=2; i++)
            {
				sTemp = "sit6";
				if (i==2) sTemp = "sit7";
				sld = GetCharacter(NPC_GenerateCharacter("Bandit_"+i, "pirate_"+i, "man", "man", 20, FRANCE, 5, true));
				FantomMakeCoolFighter(sld, 20, 70, 40, BLADE_NORMAL, "pistol3", 70);
				sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
				LAi_SetSitType(sld);
				LAi_group_MoveCharacter(sld, "PeaceGroup");
				LAi_SetCheckMinHP(sld, sti(sld.chr_ai.hp)-1, false, "Fr2Letter_FightAfterShot");
				ChangeCharacterAddressGroup(sld, "Villemstad_tavern", "sit", sTemp);
			}
			LAi_SetActorType(npchar);
            LAi_ActorGoToLocator(npchar, "reload", "reload1", "", 2.0);
			DialogExit();
		break;
 		case "Step_F2_12":
			dialog.text = "Никогда я, " + GetFullName(npchar) + ", испанский капер, не пасовал перед ладроном! Я потопил в окресностях Кюрасао двадцать кораблей, так что тебе меня не запугать. Лучше принять смерть, но сохранить честь!";
			link.l1 = "Ну что же, прими смерть, раз так желаешь. Я узнал все, что нужно...";
			link.l1.go = "Step_F2_13";
		break;
 		case "Step_F2_13":
			AddQuestRecord("Fra_Line_2_DelivLetter", "5");
			LAi_SetCurHPMax(npchar);
			QuestAboardCabinDialogExitWithBattle("");
			pchar.questTemp.State = "Fr2Letter_NiceResult";
			DialogExit();
		break;
//********************************* Похищение донны Анны. Квест №3 **********************************
 		case "Step_F3_1":
			dialog.text = "Хосе мертв... Видит Господь, я этого не хотела... Он нашел письмо, которое я писала ему, и запер меня здесь...";
			link.l1 = "Я знаю, сеньора, догадался... Сейчас я попрошу вас следовать за мной. Будте внимательны, сударыня...";
			link.l1.go = "Step_F3_2";
		break;
 		case "Step_F3_2":
			pchar.questTemp.State = "Fr3TakeAnna_HavanaOut";
			Pchar.quest.Fr3TakeAnna_DelivShip.win_condition.l1 = "location";
			Pchar.quest.Fr3TakeAnna_DelivShip.win_condition.l1.location = "Cuba2";
			Pchar.quest.Fr3TakeAnna_DelivShip.win_condition = "Fr3TakeAnna_DelivShip";
			LAi_SetActorType(npchar);
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;
//********************************* Проблемы донны Анны. Квест №5 **********************************
 		case "Step_F5_1":
			dialog.text = "Да, это так. Сеньор, донне де Лейва грозит смертельная опасность! Генерал-губернатор Гаваны Франсиско Орегон-и-Гаскон предпринял расследование по этому поводу и выяснил все об обстоятельствах бегства донны Анны на Тортугу.\n"+
				          "Все, что он узнал, он сообщил родственникам убитого дона Хосе. А дон Хосе Рамирес де Лейва - потомок старинного кастильского рода. По законам чести донна Анна должна поплатиться за свою неверность.";
			link.l1 = "До Тортуги им не добраться...";
    		link.l1.go = "Step_F5_2";
		break;
 		case "Step_F5_2":
			dialog.text = "Ошибаетесь, сеньор. Сюда уже прибыли двоюродные братья дона Хосе. Мне довелось их видеть, и поверьте мне, это настоящие головорезы. Когда кастильский дворянин свершает месть, ему не страшен сам дьявол!";
			link.l1 = "Ну, дьявола я им не обещаю, зато могу показаться собственной персоной. Не подскажете, где я могу их найти?";
    		link.l1.go = "Step_F5_3";
		break;
 		case "Step_F5_3":
			dialog.text = "Вчера они отправились в джунгли, в городе их нет. По всей видимости, они налаживают отношения с контрабандистами, чтобы скрытно проникнуть на Торгуту.";
			link.l1 = "Надо успеть перехватить их в джунглях! Если они успеют вернуться в город, мне придется воевать со всем испанским гарнизоном. Спасибо вам, уважаемая Инес. Я никогда не верил в женскую дружбу, но вы, отважная женщина, убедили меня в обратном. Прощайте.";
    		link.l1.go = "exit";
			pchar.questTemp.State = "Fr5AnnaHant_SeekHoseBrothers";
			AddQuestRecord("Fra_Line_5_KillAnnaHanters", "3");			
			npchar.LifeDay = 1; // уберем нпс
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
			locations[FindLocation("Mayak10")].DisableEncounters = true; //энкаутеры закроем
			LAi_LocationDisableOfficersGen("Mayak10", true); //офицеров не пускать
			Pchar.quest.Fr5Anna_InShore.win_condition.l1 = "location";
			Pchar.quest.Fr5Anna_InShore.win_condition.l1.location = "Mayak10";
			Pchar.quest.Fr5Anna_InShore.win_condition = "Fr5Anna_InShore";
		break;
 		case "Step_F5_4":
			dialog.text = "Фуф, это очень хорошо. Теперь донне Анне можно будет вздохнуть спокойно...";
			link.l1 = "Полагаю, что да.";
    		link.l1.go = "exit";
		break;
//********************************* Вызволение Рока Бразильца. Квест №7 **********************************
 		case "Step_F7_1":
    		if (LAi_group_GetTarget(pchar) <= 0)
			{
				dialog.text = "Я рад и счастлив, приятель. Но тебя я не знаю, может ты скажешь, кто тебя сюда отправил?";
				link.l1 =  "Не кто иной, как месье Бетран д'Ожерон. Полагаю, ты знаешь, кто это... Ну ладно, теперь давай убираться отсюда на корабль, нас ждут на Тортуге. Ты можешь держать оружие?";
    			link.l1.go = "Step_F7_2";
			}
			else
			{
				dialog.text = "Я очень рад...";
				link.l1 = "Договорим позже, надо разделаться тут со всеми...";
    			link.l1.go = "Step_F7_Late";
			}
		break;
 		case "Step_F7_2":
    		if (LAi_group_GetTarget(pchar) <= 0)
			{
				dialog.text = "Увы, нет. Местные святые отцы переломали мне руки...";
				link.l1 =  "Это плохо... Тогда следуй за мной по пятам - будем прорываться.";
    			link.l1.go = "Step_F7_3";
			}
			else
			{
				dialog.text = "Увы, нет...";
				link.l1 = "Договорим потом, надо разделаться со всеми здесь!";
    			link.l1.go = "Step_F7_Late";
			}
		break;
 		case "Step_F7_3":
			dialog.text = "Хорошо, как скажешь. У меня выбор невелик...";
			link.l1 = "Тогда вперед, корсар! И не отставай.";
    		link.l1.go = "Step_F7_4";
		break;
 		case "Step_F7_4":
			bDisableFastReload = false;
			AddQuestRecord("Fra_Line_7_RockBras", "2");
			Pchar.quest.Fr7RockBras_DelivShip.win_condition.l1 = "location";
			Pchar.quest.Fr7RockBras_DelivShip.win_condition.l1.location = "Cuba1";
			Pchar.quest.Fr7RockBras_DelivShip.win_condition = "Fr7RockBras_DelivShip";
			LAi_SetActorType(npchar);
			LAi_group_MoveCharacter(npchar, LAI_GROUP_PLAYER);
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;
 		case "Step_F7_5":
			dialog.text = "Послушай, я хочу тебя отблагодарить за спасение моей жизни. Деньгами я тебе помочь не могу - должен буду сейчас изрядную сумму уплатить в бюджет Франции.\n"+
				          "Но я могу дать тебе вещицу, что значит для корсара гораздо больше, чем деньги.";
			link.l1 = "Отказываться не буду, тем более, что мне очень интересно...";
    		link.l1.go = "Step_F7_6";
		break;
		case "Step_F7_6":
            dialog.text = "В общем, слушай меня внимательно... Под городом Форт де Франс, что на Мартинике, есть подземелье. В этом подземелье у меня припрятана одна очень ценная и нужная нашему брату вещища...\n"+
				          "Что это - говорить не буду, сам увидишь... В общем, я дарю тебе то, что ты там найдешь.";
			link.l1 = "Однако, необычный подарок, прямо скажем... Ну да ладно, дареному коню в зубы не глядят. Прощай, Рок, выздоравливай...";
            link.l1.go = "Step_F7_7";
        break;
		case "Step_F7_7":
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload3", "none", "", "", "", 6.0);
			DeleteAttribute(npchar, "RebirthPhantom");
         	npchar.LifeDay = 0; // уберем нпс
			pchar.GenQuestRandItem.FortFrance_Dungeon = true;
			pchar.GenQuestRandItem.FortFrance_Dungeon.randitem3 = "cirass5";
			DialogExit();
        break;
 		case "Step_F7_8":
			dialog.text = "Ты опоздал... Всю эту неделю они пытали меня... Теперь... меня уже ничто не спасет... У-у-угх...";
			link.l1 = "Черт возьми, он умер!";
    		link.l1.go = "Step_F7_9";
		break;
 		case "Step_F7_9":
			bDisableFastReload = true;
			LAi_KillCharacter(npchar);
			DeleteAttribute(npchar, "RebirthPhantom");
         	npchar.LifeDay = 0; // уберем нпс
			pchar.questTemp.State = "Fr7RockBras_RockIsAgony";
			DialogExit();
		break;
 		case "Step_F7_Late":
			bDisableFastReload = true;
			LAi_ActorWaitDialog(npchar, pchar);
			DialogExit();
		break;
//********************************* Объехать трех корсаров. Квест №8 **********************************
        case "Step_F8_1":
            dialog.text = "Ага, этот мерзавец сам не может прийти за мной и посылает своих прихвостней! Надеюсь, твоя душа, прежде чем попасть в ад, успеет шепнуть ему, что ждет каждого, кто посмеет...";
            link.l1 = "Утомил. Займемся делом, капитан...";
            link.l1.go = "Step_F8_2";
        break;
 		case "Step_F8_2":
			chrDisableReloadToLocation = true;
			LocatorReloadEnterDisable("PortRoyal_tavern", "reload2_back", true);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "CanFightCurLocation");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

	}
}
