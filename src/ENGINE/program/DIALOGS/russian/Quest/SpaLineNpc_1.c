// диалоговый файл №1 на испанку
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	int i;

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
            //Квест №1, спасение трех горожан.
            if (npchar.id == "SpaPrisoner1" || npchar.id == "SpaPrisoner2" || npchar.id == "SpaPrisoner3")
            {
    			if (pchar.questTemp.State == "SaveFromMorgan_toPrison")
    			{
                    dialog.text = "Сеньор, кто вы?";
        			link.l1 = "К черту разговоры, сейчас я освобожу вас, и мы выбираем якорь. Или вам нравится английская баланда?.. Рассредоточьтесь и, не привлекая внимания, направляйтесь к моему кораблю. Ваша задача - добраться до моего корабля живыми. Через некоторое время я устрою небольшую заварушку, которая отвлечет внимание, так что вам будет легче. Я надеюсь, вам все понятно?";
        			link.l1.go = "Step_S1_1";
                }
                else
                {
                    dialog.text = LinkRandPhrase("Господь услышан наши молитвы!", "Если бы вы знали, как мы рады вас видеть...", "Спасибо сеньору генерал-губернатору, он не забыл о нас...");
        			link.l1 = "Да, да... Но прошу вас, не надо говорить пока мы не вырвались отсюда...";
        			link.l1.go = "exit";
                }
            }
            if (pchar.questTemp.State == "TakeRockBras_toNearestMaracaibo")// квест №3, Рок Бриазилец в каюте на абордаже
            {
    			dialog.text = "Ты победил, проклятый кастилец.";
    			link.l1 = "Очень хорошо, что ты решил сдаться. Я к тебе не просто так, а с предложением.";
    			link.l1.go = "Step_S3_1";
            }
            if (npchar.id == "ServantDEstre" && pchar.questTemp.State == "Sp4Detection_toTortuga")// квест №4, базар со слугой д'Эстре предварительный.
            {
    			dialog.text = "Что вы хотели, месье? Хозяина нет дома.";
    			link.l1 = "Понятно...";
    			link.l1.go = "exit";
            }
            if (npchar.id == "ServantDEstre" && pchar.questTemp.State == "Sp4Detection_toDEstre")// квест №4, базар со слугой д'Эстре
            {
    			dialog.text = "Что вы хотели, месье?";
    			link.l1 = "Мне нужен твой хозяин, Анри д'Эстре.";
    			link.l1.go = "Step_S4_1";
            }
            if (npchar.id == "ServantDEstre" && pchar.questTemp.State == "Sp4Detection_toDEstre_1")// квест №4, базар со слугой д'Эстре
            {
    			dialog.text = "Месье, вам нужно поторапливаться!";
    			link.l1 = "Да, я понял.";
    			link.l1.go = "exit";
            }
            if (npchar.id == "AnryDEstre")// квест №4, базар с самим д'Эстре
            {
    			dialog.text = "Черт возьми, извольте немедленно объясниться - почему вы преследуете меня?!!";
    			link.l1 = "Мне нужно было срочно поговорить с вами, прошу меня извинить, месье...";
    			link.l1.go = "Step_S4_2";
            }
            if (npchar.id == "LaVega_Comendant") // ==>  квест №5, фантом Мэнсфилада при штурме Ла Веги
            {
    			dialog.text = "Ну, что скажешь, каталонская бестия?";
    			link.l1 = "Сегодня мы закончим давний спор с буканьерами. Вы все будете уничтожены, а Эспаньола будет населена только испанскими колонистами!";
    			link.l1.go = "Step_S5_1";
            }
            if (npchar.id == "Boozer") // ==>  квест №6, базар с алкашом в таверне
            {
    			dialog.text = "А?.. Че тее надо?!";
    			link.l1 = "Да ничего особенного. Просто хотел дать тебе несколько монет - ты ведь мучаешься, а так здоровье поправишь...";
    			link.l1.go = "Step_S6_1";
            }
            if (npchar.id == "Messanger" && pchar.questTemp.State == "Sp6TakeMess_waitMessanger") //квест №6, базар с голландским вестовым
            {
    			dialog.text = "Что вы хотите?";
    			link.l1 = "Я - доверенное лицо генерал-губернатора Кюрасао. Мне необходимо переговорить с вами. Давайте пройдем в таверну, побеседуем...";
    			link.l1.go = "Step_S6_8";
    			link.l2 = "Я хочу, чтобы ты отдал мне дипломатическую переписку. И давай по тихому, а то выпущу кишки...";
    			link.l2.go = "Step_S6_11";
            }
            if (npchar.id == "Messanger" && pchar.questTemp.State == "Sp6TakeMess_Action") //квест №6, базар с голландским вестовым
            {
    			dialog.text = "Мне сказали, что здесь меня ожидает доверенное лицо Стэвезанта. Кто вы и что вам нужно?";
    			link.l1 = "Кто я - не важно, а нужны мне ваши бумаги.";
    			link.l1.go = "Step_S6_13";
			}
            if (npchar.id == "OurCaptain_1") //квест №7, базар с Пардалем его на корабле до битвы
            {
    			dialog.text = "Черт возьми, не время для политеса! Меня атакуют ладроны, помоги мне!!!";
    			link.l1 = "Хорошо.";
    			link.l1.go = "Exit";
			}
            if (npchar.id == "OurCaptain_1" && pchar.questTemp.State == "Sp7SavePardal_GoodWork") //квест №7, базар с Пардалем его на корабле
            {
    			dialog.text = "Дьявол, ты оказался здесь как нельзя вовремя! Даже не знаю, что бы я делал без твоей помощи... Скажи мне, кому я обязан своим спасением?";
    			link.l1 = "Меня зовут " + GetFullName(pchar) +". А послал меня к тебе на выручку Франсиско Орегон-и-Гаскон. Ему стало известно, что эскадра французских бандитов вышла специально для боя с тобой.";
    			link.l1.go = "Step_S7_1";
			}		
            if (npchar.id == "OurCaptain_1" && pchar.questTemp.State == "Sp7SavePardal_2GoodWork") //квест №7, базар с Пардалем его на корабле
            {
    			dialog.text = "Поговорили, и хватит на этом...";
    			link.l1 = "Согласен...";
    			link.l1.go = "exit";
			}				
            if (npchar.id == "SpaTalkOfficer" && pchar.questTemp.State == "Sp10Maracaibo_TalkWithOfficer") //квест №10, базар c оффом на выходе
            {
    			dialog.text = "Дорогу!!!";
    			link.l1 = "Полегче, милейший! Куда это ты так несешься?";
    			link.l1.go = "Step_S10_1";
			}

		break;
