// Герцог Альбермаль и другие
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	float locx, locy, locz;
	
    switch (Dialog.CurrentNode)
    {
        case "First time":
            dialog.text = "Если вы это видите, то это бага. Надо срочно сообщить Эдди!";
            link.l1 = "...";
            link.l1.go = "exit";

//>>>>>>>>>>>>========= Разброс диалогов по персонажам =====================
            // ==> Боцман Морриса Уильямса
            if (pchar.questTemp.State == "MorrisWillams_ModifordGood5000" && npchar.id == "Bocman_Willams")// перехват на посыльного Морриса Уильямса
            {
    			dialog.text = "Хей, капитан, ты кажется собрался выходить в море?";
    			link.l1 = "Да, я ухожу. А тебе что за дело до этого и кто ты?";
    			link.l1.go = "Step_E6_1";
            }
            if (pchar.questTemp.State == "MorrisWillams_GoToHimselfBussines" && npchar.id == "Bocman_Willams")// перехват на посыльного Морриса Уильямса второй раз
            {
    			dialog.text = "Капитан, добрый день. Это опять я, боцман с 'Фаворита'.";
    			link.l1 = "Что скажешь на этот раз?";
    			link.l1.go = "Step_E6_4";
            }
			// ==> Джилиан Эттербери.
			if (pchar.questTemp.State == "MorrisWillams_ArrestedSeekInfoWomen" && npchar.id == "Gillian Atterbury") // англ.линейка квест №6, разговор с Джиллиан Эттербери
            {
    			dialog.text = "Что вы хотите от меня?";
                link.l2 = "Послушайте, святой отец посоветовал мне обратиться к вам.";
                link.l2.go = "Step_E6_25";
            }
			if (pchar.questTemp.State == "MorrisWillams_SeekHusband" && npchar.id == "Gillian Atterbury") // англ.линейка квест №6, разговор с Джиллиан Эттербери после дачи задания
            {
    			dialog.text = "Вы что-нибудь узнали о моем муже, сэр?";
    			link.l1 = "Пока нет, но я этим занимаюсь. Я сделаю все, чтобы отыскать его.";
    			link.l1.go = "exit";
            }
			if (npchar.id == "Gillian Atterbury" && CheckAttribute(npchar, "TempQuest.SavedHusband")) // разговор с Джиллиан Эттербери, если мы спасли ее мужа
            {
                dialog.text = NPCStringReactionRepeat("Сэр, я никогда не забуду, что вы для нас сделали. Вы спасли жизнь моему мужу и избавили нашу семью от долгов. Мы назовем нашего ребенка в вашу честь.",
                         "Это опять вы, сэр. Я так рада вас видеть!", "сэр, мы видимся с вами уже в который раз сегодня. Это просто прекрасно!",
                         "Сэр, сколько бы вы не обращались ко мне, знайте, что я вам всегда рада.", "block", 1, npchar, Dialog.CurrentNode);
    			link.l1 = HeroStringReactionRepeat("Ну что вы, пустое... Но знаете, все равно приятно...",
                                               "Хорошо...",
                                               "Я тоже так думаю...",
                                               "Хм, до чего приятно! Спасибо.", npchar, Dialog.CurrentNode);
    			link.l1.go = "exit";
    			//ChangeCharacterReputation(pchar, 1); // чит, но вряд ли кто-то найдет TO_DO eddy
            }
            // ==> Один из бандитов в пещере, удерживающий мужа Джилиан
            if (pchar.questTemp.State == "MorrisWillams_SeekHusband" && npchar.id == "Bandit3")   // базар в пещере перед боевкой
            {
    			dialog.text = "Черт...";
    			link.l1 = "Кто вы и что вы здесь делаете?!!";
    			link.l1.go = "Step_E6_30";
            }
            // ==> Муж Джилиан
            if (pchar.questTemp.State == "MorrisWillams_AfterFight" && npchar.id == "Jillians_Husband")   // базар в пещере с мужем, англ.линейка, квест №6
            {
    			dialog.text = "Вы спасли мне жизнь! Спасибо!";
    			link.l1 = "Благодарости потом. Тебя зовут Том Эттребери?";
    			link.l1.go = "Step_E6_32";
            }
            // ==> Спасишийся перец в поселении буканьеров
            if (pchar.questTemp.State == "ToMansfield_GoOn")   // разговор со спасшимся перцем, анл.линейка, квест №8
            {
    			dialog.text = "Английский капер! Слава Господу!";
    			link.l1 = "Что здесь произошло?";
    			link.l1.go = "Step_E8_1";
            }
            if (pchar.questTemp.State == "ToMansfield_CatchToPardal")   // остаточный разговор со спасшимся перцем
            {
    			dialog.text = "Тебе нужно спешить. Галеоны ушли почти сутки назад, ты можешь не успеть.";
    			link.l1 = "Да, я знаю. Действительно нужно спешить...";
    			link.l1.go = "exit";
            }


        break;

        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        
//*************************************** Диалог боцмана Морриса Уильямса *********************************
 		case "Step_E6_1":
			dialog.text = "Я боцман с фрегата 'Фаворит'. Совсем недавно я видел тебя у нас на палубе, ты говорил с кэпом...";
			link.l1 = "Верно, продолжай.";
			link.l1.go = "Step_E6_2";
		break;

 		case "Step_E6_2":
			dialog.text = "Наш кэп в таверне, он уже выпил целый бочонок рома. Он послал меня найти тебя. Он ждет, это срочно.";
			link.l1 = "Хорошо, иду немедленно.";
			link.l1.go = "Step_E6_3";
		break;

 		case "Step_E6_3":
			chrDisableReloadToLocation = false;
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 40.0);
            LAi_SetPlayerType(pchar);
            sld = characterFromID("Morris_Willams");
            LAi_SetHuberTypeNoGroup(sld);
			FreeSitLocator("PortRoyal_tavern", "sit10");
            ChangeCharacterAddressGroup(sld, "PortRoyal_tavern", "sit", "sit10");
            LocatorReloadEnterDisable("PortRoyal_town", "gate_back", true); //на всякий случай, чтобы геймер не слинял.
            LocatorReloadEnterDisable("PortRoyal_town", "reload1_back", true);
            NextDiag.TempNode = "First time";
            DialogExit();
		break;

 		case "Step_E6_4":
			dialog.text = "Хорошего мало, вернее вообще нет. Наш кэп Уильямс только что был арестован на выходе из таверны.\n"+
                          "Человек 10 солдат окружили его, начальник тюрьмы показал ему какую-то бумагу и кэпа увели.";
			link.l1 = "И ты не вступился за него? Где твоя команда?";
			link.l1.go = "Step_E6_5";
		break;

 		case "Step_E6_5":
			dialog.text = "Команды нет, капитан, все разбежались. Осталось несколько человек на корабле. Да и корабль-то тоже арестован...\n"+
                          "В общем, дела идут плохо, очень плохо...";
			link.l1 = "Я иду к начальнику тюрмы, узнаю что к чему. Не дрейфь, иди на свою посудину и следи за порядком. Вытащу я твоего капитана, можешь не сомневаться.";
			link.l1.go = "Step_E6_6";
		break;

 		case "Step_E6_6":
			chrDisableReloadToLocation = false;
    	    npchar.LifeDay = 1; // уберем нпс на след.день.
    	    SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
			pchar.questTemp.State = "MorrisWillams_ArrestedToPrison";
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 40.0);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "15");
            ref sld = characterFromID("Morris_Willams");
			LAi_SetStayType(sld);
            ChangeCharacterAddressGroup(sld, "PortRoyal_prison", "goto", "goto9");
            RemoveCharacterEquip(sld, BLADE_ITEM_TYPE);
            RemoveCharacterEquip(sld, GUN_ITEM_TYPE);
            LAi_SetPlayerType(pchar);
			DialogExit();
		break;

