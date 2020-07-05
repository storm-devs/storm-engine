// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Послушайте, если вы не по делам порта, то не стоит меня вопросами донимать.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Извините, но портовые дела меня сейчас не интересуют.", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> квест №6 англ.линейки, базар о пинасе
            if (pchar.questTemp.State == "MorrisWillams_SeekSpanishGoods")
            {
    			link.l1 = "Послушай, у тебя в порту не было испанцев в последнее время?";
    			link.l1.go = "Step_E6_1";
            }
		break;
//************************************* Диалоги по англ.линейке *******************************
		case "Step_E6_1":
            dialog.text = "Испанец здесь редкий гость. Но один совсем недавно точно бросил здесь якорь, причем под охраной солдат гарнизона.";
			link.l1 = "Вот это да! С чего это английским солдатам охранять испанский корабль?";
			link.l1.go = "Step_E6_2";
		break;

		case "Step_E6_2":
            dialog.text = "Да черт его знает, не иначе приказ генерал-губернатора.";
			link.l1 = "А где сейчас этот испанец? И что за корабль?";
			link.l1.go = "Step_E6_3";
		break;

		case "Step_E6_3":
            dialog.text = "Пинас 'Санта Агуэда', ушел вчера с попутным ветром курсом на Сан-Хуан. А что тебе испанец?";
			link.l1 = "Хочу свидеться с капитаном, он мне должен...";
			link.l1.go = "Step_E6_4";
		break;

		case "Step_E6_4":
            dialog.text = "Хех, хорошо... Но ты учти, что ветер сохранился. Если ты хочешь увидеть корму пинаса до Сан-Хуана, тебе нужно выйти в море прямо сейчас. Да и то, догонишь ли - большой вопрос...";
			link.l1 = "Все-таки попытаюсь. До свидания и будь здоров.";
			link.l1.go = "exit";
			AddQuestRecord("Eng_Line_6_MorrisWillams", "9");
			pchar.questTemp.State = "MorrisWillams_CatchUpPinnace";
            SaveCurrentQuestDateParam("questTemp");
			pchar.quest.MorrisWillams_CheckTimeForForward.win_condition.l1 = "location";
			pchar.quest.MorrisWillams_CheckTimeForForward.win_condition.l1.location = "Jamaica";
			pchar.quest.MorrisWillams_CheckTimeForForward.win_condition = "MorrisWillams_CheckTimeForForward";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