//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Спасение трех горожан. Квест №1 **********************************
 		case "Step_S1_1":
			dialog.text = "Да, сеньор. Спасибо и да хранит вас Господь!";
			link.l1 = "Аминь! А сейчас поторопитесь...";
			link.l1.go = "Step_S1_2";
		break;
 		case "Step_S1_2":
            AddQuestRecord("Spa_Line_1_SaveCitizens", "2");
			pchar.questTemp.State = "SaveFromMorgan_OutOfPrison";
            for (i=1; i<=3; i++)
            {
                sld = characterFromId("SpaPrisoner"+i);
                ChangeCharacterAddressGroup(sld, "PortRoyal_Prison", "goto", "goto12");
                LAi_SetActorType(sld);
                LAi_ActorRunToLocation(sld, "reload", "reload1", "none", "", "", "", 10.0);
                AddPassenger(pchar, sld, false);
                SetCharacterRemovable(sld, false);
            }
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
		break;
//********************************* Работа на инквизицию. Квест №3 **********************************
 		case "Step_S3_1":
			dialog.text = "Что за предложение?";
			link.l1 = "Предложение встречи с монсеньором Антониу де Соуза.";
			link.l1.go = "Step_S3_2";
		break;
 		case "Step_S3_2":
			dialog.text = "Не знаю такого, кто это?";
			link.l1 = "Очень важный человек в Святой Инквизиции, большая шишка. Так что тебе повезло.";
			link.l1.go = "Step_S3_3";
		break;
 		case "Step_S3_3":
			dialog.text = "Вот проклятье, сожалею, что сдался. Лучше бы мне умереть в бою, чем мучаясь...";
			link.l1 = "Возможно. Ну а теперь нам пора, в Сантьяго нас ждут с нетерпением.";
			link.l1.go = "Step_S3_4";
		break;
 		case "Step_S3_4":
 		    pchar.questTemp.State = "TakeRockBras_RockTaken";
            AddQuestRecord("Spa_Line_3_RockBrasilian", "5");
            pchar.quest.TakeRockBras_DieHard.over = "yes";
            Island_SetReloadEnableGlobal("Maracaibo", true);
		    QuestAboardCabinDialogQuestSurrender();
            DialogExit();
		break;