//*********************************** Диалоги Джиллиан Эттербери *************************************
		case "Step_E6_25":
			dialog.text = "В чем дело, сэр?";
			link.l1 = "Не пугайтесь, я не причиню вам зла. Мне всего лишь нужно свести счеты с ростовщиком. Насколько я понимаю, у нас общие интересы.";
			link.l1.go = "Step_E6_26";
		break;

        case "Step_E6_26":
			dialog.text = "(плача) сэр, это ужасно. Мой муж Том пропал.";
			link.l1 = "Успокойтесь и расскажите подробней, пожалуйста.";
			link.l1.go = "Step_E6_27";
		break;

        case "Step_E6_27":
			dialog.text = "Мы задолжали ростовщику деньги и не выплатили долг вовремя. Тогда он пришел к нам домой и заявил мужу, что бросит его в тюрьму, если в течение недели мы не вернем долг с процентами.\n"+
                     "Мы отдали все, что у нас было, но это не покрыло процентов, набежавших за год на сумму долга.\n"+
                     "Муж просил его об отсрочке платежа, но получил лишь еще неделю времени на возврат оставшейся суммы.\n"+
                     "Тогда муж пошел искать деньги. Денег он не раздобыл, но узнал нечто такое, что, по его словам, позволит нам не бояться оказаться в тюрьме.\n"+
                     "Вчера он весь день пропадал в городе, потом заскочил на минуту, собрался и ушел в ночь. Он до сих пор не вернулся. Я очень боюсь!";
			link.l1 = "А куда он пошел? Он хоть что-нибудь сказал?";
			link.l1.go = "Step_E6_28";
		break;

        case "Step_E6_28":
			dialog.text = "Он сказал только, что идет за город. Больше ничего, он очень спешил. (плачет) сэр, я умоляю вас, помогите мне найти моего мужа!";
			link.l1 = "Я сделаю все, что в моих силах.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_SeekHusband";
			AddQuestRecord("Eng_Line_6_MorrisWillams", "20");
            Pchar.quest.MorrisWillams_HusbandInCave.win_condition.l1 = "location";
            Pchar.quest.MorrisWillams_HusbandInCave.win_condition.l1.location = "Jamaica_Grot";
            Pchar.quest.MorrisWillams_HusbandInCave.win_condition = "MorrisWillams_HusbandInCave";
		break;



