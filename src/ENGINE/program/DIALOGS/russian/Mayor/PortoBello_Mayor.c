// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "Sp9SaveCumana_toPortoBello")//квест №9 испанки.
			{
				dialog.text = "Кто вы и что вам нужно?";
				link.l1 = "Меня зовут " + GetFullName(pchar) + ", прошу ознакомиться с данным документом. Это верительное письмо на мое имя за подписью генерал-губернатора Франсиско Орегон-и-Гаскона.";
				link.l1.go = "Step_S10_1";			
			}
			if (pchar.questTemp.State == "Sp9SaveCumana_toCompOut")//квест №9 испанки, после того, как 0 компаньонов.
			{
				dialog.text = "Та-а-ак, капитан, рад вас видеть. Что скажете?";
				link.l1 = "Я по поводу задания. Вы готовы передать в мое распоряжение названные галеоны?";
				link.l1.go = "Step_S10_5";			
			}
			if (pchar.questTemp.piratesLine == "Panama_toPortoBello")//пиратка, штурм Панамы
			{
				dialog.text = "";
				link.l1 = "";
				link.l1.go = "Step_S10_5";
			}
		break;
        //==> Девятый квест исп.линейки: сопроводить четыре золотых галеона.
        case "Step_S10_1":
			dialog.text = "Та-а-ак, вижу. Очень хорошо, я давно жду вас. Вы в курсе того, какова цель вашего визита сюда?";
			link.l1 = "В общих чертах, я вроде бы должен сопроводить куда-то некие корабли...";
			link.l1.go = "Step_S10_2";
  			TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
        break;
        case "Step_S10_2":
			dialog.text = "Хм, правильно. Я даю вам в распоряжение четыре галеона, груженые золотом. Вам необходимо доставить их в целости и сохранности до необитаемого острова Кайман. Вам все ясно?";
			link.l1 = "Не совсем... Если позволите, я выскажу свое мнение по данному поводу.";
			link.l1.go = "Step_S10_3";
        break;
        case "Step_S10_3":
			dialog.text = "Да, конечно.";
			link.l1 = "Я понимаю, когда речь идет о доставке золотых галеонов в укрепленный город, под защиту мощного форта. Но везти их к необитаемому острову...";
			link.l1.go = "Step_S10_4";
        break;
        case "Step_S10_4":
			dialog.text = "Это место сбора большой эскадры, которая отправляется в Старый Свет. Там вас будет ожидать мощный испанский флот, прибывающий из Белиза. Так что нет никаких причин для беспокойства, это наша новая тактическая уловка для борьбы с ладронами: раньше они караулили подступы к Белизу, а в этот раз оттуда выйдут только усиленные военные галеоны, которые пиратам не по зубам.\n"+
				          "Теперь вам все ясно?";
			link.l1 = "Почти. А где эти галеоны, где их капитаны?";
			link.l1.go = "Step_S10_5";
        break;
        case "Step_S10_5":
			if (GetCompanionQuantity(pchar) > 1)
			{
				dialog.text = "Капитаны будут переданы в ваше распоряжение как только вы избавитесь от лишних кораблей в составе вашей эскадры. Вы должны иметь только один ваш корабль.";
				link.l1 = "Я понял. Буду у вас позже, как только утрясу со своей эскадрой...";
				link.l1.go = "exit";
				pchar.questTemp.State = "Sp9SaveCumana_toCompOut";
			}
			else
			{
				dialog.text = "Галеоны находятся на рейде Порто Белло, а капитаны сию же минуту передаются в ваше распоряжение. Собственно, вы можете приступать к выполнению задания немедленно.";
				link.l1 = "Очень хорошо. Я каким-то образом ограничен в сроках?";
				link.l1.go = "Step_S10_6";
			}
        break;
        case "Step_S10_6":
			dialog.text = "Нет, главное - чтобы все галеоны добрались до Каймана в целости и с тем же содержимым трюма, что и сейчас. Я имею в виду золото. Сейчас его в трюмах кораблей 5000 штук, что в мерах веса составляет 10000 центнеров.";
			link.l1 = "Я понимаю. Ну что же, я приступаю к выполнению.";
			link.l1.go = "Step_S10_7";
        break;
        case "Step_S10_7":
            pchar.questTemp.State = "Sp9SaveCumana_toCompOut";
			AddQuestRecord("Spa_Line_9_FourGolgGaleons", "2");
            ref sld;
            for (int i=1; i<=4; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Captain_"+i, "trader_"+(rand(15)+1), "man", "man", 15, SPAIN, -1, true));
                FantomMakeCoolSailor(sld, SHIP_GALEON_L, "", CANNON_TYPE_CANNON_LBS16, 30, 15, 15);
                FantomMakeCoolFighter(sld, 15, 10, 10, BLADE_SHORT, "pistol1", 0);
                sld.Ship.Cargo.Goods.Balls = 48;
                sld.Ship.Cargo.Goods.Grapes = 10;
                sld.Ship.Cargo.Goods.Knippels = 48;
                sld.Ship.Cargo.Goods.Bombs = 48;
                sld.Ship.Cargo.Goods.Weapon = 0;
                sld.Ship.Cargo.Goods.Powder = 80;
				sld.Ship.Cargo.Goods.Food = 300;
				sld.Ship.Cargo.Goods.Gold= 1250;
				SetCharacterRemovable(sld, false);
				sld.CompanionEnemyEnable = false; 
				sld.Abordage.Enable = false; // НЕЛЬЗЯ!
                SetCompanionIndex(pchar, -1, sti(sld.index));
                Pchar.quest.Sp9SaveCumana_toCaimanBattle.win_condition.l1 = "location";
                Pchar.quest.Sp9SaveCumana_toCaimanBattle.win_condition.l1.location = "Caiman";
                Pchar.quest.Sp9SaveCumana_toCaimanBattle.win_condition = "Sp9SaveCumana_toCaimanBattle";
            }
			DialogExit();
        break;
		//************************** Квестовые штрумы ************************
		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;
        //==> пиратка, штурм Панамы
        case "PQ8_MayorPortoBello":
			dialog.text = "Дьявол!!! Как быстро вы сумели взять город!";
			link.l1 = "Ха! Понимаю ваше удивление. Но это только начало...";
			link.l1.go = "Step_P1_1";
			AfterTownBattle();
        break;
        case "Step_P1_1":
			dialog.text = "Нам было известно, что пираты собираются напасть на Панаму. Мой город вам взять удалось, но Панаму... Джунгли убьют вас, сеньор, поверьте мне.";
			link.l1 = "Будет трудно, но мы справимся, сеньор губернатор. Не извольте беспокоиться, так сказать... Кстати, а откуда вы узнали о нашем походе?";
			link.l1.go = "Step_P1_2";
        break;
        case "Step_P1_2":
			dialog.text = "Я? От генерал-губернатора. А откуда он узнал - понятия не имею.";
			link.l1 = "Ясно. Ну что же, это уже интересно...";
			link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
            SetReturn_Gover_Dialog_Exit(NPChar);
            chrDisableReloadToLocation = false;
			AddDialogExitQuestFunction("PQ8_MorganInPortoBello");
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