//********************************* Расследование убийства. Квест №4 **********************************
 		case "Step_S4_1":
			dialog.text = "Он здесь на улице, прямо возле дома. Он куда-то очень торопился, но его остановил разговором знакомый флибустьер с минуту назад. Разговор он закончит быстро и побежит по своим делам. Так что если хотите застать его - поторопитесь.";
			link.l1 = "Хорошо, поспешу...";
			link.l1.go = "exit";
			pchar.questTemp.State = "Sp4Detection_toDEstre_1";
			pchar.quest.Sp4Detection_Race_1.win_condition.l1 = "location";
			pchar.quest.Sp4Detection_Race_1.win_condition.l1.location = "Tortuga_town";
			pchar.quest.Sp4Detection_Race_1.win_condition = "Sp4Detection_Race_1";
    	    NPChar.LifeDay = 0;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
		break;
 		case "Step_S4_2":
			dialog.text = "Хорошо, извиняю... С кем имею честь разговаривать?";
			link.l1 = "Меня зовут " + GetFullName(pchar) + ". Я пришел узнать у вас, кто такая донна Анна, которую вы доставили месье д'Ожерону? ";
			link.l1.go = "Step_S4_3";
		break;
 		case "Step_S4_3":
			dialog.text = "Вы, сударь, суете свой нос не туда, куда нужно. Это может кончиться для вас весьма плачевно...";
			link.l1 = "Я это знаю. Но тем не менее, я еще раз спрошу вас: не является ли донна Анна женой, а теперь уже вдовой, коменданта Гаваны дона Хуана Рамиреса де Лейва?";
			link.l1.go = "Step_S4_4";
		break;
 		case "Step_S4_4":
			dialog.text = "Если так оно и есть, то что?";
			link.l1 = "Тогда, сударь, я вынужден буду убить вас.";
			link.l1.go = "Step_S4_5";
		break;
 		case "Step_S4_5":
			dialog.text = "Весьма прямолинейное утверждение. Ну а почему же сразу - и убить? Лично вам, насколько мне не изменяет память, я не причинил никакого вреда. Не подумайте, что я испугался, мне просто нужно понять причину вашего появления здесь.";
			link.l1 = "Причина проста - законы чести. Вы посягнули на не принадлежащее вам - женщину другого мужчины. При этом убили пять человек!";
			link.l1.go = "Step_S4_6";
		break;
 		case "Step_S4_6":
			dialog.text = "У-у-у, да вы весьма осведомлены об этом моем приключении, даже знаете, скольких испанских дьяволов я отправил на тот свет...  Я впечатлен. Так кто же вы, сударь?";
			link.l1 = "Я испанский капер, и сейчас занимаюсь расследованием убийства коменданта Гаваны и исчезновения его вдовы. Да, совсем забыл - по поручению генерал-губернатора Гаваны дона Франсиско Орегон-и-Гаскона.";
			link.l1.go = "Step_S4_7";
		break;
 		case "Step_S4_7":
			dialog.text = "Ага, вот теперь мне все ясно и я спокойно могу отправлять вас на тот свет. К оружию, сеньор!";
			link.l1 = "Всенепременно, сударь!";
			link.l1.go = "Step_S4_8";
		break;
 		case "Step_S4_8":
            LAi_SetPlayerType(pchar);
			pchar.questTemp.State = "Sp4Detection_toDEstre_2";
            LAi_SetHP(npchar, 300, 300);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Sp4Detection_YouWin");
    	    NPChar.LifeDay = 0;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Уничтожение Ла Веги. Квест №5 **********************************
 		case "Step_S5_1":
			dialog.text = "Агрх, испанский пес! Старый буканьер дорого продаст свою жизнь...";
			link.l1 = "Да, пора заканчивать со всем этим...";
			link.l1.go = "Step_S5_2";
			AfterTownBattle();
		break;
 		case "Step_S5_2":
            LAi_SetPlayerType(pchar);
            LAi_group_MoveCharacter(npchar, LAI_GROUP_TmpEnemy);
            LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck(LAI_GROUP_TmpEnemy, "Sp5LaVegaAttack_AfterFight");
    	    NPChar.LifeDay = 0;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
			SetReturn_Gover_Dialog_Exit(NPChar);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Перехват посыльного Сювесанта. Квест №6 **********************************
 		case "Step_S6_1": //алкаш
			dialog.text = "О-о-о, это дело нужное! Давай деньги...";
			link.l1 = "Сначала кое-что сделай для меня, потом получишь свои деньги.";
			link.l1.go = "Step_S6_2";
		break;
 		case "Step_S6_2":
			dialog.text = "К-хе, а я было обрадовался... Что я должен сделать?";
			link.l1 = "Через некоторое время из резиденции выйдет голландец и направится в порт, на корабль. Тебе нужно передать ему, что я, доверенное лицо генерал-губернатора Стэвезанта, жду его в комнате таверны, наверху.";
			link.l1.go = "Step_S6_3";
		break;
 		case "Step_S6_3":
			dialog.text = "И сколько ты мне дашь?";
			link.l1 = "Хм, ну на сотню можешь смело рассчитывать.";
			link.l1.go = "Step_S6_4";
		break;
 		case "Step_S6_4":
			dialog.text = "Сто монет сейчас же - и я все сделаю. А так с места мне не сдвинуться - жуткое похмелье...";
			link.l1 = "Держи свои деньги. И принимайся за дело, я жду.";
			link.l1.go = "Step_S6_5";
		break;
 		case "Step_S6_5":
			dialog.text = "Да не волнуйся, сейчас пропущу стаканчик - и все тебе будет...";
			link.l1 = "Поторопись со стаканчиком, не пропусти вестового. И помни, я жду голландца в комнате таверны наверху!";
			link.l1.go = "Step_S6_6";
            AddMoneyToCharacter(pchar, -100);
		break;
 		case "Step_S6_6":
			pchar.questTemp.State = "Sp6TakeMess_Action";
			AddQuestRecord("Spa_Line_6_TakeMessangeer", "4");
			GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(npchar, "Tortuga_tavern", "tables", LAi_FindNearestFreeLocator("tables", locx, locy, locz));
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1_back", "none", "", "", "", -1);
            Pchar.quest.Sp6TakeMess_WaitMessanger.win_condition.l1 = "location";
            Pchar.quest.Sp6TakeMess_WaitMessanger.win_condition.l1.location = "Tortuga_tavern_upstairs";
            Pchar.quest.Sp6TakeMess_WaitMessanger.win_condition = "Sp6TakeMess_WaitMessanger";
			pchar.quest.Sp6TakeMess_Interception.over = "yes";
			LocatorReloadEnterDisable("Tortuga_tavern", "reload1_back", true);
            DialogExit();
		break;
		//=====>>> вестовой при попытке взять его в городе.
		case "Step_S6_11": 
			dialog.text = "Хм, ну давай, попробуй...";
			link.l1 = "Аргх!";
			link.l1.go = "Step_S6_7";
		break;
 		case "Step_S6_7":
			LAi_RemoveCheckMinHP(sld);
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "FRANCE_CITIZENS");
			LAi_group_Attack(NPChar, Pchar);
			SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
            DialogExit();
		break;
 		case "Step_S6_8":
			dialog.text = NPCStringReactionRepeat("Если вы хотите мне что-либо сказать, то говорите немедленно и здесь.",
                          "Опять ты? Я же сказал тебе, что никуда не пойду!", "Ну все, ты меня утомил... Стража, держи его, это шпион!", "none", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Давайте пройдем в таверну...", "Послушайте...", "А, что?!", "none", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_S6_9", "Step_S6_12", "Step_S6_7", "none", npchar, Dialog.CurrentNode);
		break;
 		case "Step_S6_9":
			dialog.text = "Ни в какую таверну я идти не собираюсь. Говорите, что у вас ко мне?";
			link.l1 = "Знаете... гм...";
			link.l1.go = "Step_S6_10";
		break;
 		case "Step_S6_10":
			dialog.text = "Понятно... Я, пожалуй, сообщу в порту куда следует о ваших странных действиях... Полагаю, теперь вам не так просто будет покинуть этот город.";
			link.l1 = "Хм...";
			link.l1.go = "Step_S6_12";
		break;
 		case "Step_S6_12":
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "reload2_back", "none", "", "", "Sp6TakeMess_MessengerOut", -1);
            LAi_Actor2WaitDialog(npchar, pchar);
            DialogExit();
		break;
		//=====>>> вестовой захвате в комнате таверны.
 		case "Step_S6_13":
			dialog.text = "Это дипломатическая переписка, вы сильно рискуете...";
			link.l1 = "Я знаю, но если ты не отдашь мне их добровольно, то мне придется снять их с твоего тела. Выбирай – жизнь или бумаги.";
			link.l1.go = "Step_S6_14";
		break;
 		case "Step_S6_14":
			dialog.text = "И жизнь, и бумаги! Пожалуй, я лучше посмотрю на цвет твоей крови...";
			link.l1 = "Хм, ну как знаешь...";
			link.l1.go = "Step_S6_15";
		break;
		case "Step_S6_15":
			bDisableFastReload = false; // открыть переходы.
			LocatorReloadEnterDisable("Tortuga_tavern", "reload1_back", false);
			LAi_SetPlayerType(pchar);
            LAi_SetWarriorType(npchar);
			LAi_group_MoveCharacter(npchar, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Спасение Пардаля. Квест №7 **********************************
 		case "Step_S7_1": 
			dialog.text = "Значит, я так же обязан ему, как и тебе. Очень хорошо, у меня будет возможность отблагодарить его.";
			link.l1 = "Отлично. Ну что же, я должен возвращаться в Гавану к генерал-губернатору. Нужно доложить ему об удачном завершении дела.";
			link.l1.go = "Step_S7_2";
		break;
 		case "Step_S7_2":
			dialog.text = "Я, пожалуй, еще задержусь в этих водах. Возможно мне повезет...";
			link.l1 = "Желаю тебе удачи, Маноэль. Прощай.";
			link.l1.go = "exit";
			pchar.questTemp.State = "Sp7SavePardal_2GoodWork";
		break;
//********************************* Спасение Пардаля. Квест №7 **********************************
 		case "Step_S10_1": 
			dialog.text = "Англичане!!";
			link.l1 = "Что ты сказал?";
			link.l1.go = "Step_S10_2";
		break;
 		case "Step_S10_2": 
			dialog.text = "Я сказал, что к городу движется эскадра английских пиратов! Дьявол, будет бой! Не зря мы готовились в последнее время...";
			link.l1 = "Похоже, что я как нельзя вовремя...";
			link.l1.go = "Step_S10_3";
		break;
 		case "Step_S10_3": 
			dialog.text = "Что?.. А-а-а, дайте дорогу сеньор, мне нужно известить генерал-губернатора...";
			link.l1 = "Проходите, офицер.";
			link.l1.go = "Step_S10_4";
		break;
 		case "Step_S10_4": 
			pchar.questTemp.State = "Sp10Maracaibo_toGovernor";
			AddQuestRecord("Spa_Line_10_Maracaibo", "2");
			LocatorReloadEnterDisable("Maracaibo_town", "gate", true);
			LocatorReloadEnterDisable("Maracaibo_town", "reload1_back", true);
			LAi_SetPlayerType(pchar);
			LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload3_back", "none", "", "", "", -1);
            DialogExit();
		break;




    }
}