//**************************** Диалоги бандита в пещере, поиски мужа Джиллиан ****************************
 		case "Step_E6_30":
			dialog.text = "Кончайте его, нам свидетели ни к чему...";
			link.l1 = "Хм...";
			link.l1.go = "Step_E6_31";
		break;

 		case "Step_E6_31":
            LAi_SetPlayerType(pchar);
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "MorrisWillams_AftreFightingInCave");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

//************************************** Диалоги мужа Джиллиан ***********************************
 		case "Step_E6_32":
			dialog.text = "Да, а откуда Вы знаете? Хотя мне плевать, я рад вас видеть больше, чем Архангела Гавриила!";
			link.l1 = "Расскажи мне, что произошло?";
			link.l1.go = "Step_E6_33";
		break;

 		case "Step_E6_33":
			dialog.text = "Даже не знаю с чего начать... Где-то год назад мы вязли денег в долг у ростовщика...";
			link.l1 = "Все это мне известно, рассказывай с того момента, как ты ушел за город ночью. Что ты раскопал о ростовщике?";
			link.l1.go = "Step_E6_34";
		break;

 		case "Step_E6_34":
			dialog.text = "Этот дьявол оказался связан с контрабандистами! Вчера я заметил, что в его дом с заднего хода зашел странный незнакомец. Я подождал, когда он выйдет и отправился за ним.\n"+
                          "Мне удалось проследить за ним практически до этого места, когда я попал в засаду. Меня схватили и приволокли сюда. Они хотели убить меня, но решили сначала выяснить кто я, и возможно, заработать на моем пленении.\n"+
                          "Вы поспели вовремя, сэр. Благодарю вас от всего сердца!";
			link.l1 = "Все ясно. Теперь беги в город к жене, она очень напугана твоим исчезновением. Считайте, что вашего долга ростовщику нет. Но об этом деле никому ни слова, а лучше уезжайте из города.";
			link.l1.go = "Step_E6_35";
		break;

 		case "Step_E6_35":
			Characters[GetCharacterIndex("Gillian Atterbury")].TempQuest.SavedHusband = 1; // чтоб Джиллиан помнила об услуге...
            AddQuestRecord("Eng_Line_6_MorrisWillams", "21");
            LAi_SetPlayerType(pchar);
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 5.0);
            //ChangeCharacterReputation(pchar, 5); TO_DO eddy
            pchar.questTemp.State = "MorrisWillams_AfterFightGoRedmond";
			DialogExit();
		break;

