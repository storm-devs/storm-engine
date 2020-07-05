// Эдвард Мэнсфилд, глава буканьеров и авторитетный пират, английская линейка
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
    
// ============================================================================
// ============================= блок angry ==========>>>>>>>>>>>>>>>>>>>>>>>>>
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        if (!CheckAttribute(npchar, "angry.first")) //ловушка первого срабатывания
        {
            NextDiag.TempNode = NextDiag.CurrentNode;
            Dialog.CurrentNode = "AngryExitAgain";
            npchar.angry.first = 1;
        }
        else
        {
            switch (npchar.angry.kind) //сюда расписываем реакцию ангри. В npchar.angry.name пробелы удалены!!!
            {
                case "repeat":
                    if (npchar.angry.name == "Firsttime") Dialog.CurrentNode = "AngryRepeat_1";
                break;
            }
        }
    }
// <<<<<<<<<<<<<<<<<<<<<<======= блок angry ===================================
// ============================================================================
	switch(Dialog.CurrentNode)
	{
		case "First time":
            dialog.text = NPCStringReactionRepeat("У тебя ко мне дело? Нет? Тогда выметайся отсюда!",
                         "Я кажется ясно выразился...", "Хотя я выразился и ясно, но ты продолжаешь отвлекать меня!",
                         "Хм, да ты не в меру хамовит, мон шер...", "repeat", 30, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Уже ухожу.",
                                               "Да, " + npchar.name + ", совершенно ясно.",
                                               "Извини, " + npchar.name + "...",
                                               "Ай, ай...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

 		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;
		
		//********************* Английская линейка *********************
 		case "EngLine_quests":
			dialog.text = "Я слушаю вас, капитан.";
			link.l1 = "Я прибыл к вам по поручению генерал-губернатора Ямайки сэра Томаса Мэдифорда.";
			link.l1.go = "Step_1";
  		break;
 		case "Step_1":
			dialog.text = "Слушаю внимательно.";
			link.l1 = "Сэр Томас Мэдифорд просит вас собрать каперскую эскадру, возглавить ее и атаковать голландский форпост в Карибском море - город Кюрасао.";
			link.l1.go = "Step_2";
  		break;
 		case "Step_2":
			dialog.text = "Хм... А в чем причина такого решения? Признаюсь, оно выглядит странным. Англичане не нападают на голландцев. Да, у государств есть противоречия в торговой экспансии, но они распространяются на Индийский океан. Здесь же - Новый свет.";
			link.l1 = "Сэр, между Англией и Голландией разразилась очередная торговая война. Более того, британский военный флот совершил нападение на голландцев у берегов Европы, но был отбит.";
			link.l1.go = "Step_3";
  		break;
 		case "Step_3":
			dialog.text = "Понятно. Благодарю за информацию. Я этого не знал.";
			link.l1 = "Итак, я жду вашего согласия на экспедицию.";
			link.l1.go = "Step_4";
  		break;
 		case "Step_4":
			dialog.text = "Друг мой, я не могу вам дать положительного ответа на просьбу генерал-губернатора Ямайки.";
			link.l1 = "Теперь МНЕ кажется ваш ответ весьма странным.";
			link.l1.go = "Step_5";
  		break;
 		case "Step_5":
			dialog.text = "Ничего странного. Моя настоящая фамилия - Эдвард Мансвельт. Я - голландец.";
			link.l1 = "Но вы английский капер и пользуетесь покровительством сэра Томаса Мэдифорда. Мне кажется неразумным такой категоричный отказ от его просьбы.";
			link.l1.go = "Step_6";
  		break;
 		case "Step_6":
			dialog.text = "Вам может казаться все, что угодно. Я действительно имею английский каперский патент, но это отнюдь не означает того, что я буду резать глотки моим соотечественникам. Право выбора действий всегда за мной. Я ответил вам, более мне нечего сказать. Прощайте.";
			link.l1 = "Что ж, если вам больше нечего сказать, то прощайте.";
			link.l1.go = "exit";
            NextDiag.TempNode = "First time";
            AddQuestRecord("Eng_Line_2_Talking", "3");

            pchar.questTemp.Count = makeint(sti(pchar.questTemp.Count) + 1);
            if (pchar.questTemp.Count == "4")
			{
				pchar.questTemp.State = "after_talks_with_pirates";
			}
  		break;
 		case "No_Mansfield_first_time":
			dialog.text = "Добрый день, меня зовут Алистер Гуд. После гибели Мансфилда я выбран главой буканьеров.";
			link.l1 = "Будем знакомы, Алистер.";
			link.l1.go = "exit";
			NextDiag.TempNode = "No_Mansfield";
  		break;
 		case "No_Mansfield":
			dialog.text = "Мне сейчас некогда с тобой разговаривать. Зайди позже.";
			link.l1 = "Я уже ухожу.";
			link.l1.go = "exit";
  		break;
		//********************* Голландская линейка *********************
  		case "HolLine3_WormEngPlans":
			dialog.text = "Что привело тебя ко мне?";
			link.l1 = "Я прибыл к вам по поручению генерал-губернатора Кюрасао Питера Стэвезанта.";
			link.l1.go = "Step_H3_1";
        break;
 		case "Step_H3_1":
			dialog.text = "Очень хорошо. Что нужно от меня генерал-губернатору Стэвезанту?";
			link.l1 = "Эдвард, англичане напали на Нидерланды, началась очередная торговая война.";
			link.l1.go = "Step_H3_2";
  		break;
 		case "Step_H3_2":
			dialog.text = "Печальная новость, у меня дела в Порт Рояле...";
			link.l1 = "Вот пакет от Стэвезанта, прошу ознакомиться.";
			link.l1.go = "Step_H3_3";
  		break;
 		case "Step_H3_3":
			dialog.text = "Так, ну давай посмотрим... Хм, он просит меня информировать его о том, что хочет предпринять Мэдифорд на Карибах в рамках участия в войне.";
			link.l1 = "Совершенно верно. Знать это - жизненная необходимость для голландских поселений.";
			link.l1.go = "Step_H3_4";
            TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
  		break;
 		case "Step_H3_4":
			dialog.text = "Все дело в том, что я не знаю ничего о планах Мэдифорда по этой войне. У меня с ним дела другого порядка, политика здесь ни при чем.";
			link.l1 = "И что мне ответить Стэвезанту? Что вы ничего не знаете и ничем не сможете помочь своей родине в трудный час?";
			link.l1.go = "Step_H3_5";
  		break;
 		case "Step_H3_5":
			dialog.text = "Не надо пафосных выражений, молодой человек, я знаю, кто я такой. Но я действительно ничего не ведаю о планах Мэдифорда. Однако, вот что я тебе расскажу.\n"+
                          "Буквально на прошлой неделе ко мне заявился некий посланец от Мэдифорда, который уговаривал меня организовать экспедицию на Кюрасао. Я, естественно, отказался. Отказались также Морган, Джекмен и Джон Моррис.\n"+
                          "Итого получается, что известный пройдоха Томас Мэдифорд пытался открыть боевые действия против Нидерландов с помощью пиратов. Хитер, что тут еще скажешь...";
			link.l1 = "Получается, что Мэдифорд планирует напасть на Кюрасао?";
			link.l1.go = "Step_H3_6";
  		break;
 		case "Step_H3_6":
			dialog.text = "Планировать нападение на Кюрасао он может, а вот выполнить данное мероприятие - вряд ли. У него недостаточно сил для такой операции, не от хорошей жизни он пытался привлечь корсарство.\n"+
                          "Да и вообще, самой надежной охраной Порт Рояля от тех же испанцев является постоянное присутствие некоторого количества пиратов на рейде Ямайки. Так что нет, Кюрасао сейчас ему не по зубам.";
			link.l1 = "Ну что же, все ясно, нападения можно не опасаться...";
			link.l1.go = "Step_H3_7";
  		break;
 		case "Step_H3_7":
			dialog.text = "Я бы так категорично не мыслил. Дело в том, что раз попытка привлечения корсаров к нападению была, значит Мэдифорд все-таки имеет приказ открыть боевые действия. Так что мира здесь между Англией и Голландией не будет точно.\n"+
                          "Вот что я тебе предложу сделать - отправляйся немедленно в Порт Рояль к Генри Моргану.";
			link.l1 = "К Генри Моргану, адмиралу корсаров?";
			link.l1.go = "Step_H3_8";
  		break;
 		case "Step_H3_8":
			dialog.text = "Да, именно так. И не беспокойся, адмиралом он стал не без моего участия. Если быть точным, то этот титул именно я ему и передал. Попросишь его от моего имени помочь тебе.";
			link.l1 = "И он поможет?";
			link.l1.go = "Step_H3_9";
  		break;
 		case "Step_H3_9":
			dialog.text = "Если сможет помочь, то сделает это.";
			link.l1 = "Эдвард, вы не напишите мне письмо для Моргана, чтобы он поверил моим словам?";
			link.l1.go = "Step_H3_10";
  		break;
 		case "Step_H3_10":
			dialog.text = "Нет, это не обязательно. Вряд ли кто-то в своем уме будет вводить в заблуждение Моргана, такой человек долго не проживет. Он тебе поверит.";
			link.l1 = "Эдвард Мансвельд, вы настоящий сын своего Отечества. Спасибо вам за помощь!";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
    		pchar.questTemp.State = "WormEnglPlans_toMorgan";
    		AddQuestRecord("Hol_Line_3_WormEnglishPlans", "2");
    		QuestSetCurrentNode("Henry Morgan", "HolLine3_fromMansfield");
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "SentJons_HouseF3", "goto", "goto1");
            LAi_SetHuberStayType(characterFromId("Henry Morgan"));
          break;


// ======================== блок нод angry ===============>>>>>>>>>>>>>>>
		case "AngryRepeat_1":
            dialog.text = RandPhraseSimple("Пошел вон отсюда!", "Вон из моего дома!");
			link.l1 = "Ой-ой-ой...";
		    link.l1.go = "AngryExitAgainWithOut";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся с Мэнсфилдом через 30 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Тебе не стоит доставать меня, запомни это.";
        			link.l1 = "Я понял, Эдвард, больше не буду.";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
            }
    	break;
		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
		break;
		case "AngryExitAgainWithOut":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
            DoReloadCharacterToLocation("LaVega_town","reload","reload6");
		break;
// <<<<<<<<<<<<============= блок нод angry =============================
	}
}
