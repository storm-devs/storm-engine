// Джон Моррис, авторитетный корсар по английской линейке
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		case "First time":
			dialog.text = "Прошу покинуть мой дом. Я занят, и у меня нет времени разговаривать с вами.";
			link.l1 = "Я уже ухожу.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			if (pchar.questTemp.Sharp == "begin" && rand(1))
			{
				link.l1 = "Джон, я слышал о ловцах жемчуга, что промышляют в наших водах. Ты не в курсе этого вопроса?";
				link.l1.go = "SharpPearl_1";
			}
		break;

 		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

        case "I_know_you_good":
			dialog.text = LinkRandPhrase(GetFullName(pchar) + ", рад тебя видеть! Зачем пожаловал на этот раз?",
                                         RandSwear() + " Кого я вижу?! Да это мой друг " + GetFullName(pchar) + ". Какими судьбами на острове?",
                                         "Так так, "+ GetFullName(pchar)+ " у меня в гостях. Хотел бы я еще раз увидеть как ты управляешься с клинком... Ну что, какие вопросы, друг?");
			link.l1 = RandPhraseSimple("Просто заскочил проведать тебя.", "Мелкие дела, ничего существенного.");
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			if (pchar.questTemp.Sharp == "begin" && rand(1))
			{
				link.l1 = "Джон, я слышал о ловцах жемчуга, что промышляют в наших водах. Ты не в курсе этого вопроса?";
				link.l1.go = "SharpPearl_1";
			}
		break;

		//********************* капитан Шарп *********************
		case "SharpPearl_1":
			dialog.text = "Нет, не в курсе. Это - к Шарпу, приятель. Язык не поворачивается назвать его капитаном...";
			link.l1 = "А ты не знаешь, где его можно найти?";
			link.l1.go = "SharpPearl_2";
			pchar.questTemp.Sharp = "seekSharp";
		break;
		case "SharpPearl_2":
			dialog.text = "Понятия не имею. Меня жемчужные дела не интересуют. Хотя, на твоем месте я бы поискал его в борделях. Рано или поздно ты его там застанешь...";
			link.l1 = "Ясно, Джон, спасибо.";
			link.l1.go = "exit";
		break;
		//********************* Английская линейка *********************
 		case "EngLine_quests":
            dialog.text = "Прошу покинуть мой дом. Я занят, и у меня нет времени разговаривать с вами.";
			link.l1 = "Я уже ухожу.";
			link.l1.go = "exit";
			switch (pchar.questTemp.State)   // Доступ к квестам
            {
	     		case "go_talks_with_pirates":
	    			dialog.text = "Говори, зачем пришел?";
	    			link.l1 = "Я пришел к тебе по поручению генерал-губернатора Ямайки, сэра Томаса Мэдифорда.";
	    			link.l1.go = "Step_1";
	      		break;
            }
		break;

 		case "Step_1":
			dialog.text = "Уже интересно! Мэдифорд меня не жалует, что же его заставило послать ко мне пирата?";
			link.l1 = "Мэдифорд собирает эскадру для атаки Кюрасао.";
			link.l1.go = "Step_2";
  		break;
  		
 		case "Step_2":
			dialog.text = "Без меня.";
			link.l1 = "Почему?";
			link.l1.go = "Step_3";
  		break;
  		
 		case "Step_3":
			dialog.text = "Генерал-губернатор Ямайки относится ко мне, мягко говоря, не дружелюбно. Поэтому его приглашение поучаствовать в деле вызывает у меня серьезные опасения. Что-то здесь не так.";
			link.l1 = "Возможно, Мэдифорд сменил гнев на милость.";
			link.l1.go = "Step_4";
  		break;
  		
 		case "Step_4":
			dialog.text = "Плохо ты знаешь Мэдифорда. Нет, я не так глуп, что-то здесь не то. Возможно, предстоит кровавая баня, а мне как-то не хочется умываться кровью ради генерал-губернатора Ямайки. Так что без меня, и удачи тебе, корсар.";
			link.l1 = "Спасибо. И тебе удачи, друг.";
			link.l1.go = "exit";
			
            AddQuestRecord("Eng_Line_2_Talking", "5");
            pchar.questTemp.Count = makeint(sti(pchar.questTemp.Count) + 1);
            if (pchar.questTemp.Count == "4")   {pchar.questTemp.State = "after_talks_with_pirates";}
            NextDiag.TempNode = "First time";
  		break;

 		case "Lets_go_bussines":
			dialog.text = "Ага, вот и ты снова. Что теперь скажешь?";
			link.l1 = "Я участвую в деле с Морганом. Ты готов к штурму Белиза?";
			link.l1.go = "Step_5";
  		break;

 		case "Step_5":
            if (GetCompanionQuantity(pchar) != 4)
            {
    			dialog.text = "Я готов выступать немедленно.";
    			link.l1 = "Тогда вперед!";
    			link.l1.go = "Step_6";
            }
            else
            {
    			dialog.text = "Я видел в на рейде четыре корабля. Если еще и я пойду с вами, то при штурме мы будем только мешать друг другу. Так что, без меня на этот раз.";
    			link.l1 = "Ну что ж, как знаешь, будь здоров.";
    			link.l1.go = "exit";
                NextDiag.TempNode = "First time";
            }
            
  		break;

 		case "Step_6":
			AddQuestRecord("Eng_Line_3_Morgan", "5");
            LAi_SetActorType(NPChar);
            LAi_ActorSetStayMode(NPChar);
            ChangeCharacterAddressGroup(NPChar, "PortSpein_houseF2", "goto", "goto3");
            CharacterIntoCompanionAndGoOut(pchar, npchar, "reload", "reload1", 6, false);
            Fantom_SetBalls(npchar, "pirate");
            npchar.CompanionEnemyEnable = false;  // нет отпора при обстреле
            pchar.questTemp.CompanionQuantity = makeint(sti(pchar.questTemp.CompanionQuantity) + 1);
            pchar.questTemp.CompanionQuantity.Morris = true;
            NextDiag.TempNode = "Have_hot_bussines";
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;

  		break;

	 	case "Have_hot_bussines":   // Если геймеру вдруг захочется еще пообщаться, не знаю, сможет ли. Но на всякий случай.
			dialog.text = "Мы должны поторопится, время не ждет.";
			link.l1 = "Мы все успеем.";
			link.l1.go = "exit";
            NextDiag.TempNode = "Have_hot_bussines";
  		break;

	 	case "Gold_found_in_fort":    // Нода разговора в форте Куманы
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Morris") && !IsCompanion(characterFromID("John Morris")))
    		{
            	dialog.text = "Проклятье, мой галеон пошел ко дну! Я потерял корабль и добрую четверть команды. Жаль хороших парней..."+
                              " Эх, все одно для оставшихся в живых жизнь продолжается, а наградой нам будет золото Куманы. Мы нашли золото в этой комнате под досками, которыми устлан пол. Ты - настоящий корсар.";
            }
            else
            {
            	dialog.text = "Долго искать не пришлось, черт возьми! Мы нашли золото в этой комнате под досками, которыми устлан пол. Ты - настоящий корсар.";
            }
			link.l1 = "Спасибо. Рад слышать это от тебя.";
			link.l1.go = "Step_7";
        break;

	 	case "Step_7":
			dialog.text = "Я ухожу к себе на Тринидад. Ты всегда сможешь найти меня там.";
			link.l1 = "Всего хорошего, Джон. Еще встретимся!";
			link.l1.go = "Step_8";
        break;
        
	 	case "Step_8":
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 15.0);
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
        break;

		//********************* Французская линейка *********************
        case "FraLine8_talk":
            dialog.text = "Кто ты такой и что тебе нужно?";
            link.l1 = "Я капитан " + GetFullName(pchar) + ". А нужен мне пустяк. К тебе наверняка прибудут гонцы от генерал-губернатора Мэдифорда...";
            link.l1.go = "Step_F8_1";
        break;
        case "Step_F8_1":
            dialog.text = "Что за гонцы? Говори прямо, не надо мне тут намекать!";
            link.l1 = "Хорошо, Джон. Вопрос по сути следующий - Мэдифорд готовит нападение на голландцев, и хочет подключить к этому делу Береговое братство.";
            link.l1.go = "Step_F8_2";
        break;
        case "Step_F8_2":
            dialog.text = "Ну и что?";
            link.l1 = "Мне важно, чтобы ты не участвовал в этой авантюре.";
            link.l1.go = "Step_F8_3";
        break;
        case "Step_F8_3":
            dialog.text = "Хех, мне не нравится идея нападать на голландцев и я, пожалуй, соглашусь на твое предложение. Но тебе придется кое-что сделать для меня.";
            link.l1 = "Что именно?";
            link.l1.go = "Step_F8_4";
        break;
        case "Step_F8_4":
            dialog.text = "Не знаю, знаком ли ты с этой историей... Ну, в общем, дело в следующем: некий весельчак, капитан Гей, устроил прогулку по морю, не разбирая, где свои, где чужие, пустил по миру пару английских купцов, а когда запахло жареным, свалил все на меня. Я тогда тоже был в походе, а у Мэдифорда давно руки чесались меня приструнить. В общем, в Порт Рояль мне доступа сейчас нет.";
            link.l1 = "И что ты предлагаешь?";
            link.l1.go = "Step_F8_5";
        break;
        case "Step_F8_5":
            dialog.text = "Понимаешь, мне все одно выгодна вся эта заварушка. Если Мэдифорд попросит меня об одолжении, то почитай обвинения с меня сняты. Ну а если ты сумеешь раздобыть документы, доказывающие, что не я устроил тот беспредел, в котором меня обвиняют - тоже хорошо. Второй вариант мне предпочтительней, так как с голландцами я портить отношения не хочу.";
            link.l1 = "Что ж, услуга за услугу. Постараюсь добыть документ, оправдывающий тебя. Что порекомендуешь сделать?";
            link.l1.go = "Step_F8_6";
        break;
        case "Step_F8_6":
            dialog.text = "Порыться в документах капитана Гея, мне нужен его судовой журнал.";
            link.l1 = "Ну что же, я все понял. Буду действовать, а ты пока ничего не предпринимай до моего возвращения.";
            link.l1.go = "Step_F8_7";
        break;
        case "Step_F8_7":
            dialog.text = "Поторопись...";
            link.l1 = "Всенепременно!";
            link.l1.go = "exit";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "6");
			NextDiag.TempNode = "Step_F8_Result";
			// ==> открываем дверь таверны Порт Рояля - там кэп Гей.
			LocatorReloadEnterDisable("PortRoyal_tavern", "reload2_back", false);
			LAi_LocationFightDisable(&Locations[FindLocation("PortRoyal_tavern_upstairs")], false);
            ref sld = GetCharacter(NPC_GenerateCharacter("CaptainGay", "bocman", "man", "man", 40, PIRATE, -1, true));
			FantomMakeCoolFighter(sld, 40, 100, 90, "blade34", "pistol5", 150);
            sld.Dialog.Filename    = "Quest\FraLineNpc_2.c";
        	sld.name 	= "Капитан";
        	sld.lastname 	= "Гей";
			sld.items.ShipsJournal = 1;
			sld.money = 65720;
			sld.items.potion4 = 4;
			sld.items.potionrum = 1;
			sld.items.potionwine = 2;
			sld.items.sculMa2 = 1;
			sld.items.jewelry17 = 30;
			sld.items.jewelry18 = 15;
			sld.SaveItemsForDead = true; // сохранять на трупе вещи
			sld.DontClearDead = true; // не убирать труп через 200с
			LAi_NoRebirthEnable(sld);
			LAi_SetLoginTime(sld, 0.0, 24.0);
			LAi_SetStayType(sld); 
			ChangeCharacterAddressGroup(sld, "PortRoyal_tavern_upstairs", "goto", "goto1");
		break;
        case "Step_F8_Result":
            dialog.text = "Ну как успехи, капитан? Раздобыл судовой журнал этого мерзавца Гея?";
			if (CheckCharacterItem(pchar, "ShipsJournal"))
			{
				link.l1 = "Новостей две, и обе хорошие, что бывает нечасто. Гей отправился к праотцам. А его журнал - держи, вот он.";
				link.l1.go = "Step_F8_8";
			}
			else
			{
				link.l1 = "Нет еще... Но я работаю над этим, все будет...";
				link.l1.go = "exit";
			}
        break;
        case "Step_F8_8":
            dialog.text = "Так, так... Да, я предполагал, что этот павлин во всех красках распишет свои подвиги. Ну что ж, эту тетрадку я переправлю с надежными людьми Мэдифорду, и уверен, больше он меня шантажировать не будет. А ты можешь успокоить голландцев - их я пальцем не трону!";
            link.l1 = "Отлично, Моррис. Ну, а теперь мне пора, прощай...";
            link.l1.go = "exit";
			pchar.questTemp.Count = makeint(pchar.questTemp.Count)+1;
			NextDiag.TempNode = "I_know_you_good";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "7");
            TakeItemFromCharacter(pchar, "ShipsJournal");
        break;

		//********************* Пиратская линейка *********************
		case "PL_Q8_deck":
			dialog.text = "И что ты делаешь у меня на палубе? Ведь идем на Порто Белло!";
			link.l1 = "Да, я помню...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;
		case "PL_Q8_deck_1":
			dialog.text = "Тебе не стоит терять время, ты уж поверь мне.";
			link.l1 = "Да, я знаю.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;


	}
}