//************************************** Диалоги спасшегося буканьера ***********************************
  		case "Step_E8_1":
			dialog.text = "Испанцы на трех галеонах подошли к бухте, высадились и взяли наше поселение штурмом.";
			link.l1 = "Черт возьми! А где Мэнсфилд?";
			link.l1.go = "Step_E8_2";
		break;

   		case "Step_E8_2":
			dialog.text = "Убит, как и многие другие. Где-то половина буканьеров успела отступить из поселения. И хорошо, что они ушли, иначе конец пришел бы всем.";
			link.l1 = "Что-то я не узнаю буканьеров. Когда это вы отступали перед испанцами?";
			link.l1.go = "Step_E8_3";
		break;

   		case "Step_E8_3":
			dialog.text = "Их было очень много и вооружены они были лучше некуда.\n"+
                          "Они сняли с кораблей кулеврины и просто расстреливали нас, тогда как мы ружейным огнем не могли их достать.\n"+
                          "Мы предприняли вылазку на испанскую батарею, но они тремя мушкетными залпами уничтожили всю атакующую группу.\n"+
                          "Никто из парней не выжил - эти дьяволы тут же бросились в контратаку, а по дороге добили раненых...";
			link.l1 = "Черт возьми! Я приехал к Мэнсфилду по поручению Мэдифорда. Я должен забрать приготовленное для него золото.";
			link.l1.go = "Step_E8_4";
		break;

   		case "Step_E8_4":
			dialog.text = "Все забрали испанцы. Мэнсфилд мертв.";
			link.l1 = "Что еще ты можешь сказать?";
			link.l1.go = "Step_E8_5";
		break;

   		case "Step_E8_5":
			dialog.text = "Я знаю, что они ушли на Кубу, в Сантьяго.";
			link.l1 = "Откуда ты это знаешь?";
			link.l1.go = "Step_E8_6";
		break;

   		case "Step_E8_6":
			dialog.text = "Когда сюда ворвались испанцы, я притворился мертвым и лежал вон там, в стороне.\n"+
                          "Я немного понимаю по-испански и слышал разговор их капитана, они его звали Маноэлем.\n"+
                          "Они обыскали поселение, погрузили на корабли все, что нашли и ушли на Сантьяго. Во всяком случае, готовились к этому. Судя по разговору, они и пришли из Сантьяго.";
			link.l1 = "Маноэль... Уж не Маноэль ли это Риверо Пардаль? Тот самый единственный испанский корсар в этих водах? Хм... Ладно, спасибо тебе. Скоро вернутся твои товарищи. Передай им, что я отомщу.";
			link.l1.go = "Step_E8_7";
		break;

   		case "Step_E8_7":
			dialog.text = "Спасибо. Если хочешь застать испанцев в пути, то поторопись.";
			link.l1 = "Не волнуйся, галеоны-то уж я успею нагнать.";
			link.l1.go = "exit";
			pchar.questTemp.State = "ToMansfield_CatchToPardal";
			AddQuestRecord("Eng_Line_8_ToMansfield", "2");
		break;
		
        case "You_baster":
            dialog.text = "Я и представить себе не мог, что ты со мной так поступишь...";
            link.l1 = "Процитирую вас, сэр: 'Мне нечего вам более сказать. Политика вершит судьбы государств, нечего и думать о судьбах отдельных... генерал-губернаторов.'. Прощайте.";
            link.l1.go = "exit";
            NextDiag.TempNode = "You_baster";
			pchar.questTemp.jailCanMove = true;
        break;
    }
}
