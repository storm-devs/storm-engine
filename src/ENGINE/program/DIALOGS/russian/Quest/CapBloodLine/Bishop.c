// Блад
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
    string sLocator;
    int iTime;
    string sTemp;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

/*
	switch(Pchar.questTemp.CapBloodLine.stat)
    {
  		case "1":
            Dialog.CurrentNode = "First Bishop";
		break;
		
		case "2":
            Dialog.CurrentNode = "First Guard";
		break;
		
		case "3":
            Dialog.CurrentNode = "First Stid";
		break;
		
		case "4":
            Dialog.CurrentNode = "First MsStid";
		break;

    }
    */


	switch(Dialog.CurrentNode)
	{
	    // --> Бишоп
		// ----------------------------------- Диалог первый - первая встреча
		case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
        case "Exit_Away":
            
            LAi_SetActorTypeNoGroup(npchar);
            LAi_ActorGoToLocation(npchar, "reload", Pchar.questTemp.CapBloodLine.sLocator, "none", "", "", "", sti(Pchar.questTemp.CapBloodLine.iTime));
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        
        case "Exit_RunAway":

            LAi_SetActorTypeNoGroup(npchar);
            LAi_ActorRunToLocation(npchar, "reload", Pchar.questTemp.CapBloodLine.sLocator, "none", "", "", "", sti(Pchar.questTemp.CapBloodLine.iTime));
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        
        case "GFight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
            sld = &characters[GetCharacterIndex("Dragun_0")];
            LAi_SetCheckMinHP(sld, 1, true, "Dragun_0_CheckMinHP");
            LAi_SetImmortal(sld, false);
   	        LAi_SetWarriorType(sld);
            LAi_group_MoveCharacter(sld, "TmpEnemy");
            /*
            LAi_group_SetHearRadius("TmpEnemy", 100.0);
            LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_Attack(sld, Pchar);
			*/
            LAi_group_FightGroups("TmpEnemy", LAI_GROUP_PLAYER, true);
			AddDialogExitQuest("MainHeroFightModeOn");
			//LAi_ActorRunToLocation(NPChar, "goto", "goto6", "none", "", "", "", 20);
			LAi_ActorGoToLocator(NPChar, "goto", "goto6", "", -1);

		break;
        
        case "fight1":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
   	        LAi_SetWarriorType(NPChar);
            LAi_group_MoveCharacter(NPChar, "TmpEnemy");
            LAi_group_SetHearRadius("TmpEnemy", 100.0);
            LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_Attack(NPChar, Pchar);
			AddDialogExitQuest("MainHeroFightModeOn");
			Spain_spyDie("");
			AddQuestRecord("WeaponsForEscape", "5");
			
		break;
		
        case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
   	        LAi_SetWarriorType(NPChar);
            LAi_group_MoveCharacter(NPChar, "TmpEnemy");
            LAi_group_SetHearRadius("TmpEnemy", 100.0);
            LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_Attack(NPChar, Pchar);
			AddDialogExitQuest("MainHeroFightModeOn");

		break;
		
		case "Finish":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			RestoreBridgetown();
			initMainCharacterItem();
            ref mc = GetMainCharacter();
            mc.Ship.Type = GenerateShip(SHIP_ARABELLA, true);
            mc.Ship.name="Арабелла";
            SetBaseShipData(mc);
            mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS32;
            SetCrewQuantityFull(mc);

            SetCharacterGoods(mc,GOOD_FOOD,2000);
        	SetCharacterGoods(mc,GOOD_BALLS,2000);//2000);
            SetCharacterGoods(mc,GOOD_GRAPES,300);//700);
            SetCharacterGoods(mc,GOOD_KNIPPELS,400);//700);
            SetCharacterGoods(mc,GOOD_BOMBS,2000);//1500);
            SetCharacterGoods(mc,GOOD_POWDER,2000);
            SetCharacterGoods(mc,GOOD_PLANKS,10);
            SetCharacterGoods(mc,GOOD_RUM,40);//600);
            SetCharacterGoods(mc,GOOD_WEAPON,2000);//2000);
            DoReloadCharacterToLocation(Pchar.HeroParam.Location, Pchar.HeroParam.Group, Pchar.HeroParam.Locator);
		break;
		
		case "NextQuest":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			Pchar.questTemp.CapBloodLine.stat = "";
            CapBloodLineInit("");
		break;


          


		
		case "First time":
        	dialog.text = "Ошибка";
            link.l1 = "...";
            link.l1.go = "Exit";
            npchar.quest.meeting = "1";
            
            if (npchar.id == "Pitt")
            {
     			if (bBettaTestMode)
                {
			        link.l0 = "BetaTest - пропустить и начать игру.";
			        link.l0.go = "Finish";
                }
                
                if (Pchar.questTemp.CapBloodLine.stat == "Begining")
                {
                    dialog.text = "Лорд Гилдой тяжело ранен... он здесь в доме я перетащил его туда... он послал меня за вами... Скорее к нему... скорей!";
            		link.l1 = "Спасибо Питт, я пойду к нему, а ты останься тут, и если увидишь, что сюда скачут королевские драгуны. немедленно предупреди нас.";
                    link.l1.go = "Exit_Away";
                    Pchar.questTemp.CapBloodLine.sLocator = "reload1";
                    Pchar.questTemp.CapBloodLine.iTime = -1;
           			if (bBettaTestMode)
                    {
				        link.l3 = "BetaTest - Ко второму квесту.";
				        link.l3.go = "NextQuest";
                    }
            		break;
        		}
        		
                if (Pchar.questTemp.CapBloodLine.stat == "CureMisStid")
                {
                    dialog.text = "Питер, полковник Бишоп разыскивает тебя весь вечер, у жены губернатора опять приступ. Тебе нужно срочно идти в резиденцию губернатора Стида.";
            		link.l1 = "Спасибо Питт, он сказал мне.";
            		link.l1.go = "Exit";
            		break;
        		}
                if (Pchar.questTemp.CapBloodLine.stat == "WakerOfferComplited")
                {
                    dialog.text = "Как успехи, друг мой?";
            		link.l1 = "Говори потише, ибо на карту поставлена наша дальнейшая судьба, коллега.";
            		link.l1.go = "PStep_0";
            		break;
        		}
        		
                dialog.text = "Извини Питер, мне нужно идти работать.";
        		link.l1 = "Хорошо, иди.";
        		link.l1.go = "Exit";

    		}
    		
            if (npchar.id == "Beyns")
            {
                dialog.text = "О, здравствуйте доктор Блад, как хорошо, что Вы пришли. Лорд Гилдой тяжело ранен... Он лежит сейчас в спальне на втором этаже, в западном крыле дома...";
        		link.l1 = "Я прибыл, как только смог. Я немедленно поднимусь к нему, а Вы пока постарайтесь раздобыть горячей  воды и полотна.";
        		link.l1.go = "EBStep_0";
    		}
    		
            if (npchar.id == "CapGobart")
            {
                dialog.text = "Я - капитан Гобарт из драгун полковника Кирка, Вы укрываете мятежников? Что за клаека лежит на втором этаже?";
        		link.l1 = "Мы не укрываем мятежников, сэр. Этот джентльмен ранен...";
        		link.l1.go = "CGStep_1";
        		DragunInvansion4();
    		}
            
            if (npchar.id == "Bridgetown_Mayor" )
            {
                if( Pchar.questTemp.CapBloodLine.stat == "CureMisStid" )
                {
                    dialog.text = "Я уже хотел, было, послать за Вакером. Почему так долго?";
            		link.l1 = "Я вынужден был задержаться. Прошу прощения, губернатор.";
            		link.l1.go = "SStep_0";
            		link.l2 = "Ваши люди задержали меня, губернатор Стид. Как выяснилось, Ваши приказы подлежат обсуждению, и ещё как - некоторые индивидуумы активно препятствуют мне по делу лечения испанских солдат.";
            		link.l2.go = "SStep_1";
            		break;
                }
                
                if( Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape3" )
                {
                    dialog.text = "Приветствую, доктор Блад. Чем обязан Вашему визиту?";
            		link.l1 = "Добрый день, губернатор. Я принёс ещё профилактического экстракта для Вашей супруги и хотел бы иметь возможность осмотреть её.";
            		link.l1.go = "SStep_9";
            		break;
                }
                dialog.text = "Что вам угодно доктор Блад?";
          		link.l1 = "Нет, ничего, извините за беспокойство.";
          		link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "MisStid" && Pchar.questTemp.CapBloodLine.stat == "CureMisStid")
    		{
            	dialog.text = "О, доктор Блад! Наконец-то... пожалуйста, помогите мне!";
        		link.l1 = "Добрый вечер, миссис Стид. Не волнуйтесь, Вы должны успокоиться и перестать жмуриться - потерпите и посмотрите на меня, мне нужно видеть ваши глаза.";
        		link.l1.go = "MSStep_0";
    		}
    		
    		if (npchar.id == "Nettl" ) // && Pchar.questTemp.CapBloodLine.stat == "CureMisStid"
    		{
            	dialog.text = "Эй, парень... я в трезвом уме и… и... здравии. Каком здра... здравии? Ах да, трезвом... да, трезвом. Не знаешь ли, как можно исч... исчезнуть где-то, аа... а потом появится где-то... где-то совсем не там, где... не там, где исчез?";
        		link.l1 = "Я бы и сам хотел это знать, старина.";
        		link.l1.go = "NStep_0";
    		}
    		
    		if (npchar.id == "Waker")
    		{
                if(Pchar.questTemp.CapBloodLine.stat == "CureMisStid")
                {
                	dialog.text = "Доктор Блад, как я рад Вас видеть! Я искал Вас сегодня, но поиски мои не увенчались успехом - Вы передвигаетесь по городу, словно призрак.";
            		link.l1 = "Жизнь научила. Где я могу найти мистера Дэна?" ;
            		link.l1.go = "WStep_0";
            		link.l2 = "Добрый вечер. Где я могу найти мистера Дэна?";
            		link.l2.go = "WStep_1";
                }
                
                if(Pchar.questTemp.CapBloodLine.stat == "WakerOffer")
                {
                	dialog.text = "Я слышал, миссис Стид отнимает у Вас уйму времени. Что ж, молодость и привлекательная внешность, доктор Блад! Молодость и красота! Это даёт врачу огромное преимущество, особенно когда он лечит дам!";
            		link.l1 = "Мне кажется, я угадываю Вашу мысль. Поделитесь ею не со мной, а с губернатором Стидом. Быть может, это его позабавит. Если у Вас всё, то с Вашего позволения..." ;
            		link.l1.go = "WStep_3";

                }
    		}
    		
    		if (npchar.id == "Den")
    		{
            	dialog.text = "Вечер добрый, доктор Блад. Могу я узнать, что Вы делали у меня дома?";
        		link.l1 = "У миссис Стид приступ мигрени, меня срочно вызвали к ней. Я осмотрел её и вынужден был отправиться к Вам в столь поздний час за необходимым лекарством. Не обнаружив Вас дома, я решил подождать, но уже собирался уходить." ;
        		link.l1.go = "DStep_0";
    			if (GetCharacterItem(pchar,"migraine_potion") > 0)
    			{
            			link.l2 = "У миссис Стид приступ мигрени, меня срочно вызвали к ней. Я осмотрел её и вынужден был отправиться к Вам в столь поздний час за необходимым лекарством. Не сумев Вас разыскать, я позволил себе взять лекарство без разрешения - ситуация крайне серьёзная. Прошу прощения.";
            			link.l2.go = "DStep_1";
    			}
        		link.l3 = "Я просто зашёл передать, что Вас разыскивал доктор Вакер, мистер Дэн.";
        		link.l3.go = "DStep_2";
    		}
    		

    		if (npchar.id == "Griffin")
    		{
                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape1")
                {
                	dialog.text = "Какого дьявола ты вваливаешься без стука, проклятый... а, это Вы, доктор Блад!";
            		link.l1 = "Добрый день, мистер Гриффин. Прошу прощения за бесцеремонное вторжение, но у меня к Вам дело, не терпящее отлагательств.";
            		link.l1.go = "GRStep_0";
            		break;
                }
                
                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape")
                {

                    dialog.text = "Ты кто такой, дьявол тебя разбери?!";
            		link.l1 = "Я доктор Блад, врач Бриджтауна." ;
            		link.l1.go = "GRStep_10";
            		break;

                }
                
                dialog.text = "Что ты здесь забыл дьявол тебя разбери?!";
                link.l1 = "Я доктор Блад, уже ухожу." ;
                link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "Hells")
    		{
                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape1_1")
                {
                	dialog.text = "Какого дьявола вам всем надо?! Защищайся!..";
            		link.l1 = "Спокойно, я - Питер Блад, врач Бриджтауна. Я не причиню Вам вреда.";
            		link.l1.go = "HStep_0";
            		link.l2 = "Раз уж ты просишь...";
            		link.l2.go = "fight1";
            		//LAi_ActorDialog(npchar, pchar, "", 2.0, 0);
                    //Spain_spyDie("");
            		break;
                }

                dialog.text = "Что ты здесь забыл дьявол тебя разбери?!";
                link.l1 = "Я доктор Блад, уже ухожу." ;
                link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "Quest_Smuggler")
    		{
                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape" && sti(Pchar.reputation) >= 50)
                {
                	dialog.text = "Слыхал о Вас, доктор Блад. Вы стали, в какой-то мере, звёздной личностью среди местных рабов, что гниют на плантациях. Чем же я могу Вам услужить?";
            		link.l1 = "С Вашего позволения, мы будем говорить тише. Всё дело в том, что мне нужно оружие...";
            		link.l1.go = "QSStep_0";
            		break;
                }

                dialog.text = "Здравствуйте доктор Блад, если узнают, что Вы со мной разговаривали, то Вас запорют до смерти. Так, что вам лучшей идти своей дорогой.";
                link.l1 = "Меня могу запороть до смерти и без этого... Хотя Вы правы - мне пора." ;
                link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "Spain_spy" )
    		{
            	dialog.text = "Эй, ты! Подожди...";
        		link.l1 = "Чем могу помочь?";
        		link.l1.go = "SSStep_0";
    		}

            
		break;
		
		
		case "First Bishop":
        	dialog.text = "У жены губернатора опять приступ. Немедленно отправляйся в резиденцию Стида и делай то, что должен.\nКогда закончишь, чтоб сразу сюда... и не дай боже тебе оказаться близ испанских пациентов!\nЗавтра я найду, чем тебя занять поважнее. Есть...";
            link.l1 = "...есть Вакер и Бронсон, которых Вы всегда можете порекомендовать губернатору, сославшись на мою чрезвычайную занятость на плантации. Врачи те ещё, но всё же люди образованные и трудолюбивые. В ином же случае, если губернатор Стид потребует доложить о состоянии раненых, я вынужден буду ему ответить, а после отправиться к беднягам и осмотреть их.";
            link.l1.go = "BStep_0";
            npchar.quest.meeting = "1";
		break;
		
		case "BStep_0":
        	dialog.text = "Не указывай мне, пёс!\nЛечи скорее знатное семейство Стид, или снисходительное отношение к тебе больного человека сменится холодным равнодушием политика.\nТогда посмотрим, как тебе понравится мотыжить на плантации вместо того, чтобы свободно шататься по городу! Помни своё место, раб.";
    		link.l1 = "Полагаю, Вы многим рискуете, задерживая меня, когда миссис Стид нуждается в скорой помощи.";
    		link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "houseSp1";
            Pchar.questTemp.CapBloodLine.iTime = 20;
            
            LAi_LockFightMode(pchar, false);
            chrDisableReloadToLocation = false;
            LAi_LocationFightDisable(loadedLocation, false);
            Pchar.questTemp.CapBloodLine.stat = "CureMisStid";
            //сроки 4 часа
            PChar.quest.CapBloodLineTimer_1.win_condition.l1            = "Timer";
            PChar.quest.CapBloodLineTimer_1.win_condition.l1.date.hour  = 4;
            PChar.quest.CapBloodLineTimer_1.win_condition.l1.date.day   = GetAddingDataDay(0, 0, 1);
            PChar.quest.CapBloodLineTimer_1.win_condition.l1.date.month = GetAddingDataMonth(0, 0, 0);
            PChar.quest.CapBloodLineTimer_1.win_condition.l1.date.year  = GetAddingDataYear(0, 0, 0);
            PChar.quest.CapBloodLineTimer_1.function = "CapBloodLine_q1_Late";
            AddQuestRecord("CapBloodLine_q1", "1");

		break;
		
		// -->Эндрю Бейнс

		case "EBStep_0":

            dialog.text = "Я принесу все, что Вы просите, прошу Вас, только поспешите, милорд совсем плох.";
        	link.l1 = "Я сделаю все что в моих силах.";
            link.l1.go = "Exit_RunAway";
            DeleteAttribute(npchar, "talker");
            Pchar.questTemp.CapBloodLine.sLocator = "Reload5";
            Pchar.questTemp.CapBloodLine.iTime = 5;
            
		break;
		
		case "EBStep_1":

            dialog.text = "Доктор, я подготовил, все что Вы просили.";
        	link.l1 = "Спасибо, поставьте там.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "EBStep_2";
            AddDialogExitQuestFunction("CureLordMovie");

		break;
		
		case "EBStep_2":

            dialog.text = "Что с лордом Гилдоем? Он выживет?";
        	link.l1 = "Худшее уже позади. Я обработал раны, дал лекарство, теперь он поправится, но ему нужен полный покой...";
            link.l1.go = "Exit";
            NextDiag.TempNode = "EBStep_3";
            AddDialogExitQuestFunction("DragunInvansion");

		break;
		
		case "EBStep_3":

            dialog.text = "Мне кажется, Вам следует воспользоваться советом Джереми Питта и раздобыть себе оружие.";
        	link.l1 = "Хорошо я так и поступлю.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "EBStep_3";

		break;
		
		//--> капитан Гобард

		case "CGStep_1":
        	dialog.text = "Это ясно  без слов! Нет нужды  спрашивать, где  ранен  этот проклятый мятежник... Взять его, ребята!";
    		link.l1 = "Во имя человечности,  сэр! Мы  живем в Англии, а не в Танжере. Этот человек тяжело  ранен,  его  нельзя трогать без опасности для жизни.";
    		link.l1.go = "CGStep_2";

		break;
		
		case "CGStep_2":
        	dialog.text = "Ах, так я еще должен заботиться о здоровье мятежников! Черт побери! Вы думаете,  что  мы  будем его  лечить? Вдоль всей  дороги  от  Вестона  до Бриджуотера расставлены виселицы, и он подойдет  для любой из них. Полковник Кирк научит этих дураковпротестантов кое-чему такому, о чем будут помнить их дети, внуки и правнуки!";
    		link.l1 = "Прекрасно, но если вы это сделаете, боюсь, что завтра могут повесить Вас. Он не принадлежит к категории людей, которых вы можете вздернуть, не  задавая  вопросов.  Он имеет право требовать суда, суда пэров.";
    		link.l1.go = "CGStep_3";

		break;
		
		case "CGStep_3":
        	dialog.text = "Суда пэров?";
    		link.l1 = "Разумеется. Любой человек, если он  не идиот или не  дикарь, прежде чем посылать  человека на  виселицу, спросил бы его фамилию. Этот человек - лорд Гилдой.";
    		link.l1.go = "CGStep_4";

		break;
		
		case "CGStep_4":
        	dialog.text = "Этот человек мятежник и его доставят в Бриджуотер, в тюрьму.";
    		link.l1 = "Он не перенесет этого пути. Его нельзя сейчас трогать.";
    		link.l1.go = "CGStep_5";

		break;
		
		case "CGStep_5":
        	dialog.text = "Тем хуже для него. Мое дело - арестовывать мятежников!\nА это что за тип, который прятался в камине? Еще один вельможа? Его-то я уж точно повешу собственными руками.";
    		link.l1 = "Вы угадали,  капитан. Это виконт Питт, двоюродный брат сэра Томаса Вернона, женатого на красотке Молли Кирк - сестре вашего  полковника.";
    		link.l1.go = "CGStep_6";

		break;

		case "CGStep_6":
        	dialog.text = "Ты лжешь, не правда ли? Клянусь богом, он издевается надо мной!";
    		link.l1 = "Если вы в  этом  уверены, то повесьте его и увидите, что с вами сделают.";
    		link.l1.go = "CGStep_7";
		break;

		case "CGStep_7":
        	dialog.text = "Дьявол. Да кто ты такой, черт бы тебя побрал? И Как ты здесь очутился?";
    		link.l1 = "Моё имя Питер Блад, я врач, и меня привезли сюда для оказания помощи раненому.";
    		link.l1.go = "CGStep_8";
		break;
		
		case "CGStep_8":
        	dialog.text = "Ну и кто же тебя пригласил? Мятежники?\nВзять  его! Свяжите и этих тоже. Мы покажем вам, как укрывать мятежников!";
    		link.l1 = "Ну уж нет. Этого я допустить не могу!";
    		link.l1.go = "GFight";

		break;
		
		
		// -->Стражник на входе
		
		case "First Guard":
        	dialog.text = "Ты куда собрался?";
    		link.l1 = "Моё имя Питер Блад, я здесь по распоряжению полковника Бишопа - мне приказано явиться для осмотра и лечения супруги губернатора.";
    		link.l1.go = "GStep_0";

		break;
		
		case "GStep_0":

            dialog.text = "Да, припоминаю тебя. Ты лечил моего брата и прочих с ''Прайд оф Дивон'', а также всю эту испанскую мразь, что чудом уцелела.\nБронсон мой друг.. а ты его разоряешь, равно как и Вакера, успевая всё и везде. Такие ловкачи среди рабов - редкость, и долго они не живут.";
        	link.l1 = "Прошу прощения, но в данный момент нездоровится супруге губернатора, и задерживая меня, Вы рискуете оказаться в одной клетке с ''испанской мразью''.";
            link.l1.go = "Exit";
            LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);//отпираем резеденцию
            //LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);
            LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", false);
            //солдата на место
            LAi_SetLoginTime(npchar, 6.0, 23.0);
            npchar.protector = false;
            npchar.protector.CheckAlways = 0;
            npchar.dialog.filename = "Quest\CapBloodLine\questNPC.c";

		break;
		
		case "GStep_1":

            dialog.text = "О, доктор Блад! На этот раз я что-то не в курсе, чтобы губернатор Стид посылал за Вами. Что Вам нужно?";
            if (GetCharacterItem(pchar,"migraine_potion") > 0)
            {
       	        link.l1 = "У меня на руках лекарство для профилактики мигрени. Моя задача состоит в том, чтобы доставить его миссис Стид и осмотреть её. Вы считаете, задерживать меня в такой момент - хорошая мысль?";
                link.l1.go = "Exit";
                LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);//отпираем резеденцию
                //LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);
                LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", false);
                //солдата на место
                npchar.protector = false;
                npchar.protector.CheckAlways = 0;
                npchar.dialog.filename = "Quest\CapBloodLine\questNPC.c";

            }
            else
            {
           	    link.l1 = "Раз уж я здесь, логично заключить, что мне нужно попасть к губернатору.";
                link.l1.go = "GStep_2";
            }

		break;
		
 		case "GStep_2":

            dialog.text = "Вон оно как. Но раз уж я Вас остановил, логично будет заключить, что в эту дверь Вам войти не удастся, пока я не узнаю о соответствующем распоряжении, отданном губернатором Стидом.";
        	link.l1 = "В таком случае, не смею более Вас отвлекать.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "GStep_3";
		break;
		
		case "GStep_3":

            dialog.text = "Ну что ещё?";
            if (GetCharacterItem(pchar,"migraine_potion") > 0)
            {
       	        link.l1 = "У меня на руках лекарство для профилактики мигрени. Моя задача состоит в том, чтобы доставить его миссис Стид и осмотреть её. Вы считаете, задерживать меня в такой момент - хорошая мысль?";
                link.l1.go = "Exit";
                LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);//отпираем резеденцию
                //LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", false);
                LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", false);
                //солдата на место
                npchar.protector = false;
                npchar.protector.CheckAlways = 0;
                npchar.dialog.filename = "Quest\CapBloodLine\questNPC.c";

            }
            else
            {
            	link.l1 = "Проверял - жив ты или нет.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GStep_3";
            }

		break;

        // --> Стид


 		case "SStep_0":

            ChangeCharacterReputation(PChar, 5);
            dialog.text = "Как бы там ни было, сейчас главное - моя жена. У неё опять приступ мигрени, она перевозбуждена и отвергает все попытки уложить её в постель!";
        	link.l1 = "Могу я увидеть её?";
            link.l1.go = "SStep_2";
		break;
		
 		case "SStep_1":

            dialog.text = "Как бы там ни было, сейчас главное - моя жена. У неё опять приступ мигрени, она перевозбуждена и отвергает все попытки уложить её в постель!";
        	link.l1 = "Могу я увидеть её?";
            link.l1.go = "SStep_2";
		break;

 		case "SStep_2":

            dialog.text = "Да-да, конечно - она в спальне на втором этаже. Вон там - дверь справа от вас и вверх по лестнице. Проходите.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_3";

			sld = GetCharacter(NPC_GenerateCharacter("MisStid", "AnnaDeLeiva", "woman", "towngirl2", 10, ENGLAND, 3, false));
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
			sld.name = "Миссис";
			sld.lastname = "Стид";
			sTemp = GetNationNameByType(ENGLAND) + "_citizens";
            LAi_group_MoveCharacter(sld, sTemp);
			LAi_SetStayTypeNoGroup(sld);
			ChangeCharacterAddressGroup(sld, "CommonBedroom", "goto","goto5");
			AddQuestRecord("CapBloodLine_q1", "2");
		break;
		
		
		case "SStep_3":

            sld = &characters[GetCharacterIndex("MisStid")];
            if (sld.quest.meeting != "1")
            {
                dialog.text = "Доктор Блад, осмотрите мою жену поскорее.";
                link.l1 = "Уже иду.";
                link.l1.go = "Exit";
            }
            else
            {
                dialog.text = "Как она, доктор? Вы...";
                link.l1 = "Всё будет в порядке, губернатор Стид, но мне необходимо немедленно отправиться за нужными лекарствами домой к аптекарю. Просто решил поставить Вас в известность.";
                link.l1.go = "SStep_4";
                
                link.l2 = "Всё будет в порядке, губернатор Стид, если мне удастся застать дома мистера Дэна и разбудить его. Срочно нужны лекарства, и я прошу вас выдать мне средства для их покупки.";
                link.l2.go = "SStep_5";
            }
		break;
		
		case "SStep_4":

            dialog.text = "Прошу Вас, поторопитесь!";
            link.l1 = "Уже иду.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_4";

		break;
		
		case "SStep_5":

            dialog.text = "О, конечно, разумеется... сколько вам нужно?";
            link.l1 = "500 пиастров.";
            link.l1.go = "SStep_6";
            link.l2 = "1000 пиастров.";
            link.l2.go = "SStep_7";
            link.l3 = "2500 пиастров, сэр. Это очень дорогой целительный экстракт.";
            link.l3.go = "SStep_8";

		break;
		
		case "SStep_6":

            AddMoneyToCharacter(pchar, 500);
            dialog.text = "Вот возьмите 500 пиастров.";
            link.l1 = "Я должен поспешить.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_4";
            AddQuestRecord("CapBloodLine_q1", "4");
            AddQuestUserData("CapBloodLine_q1", "iMoney", 500);

		break;
		
		case "SStep_7":

            AddMoneyToCharacter(pchar, 1000);
            dialog.text = "Вот возьмите 1000 пиастров, я надеюсь, что это для мистера Дэна.";
            link.l1 = "Я должен поспешить.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_4";
            AddQuestRecord("CapBloodLine_q1", "4");
            AddQuestUserData("CapBloodLine_q1", "iMoney", 1000);

		break;
		
		case "SStep_8":

            AddMoneyToCharacter(pchar, 2500);
            dialog.text = "Это большая сумма, доктор. Я выдам её, но прошу отчитаться по возвращении - я всё проверю.";
            link.l1 = "Я должен поспешить.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_4";
            AddQuestRecord("CapBloodLine_q1", "5");


		break;
		
		case "SStep_9":

            dialog.text = "Право слово, конечно, я даю Вам такую возможность. Вы считаете, следует опасаться нового приступа?";
            link.l1 = "Я прибыл сюда, чтобы предотвратить его, сэр. С Вашего позволения...";
            link.l1.go = "Exit";
            NextDiag.TempNode = "First time";
            LocatorReloadEnterDisable("Bridgetown_Townhall", "reload3", false);
            LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", false);
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape3_1";

		break;

        //--> Миссис Стид

        case "MSStep_0":

            dialog.text = "Вы можете что-то сделать, доктор?";
        	link.l1 = "Похоже, никак не обойтись без лекарств, но аптека давно закрыта. Мне придётся пойти к мистеру Дэну домой, чтобы получить всё необходимое. Попейте тёплой воды и ждите меня, я вернусь так скоро, как только смогу!";
            link.l1.go = "Exit";
            NextDiag.TempNode = "MSStep_1";
            
            sld = GetCharacter(NPC_GenerateCharacter("Waker", "usurer_5", "man", "man", 7, ENGLAND, 3, false));
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
            sld.name = "доктор";
            sld.lastname = "Вакер";
            sTemp = GetNationNameByType(ENGLAND) + "_citizens";
            LAi_group_MoveCharacter(sld, sTemp);
			ChangeCharacterAddressGroup(sld, "CommonPirateHouse", "goto","goto6");
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorDialog(sld, pchar, "", 2.0, 0);
            
            pchar.quest.CureMisStid.win_condition.l1 = "item";
			pchar.quest.CureMisStid.win_condition.l1.item= "migraine_potion";
			pchar.quest.CureMisStid.function = "CapBloodLine_q1_End";
			
			sld = ItemsFromID("migraine_potion");
			sld.shown = true;
			sld.startLocation = "CommonPirateHouse";
			sld.startLocator = "item1";
			
			AddQuestRecord("CapBloodLine_q1", "3");


		break;
		
        case "MSStep_1":

            dialog.text = "Вы принесли лекарства, доктор?";
            if(Pchar.questTemp.CapBloodLine.stat == "CureMisStid_Complite")
            {
                link.l1 = "Да, оно быстро поставит Вас на ноги.";
                link.l1.go = "MSStep_2";
                TakeItemFromCharacter(Pchar, "migraine_potion");

            }
            else
            {
                link.l1 = "Еще нет, я как раз шел за ними.";
                link.l1.go = "Exit";
            }

		break;
		
        case "MSStep_2":
        
        NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		//Вывести экран в темноту, выполнить квест questFadeOut, вернуть всё обратно, выполнить квест questFadeIn
        //LAi_Fade("", "CapBloodLine_q1_Complited");
        CapBloodLine_q1_Complited("");

		break;
		
        case "MSStep_3":

            dialog.text = "Здравствуйте, доктор Блад. Мне значительно лучше и, похоже, я действительно пошла на поправку - всё благодаря Вам.";
        	link.l1 = "Благодарю, миссис Стид. Я здесь, чтобы...";
            link.l1.go = "MSStep_4";
            DeleteAttribute(npchar, "talker");

		break;
		
        case "MSStep_4":

            dialog.text = "Я вижу, Вы не сводите глаз с этого колечка... быть может, оно Ваше?";
        	link.l1 = "Мм... моё?";
            link.l1.go = "MSStep_5";
           	link.l1 = "Нет, увы, оно принадлежит не мне.";
            link.l1.go = "MSStep_5";

		break;
		
        case "MSStep_5":

            dialog.text = "Дело в том, что оно и не моё. Буквально вчера подобного кольца у меня не было, но сегодня утром я обнаружила его там, где оно находится сейчас. Эх, если бы мой муж не был уверен в том, что во время появления кольца он был со мной, и никто появиться здесь просто не мог, он бы...";
        	link.l1 = "Очень интересно.";
            link.l1.go = "MSStep_6";

		break;
		
        case "MSStep_6":

            dialog.text = "Колечко простенькое, без изысков. Не мой стиль, да и… забирайте его, доктор Блад. Чьё бы оно ни было, мне оно ни к чему.";
        	link.l1 = "Что Вы, миссис Стид! Я не могу принять его... впрочем, я могу попробовать выяснить для Вас, чьё оно и как сюда попало.";
            link.l1.go = "MSStep_7";

		break;
		
        case "MSStep_7":

            dialog.text = "Пусть будет так. А теперь я вынуждена просить Вас покинуть меня.";
        	link.l1 = "Вот, я принёс Вам ещё немного чудодейственного экстракта, что и было изначальной целью моего визита. Всего наилучшего.";
            TakeItemFromCharacter(Pchar, "migraine_potion");
            link.l1.go = "Exit";
            NextDiag.TempNode = "MSStep_8";


		break;
		
        case "MSStep_8":

            dialog.text = "Досвидание доктор Блад.";
        	link.l1 = "Всего доброго.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "MSStep_8";
		break;
		

		
        //--> Нэтталл

        case "NStep_0":

            dialog.text = "Ээ... если узнаешь... если способ узнаешь, то... меня не брось - я помогу. Пра... правда.";
        	link.l1 = "Отправляйся-ка домой, старый морской волк. Тебе надо поспать.";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.TalkWithNettl = true;
            Pchar.questTemp.CapBloodLine.sLocator = "reload5_back";
            Pchar.questTemp.CapBloodLine.iTime = 20;

		break;
		
        //--> Вакер

        case "WStep_0":
        
            AddCharacterExpToSkill(pchar, "Sneak", 50);
            dialog.text = "Я бы и сам рад узнать, где он. Жду его здесь уже битый час, а его всё нет и нет. Уж не сгинул ли бедняга с перепоя?\nА меж тем, у меня к Вам разговор...";
        	link.l1 = "Я очень тороплюсь, доктор Вакер, мне необходимо найти мистера Дэна как можно скорее. Разговор придётся отложить.";
            link.l1.go = "WStep_2";

		break;
		
        case "WStep_1":

            dialog.text = "Я бы и сам рад узнать, где он. Жду его здесь уже битый час, а его всё нет и нет. Уж не сгинул ли бедняга с перепоя?\nА меж тем, у меня к Вам разговор...";
        	link.l1 = "Я очень тороплюсь, доктор Вакер, мне необходимо найти мистера Дэна как можно скорее. Разговор придётся отложить.";
            link.l1.go = "WStep_2";

		break;
		
        case "WStep_2":

            dialog.text = "Что ж, я и сам не в настроении обсуждать всё то, что обсудить необходимо, именно здесь и сейчас. Я, пожалуй, пойду, а Вы, если угодно, дождитесь Дэна. Поговорим завтра. Как только найдёте свободную минуту, приходите в таверну. Очень на Вас надеюсь.";
        	link.l1 = "Досвидания.";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "reload1";
            Pchar.questTemp.CapBloodLine.iTime = 5;

            sld = GetCharacter(NPC_GenerateCharacter("Den", "usurer_1", "man", "man", 7, ENGLAND, 3, false));
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
            sld.name = "мистер";
            sld.lastname = "Дэн";
            sTemp = GetNationNameByType(ENGLAND) + "_citizens";
            LAi_group_MoveCharacter(sld, sTemp);
			ChangeCharacterAddressGroup(sld, "BridgeTown_town", "goto","goto3");
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorDialog(sld, pchar, "", 2.0, 0);
            AddQuestRecord("CapBloodLine_q2", "1");

		break;
		//Предложение Вакера - вторая встреча
        case "WStep_3":

            dialog.text = "Не будьте так вспыльчивы, мой друг. Вы неправильно поняли, у меня вовсе не было таких мыслей. Я хочу помочь Вам!\nВедь рабство должно быть очень неприятно для такого талантливого человека, как Вы.";
        	link.l1 = "Какая проницательность!";
            link.l1.go = "WStep_4";

		break;
		
        case "WStep_4":

            dialog.text = "Я не дурак, дорогой коллега. Я вижу человека насквозь и могу даже сказать, что он думает.";
        	link.l1 = "Вы убедите меня в этом, если скажете, о чём думаю я.";
            link.l1.go = "WStep_5";

		break;
		
        case "WStep_5":

            dialog.text = "Не раз наблюдал я за Вами, когда Вы тоскливо всматривались в морскую даль. И Вы полагаете, что я не знаю Ваших мыслей?\nЕсли бы Вам удалось спастись из этого ада, Вы могли бы, как свободный человек, с удовольствием и выгодой для себя всецело отдаться своей профессии, украшением которой Вы являетесь.\nМир велик, и, кроме Англии, есть ещё много стран, где такого человека, как Вы, всегда тепло встретят. Помимо английских колоний, есть и другие.\nОтсюда совсем недалеко до голландской колонии Кюрасао. В это время года туда вполне можно добраться даже на небольшой лодке. Кюрасао может стать мостиком в огромный мир. Он откроется перед Вами, как только Вы освободитесь от цепей.\nЧто Вы на это скажете?";
        	link.l1 = "У меня нет денег, а ведь для такого путешествия их потребуется немало.";
            link.l1.go = "WStep_6";

		break
		
        case "WStep_6":

            dialog.text = "Разве я не сказал, что хочу быть Вашим другом?";
        	link.l1 = "Почему?";
            link.l1.go = "WStep_7";
           	link.l2 = "Это очень благородно с Вашей стороны, коллега. Именно так поступил бы и я, если бы мне представился подобный случай.";
            link.l2.go = "WStep_8";
            
		break;
		
        case "WStep_7":

            dialog.text = "Вы себе не представляете, как обливается кровью моё сердце при виде коллеги, изнывающего в рабстве и лишённого возможности применить на деле свои чудесные способности! К чему Вам томиться в оковах, хоть и невидимых глазу, когда Вы можете отправиться к горизонту, сделать этот мир хоть капельку лучше...\nДа и нам оставить нашу работу в Бриджтауне... нда?..";
        	link.l1 = "Для побега, помимо мужества, нужны и деньги. Шлюп обойдётся, вероятно, тысяч в двадцать.";
            link.l1.go = "WStep_9";

		break;
		
        case "WStep_8":

            dialog.text = "Значит, Вы согласны?";
        	link.l1 = "Для побега, помимо мужества, нужны и деньги. Шлюп обойдётся, вероятно, тысяч в двадцать.";
            link.l1.go = "WStep_9";

		break;
		
        case "WStep_9":

            dialog.text = "Деньги Вы получите! Это будет заём, который Вы нам вернёте... вернёте мне, когда сможете.";
        	link.l1 = "Мне необходимо подготовиться, всё обдумать и согласовать с нужными людьми. Завтра мы продолжим нашу беседу. Вы приоткрыли мне двери надежды, коллега!";
            link.l1.go = "Exit";
            int n= FindLocation("Bridgetown_tavern");
            locations[n].reload.l2.disable = true;
            Pchar.questTemp.CapBloodLine.stat = "WakerOfferComplited";
            AddQuestRecord("CapBloodLine_q2", "4");
            NextDiag.TempNode = "WStep_10";

		break;
		
        case "WStep_10":

            dialog.text = "До завтра, коллега.";
        	link.l1 = "Досвидания.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "WStep_10";

		break;
		
        //--> Мистер Дэн

        case "DStep_0":

            dialog.text = "Какой кошмар! Вот, у меня есть с собой склянка - возьмите. Сам страдаю не первый день, а потому всегда держу при себе. Но ничего, у меня запасено ещё немного. Берите даром, доктор - это же мой гражданский долг!";
        	link.l1 = "Всего доброго.";
            link.l1.go = "Exit_Away";
            GiveItem2Character(Pchar, "migraine_potion");

		break;

        case "DStep_1":

            dialog.text = "Что Вы! На кону здоровье миссис Стид - Вы правильно поступили! Поторопитесь же!";
        	link.l1 = "Всего доброго.";
            link.l1.go = "Exit_Away";
            ChangeCharacterReputation(PChar, 5);

		break;
		
        case "DStep_2":

            dialog.text = "Хм... ясно.";
        	link.l1 = "Всего доброго.";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "houseSp1";
            Pchar.questTemp.CapBloodLine.iTime = 5;

		break;
		
        case "DStep_3":

            dialog.text = "Здравствуйте, доктор Блад. Чем могу Вам помочь?";
        	link.l1 = "Добрый день, мистер Дэн. Я здесь по распоряжению его превосходительства губернатора Стида. Мне срочно необходим Ваш чудный экстракт для профилактики мигрени.";
            link.l1.go = "DStep_4";

		break;
		
        case "DStep_4":

            dialog.text = "О, конечно, вот возьмите - у меня немного осталось. Передайте поклон от меня губернатору.";
        	link.l1 = "Благодарю Вас. Позвольте откланяться.";
            link.l1.go = "Exit";
            GiveItem2Character(Pchar, "migraine_potion");
            NextDiag.TempNode = "DStep_5";

		break;
		
        case "DStep_5":

            dialog.text = "Вам что-то необходимо?";
        	link.l1 = "Нет, спасибо.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "DStep_5";

		break;
		
		//--> Джереми Питт

		 case "PStep_0":

            dialog.text = "Что? О чём ты говоришь?";
        	link.l1 = "Я говорю о возможности завтрашним утром прикупить шлюп на верфи, собрать людей с плантации и из города - разумеется, исключительно тех, кому можно доверять - и отправиться к горизонту на юго-запад, к Голландской колонии Кюрасао.";
            link.l1.go = "PStep_1";

		break;

        case "PStep_1":

            dialog.text = "Не в бреду ли ты, доктор? Ни денег, ни влияния среди рабов, ни связного в городе - ничего этого у нас нет и быть не может.";
        	link.l1 = "Послушай меня, Джереми Питт. Ты - единственный штурман среди нас. Без тебя мне не добраться до нужного места, даже если я соберу людей и благополучно отплыву на шлюпе аккурат за полночь завтрашнего дня. Ты нужен мне сейчас так же, как я когда-то был нужен тебе в усадьбе Оглторпа...";
        	link.l1.go = "PStep_2";  //(+ небольшой % авторитета)
        	link.l2 = "Хочешь ты убраться отсюда или нет?";
        	link.l2.go = "PStep_3";

		break;

        case "PStep_2":

            dialog.text = "Хорошо, я понял, я всё понял. Сделаю всё, что в моих силах, чтобы помочь...";
        	link.l1 = "Помни, что, выдав себя, ты погубишь всё: ведь ты - единственный штурман среди нас, и без тебя бегство невозможно.";
            link.l1.go = "Exit";
            AddCharacterExpToSkill(pchar, "LeaderShip", 50);
            NextDiag.TempNode = "PStep_4";
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape";
            AddQuestRecord("CapBloodLine_q2", "5");
            AddQuestRecord("WeaponsForEscape", "1");

		break;
		
        case "PStep_3":

            dialog.text = "Хорошо, я понял, я всё понял. Сделаю всё, что в моих силах, чтобы помочь...";
        	link.l1 = "Помни, что, выдав себя, ты погубишь всё: ведь ты - единственный штурман среди нас, и без тебя бегство невозможно.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "PStep_4";
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape";
            AddQuestRecord("CapBloodLine_q2", "5");
            AddQuestRecord("WeaponsForEscape", "1");
		break;
		
        case "PStep_4":

            dialog.text = "Набери людей и подготовь всё необходимое. Я буду ждать тебя здесь.";
        	link.l1 = "До встречи.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "SStep_4";
		break;
		
        case "PStep_5":

            dialog.text = "Питер, сюда скачут драгуны Танжерского полка, они ищут участников восстания...";
        	link.l1 = "Думаю нам нечего бояться, ведь мы живем в  христианской стране, а христиане не воюют с ранеными и с теми, кто их приютил. А вот Питту лучше куда-нибудь исчезнуть.";
            link.l1.go = "PStep_6";
            //NextDiag.TempNode = "SStep_4";
            
		break;
		
        case "PStep_6":

            dialog.text = "Хорошо я спрячусь в каминную трубу, но ты плохо знаешь Полковника Кирка, он сущий дьявол. А у тебя нет даже шпаги, чтобы постоять за себя.\nВозьми мою, я спрятал её на балконе в сундуке. И поспеши, драгуны вот-вот ворвутся в усадьбу.";
        	link.l1 = "Спасибо, я схожу за ней, но надеюсь она мне не понадобится.";
            link.l1.go = "Exit_RunAway";
            Pchar.questTemp.CapBloodLine.sLocator = "reload2";
            Pchar.questTemp.CapBloodLine.iTime = 5;
            AddDialogExitQuestFunction("DragunInvansion2");
            
		break;
		
        case "PStep_7":

            dialog.text = "Здравствуй Питер, сегодня исполняется ровно месяц, как мы работаем на сахарных плантациях полковника Бишопа. Порой мне кажется, что уж лучше бы нас казнили - так тяжело нам приходится. Все настолько истощены, что просто валятся с ног\nТолько вчера бывший кузнец из Бриджуотера в назидание остальным был насмерть запорот плетьми, а он всего лишь возмутился строгостью надсмотрщика Кента. Нужно что-то делать Питер, иначе мы не доживем и до зимы.";
        	link.l1 = "Я постоянно  думаю об этом, но просто бежать отсюда, без какой-либо поддержки извне слишком рискованно, взять хотя бы того беднягу, который осмелился  бежать две недели назад и был пойман.  Его выпороли, а после выжгли на  лбу буквы ''Б.К.'', чтобы  до конца жизни все знали, что это беглый каторжник.";
            link.l1.go = "PStep_8";
        break;
        
        case "PStep_8":
            dialog.text = "К счастью для страдальца, он умер от побоев\nНо я пришел к тебе не жаловаться на тяготы, а чтобы сообщить, что тебя зачем-то разыскивает Бишоп. Постарайся не злить его, а то мы рискуем остаться без твоей врачебной помощи.";
        	link.l1 = "Спасибо Питт, тогда я пойду к нему, и обещаю, что буду, кроток как овечка.";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "reload1";
            Pchar.questTemp.CapBloodLine.iTime = -1;
            NextDiag.TempNode = "First time";
            Pchar.questTemp.CapBloodLine.stat = "";
            chrDisableReloadToLocation = false;
		break;
		
		//-->оружейние Гриффин

        case "GRStep_0":

            dialog.text = "При всём уважении, доктор, коего Вы бесспорно заслуживаете... провалитесь к морскому чёрту ваши неотложные дела!";
        	link.l1 = "Сожалею, что застал Вас не в лучшем расположении духа. Быть может, я могу Вам чем-то помочь, после чего Вы меня выслушаете?";
            link.l1.go = "GRStep_1";

		break;
		
        case "GRStep_1":

            dialog.text = "Помочь мне? Вы?! Чем может своевольный раб помочь...";
        	link.l1 = "Похоже, Вы недооцениваете меня, мистер Гриффин. Что ж, Вам и многим другим ещё предстоит об этом пожалеть!";
            link.l1.go = "GRStep_2";
        	link.l2 = "Почему нет? Я нуждаюсь в Вашей помощи и, похоже, предложить могу в данной ситуации разве что помощь взаимную.";
            link.l2.go = "GRStep_3";
		break;
		
        case "GRStep_2":

            AddQuestRecord("WeaponsForEscape", "4");
            dialog.text = "Убирайся прочь, доктор!";
            link.l1.go = "Exit";
            pchar.quest.PrepareToEscape2.win_condition.l1          = "location";
            pchar.quest.PrepareToEscape2.win_condition.l1.location = "Bridgetown_town";
            pchar.quest.PrepareToEscape2.function                  = "LoginSpain_spy";
        	Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape2";

		break;
        case "GRStep_3":

            dialog.text = "Что ж, посмотрим. Дело в том, что сегодня ко мне зашёл испанец. Он не назвал своего имени, но я по глазам понял, что человек этот готов на всё. Не знаю, откуда он взялся и как попал в город, но ему нужно было оружие. Да, доктор, я испугался... а потому не могу обратиться к властям сейчас, потому что должен был сделать это раньше. Испанец отказался ждать, пока я изготовлю заказ, и взял мои образчики со стены - несколько абордажных сабель, длинноствольный мушкет и пара пистолетов. Треклятый испанский боров!";
        	link.l1 = "...знаю, знаю - ''к морскому дьяволу''.";
            link.l1.go = "GRStep_2";
            NextDiag.TempNode = "GRStep_2";
           	link.l2 = "Он не оплатил оружие?";
            link.l2.go = "GRStep_4";

		break;
        case "GRStep_4":

            dialog.text = "К дьяволу оплату! На образчиках выгравировано моё имя! Если он вступит в бой с моим оружием в руках, кто-то обязательно это заметит… да и без боя его поймают рано или поздно! Меньше всего мне хочется кончить жизнь на рее сейчас.";
        	link.l1 = "Если Вы скажете мне, куда отправился испанец, я попробую решить Вашу проблему. Взамен я лишь прошу отдать мне то, что забрал этот человек.";
            link.l1.go = "GRStep_5";

		break;
		
        case "GRStep_5":

            dialog.text = "Оружие с моим именем на клинке у испанца или раба... да какая дьяволу разница?! Мы вместе будем висеть на центральной площади завтра к рассвету!";
        	link.l1 = "Дьяволу, думаю, никакой. Хорошо. Я верну Вам оружие, а Вы взамен к сегодняшней ночи подготовите для меня несколько абордажных сабель и пистолетов без Ваших опознавательных знаков. Идёт?";
            link.l1.go = "GRStep_6";

		break;
		
        case "GRStep_6":

            dialog.text = "Испанский головорез выпотрошит тебя, доктор. Но меня это не касается. Верни мне образчики, и я сделаю для тебя то, что ты просишь. За полторы тысячи пиастров. Он отправился в порт к рыбаку Хелльсу, чей дом неподалёку от дома аптекаря Дэна.";
        	link.l1 = "Я скоро вернусь, любезнейший мистер Гриффин. Готовьте всё необходимое.";
            link.l1.go = "GRStep_7_1";
           	link.l2 = "Не слишком ли много Вы просите? Я и без того дал согласие рискнуть жизнью ради Вашей репутации. Тысяча и ни фунтом больше.";
            link.l2.go = "GRStep_7_2";

            sld = GetCharacter(NPC_GenerateCharacter("Spain_spy", "shipowner_13", "man", "man", 7, ENGLAND, -1, false));
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
            sld.name = "";
            sld.lastname = "Испанец";
            GiveItem2Character(sld, "Griffins_Weapon");
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
            sld.DontClearDead = true;
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorSetLayMode(sld);
            LAi_SetImmortal(sld, true);
			ChangeCharacterAddressGroup(sld, "CommonRoom_MH2", "goto","goto2");
			
            sld = &characters[GetCharacterIndex("Hells")];
            LAi_SetActorType(sld);
            LAi_ActorDialog(sld, pchar, "", 1.0, 0);

		break;
		
        case "GRStep_7_1":

            dialog.text = "Хорошо, поторопитесь.";
        	link.l1 = "Вам не придётся долго ждать.";
            link.l1.go = "Exit";
            Pchar.questTemp.CapBloodLine.iMoney = 1500;
            NextDiag.TempNode = "GRStep_8";
			Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1_1";
            AddQuestRecord("WeaponsForEscape", "3");
            AddQuestUserData("WeaponsForEscape", "iMoney", 1500);

		break;
		
        case "GRStep_7_2":

            dialog.text = "Хорошо, поторопитесь.";
        	link.l1 = "Вам не придётся долго ждать.";
            link.l1.go = "Exit";
            Pchar.questTemp.CapBloodLine.iMoney = 1000;
            NextDiag.TempNode = "GRStep_8";
			Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1_1";
            AddQuestRecord("WeaponsForEscape", "3");
            AddQuestUserData("WeaponsForEscape", "iMoney", 1000);

		break;
		
        case "GRStep_8":

            dialog.text = "Вы пришли раньше, чем я ожидал Вас увидеть. Что произошло?";
            if (GetCharacterItem(pchar,"Griffins_Weapon") > 0)
            {
            	link.l1 = "Вот ваши образчики, мистер Гриффин - все до единого. Испанский головорез более не представляет опасности.";
                link.l1.go = "GRStep_9";
                TakeItemFromCharacter(Pchar, "Griffins_Weapon");
            }
            else
            {
            	link.l1 = "Я просто зашел Вас проведать.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GRStep_8";
            }

		break;
		
        case "GRStep_9":

            dialog.text = "Хм... что ж, меня даже не интересует, как Вам это удалось и многих ли Вы убили в процессе. Сегодня к полуночи точно такой же свёрток будет готов для Вас. А теперь попрошу меня оставить, предварительно, конечно, оплатив мой труд.";
            if (makeint(pchar.money) >= sti(Pchar.questTemp.CapBloodLine.iMoney))
            {
           	    link.l1 = "Да, безусловно, дорогой мистер Гриффин. Вот Ваши деньги. Я зайду в назначенный час.";
                link.l1.go = "Exit";
                AddMoneyToCharacter(pchar, (-sti(Pchar.questTemp.CapBloodLine.iMoney)));
                Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1_3";
                NextDiag.TempNode = "GRStep_12";
            }
            else
            {
           	    link.l1 = "У меня сейчас нет при себе нужной суммы я зайду позднее.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GRStep_13";
            }
            AddQuestRecord("WeaponsForEscape", "6");

		break;
		
        case "GRStep_10":

            dialog.text = "Да, что-то припоминаю. Но какого якоря тебе здесь надо?";
        	link.l1 = "Мне... не помешало бы кое-что из Вашего ассортимента.";
            link.l1.go = "GRStep_11";

		break;
		
        case "GRStep_11":

            dialog.text = "Что?! Оружие... тебе?! Ты сошёл с ума, морской дьявол! Убирайся!";
        	link.l1 = "";
            link.l1.go = "Exit";
            pchar.quest.PrepareToEscape2.win_condition.l1          = "location";
            pchar.quest.PrepareToEscape2.win_condition.l1.location = "Bridgetown_town";
            pchar.quest.PrepareToEscape2.function                  = "LoginSpain_spy";
        	Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape2";


		break;
		
        case "GRStep_12":

            if (Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape1_3" && GetHour() > 22)
            {
                dialog.text = "Вот, возьмите,я для Вас этого не делал.";
                link.l1 = "Спасибо.";
                link.l1.go = "Exit";
                GiveItem2Character(Pchar, "Weapon_for_escape");
                NextDiag.TempNode = "First time";
          		CloseQuestHeader("WeaponsForEscape");
                Pchar.questTemp.CapBloodLine.stat = "ReadyToEscape";
            }
            else
            {
                dialog.text = "Оружие для Вас еще не готово, приходите ближе к ночи.";
                link.l1 = "Хорошо.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GRStep_12";
            
            }

		break;
		
        case "GRStep_13":

            dialog.text = "Вы принесли деньги?";
            if (makeint(pchar.money) >= sti(Pchar.questTemp.CapBloodLine.iMoney))
            {
                link.l1 = "Да, вот Ваши деньги. Я зайду в назначенный час.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GRStep_12";
                AddMoneyToCharacter(pchar, (-sti(Pchar.questTemp.CapBloodLine.iMoney)));
                Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1_3";
            }
            else
            {
           	    link.l1 = "У меня сейчас нет при себе нужной суммы я зайду позднее.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "GRStep_13";
            }


		break;
		
        case "GRStep_14":

            dialog.text = "Боже правый!..";
        	link.l1 = "Не совсем. Тем не менее, я зашёл узнать - быть может, Вы передумали?..";
            link.l1.go = "GRStep_15";

		break;
		
        case "GRStep_15":

            dialog.text = "Ппе... передумал?";
        	link.l1 = "Оружие, уважаемый мистер Гриффин. Мне нужно оружие. Всего лишь несколько абордажных сабель и пара пистолетов, если Вас не затруднит.";
            link.l1.go = "GRStep_16";

		break;

        case "GRStep_16":

            dialog.text = "Я... я не могу, сэр. При всём желании, меня же... повесят!";
        	link.l1 = "Успокойтесь, мне вовсе не нужно, чтобы Вы что-то делали официально. Никаких опознавательных гравировок на эфесе, никаких бумаг и отчётов - всё тихо и мирно. Никто и никогда не узнает о том, что это произошло.";
            link.l1.go = "GRStep_17";

		break;
		
        case "GRStep_17":

            dialog.text = "Ммм... ладно, хорошо. У меня есть всё это, вот - возьмите. Надеюсь, этого достаточно.";
        	link.l1 = "Более чем. Премного благодарен.";
            link.l1.go = "Exit";
            
            GiveItem2Character(Pchar, "Weapon_for_escape");
            AddQuestRecord("WeaponsForEscape", "9");
       		CloseQuestHeader("WeaponsForEscape");
            Pchar.questTemp.CapBloodLine.stat = "ReadyToEscape";
            NextDiag.TempNode = "First time";

		break;


        //-->рыбак Хелльс
		
        case "HStep_0":

            dialog.text = "Врач... что тебе здесь надо?";
        	link.l1 = "Мне нужен был человек, чьё бездыханное тело сейчас лежит у Ваших ног.";
            link.l1.go = "HStep_1";

		break;
		
        case "HStep_1":

            dialog.text = "Желаешь отомстить за ублюдка-дружка? Давай, щенок!";
        	link.l1 = "Вы слишком агрессивны, мистер Хелльс. Я пропишу Вам успокоительное. Дело в том, что я лишь хочу поблагодарить Вас за облегчение моей задачи. Я здесь с целью извлечения кое-какого имущества, которое этот мёртвый господин имел неосторожность похитить у мистера Гриффина.";
            link.l1.go = "HStep_2";

		break;
		
        case "HStep_2":

            dialog.text = "Я не отдам эти деньги! Ублюдок напал на меня, а Вы просто хотите...";
        	link.l1 = "...забрать оружие, коим он собирался и наверняка воспользовался при нападении. Ничего больше. Его деньги по справедливости принадлежат Вам.";
            link.l1.go = "HStep_3";

		break;
		
        case "HStep_3":

            dialog.text = "Можешь забрать, ему, я думаю оно больше не к чему.";
        	link.l1 = "Благодарю.";
            link.l1.go = "HStep_4";
            NextDiag.TempNode = "First time";
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1_2";
		break;
		
        case "HStep_4":
        
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
            Spain_spyDie("");
		break;
		
        //-->Испанский шпион
        
        case "SSStep_0":
        
            dialog.text = "Мне нужен оружейник. Где оружейник?";
        	link.l1 = "Его дом тут неподалёку. Вооон там.";
            link.l1.go = "SSStep_1";
           	link.l2 = "С чего это мне указывать тебе путь? Что тебе нужно от оружейника?";
            link.l2.go = "SSStep_2";
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape2_1";
            chrDisableReloadToLocation = false;
            LocatorReloadEnterDisable("BridgeTown_town", "houseSp2", true);
            
		break;
		
        case "SSStep_1":

            dialog.text = "*хриплый смех*";
        	link.l1 = "";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "houseSp2";
            Pchar.questTemp.CapBloodLine.iTime = 20;
            AddQuestRecord("WeaponsForEscape", "7");

		break;
		
        case "SSStep_2":

            dialog.text = "Тогда проваливай, пока я тебя не выпотрошил.";
        	link.l1 = "Это было бы проблематично.";
            link.l1.go = "Exit_Away";
            Pchar.questTemp.CapBloodLine.sLocator = "houseSp2";
            Pchar.questTemp.CapBloodLine.iTime = 20;
            AddQuestRecord("WeaponsForEscape", "7");

		break;
		
        case "SSStep_3":

            dialog.text = "(Гриффину):  Английская собака!.. Я убью каждого в этом городе, а начну с тебя, упёртый старик! Ты не понимаешь, вот-вот ''Синко Льягас''... А ТЫ что тут делаешь?!";
        	link.l1 = "Позвольте Вас прервать.";
            link.l1.go = "SSStep_4";
            //NextDiag.TempNode = "SSStep_4";
            //AddDialogExitQuest("SpyTalk");
		break;
		
        case "SSStep_4":

            dialog.text = "Что ж, раз уж ты пришёл... ты следующий!";
        	link.l1 = "Нет, пожалуй, я буду предыдущим...";
            link.l1.go = "fight";
            sld = &characters[GetCharacterIndex("Griffin")];
            sld.dialog.currentnode = "GRStep_14";

		break;
		
		//--> Контрабандист
		
        case "QSStep_0":

            dialog.text = "Стоп, стоп, стоп... просить о такой услуге немногим лучше, чем просить меня Вас отсюда увезти. Причём, увозить Вас с этих земель я не рискну и под страхом смерти, ибо в случае моего с Вами согласия, я умру куда как более мучительно… но оружие. Надо быть человеком достаточно дерзким и немного безумным, чтобы об этом попросить в таверне, будучи привилегированным рабом. И ещё большим безумцем нужно быть, чтобы согласиться помочь Вам в этом...";
        	link.l1 = "Раз уж Вы столь многословны, значит, интерес Ваш в этом есть, осталось лишь договориться о цене.";
            link.l1.go = "QSStep_1";
		break;
		
        case "QSStep_1":

            dialog.text = "Вы очень проницательный доктор. Что ж, не буду скрывать - Вы правы. Меня интересует кое-что, но, боюсь, Вы рискнуть не осмелитесь...";
        	link.l1 = "О чём же речь?";
            link.l1.go = "QSStep_2";
		break;
		
        case "QSStep_2":

            dialog.text = "Я знаю, что у Вас есть доступ в усадьбу губернатора...";
        	link.l1 = "Не стоит продолжать. Я крайне тороплюсь, а шанс, что губернатору или его супруге станет сейчас же хуже, ничтожно мал. В иной ситуации дорога в усадьбу для меня закрыта.";
            link.l1.go = "QSStep_3";

		break;
		
        case "QSStep_3":

            dialog.text = "Уверен, ради десятка абордажных сабель и нескольких мушкетов, что имеются у меня по счастливой случайности прямо при себе, а также, скажем, трёх тысяч всегда блестящих денег, Вы найдёте способ попасть к губернатору.";
        	link.l1 = "Что от меня требуется?.";
            link.l1.go = "QSStep_4";

		break;
		
        case "QSStep_4":

            dialog.text = "Небольшое колечко. Всего лишь.";
        	link.l1 = "Колечко? Оно на руке его супруги, как я думаю. И мне предстоит его снять и принести?..";
            link.l1.go = "QSStep_5";

		break;
		
        case "QSStep_5":

            dialog.text = "Вряд ли она носит его, мой дорогой доктор. Тем не менее, оно у неё. Маленькое золотое колечко - простое, как три пиастра, но чертовски для меня важное. Скупщик даст Вам за него едва ли пятую долю того, что предлагаю я. Только не спрашивайте, почему. Просто ответьте.";
        	link.l1 = "Вернусь так быстро, как только смогу. Приготовьте деньги и товар.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "QSStep_6";
            
            sld = &characters[GetCharacterbyLocation("Bridgetown_town", "soldiers", "soldier2")];
            sld.dialog.filename = "Quest\CapBloodLine\Bishop.c";
            sld.Dialog.CurrentNode = "GStep_1";
            sld.protector = true;
            sld.protector.CheckAlways = 1 ;
            LocatorReloadEnterDisable("BridgeTown_town", "reload3_back", true);
            LocatorReloadEnterDisable("Bridgetown_Townhall", "reload3", true);
            LocatorReloadEnterDisable("BridgeTown_town", "reloadR1", true);
            
            sld = characterFromID("MisStid");
            sld.Dialog.CurrentNode = "MSStep_3";
        	sld.talker = 10;
            ChangeCharacterAddressGroup(sld, "Bridgetown_TownhallRoom", "barmen","bar1");
            
            sld = characterFromID("Den");
            LAi_SetCitizenTypeNoGroup(sld);
            LAi_SetStayTypeNoGroup(sld);
            sld.Dialog.CurrentNode = "DStep_3";
			ChangeCharacterAddressGroup(sld, "CommonPirateHouse", "goto","goto6");
            
            sld = ItemsFromID("MsStid_ring");
			sld.shown = true;
			sld.startLocation = "Bridgetown_TownhallRoom";
			sld.startLocator = "item1";
            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape3";


		break;
		
		
        case "QSStep_6":

            if (GetCharacterItem(pchar,"MsStid_ring") > 0)
			{
                dialog.text = "Доктор вернулся! Замечательно!";
                link.l1 = "Как Вы узнали про кольцо?";
    			link.l1.go = "QSStep_7";
                link.l2 = "Вот Ваша вещь. Так что с золотом и оружием?";
    			link.l2.go = "QSStep_12";
			}
			else
			{
                dialog.text = "Что-то еще?";
            	link.l1 = "Нет.";
                link.l1.go = "Exit";
                NextDiag.TempNode = "QSStep_6";
            }

		break;
		
        case "QSStep_7":

            dialog.text = "Узнал? Дорогой доктор, я о нём ни малейшего понятия не имел.";
        	link.l1 = "Меня слегка озадачивает Ваш ответ. Что это за кольцо? Откуда оно?";
            link.l1.go = "QSStep_8";

		break;
		
        case "QSStep_8":

            dialog.text = "Это колечко крайне важное для меня, но Вам от него никакого толку. Мне повезло вновь обрести его, а потому я готов выполнить свою часть сделки и отдать Вам всё то, что обещал.";
        	link.l1 = "А может, раз уж про кольцо Вы не знали, пойти и узнать про него у кого-нибудь? Быть может, мне подскажут на рыночной площади? Гляньте… как маняще оно поблёскивает.";
            link.l1.go = "QSStep_10";

		break;
		
        case "QSStep_10":

            dialog.text = "*смеётся* Презабавный капитан Блад уже почуял вкус к жизни с новой силой... дай сюда!";
        	link.l1 = "Я считаю, сэр, Вам лучше вернуть мне это колечко и объяснить, почему Вы назвали меня капитаном.";
            link.l1.go = "QSStep_11";
            TakeItemFromCharacter(Pchar, "MsStid_ring");

		break;
		
        case "QSStep_11":

            dialog.text = "Не трать на это своё драгоценное время, Питер Блад. Вот твои деньги и оружие - поторопись. Кто знает, быть может, судьба свела нас неслучайно.";
        	link.l1 = "Что ж, ладно.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "First time";
            GiveItem2Character(Pchar, "Weapon_for_escape");
            NextDiag.TempNode = "First time";
       		CloseQuestHeader("WeaponsForEscape");
            AddMoneyToCharacter(pchar, 3000);
            Pchar.questTemp.CapBloodLine.stat = "ReadyToEscape";

		break;
		
        case "QSStep_12":

            dialog.text = "Премного благодарен. Вот Ваши деньги и всё необходимое. Прощайте.";
        	link.l1 = "Всего наилучшего.";
            link.l1.go = "Exit";
            NextDiag.TempNode = "First time";
            GiveItem2Character(Pchar, "Weapon_for_escape");
            NextDiag.TempNode = "First time";
       		CloseQuestHeader("WeaponsForEscape");
            AddMoneyToCharacter(pchar, 3000);
            TakeItemFromCharacter(Pchar, "MsStid_ring");
            Pchar.questTemp.CapBloodLine.stat = "ReadyToEscape";

		break;
		

		
	}
}