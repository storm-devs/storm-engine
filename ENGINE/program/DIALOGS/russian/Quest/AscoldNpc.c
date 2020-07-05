
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

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
            if (pchar.questTemp.Ascold == "SeekInformatorHavanna" || pchar.questTemp.Ascold == "2SeekInformatorHavanna")
            {
            	dialog.text = "Добрый день, кто вы и что вы хотите?";
        		link.l1 = "Меня зовут " + GetFullName(pchar) + ". Я прибыл сюда по поручению Аскольда.";
        		link.l1.go = "Step_1";
            }
            if (pchar.questTemp.Ascold == "ReciveInformationManowar")
            {
            	dialog.text = "Я пожалуй, воздержусь от контактов с вами, вы мне не нравитесь, хоть и друг Аскольда.";
        		link.l1 = "Ну как знаешь...";
        		link.l1.go = "exit";
            }
        break;
//<<<<<<<<<<<<======== Разброс диалогов по персонажам ======================
 		case "Step_1":
        	dialog.text = "Какого Аскольда?";
    		link.l1 = "Обыкновенного, который на Гваделупе живет. Другого на Карибах нет.";
    		link.l1.go = "Step_2";
		break;
 		case "Step_2":
        	dialog.text = "Говорите тише, я вас прекрасно понял. Что нужно моему другу Аскольду?";
    		link.l1 = "Есть одна проблема. Некоторе время назад из Гаваны ушел мановар, под завязку груженный предметами индейского культа.";
    		link.l1.go = "Step_3";
		break;
 		case "Step_3":
        	dialog.text = "Ну, под завязку - это громко сказано. Были там предметы индейского культа, но не так уж и много.\n"+
                          "Я был откомандирован в Гавану и лично оформлял инвентарный отгрузочный список.";
    		link.l1 = "Вот он-то и нужен Аскольду, этот список.";
    		link.l1.go = "Step_4";
		break;
 		case "Step_4":
        	dialog.text = "Знал бы, что он ему нужен - сделал бы копию. Ну а сейчас не вспомню уже и половины всего.\n"+
                          "Теперь даже не знаю, как ему помочь.";
    		link.l1 = "Я понимаю. Единственное, что мне нужно знать, это куда ушел мановар.";
    		link.l1.go = "Step_5";
		break;
 		case "Step_5":
        	dialog.text = "Он ушел к бухте Аматике, что южней Белиза.";
    		link.l1 = "Вот это да! Странное место сбора... И что, так и ушел один?";
    		link.l1.go = "Step_6";
		break;
 		case "Step_6":
        	dialog.text = "Да, ушел один. Дело в том, что бухта Аматике - место сбора и других кораблей с испанских колоний в Америке. Представляешь, там дальше вглубь Мейна нашли заброшенный индейский храм, и сейчас там работает королевская эскпедиция. А уж после того, как эскпедиция закончит работу, эскадра должна идти в Старый Свет.\n"+
                          "Это все, что я знаю.";
    		link.l1 = "Этого хватает, спасибо, друг. Передам от тебя привет Аскольду.";
    		link.l1.go = "Step_7";
		break;
 		case "Step_7":
        	dialog.text = "Эй, друг, подожди! А деньги?";
    		link.l1 = "Послушай, я твой новый друг, Аскольд - старый. А деньги приносят друзья старые. Так что прощай, амиго и до новых встреч!";
    		link.l1.go = "exit";
			npchar.LifeDay = 5;
			SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
            AddQuestRecord("Ascold", "7");
            pchar.questTemp.Ascold = "ReciveInformationManowar";
            Pchar.quest.Ascold_AttackTenManowars.win_condition.l1 = "location";
            Pchar.quest.Ascold_AttackTenManowars.win_condition.l1.location = "Beliz";
            Pchar.quest.Ascold_AttackTenManowars.win_condition = "Ascold_AttackTenManowars";
		break;



        case "Step_":
            dialog.text = "";
            link.l1 = "";
            link.l1.go = "Step_";
        break;
 		case "Exit":
            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
    }
}
