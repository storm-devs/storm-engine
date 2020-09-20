// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "М-да, позвольте угадаю... Опять ничего существенного?",
                          "Послушайте, я финансами оперирую, а не на вопросы отвечаю...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Вы угадали, простите...", "Я понимаю...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "MorrisWillams_ArrestedToUsurer")
            {
                link.l1 = "Мне нужно поговорить с вами о Моррисе Уильямсе.";
                link.l1.go = "Step_E6_1";
            }
			if (pchar.questTemp.State == "MorrisWillams_AfterFightGoRedmond")
            {
                link.l1 = "Ваших партнеров вы больше не увидите.";
                link.l1.go = "Step_E6_5";
            }
		break;

		case "Step_E6_1":
			dialog.text = "Что случилось с Моррисом Уильямсом?";
			link.l1 = "Вы что, не знаете, что он в тюрьме?!";
			link.l1.go = "Step_E6_2";
		break;
		case "Step_E6_2":
			dialog.text = "Знаю, конечно. А что случилось-то, он умер?";
			link.l1 = "Нет, не умер, но он очень страдает. Вы, наверное, знаете, что ему просто жутко не повезло, иначе долг был бы вам выплачен.";
			link.l1.go = "Step_E6_3";
		break;
		case "Step_E6_3":
			dialog.text = "Но он же не выплачен!";
			link.l1 = "И что, вас не заботит, что капер может сгнить в тюрьме?!";
			link.l1.go = "Step_E6_4";
		break;
		case "Step_E6_4":
			dialog.text = "Меня беспокоит только собственное здоровье, а на остальное мне наплевать, сэр. Я больше не желаю говорить об этом.";
			link.l1 = "Я это дело так не оставлю. Мы встретимся еще...";
			link.l1.go = "exit";
            pchar.questTemp.State = "MorrisWillams_ArrestedSeekInfo";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "18");
		break;
		case "Step_E6_5":
			dialog.text = "О ком это вы?";
			link.l1 = "Я о контрабандистах, с которыми вы имеете дела. Я пустил им кровь.";
			link.l1.go = "Step_E6_6";
		break;
		case "Step_E6_6":
			dialog.text = "Хм... И что же вам нужно?";
			link.l1 = "Долговые расписки Морриса Уильямса для начала. И оставьте в покое семью Эттербери, которая имела неосторожность взять у вас деньги в долг год назад.";
			link.l1.go = "Step_E6_7";
		break;
		case "Step_E6_7":
			dialog.text = "И это говорит пират?!! Ну хорошо, Уильямс немедленно будет освобожден.";
			link.l1 = "Это еще не все. За мое молчание о ваших делишках с контрабандой вы будете выплачивать мне... 10000 ежемесячно.";
			link.l1.go = "Step_E6_8";
		break;
		case "Step_E6_8":
			dialog.text = "Ха-ха! Нет, не буду.\n"+
                     "Мою связь с контрабандой вы доказать уже не сможете, только ваше слово против моего.\n"+
                     "Но мне не нужен скандал, поэтому я иду вам на уступки по Уильямсу. Большего вам не видать. Прощайте.";
			link.l1 = "Прощайте. И не забудьте о том, что вы мне обещали.";
			link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "5";
            //ChangeCharacterReputation(pchar, -2); // за попытку шантажа TO_DO eddy
            AddCharacterExpToSkill(pchar, "Accuracy", 80);
            AddCharacterExpToSkill(pchar, "Commerce", 50);
            AddCharacterExpToSkill(pchar, "Sneak ", 70);
            AddCharacterExpToSkill(pchar, "Fencing  ", 100);
            AddCharacterExpToSkill(pchar, "Defence  ", 50);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "22");
            CloseQuestHeader("Eng_Line_6_MorrisWillams");
			ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Morris_Willams")], "PortRoyal_tavern", "sit", "sit1");
			characters[GetCharacterIndex("Morris_Willams")].dialog.currentnode = "Willams_NiceTalking";
            LAi_SetSitTypeNoGroup(characterFromID("Morris_Willams"));
		break;


	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

