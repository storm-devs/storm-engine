// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
	ref sld;
	int i;
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "Sp8SaveCumana_GoodWork")//квест №8 испанки.
			{
				dialog.text = RandPhraseSimple("Сеньор, весь город благодарен вам по гроб жизни. Если бы не вы, даже страшно себе представить, что могло произойти...", "Весь город наблюдал за сражением - это была великая битва. Мы вам очень благодарны за спасение от грязных пиратов.");
				link.l1 = RandPhraseSimple("Да уж, от ладронов пощады ждать не приходится...", "Этих ладронов нужно уничтожать повсеместно...");
				link.l1.go = "exit";			
			}
		break;

		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;
		//************************** Квестовые штрумы ************************	
        //==> Одиннадцатый квест англ.линейки: захват Каракаса и Куманы.
        case "EngLine11Quest_OccupyMein":
			dialog.text = "Все, мы прекратили сопротивление. Город ваш. Что теперь будет?";
			link.l1 = "Данной мне генерал-губернатором Ямайки властью объявляю данный город владением Карла II Стюарта.";
			link.l1.go = "Step_E10_1";
			AfterTownBattle();
        break;
        case "Step_E10_1":
			dialog.text = "Т-а-ак...";
			link.l1 = "Теперь город Кумана находится под протекторатом Англии. Все налоговые сборы, торговые пошлины и прочее населению надлежит платить новой английской администрации.";
			link.l1.go = "exit";
            AddQuestRecord("Eng_Line_11_OccupyMein", "3");
            pchar.questTemp.Q11_Cumana = 1;
            NextDiag.TempNode = "Cupture_after";
			PChar.questTemp.DontSetNewDialogToMayor = true; // иначе может сменить диалог и сбойнуть
            PChar.questTemp.DontNullDeposit = true;    // чтоб не нулили ростовщика
            SetCaptureTownByNation(NPChar.City, sti(PChar.nation));
            DeleteAttribute(PChar, "questTemp.DontSetNewDialogToMayor");
            UpdateRelations();
            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
			SetReturn_Gover_Dialog_Exit(NPChar);
            chrDisableReloadToLocation = false;
            AddDialogExitQuestFunction("LaunchColonyInfoScreen"); // табличка  
        break;
        //==> Шестой квест фр.линейки: захват Куманы с Олоне.
 		case "FraLine6Quest_Attack":
			dialog.text = "Наш город пал... Я готов выслушать ваши требования.";
			link.l1 = "Нам стало известно, что в Кумане находится 200 тысяч реалов... Мы пришли сюда за этими деньгами.";
			link.l1.go = "Step_F6_1";
			group_DeleteGroup("Spa_Attack");
			AfterTownBattle();
		break;		
		case "Step_F6_1":
            dialog.text = "Хм, понятно... А если я скажу, что денег нет?";
			link.l1 = "Послушайте, сеньор, вы знаете кто такой Франсуа Олоне?";
            link.l1.go = "Step_F6_2";
        break;
		case "Step_F6_2":
            dialog.text = "Мне доводилось слышать имя этого дьявольского отродья...";
			link.l1 = "Так вот, Олоне - мой компаньон, он брал город вместе со мной. И поверьте мне - если вы будете упорствовать, то половина жителей вашего города лишится головы. И отсечет головы Олоне собственноручно на глазах остальных людей. Как вам такая перспектива?";
            link.l1.go = "Step_F6_3";
        break;
		case "Step_F6_3":
			if (GetQuestPastDayParam("questTemp") < 14)
			{
				dialog.text = "Я слышал, что такое он уже проделывал... Ну что же, я не буду строить из себя героя, тем более, что пострадают совершенно невинные люди... Все реалы в соседней комнате в секретере. Забирайте и будте прокляты!";
				link.l1 = "Осторожней в выражениях, сударь. Я хоть и не Олоне, но с удовольствием отрежу твой поганый язык...";
				link.l1.go = "Step_F6_exit";
         		pchar.GenQuestBox.Cumana_TownhallRoom = true;
				pchar.GenQuestBox.Cumana_TownhallRoom.box1.money = 200000;
				pchar.questTemp.State = "Fr5Olone_FoundMoney";
			}
			else
			{
				dialog.text = "Я слышал, что такое он уже проделывал и нисколько не сомневаюсь, что сможет проделать еще. Но денег нет, это правда. Пять дней тому назад за ними пришла эскадра. Деньги на пути в новый свет.";
				link.l1 = "Тогда мы будем пытать население города...";
				link.l1.go = "Step_F6_4";
			}
        break;
		case "Step_F6_4":
            dialog.text = "Сеньор, совершенно ясно, что мы в вашей власти. Я, как губернатор этого города, совершенно четко осознаю, в какой ситуации мы находимся. Забирайте все, что найдете, но денег, за которыми вы сюда пришли, здесь уже нет. Вы опоздали...";
			link.l1 = "Каррамба!! Я должен обсудить судьбу города со своими компаньонами!";
            link.l1.go = "Step_F6_exit";
			pchar.questTemp.State = "Fr5Olone_WeLate";
        break;
		case "Step_F6_exit":
			for (i=0; i<3; i++)
			{				
				if (i==0)	sld = characterFromID("Olone");
				else	sld = characterFromID("Captain_"+i); 
				RemoveCharacterCompanion(pchar, sld);
				sld.dialog.currentnode = "First time";
				LAi_SetStayType(sld);
				LAi_SetCurHPMax(sld);
				LAi_LoginInCaptureTown(sld, true);
			}
			Pchar.quest.Fr6Olone_TalkWithOlone.win_condition.l1 = "location";
			Pchar.quest.Fr6Olone_TalkWithOlone.win_condition.l1.location = "Cumana_town";
			Pchar.quest.Fr6Olone_TalkWithOlone.win_condition = "Fr6Olone_TalkWithOlone";
			LAi_LocationDisableOfficersGen("Cumana_town", true); //офицеров не пускать
			chrDisableReloadToLocation = false;
			NextDiag.CurrentNode = "Cupture_after";
			SetReturn_Gover_Dialog_Exit(NPChar);
			DialogExit();
        break;

	}
    UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

