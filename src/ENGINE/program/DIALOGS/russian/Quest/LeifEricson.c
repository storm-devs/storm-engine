// Квестовые атакующий кэпа, 10 штук в комплекте.
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
    int i;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		case "First time":
            if (pchar.questTemp.Ascold == "Ascold_MummyIsLive")
            {
            	dialog.text = "Ты справился с моими слугами, это хорошо. А теперь скажи мне, какой сейчас год?";
        		link.l1 = environment.date.year + "-й... Черт возьми, что происходит?!!";
        		link.l1.go = "Step_8";
            }
            if (pchar.questTemp.Ascold == "Ascold_BodyChange")
            {
            	dialog.text = "Ну вот, теперь все в порядке. Ты готов выполнить свое предназначение?";
        		link.l1 = "Что ты натворил?!! Слушай, Лейф, мать твою Эриксон, мир за это время здорово изменился. Белых людей здесь теперь очень много.";
        		link.l1.go = "Step_25";
            }
            if (pchar.questTemp.Ascold == "Ascold_SkelSlave")
            {
            	dialog.text = "Теперь ты готов. Я разрешаю тебе покинуть гробницу.";
        		link.l1 = "А что будет с моим телом?";
        		link.l1.go = "Step_27";
            }
            if (pchar.questTemp.Ascold == "Ascold_ImMummy")
            {
            	dialog.text = "Выполняй свою работу, капер.";
        		link.l1 = "Хорошо, Лейф.";
        		link.l1.go = "exit";
            }
            if (pchar.questTemp.Ascold == "Ascold_MummyFoundItems")
            {
            	dialog.text = "А-а-а, вот и ты наконец! Где предметы, за которыми я тебя посылал?";
                if  (CheckCharacterItem(pchar, "sculMa1") && CheckCharacterItem(pchar, "sculMa2") && CheckCharacterItem(pchar, "sculMa3") && CheckCharacterItem(pchar, "indian22"))
                {
                	link.l1 = "Они у меня. Забирай и отдай мне мое тело!";
            		link.l1.go = "Step_29";
                }
                else
                {
                	link.l1 = "Их нет у меня, я их не нашел...";
            		link.l1.go = "Step_30";
                }
            }
            if (pchar.questTemp.Ascold == "Ascold_BackMyBody")
            {
            	dialog.text = "Теперь я освобождаю тебя. Ты можешь идти, выход свободен.";
        		link.l1 = "Спасибо. Надеюсь, больше не свидимся.";
        		link.l1.go = "Step_31";
            }
            if (pchar.questTemp.Ascold == "Ascold_ReturnToAscold")
            {
            	dialog.text = "Прощай, капер...";
        		link.l1 = "Прощай, Лейф.";
        		link.l1.go = "exit";
            }
		break;

 		case "Exit":
            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

        case "Step_8":
        	dialog.text = "Сейчас происходит следующее - мы с тобой разговариваем.\n"+
                          "А уже произошло великое событие, ожидаемое мной на прояжении шести столетий - я восстал ото сна.";
    		link.l1 = "(про себя) Чтоб меня разорвало! (вслух) И похоже, это я тебя разбудил...";
    		link.l1.go = "Step_9";
		break;

        case "Step_9":
        	dialog.text = "Как тебя зовут и кто ты? Отвечай и будешь жить.";
    		link.l1 = "Меня зовут " + GetFullName(pchar) + ". Я - капер.";
    		link.l1.go = "Step_10";
    		link.l2 = "Не слишком ли высокий тон для облезлой черепушки?";
    		link.l2.go = "ToDeath";
		break;

        case "ToDeath":
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
    		LAi_SetPlayerType(pchar);
		    LAi_LocationFightDisable(&Locations[FindLocation("Guadeloupe_Cave")], false);
			LAi_SetFightMode(pchar, true);
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

        case "Step_10":
        	dialog.text = "Что значит 'капер'?";
    		link.l1 = "Капер - это корсар... Флибустьер, пират...";
    		link.l1.go = "Step_11";
		break;

        case "Step_11":
        	dialog.text = "Что такое 'флибустьер', 'пират'?";
    		link.l1 = "Ну, как тебе объяснить... Я свободный моряк и свободный воин. У меня свой корабль и я делаю то, что считаю нужным.";
    		link.l1.go = "Step_12";
		break;

        case "Step_12":
        	dialog.text = "Ха-ха! Да мы с тобой братья по крови! Я счастлив, что мой замысел осуществила родственная душа. Это знак большой удачи для меня...";
    		link.l1 = "Какой замысел?";
    		link.l1.go = "Step_13";
		break;

        case "Step_13":
        	dialog.text = "Какой замысел? Ха-ха-ха.... Ты здесь и разговариваешь со мной, с человеком, умершим более шести столетий назад!\n"+
                          "В этом и заключается мой замысел. Значит, мое послание потомкам было найдено...";
    		link.l1 = "Что... Кто ты?";
    		link.l1.go = "Step_14";
		break;

        case "Step_14":
        	dialog.text = "Меня зовут Лейф Эриксон. Я, так же как и ты, моряк и воин, сам себе хозяин.\n"+
                          "Только я бороздил моря северней этого места.";
    		link.l1 = "Когда я направлялся сюда, я слышал, что здесь похоронен Лейф Счастливый. Это и есть ты?";
    		link.l1.go = "Step_15";
		break;

        case "Step_15":
        	dialog.text = "Да. Я - Лейф Счасливый, сын Эрика Рыжего, потомок древних свионов, викинг и йорсалирсфари.";
    		link.l1 = "Так ты викинг, один из нации варваров, что разоряли Европу шестьсот лет назад?";
    		link.l1.go = "Step_16";
		break;

        case "Step_16":
        	dialog.text = "Викинг - это не нация, корсар. Викинг - это образ жизни, профессия моряка и воина.\n"+
                          "И ты, свободный воин, называешь нас варварами? Шестьсот лет назад мы были лучшими войнами и моряками на земле. Я был в Йорсалире и участвовал в походе на Рим, а мой брат владел Гардарикой!";
    		link.l1 = "Но как ты оказался в Америке? Америку же открыл Христофор Колумб 150 лет назад!";
    		link.l1.go = "Step_17";
		break;

        case "Step_17":
        	dialog.text = "Америкой ты называешь эти большие земли? Ха-ха-ха. Слушай меня, свободный корсар.\n"+
                          "Летом 986 года от РХ норвежский викинг Бьярни Херьюльфсон, направлявшийся в Гренландию, был отнесен к новой неведомой земле, но не пристал к ней.\n"+
                          "В 1000 году я, Лейф Эриксон, отправился на поиски этой земли. Я нашел остров Хеллюланд (Баффинова земля), затем Маркланд (Лабрадор) и Винланд (Ньюфаундленд). На Винланде я образовал поселение.\n"+
                          "Через два года я отправился дальше и нашел эту большую землю. Я пристал к ней гораздо северней.";
    		link.l1 = "Не может быть! Так Америка была открыта еще шесть столетий назад...";
    		link.l1.go = "Step_18";
		break;

        case "Step_18":
        	dialog.text = "Эта земля была окрыта мной в 1002 году.\n"+
                          "Затем я пошел вглубь земли и увидел, что земля эта бесконечна и заселена краснокожими людьми.";
    		link.l1 = "Индейцы...";
    		link.l1.go = "Step_19";
		break;

        case "Step_19":
        	dialog.text = "Нет, каждое племя называет себя по разному.\n"+
                          "Двадцать лет я прожил в племени тольтеков, которые почитали меня, белого человека, за божество. Через двадцать лет я вернулся в основанное мной поселение на Винланде и оставил там завещание потомкам.\n"+
                          "Через шестьсот лет оно было исполнено. Исполнено тобой.";
    		link.l1 = "Здорово! Ну, я пошел?";
    		link.l1.go = "Step_20";
		break;

        case "Step_20":
        	dialog.text = "Не торопись, сначала я сделаю тебе бесценный подарок за свое освобождение.";
    		link.l1 = "О-о-о, это дело я люблю. И что это за подарок?";
    		link.l1.go = "Step_21";
		break;

        case "Step_21":
        	dialog.text = "Твоя жизнь. Я дарю ее тебе в качестве благодарности.";
    		link.l1 = "Здорово... Ну, я пошел?";
    		link.l1.go = "Step_22";
		break;

        case "Step_22":
        	dialog.text = "Не торопись. За мой подарок ты кое-что сделаешь для меня.";
    		link.l1 = "Хм, и что я должен сделать?";
    		link.l1.go = "Step_23";
    		link.l2 = "Черта с два, гнилая обезьяна! Отведай-ка лучше вкус стали...";
    		link.l2.go = "ToDeath";
		break;

        case "Step_23":
        	dialog.text = "Я чувствую, что здесь рядом стоит большой город. Там, у самого главного человека в городе, находятся нужные мне вещи, я ощущаю их присутствие.\n"+
                          "Ты должен доставить мне три хрустальных черепа тольтеков - белый, розовый и голубой, а также древний предмет, именуемый странной вещью. Тогда я стану сильней и смогу восстановить свое тело. Мир ждет своего властителя...";
    		link.l1 = "(про себя) Мне бы только выбраться из этого склепа, а уж таскать сюда я ничего не буду, ищи дурака... (вслух) Хорошо, я могу идти?";
    		link.l1.go = "Step_24";
		break;

        case "Step_24":
        	dialog.text = "В общем, да. Только еще одно ма-а-аленькое дельце, для того, чтобы ты побыстрей возвращался.\n"+
                          "На время я беру твое тело, а тебе даю свое. Тоже на время.";
    		link.l1 = "Что?!!!";
    		link.l1.go = "Body_Change";
		break;

        case "Body_Change":
			LAi_SetPlayerType(pchar);
            bDisableCharacterMenu = true; //закрываем Ф2
            // ==> меняем модели
            Pchar.questTemp.Ascold.heroFaceId    = pchar.FaceId;
            Pchar.questTemp.Ascold.heroAnimation = pchar.model.animation;
			Pchar.questTemp.Ascold.reputation    = pchar.reputation;
			Pchar.questTemp.Ascold.nation		 = pchar.nation;
			Pchar.questTemp.Ascold.relation      = GetNationRelation2MainCharacter(FRANCE);
			SetNationRelation2MainCharacter(FRANCE, RELATION_ENEMY);
            npchar.model = pchar.model;
			npchar.model.animation = pchar.model.animation;			
            FaceMaker(npchar);
            SetNewModelToChar(npchar);
            pchar.model = "Mummy";
            pchar.model.animation = "man";
            pchar.FaceId = 207; //мумия
            FaceMaker(pchar);
            SetNewModelToChar(pchar);
            pchar.questTemp.Ascold = "Ascold_BodyChange";
    		LAi_SetCitizenType(npchar);
            //==> убираем офицеров
            for (i=1; i<=3; i++)
            {
                string officers = "Fellows.Passengers.Officers."+"id"+i;
                if(CheckAttribute(pchar, officers))
                {
                     RemoveOfficersIndex(pchar, sti(pchar.(officers)));
                }
            }
            //<== убираем офицеров
            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

        case "Step_25":
        	dialog.text = "Ну и что?";
    		link.l1 = "А то, что город на острове - французский, а не тольтеков, ацтеков и прочих краснокожих бестий. И я не могу разгуливать по французскому городу в таком виде!";
    		link.l1.go = "Step_26";
		break;

        case "Step_26":
        	dialog.text = "Разгуливать не надо, город нужно захватить. А в помощь я тебе дам своих слуг, их будет достаточно, чтобы взять город.";
    		link.l1 = "...";
    		link.l1.go = "exit";
            for (i=1; i<=6; i++)
            {
				sld = GetCharacter(NPC_GenerateCharacter("MySkel"+i, "Skel"+(rand(3)+1), "man", "man", 30, PIRATE, 0, true));	
				FantomMakeCoolFighter(sld, 20, 70, 70, "topor2", "pistol3", 50);  
				ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto"+(rand(2)+1));
				LAi_SetActorType(sld);
				LAi_ActorFollowEverywhere(sld, "", -1);
            }
            pchar.questTemp.Ascold = "Ascold_SkelSlave";
		break;
		
        case "Step_27":
        	dialog.text = "За это не волнуйся, я тебе его верну в целости. Твое тело мне не нужно, оно мне не нравится.\n"+
                          "Все, ты можешь идти.";
    		link.l1 = "Хорошо, все это я принесу тебе. А ты пока будь очень острожен.";
    		link.l1.go = "Step_28";
		break;

        case "Step_28":
            PChar.GenQuest.CrewSkelMode = true; //команда - скелеты 
  		    chrDisableReloadToLocation = false; // открыть выход из локации.
            pchar.questTemp.Ascold = "Ascold_ImMummy";
            //==> потом вернуть
            Pchar.location.from_sea.save = Pchar.location.from_sea;
            Pchar.location.from_sea = "none";
            Pchar.Ship.Crew.Quantity.save = Pchar.Ship.Crew.Quantity;
            Pchar.Ship.Crew.Quantity = 1000;
            Pchar.GenQuest.HunterScore2Pause = 1; //НЗГ не начисляются
			Pchar.GenQuest.ReputationNotChange = 1; //репутацию не менять
            ChangeCharacterAddress(characterFromID("Ascold"), "None", "");
            //<== потом вернуть
            Pchar.quest.Ascold_MummyFightTown.win_condition.l1 = "location";
            Pchar.quest.Ascold_MummyFightTown.win_condition.l1.location = "BasTer_ExitTown";
            Pchar.quest.Ascold_MummyFightTown.win_condition = "Ascold_MummyFightTown";

            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
        case "Step_29":
            DeleteAttribute(PChar, "GenQuest.CrewSkelMode"); // команда-скелеты в нормальное сост.
            bDisableCharacterMenu = false; //открываем Ф2
            bDisableLandEncounters = false;
            TakeItemFromCharacter(pchar, "sculMa1");
            TakeItemFromCharacter(pchar, "sculMa2");
            TakeItemFromCharacter(pchar, "sculMa3");
            TakeItemFromCharacter(pchar, "indian22");
            //==> вертаем
            Pchar.location.from_sea = Pchar.location.from_sea.save;
            DeleteAttribute(PChar, "location.from_sea.save");
            Pchar.Ship.Crew.Quantity = Pchar.Ship.Crew.Quantity.save;
            DeleteAttribute(PChar, "Ship.Crew.Quantity.save");

            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Ascold")], "BasTer_houseSp1", "goto", "goto1");
           	LAi_SetStayTypeNoGroup(characterFromID("Ascold"));
           	SetLocationCapturedState("BasTer_town", false);
            // ==> возвращаем модели
            pchar.model = npchar.model;
            pchar.model.animation = Pchar.questTemp.Ascold.heroAnimation;
            pchar.FaceId          = Pchar.questTemp.Ascold.heroFaceId;
			pchar.reputation	  = Pchar.questTemp.Ascold.reputation;
			pchar.nation		  = Pchar.questTemp.Ascold.nation;
			SetNationRelation2MainCharacter(FRANCE, sti(Pchar.questTemp.Ascold.relation));
            FaceMaker(pchar);
            SetNewModelToChar(pchar);
            npchar.model = "Mummy";
			npchar.model.animation = "man";
            FaceMaker(npchar);
            SetNewModelToChar(npchar);
            DeleteAttribute(PChar, "questTemp.Ascold.heroFaceId");
            DeleteAttribute(PChar, "questTemp.Ascold.heroAnimation");
            DeleteAttribute(PChar, "questTemp.Ascold.reputation");
			DeleteAttribute(PChar, "questTemp.Ascold.relation");
			DeleteAttribute(PChar, "GenQuest.HunterScore2Pause"); //вертаем начисление нзг
			DeleteAttribute(PChar, "GenQuest.ReputationNotChange"); //вертаем смену репы
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            pchar.questTemp.Ascold = "Ascold_BackMyBody";
            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
        case "Step_30":
        	dialog.text = "Пока не найдешь, своего тела обратно не получишь.";
    		link.l1 = "Я буду искать...";
    		link.l1.go = "exit";
		break;

        case "Step_31":
        	dialog.text = "Это в твоих интересах. Больше бесценных подарков я тебе делать не намерен.";
    		link.l1 = "Понял. Ну, будь здоров, если можно так сказать...";
    		link.l1.go = "exit";
    		chrDisableReloadToLocation = false; // открыть выход из локации.
			bDisableFastReload = false; 
    		pchar.questTemp.Ascold = "Ascold_ReturnToAscold";
    		characters[GetCharacterIndex("Ascold")].dialog.currentnode = "AfterAttackMummy";
		break;
	}
}


